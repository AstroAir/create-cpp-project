#include "table_formatter.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace utils {

TableFormatter::TableFormatter() = default;

TableFormatter::TableFormatter(const TableConfig& config) : config_(config) {}

void TableFormatter::setConfig(const TableConfig& config) {
    config_ = config;
}

void TableFormatter::setBorderStyle(TableBorderStyle style) {
    config_.borderStyle = style;
}

void TableFormatter::setMaxWidth(size_t width) {
    config_.maxTableWidth = width;
}

void TableFormatter::setTitle(const std::string& title, Color color) {
    config_.title = title;
    config_.titleColor = color;
}

void TableFormatter::showHeader(bool show) {
    config_.showHeader = show;
}

void TableFormatter::showRowNumbers(bool show) {
    config_.showRowNumbers = show;
}

void TableFormatter::setAlternateRowColors(bool enable, Color evenColor, Color oddColor) {
    config_.alternateRowColors = enable;
    config_.evenRowColor = evenColor;
    config_.oddRowColor = oddColor;
}

void TableFormatter::addColumn(const ColumnConfig& column) {
    columns_.push_back(column);
}

void TableFormatter::setColumns(const std::vector<ColumnConfig>& columns) {
    columns_ = columns;
}

void TableFormatter::setColumnWidth(size_t columnIndex, size_t width) {
    if (columnIndex < columns_.size()) {
        columns_[columnIndex].width = width;
    }
}

void TableFormatter::setColumnAlignment(size_t columnIndex, Alignment alignment) {
    if (columnIndex < columns_.size()) {
        columns_[columnIndex].alignment = alignment;
    }
}

void TableFormatter::addRow(const EnhancedTableRow& row) {
    data_.push_back(row);
}

void TableFormatter::addRow(const std::vector<std::string>& row) {
    EnhancedTableRow tableRow;
    for (const auto& cell : row) {
        tableRow.emplace_back(cell);
    }
    data_.push_back(tableRow);
}

void TableFormatter::addRow(const std::initializer_list<std::string>& row) {
    EnhancedTableRow tableRow;
    for (const auto& cell : row) {
        tableRow.emplace_back(cell);
    }
    data_.push_back(tableRow);
}

void TableFormatter::setData(const EnhancedTableData& data) {
    data_ = data;
}

void TableFormatter::clearData() {
    data_.clear();
}

void TableFormatter::addHeaderRow(const std::vector<std::string>& headers) {
    EnhancedTableRow headerRow;
    for (size_t i = 0; i < headers.size(); ++i) {
        EnhancedTableCell cell(headers[i]);
        if (i < columns_.size()) {
            cell.textColor = columns_[i].headerColor;
            cell.styles = columns_[i].headerStyles;
        } else {
            cell.textColor = Color::BrightCyan;
            cell.styles = {TextStyle::Bold};
        }
        headerRow.push_back(cell);
    }
    data_.push_back(headerRow);
}

void TableFormatter::addSeparatorRow() {
    EnhancedTableRow separatorRow;
    size_t columnCount = getActualColumnCount();
    for (size_t i = 0; i < columnCount; ++i) {
        separatorRow.emplace_back("---");
    }
    data_.push_back(separatorRow);
}

void TableFormatter::addEmptyRow() {
    EnhancedTableRow emptyRow;
    size_t columnCount = getActualColumnCount();
    for (size_t i = 0; i < columnCount; ++i) {
        emptyRow.emplace_back("");
    }
    data_.push_back(emptyRow);
}

