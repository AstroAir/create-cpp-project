#include <gtest/gtest.h>
#include "utils/table_formatter.h"

using namespace utils;

class TableFormatterTest : public ::testing::Test {
protected:
    void SetUp() override {
        formatter = std::make_unique<TableFormatter>();
    }

    std::unique_ptr<TableFormatter> formatter;
};

TEST_F(TableFormatterTest, BasicTableCreation) {
    // Add columns
    formatter->addColumn(ColumnConfig("Name", 15));
    formatter->addColumn(ColumnConfig("Status", 10));
    formatter->addColumn(ColumnConfig("Progress", 10));

    // Add data rows
    formatter->addRow({"Project A", "Active", "75%"});
    formatter->addRow({"Project B", "Complete", "100%"});
    formatter->addRow({"Project C", "Pending", "0%"});

    // Render table
    std::string result = formatter->render();
    
    // Basic checks
    EXPECT_FALSE(result.empty());
    EXPECT_TRUE(result.find("Name") != std::string::npos);
    EXPECT_TRUE(result.find("Status") != std::string::npos);
    EXPECT_TRUE(result.find("Progress") != std::string::npos);
    EXPECT_TRUE(result.find("Project A") != std::string::npos);
}

TEST_F(TableFormatterTest, StatusTableFunctionality) {
    StatusTable statusTable;
    
    statusTable.addStatusRow("File Processing", "OK", Color::Green);
    statusTable.addProgressRow("Data Import", 85, "Processing records...");
    statusTable.addErrorRow("Validation", "Schema mismatch");
    statusTable.addWarningRow("Performance", "High memory usage");
    statusTable.addInfoRow("System", "All services running");

    std::string result = statusTable.render();
    
    EXPECT_FALSE(result.empty());
    EXPECT_TRUE(result.find("File Processing") != std::string::npos);
    EXPECT_TRUE(result.find("OK") != std::string::npos);
    EXPECT_TRUE(result.find("85%") != std::string::npos);
    EXPECT_TRUE(result.find("ERROR") != std::string::npos);
    EXPECT_TRUE(result.find("WARNING") != std::string::npos);
    EXPECT_TRUE(result.find("INFO") != std::string::npos);
}

TEST_F(TableFormatterTest, TableConfiguration) {
    TableConfig config;
    config.borderStyle = TableBorderStyle::Double;
    config.showHeader = true;
    config.alternateRowColors = true;
    config.title = "Test Table";
    
    formatter->setConfig(config);
    formatter->addColumn(ColumnConfig("Column 1", 20));
    formatter->addColumn(ColumnConfig("Column 2", 20));
    
    formatter->addRow({"Row 1 Data", "Value 1"});
    formatter->addRow({"Row 2 Data", "Value 2"});
    
    std::string result = formatter->render();
    
    EXPECT_FALSE(result.empty());
    EXPECT_TRUE(result.find("Test Table") != std::string::npos);
    EXPECT_TRUE(result.find("Column 1") != std::string::npos);
    EXPECT_TRUE(result.find("Row 1 Data") != std::string::npos);
}

TEST_F(TableFormatterTest, EmptyTable) {
    std::string result = formatter->render();
    EXPECT_TRUE(result.empty());
}

TEST_F(TableFormatterTest, TableDimensions) {
    formatter->addColumn(ColumnConfig("Test", 10));
    formatter->addRow({"Data"});
    
    EXPECT_EQ(formatter->getColumnCount(), 1);
    EXPECT_EQ(formatter->getRowCount(), 1);
    EXPECT_GT(formatter->getEstimatedWidth(), 0);
    EXPECT_GT(formatter->getEstimatedHeight(), 0);
}

TEST_F(TableFormatterTest, TextAlignment) {
    std::string leftAligned = TableFormatter::alignText("test", 10, Alignment::Left);
    std::string rightAligned = TableFormatter::alignText("test", 10, Alignment::Right);
    std::string centerAligned = TableFormatter::alignText("test", 10, Alignment::Center);
    
    EXPECT_EQ(leftAligned.length(), 10);
    EXPECT_EQ(rightAligned.length(), 10);
    EXPECT_EQ(centerAligned.length(), 10);
    
    // Check that text is positioned correctly
    EXPECT_EQ(leftAligned.substr(0, 4), "test");
    EXPECT_EQ(rightAligned.substr(6, 4), "test");
    EXPECT_TRUE(centerAligned.find("test") != std::string::npos);
}

TEST_F(TableFormatterTest, TextWrapping) {
    std::vector<std::string> wrapped = TableFormatter::wrapText("This is a long text that should be wrapped", 10);
    
    EXPECT_GT(wrapped.size(), 1);
    for (const auto& line : wrapped) {
        EXPECT_LE(line.length(), 10);
    }
}

TEST_F(TableFormatterTest, DisplayWidth) {
    EXPECT_EQ(TableFormatter::getDisplayWidth("test"), 4);
    EXPECT_EQ(TableFormatter::getDisplayWidth(""), 0);
    EXPECT_EQ(TableFormatter::getDisplayWidth("hello world"), 11);
}

// Test utility functions
TEST(TableUtilsTest, CenterText) {
    std::string centered = TableUtils::centerText("test", 10);
    EXPECT_EQ(centered.length(), 10);
    EXPECT_TRUE(centered.find("test") != std::string::npos);
}

TEST(TableUtilsTest, PadText) {
    std::string padded = TableUtils::padText("test", 8);
    EXPECT_EQ(padded.length(), 8);
    EXPECT_EQ(padded.substr(0, 4), "test");
}

TEST(TableUtilsTest, TruncateText) {
    std::string truncated = TableUtils::truncateText("This is a very long text", 10);
    EXPECT_LE(truncated.length(), 10);
    EXPECT_TRUE(truncated.find("...") != std::string::npos);
}
