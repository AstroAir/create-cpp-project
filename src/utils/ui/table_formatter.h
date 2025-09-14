#pragma once

#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "../ui/terminal_utils.h"

namespace utils {

// Table cell alignment
enum class Alignment { Left, Center, Right };

// Enhanced table cell data with formatting
struct EnhancedTableCell {
    std::string content;
    Color textColor = Color::White;
    Color backgroundColor = Color::Black;
    std::vector<TextStyle> styles;
    Alignment alignment = Alignment::Left;

    EnhancedTableCell() = default;
    EnhancedTableCell(const std::string& text) : content(text) {}
    EnhancedTableCell(const std::string& text, Color color) : content(text), textColor(color) {}
    EnhancedTableCell(const std::string& text, Color color, Alignment align)
        : content(text), textColor(color), alignment(align) {}
};

// Table column configuration
struct ColumnConfig {
    std::string header;
    size_t width = 0;  // 0 = auto-size
    size_t minWidth = 3;
    size_t maxWidth = 50;
    Alignment alignment = Alignment::Left;
    Color headerColor = Color::BrightCyan;
    std::vector<TextStyle> headerStyles = {TextStyle::Bold};
    bool wordWrap = true;

    ColumnConfig() = default;
    ColumnConfig(const std::string& h) : header(h) {}
    ColumnConfig(const std::string& h, size_t w) : header(h), width(w) {}
    ColumnConfig(const std::string& h, size_t w, Alignment a) : header(h), width(w), alignment(a) {}
};

// Enhanced table border style
enum class TableBorderStyle {
    None,
    Simple,   // +---+---+
    Double,   // ╔═══╦═══╗
    Rounded,  // ╭───┬───╮
    Thick,    // ┏━━━┳━━━┓
    ASCII     // +---+---+
};

// Table configuration
struct TableConfig {
    TableBorderStyle borderStyle = TableBorderStyle::Simple;
    bool showHeader = true;
    bool showRowNumbers = false;
    bool alternateRowColors = false;
    Color evenRowColor = Color::Black;
    Color oddRowColor = Color::BrightBlack;
    size_t maxTableWidth = 120;
    std::string title;
    Color titleColor = Color::BrightWhite;
    std::vector<TextStyle> titleStyles = {TextStyle::Bold};
    bool centerTitle = true;

    // Padding
    size_t leftPadding = 1;
    size_t rightPadding = 1;
    size_t topPadding = 0;
    size_t bottomPadding = 0;
};

// Table data structure
using EnhancedTableRow = std::vector<EnhancedTableCell>;
using EnhancedTableData = std::vector<EnhancedTableRow>;

class TableFormatter {
   public:
    TableFormatter();
    explicit TableFormatter(const TableConfig& config);

    // Configuration
    void setConfig(const TableConfig& config);
    void setBorderStyle(TableBorderStyle style);
    void setMaxWidth(size_t width);
    void setTitle(const std::string& title, Color color = Color::BrightWhite);
    void showHeader(bool show);
    void showRowNumbers(bool show);
    void setAlternateRowColors(bool enable, Color evenColor = Color::Black,
                               Color oddColor = Color::BrightBlack);

    // Column management
    void addColumn(const ColumnConfig& column);
    void setColumns(const std::vector<ColumnConfig>& columns);
    void setColumnWidth(size_t columnIndex, size_t width);
    void setColumnAlignment(size_t columnIndex, Alignment alignment);

    // Data management
    void addRow(const EnhancedTableRow& row);
    void addRow(const std::vector<std::string>& row);
    void addRow(const std::initializer_list<std::string>& row);
    void setData(const EnhancedTableData& data);
    void clearData();

    // Convenience methods for adding formatted rows
    void addHeaderRow(const std::vector<std::string>& headers);
    void addSeparatorRow();
    void addEmptyRow();

    // Rendering
    std::string render() const;
    void print() const;
    void printToStream(std::ostream& stream) const;

    // Statistics and information
    size_t getColumnCount() const;
    size_t getRowCount() const;
    size_t getEstimatedWidth() const;
    size_t getEstimatedHeight() const;

    // Utility methods
    static std::string alignText(const std::string& text, size_t width, Alignment alignment);
    static std::vector<std::string> wrapText(const std::string& text, size_t width);
    static size_t getDisplayWidth(const std::string& text);  // Handles ANSI codes

   protected:
    TableConfig config_;
    std::vector<ColumnConfig> columns_;
    EnhancedTableData data_;

   private:
    // Border characters for different styles
    struct BorderChars {
        char topLeft, topRight, bottomLeft, bottomRight;
        char horizontal, vertical;
        char topJunction, bottomJunction, leftJunction, rightJunction;
        char cross;
    };

    BorderChars getBorderChars() const;
    std::vector<size_t> calculateColumnWidths() const;
    std::string renderTitle() const;
    std::string renderHeader() const;
    std::string renderSeparator(bool isTop = false, bool isBottom = false) const;
    std::string renderRow(const EnhancedTableRow& row, size_t rowIndex) const;
    std::string renderCell(const EnhancedTableCell& cell, size_t width, size_t columnIndex) const;
    std::string formatCellContent(const std::string& content, const EnhancedTableCell& cell,
                                  size_t width) const;

    // Helper methods
    size_t getActualColumnCount() const;
    EnhancedTableCell createCell(const std::string& content, size_t columnIndex) const;
    Color getRowBackgroundColor(size_t rowIndex) const;
};

// Specialized table types
class StatusTable : public TableFormatter {
   public:
    StatusTable();
    void addStatusRow(const std::string& item, const std::string& status,
                      Color statusColor = Color::Green);
    void addProgressRow(const std::string& item, int progress, const std::string& details = "");
    void addErrorRow(const std::string& item, const std::string& error);
    void addWarningRow(const std::string& item, const std::string& warning);
    void addInfoRow(const std::string& item, const std::string& info);
};

class ConfigurationTable : public TableFormatter {
   public:
    ConfigurationTable();
    void addConfigRow(const std::string& key, const std::string& value,
                      const std::string& description = "");
    void addSectionHeader(const std::string& section);
};

class ComparisonTable : public TableFormatter {
   public:
    ComparisonTable();
    void addComparisonRow(const std::string& feature, const std::string& option1,
                          const std::string& option2);
    void addComparisonRow(const std::string& feature, const std::vector<std::string>& options);
};

// Utility functions for quick table creation
namespace TableUtils {
// Quick table creation
std::string createSimpleTable(const std::vector<std::string>& headers,
                              const std::vector<std::vector<std::string>>& rows);
std::string createStatusTable(const std::vector<std::pair<std::string, std::string>>& items);
std::string createKeyValueTable(const std::unordered_map<std::string, std::string>& data);

// Table formatting helpers
std::string centerText(const std::string& text, size_t width);
std::string padText(const std::string& text, size_t width, char padChar = ' ');
std::string truncateText(const std::string& text, size_t maxWidth,
                         const std::string& suffix = "...");
}  // namespace TableUtils

}  // namespace utils