std::string TableFormatter::render() const {
    if (data_.empty() && columns_.empty()) {
        return "";
    }

    std::ostringstream oss;
    auto columnWidths = calculateColumnWidths();

    // Render title
    if (!config_.title.empty()) {
        oss << renderTitle() << "\n";
    }

    // Render top border
    if (config_.borderStyle != TableBorderStyle::None) {
        oss << renderSeparator(true, false) << "\n";
    }

    // Render header
    if (config_.showHeader && !columns_.empty()) {
        oss << renderHeader() << "\n";
        if (config_.borderStyle != TableBorderStyle::None) {
            oss << renderSeparator(false, false) << "\n";
        }
    }

    // Render data rows
    for (size_t i = 0; i < data_.size(); ++i) {
        oss << renderRow(data_[i], i) << "\n";
    }

    // Render bottom border
    if (config_.borderStyle != TableBorderStyle::None) {
        oss << renderSeparator(false, true) << "\n";
    }

    return oss.str();
}

void TableFormatter::print() const {
    std::cout << render();
}

void TableFormatter::printToStream(std::ostream& stream) const {
    stream << render();
}

size_t TableFormatter::getColumnCount() const {
    return columns_.size();
}

size_t TableFormatter::getRowCount() const {
    return data_.size();
}

size_t TableFormatter::getEstimatedWidth() const {
    auto columnWidths = calculateColumnWidths();
    size_t totalWidth = 0;

    for (size_t width : columnWidths) {
        totalWidth += width;
    }

    // Add borders and padding
    if (config_.borderStyle != TableBorderStyle::None) {
        totalWidth += (columnWidths.size() + 1);  // Vertical borders
    }
    totalWidth += columnWidths.size() * (config_.leftPadding + config_.rightPadding);

    return totalWidth;
}

size_t TableFormatter::getEstimatedHeight() const {
    size_t height = data_.size();

    if (config_.showHeader && !columns_.empty()) {
        height += 1;  // Header row
    }

    if (config_.borderStyle != TableBorderStyle::None) {
        height += 2;  // Top and bottom borders
        if (config_.showHeader && !columns_.empty()) {
            height += 1;  // Header separator
        }
    }

    if (!config_.title.empty()) {
        height += 1;  // Title
    }

    return height;
}

std::string TableFormatter::alignText(const std::string& text, size_t width, Alignment alignment) {
    size_t textWidth = getDisplayWidth(text);
    if (textWidth >= width) {
        return text;
    }

    size_t padding = width - textWidth;

    switch (alignment) {
        case Alignment::Left:
            return text + std::string(padding, ' ');
        case Alignment::Right:
            return std::string(padding, ' ') + text;
        case Alignment::Center: {
            size_t leftPad = padding / 2;
            size_t rightPad = padding - leftPad;
            return std::string(leftPad, ' ') + text + std::string(rightPad, ' ');
        }
        default:
            return text;
    }
}

std::vector<std::string> TableFormatter::wrapText(const std::string& text, size_t width) {
    std::vector<std::string> lines;
    if (width == 0) {
        lines.push_back(text);
        return lines;
    }

    std::istringstream iss(text);
    std::string word;
    std::string currentLine;

    while (iss >> word) {
        if (currentLine.empty()) {
            currentLine = word;
        } else if (currentLine.length() + 1 + word.length() <= width) {
            currentLine += " " + word;
        } else {
            lines.push_back(currentLine);
            currentLine = word;
        }
    }

    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }

    if (lines.empty()) {
        lines.push_back("");
    }

    return lines;
}

size_t TableFormatter::getDisplayWidth(const std::string& text) {
    // Simple implementation - doesn't handle ANSI escape codes
    // In a full implementation, you'd strip ANSI codes before measuring
    return text.length();
}

TableFormatter::BorderChars TableFormatter::getBorderChars() const {
    switch (config_.borderStyle) {
        case TableBorderStyle::Simple:
        case TableBorderStyle::ASCII:
            return {'+', '+', '+', '+', '-', '|', '+', '+', '+', '+', '+'};
        case TableBorderStyle::Double:
            // Use ASCII alternatives for Unicode characters to avoid compilation issues
            return {'+', '+', '+', '+', '=', '|', '+', '+', '+', '+', '+'};
        case TableBorderStyle::Rounded:
            return {'+', '+', '+', '+', '-', '|', '+', '+', '+', '+', '+'};
        case TableBorderStyle::Thick:
            return {'+', '+', '+', '+', '=', '|', '+', '+', '+', '+', '+'};
        default:
            return {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
    }
}

std::vector<size_t> TableFormatter::calculateColumnWidths() const {
    size_t columnCount = getActualColumnCount();
    std::vector<size_t> widths(columnCount, 0);

    // Initialize with configured widths or minimum widths
    for (size_t i = 0; i < columnCount && i < columns_.size(); ++i) {
        if (columns_[i].width > 0) {
            widths[i] = columns_[i].width;
        } else {
            widths[i] = columns_[i].minWidth;
        }
    }

    // Calculate required widths based on content
    for (size_t i = 0; i < columnCount; ++i) {
        // Check header width
        if (i < columns_.size() && config_.showHeader) {
            widths[i] = std::max(widths[i], getDisplayWidth(columns_[i].header));
        }

        // Check data widths
        for (const auto& row : data_) {
            if (i < row.size()) {
                widths[i] = std::max(widths[i], getDisplayWidth(row[i].content));
            }
        }

        // Apply maximum width constraint
        if (i < columns_.size()) {
            widths[i] = std::min(widths[i], columns_[i].maxWidth);
        }
    }

    return widths;
}

size_t TableFormatter::getActualColumnCount() const {
    size_t maxColumns = columns_.size();

    for (const auto& row : data_) {
        maxColumns = std::max(maxColumns, row.size());
    }

    return maxColumns;
}

std::string TableFormatter::renderTitle() const {
    if (config_.title.empty()) {
        return "";
    }

    size_t tableWidth = getEstimatedWidth();
    std::string title = config_.title;

    if (config_.centerTitle) {
        title = alignText(title, tableWidth, Alignment::Center);
    }

    return TerminalUtils::colorAndStyle(title, config_.titleColor, config_.titleStyles);
}

std::string TableFormatter::renderHeader() const {
    if (columns_.empty()) {
        return "";
    }

    EnhancedTableRow headerRow;
    for (const auto& column : columns_) {
        EnhancedTableCell cell(column.header, column.headerColor);
        cell.styles = column.headerStyles;
        cell.alignment = column.alignment;
        headerRow.push_back(cell);
    }

    return renderRow(headerRow, 0);
}

std::string TableFormatter::renderSeparator(bool isTop, bool isBottom) const {
    auto borderChars = getBorderChars();
    auto columnWidths = calculateColumnWidths();

    std::ostringstream oss;

    // Left corner
    if (isTop) {
        oss << borderChars.topLeft;
    } else if (isBottom) {
        oss << borderChars.bottomLeft;
    } else {
        oss << borderChars.leftJunction;
    }

    // Columns with separators
    for (size_t i = 0; i < columnWidths.size(); ++i) {
        // Horizontal line for column
        size_t lineWidth = columnWidths[i] + config_.leftPadding + config_.rightPadding;
        oss << std::string(lineWidth, borderChars.horizontal);

        // Junction or corner
        if (i < columnWidths.size() - 1) {
            if (isTop) {
                oss << borderChars.topJunction;
            } else if (isBottom) {
                oss << borderChars.bottomJunction;
            } else {
                oss << borderChars.cross;
            }
        }
    }

    // Right corner
    if (isTop) {
        oss << borderChars.topRight;
    } else if (isBottom) {
        oss << borderChars.bottomRight;
    } else {
        oss << borderChars.rightJunction;
    }

    return oss.str();
}

std::string TableFormatter::renderRow(const EnhancedTableRow& row, size_t rowIndex) const {
    auto columnWidths = calculateColumnWidths();
    auto borderChars = getBorderChars();

    std::ostringstream oss;

    // Left border
    if (config_.borderStyle != TableBorderStyle::None) {
        oss << borderChars.vertical;
    }

    // Cells
    for (size_t i = 0; i < columnWidths.size(); ++i) {
        // Padding
        oss << std::string(config_.leftPadding, ' ');

        // Cell content
        if (i < row.size()) {
            oss << renderCell(row[i], columnWidths[i], i);
        } else {
            oss << std::string(columnWidths[i], ' ');
        }

        // Padding
        oss << std::string(config_.rightPadding, ' ');

        // Column separator
        if (config_.borderStyle != TableBorderStyle::None) {
            oss << borderChars.vertical;
        }
    }

    return oss.str();
}

std::string TableFormatter::renderCell(const EnhancedTableCell& cell, size_t width,
                                       size_t columnIndex) const {
    std::string content = formatCellContent(cell.content, cell, width);

    // Apply alignment
    Alignment alignment = cell.alignment;
    if (alignment == Alignment::Left && columnIndex < columns_.size()) {
        alignment = columns_[columnIndex].alignment;
    }

    content = alignText(content, width, alignment);

    // Apply colors and styles
    if (cell.textColor != Color::White || !cell.styles.empty()) {
        content = TerminalUtils::colorAndStyle(content, cell.textColor, cell.styles);
    }

    return content;
}

std::string TableFormatter::formatCellContent(const std::string& content,
                                              const EnhancedTableCell& cell, size_t width) const {
    if (content.length() <= width) {
        return content;
    }

    // Truncate if too long
    if (width > 3) {
        return content.substr(0, width - 3) + "...";
    } else {
        return content.substr(0, width);
    }
}

EnhancedTableCell TableFormatter::createCell(const std::string& content, size_t columnIndex) const {
    EnhancedTableCell cell(content);

    if (columnIndex < columns_.size()) {
        cell.alignment = columns_[columnIndex].alignment;
    }

    return cell;
}

Color TableFormatter::getRowBackgroundColor(size_t rowIndex) const {
    if (!config_.alternateRowColors) {
        return Color::Black;
    }

    return (rowIndex % 2 == 0) ? config_.evenRowColor : config_.oddRowColor;
}

// StatusTable implementation
StatusTable::StatusTable() {
    TableConfig config;
    config.borderStyle = TableBorderStyle::Simple;
    config.showHeader = true;
    setConfig(config);

    addColumn(ColumnConfig("Item", 30, Alignment::Left));
    addColumn(ColumnConfig("Status", 15, Alignment::Center));
    addColumn(ColumnConfig("Details", 40, Alignment::Left));
}

void StatusTable::addStatusRow(const std::string& item, const std::string& status,
                               Color statusColor) {
    EnhancedTableRow row;
    row.emplace_back(item);
    row.emplace_back(status, statusColor);
    row.emplace_back("");
    addRow(row);
}

void StatusTable::addProgressRow(const std::string& item, int progress,
                                 const std::string& details) {
    EnhancedTableRow row;
    row.emplace_back(item);

    std::string progressStr = std::to_string(progress) + "%";
    Color progressColor = (progress >= 100)  ? Color::Green
                          : (progress >= 50) ? Color::Yellow
                                             : Color::Red;
    row.emplace_back(progressStr, progressColor);
    row.emplace_back(details);
    addRow(row);
}

void StatusTable::addErrorRow(const std::string& item, const std::string& error) {
    addStatusRow(item, "ERROR", Color::Red);
    // Update the last row to include error details
    if (!data_.empty()) {
        data_.back()[2] = EnhancedTableCell(error, Color::Red);
    }
}

void StatusTable::addWarningRow(const std::string& item, const std::string& warning) {
    addStatusRow(item, "WARNING", Color::Yellow);
    if (!data_.empty()) {
        data_.back()[2] = EnhancedTableCell(warning, Color::Yellow);
    }
}

void StatusTable::addInfoRow(const std::string& item, const std::string& info) {
    addStatusRow(item, "INFO", Color::Cyan);
    if (!data_.empty()) {
        data_.back()[2] = EnhancedTableCell(info, Color::Cyan);
    }
}

// ConfigurationTable implementation
ConfigurationTable::ConfigurationTable() {
    TableConfig config;
    config.borderStyle = TableBorderStyle::Simple;
    config.showHeader = true;
    setConfig(config);

    addColumn(ColumnConfig("Setting", 25, Alignment::Left));
    addColumn(ColumnConfig("Value", 30, Alignment::Left));
    addColumn(ColumnConfig("Description", 40, Alignment::Left));
}

void ConfigurationTable::addConfigRow(const std::string& key, const std::string& value,
                                      const std::string& description) {
    EnhancedTableRow row;
    row.emplace_back(key);
    row.emplace_back(value, Color::Cyan);
    row.emplace_back(description);
    addRow(row);
}

void ConfigurationTable::addSectionHeader(const std::string& section) {
    EnhancedTableRow row;
    EnhancedTableCell headerCell(section, Color::BrightYellow);
    headerCell.styles = {TextStyle::Bold};
    row.push_back(headerCell);
    row.emplace_back("");
    row.emplace_back("");
    addRow(row);
}

// ComparisonTable implementation
ComparisonTable::ComparisonTable() {
    TableConfig config;
    config.borderStyle = TableBorderStyle::Simple;
    config.showHeader = true;
    setConfig(config);

    addColumn(ColumnConfig("Feature", 25, Alignment::Left));
    addColumn(ColumnConfig("Option 1", 20, Alignment::Center));
    addColumn(ColumnConfig("Option 2", 20, Alignment::Center));
}

void ComparisonTable::addComparisonRow(const std::string& feature, const std::string& option1,
                                       const std::string& option2) {
    EnhancedTableRow row;
    row.emplace_back(feature);
    row.emplace_back(option1);
    row.emplace_back(option2);
    addRow(row);
}

void ComparisonTable::addComparisonRow(const std::string& feature,
                                       const std::vector<std::string>& options) {
    if (options.size() >= 2) {
        addComparisonRow(feature, options[0], options[1]);
    }
}

// TableUtils implementation
namespace TableUtils {

std::string createSimpleTable(const std::vector<std::string>& headers,
                              const std::vector<std::vector<std::string>>& rows) {
    TableFormatter formatter;

    for (const auto& header : headers) {
        formatter.addColumn(ColumnConfig(header));
    }

    for (const auto& row : rows) {
        formatter.addRow(row);
    }

    return formatter.render();
}

std::string createStatusTable(const std::vector<std::pair<std::string, std::string>>& items) {
    StatusTable table;

    for (const auto& item : items) {
        Color statusColor = (item.second == "OK" || item.second == "SUCCESS") ? Color::Green
                            : (item.second == "WARNING")                      ? Color::Yellow
                                                                              : Color::Red;
        table.addStatusRow(item.first, item.second, statusColor);
    }

    return table.render();
}

std::string createKeyValueTable(const std::unordered_map<std::string, std::string>& data) {
    ConfigurationTable table;

    for (const auto& pair : data) {
        table.addConfigRow(pair.first, pair.second);
    }

    return table.render();
}

std::string centerText(const std::string& text, size_t width) {
    return TableFormatter::alignText(text, width, Alignment::Center);
}

std::string padText(const std::string& text, size_t width, char padChar) {
    if (text.length() >= width) {
        return text;
    }
    return text + std::string(width - text.length(), padChar);
}

std::string truncateText(const std::string& text, size_t maxWidth, const std::string& suffix) {
    if (text.length() <= maxWidth) {
        return text;
    }

    if (maxWidth <= suffix.length()) {
        return text.substr(0, maxWidth);
    }

    return text.substr(0, maxWidth - suffix.length()) + suffix;
}

}  // namespace TableUtils

}  // namespace utils
