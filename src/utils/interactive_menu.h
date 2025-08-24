#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <map>
#include <optional>

namespace utils {

// Menu item types
enum class MenuItemType {
    Action,      // Executes an action
    Submenu,     // Opens a submenu
    Toggle,      // Boolean toggle
    Input,       // Text input
    Choice,      // Single choice from list
    MultiChoice, // Multiple choices from list
    Separator,   // Visual separator
    Info         // Information display
};

// Menu item definition
struct MenuItem {
    std::string id;
    std::string title;
    std::string description;
    MenuItemType type;
    bool enabled{true};
    bool visible{true};
    
    // Action callback
    std::function<bool()> action;
    
    // Submenu items (for Submenu type)
    std::vector<MenuItem> submenuItems;
    
    // Toggle state (for Toggle type)
    bool toggleState{false};
    std::function<void(bool)> toggleCallback;
    
    // Input properties (for Input type)
    std::string inputValue;
    std::string inputPrompt;
    std::function<bool(const std::string&)> inputValidator;
    std::function<void(const std::string&)> inputCallback;
    
    // Choice properties (for Choice/MultiChoice type)
    std::vector<std::string> choices;
    std::vector<int> selectedChoices;
    std::function<void(const std::vector<int>&)> choiceCallback;
    
    // Styling
    std::string icon;
    std::string color;
    bool highlighted{false};
};

// Menu configuration
struct MenuConfig {
    std::string title;
    std::string subtitle;
    std::string footer;
    bool showNumbers{true};
    bool showIcons{true};
    bool showDescriptions{true};
    bool allowBack{true};
    bool allowExit{true};
    bool clearScreen{true};
    bool showBorder{true};
    std::string borderStyle{"rounded"}; // "simple", "double", "rounded"
    std::string theme{"default"}; // "default", "dark", "light", "colorful"
};

// Interactive menu system
class InteractiveMenu {
public:
    InteractiveMenu(const MenuConfig& config = MenuConfig{});
    ~InteractiveMenu() = default;
    
    // Menu management
    void setConfig(const MenuConfig& config);
    void setTitle(const std::string& title);
    void setSubtitle(const std::string& subtitle);
    void setFooter(const std::string& footer);
    
    // Item management
    void addItem(const MenuItem& item);
    void addAction(const std::string& id, const std::string& title, std::function<bool()> action, const std::string& description = "");
    void addSubmenu(const std::string& id, const std::string& title, const std::vector<MenuItem>& items, const std::string& description = "");
    void addToggle(const std::string& id, const std::string& title, bool initialState, std::function<void(bool)> callback, const std::string& description = "");
    void addInput(const std::string& id, const std::string& title, const std::string& prompt, std::function<void(const std::string&)> callback, const std::string& description = "");
    void addChoice(const std::string& id, const std::string& title, const std::vector<std::string>& choices, std::function<void(int)> callback, const std::string& description = "");
    void addMultiChoice(const std::string& id, const std::string& title, const std::vector<std::string>& choices, std::function<void(const std::vector<int>&)> callback, const std::string& description = "");
    void addSeparator(const std::string& title = "");
    void addInfo(const std::string& id, const std::string& title, const std::string& info);
    
    // Item modification
    void setItemEnabled(const std::string& id, bool enabled);
    void setItemVisible(const std::string& id, bool visible);
    void setItemHighlighted(const std::string& id, bool highlighted);
    void updateItemTitle(const std::string& id, const std::string& title);
    void updateItemDescription(const std::string& id, const std::string& description);
    
    // Menu execution
    bool show();
    void close();
    
    // Navigation
    void goBack();
    void goToSubmenu(const std::string& submenuId);
    
    // State management
    void saveState();
    void restoreState();
    void clearState();
    
private:
    MenuConfig m_config;
    std::vector<MenuItem> m_items;
    std::vector<std::vector<MenuItem>> m_menuStack;
    std::map<std::string, std::string> m_state;
    bool m_running{false};
    
    // Display methods
    void render();
    void renderHeader();
    void renderItems();
    void renderFooter();
    void renderBorder(const std::string& content);
    
    // Input handling
    int getSelection();
    bool handleSelection(int selection);
    
    // Item handling
    bool executeAction(const MenuItem& item);
    bool handleToggle(MenuItem& item);
    bool handleInput(MenuItem& item);
    bool handleChoice(MenuItem& item);
    bool handleMultiChoice(MenuItem& item);
    void enterSubmenu(const MenuItem& item);
    
    // Utility methods
    MenuItem* findItem(const std::string& id);
    std::string formatItem(const MenuItem& item, int index) const;
    std::string getItemIcon(const MenuItem& item) const;
    std::string getItemColor(const MenuItem& item) const;
    void clearScreen();
    void waitForEnter();
};

// Menu builder for fluent interface
class MenuBuilder {
public:
    MenuBuilder();
    
    // Configuration
    MenuBuilder& title(const std::string& title);
    MenuBuilder& subtitle(const std::string& subtitle);
    MenuBuilder& footer(const std::string& footer);
    MenuBuilder& theme(const std::string& theme);
    MenuBuilder& borderStyle(const std::string& style);
    MenuBuilder& showNumbers(bool show);
    MenuBuilder& showIcons(bool show);
    MenuBuilder& showDescriptions(bool show);
    MenuBuilder& allowBack(bool allow);
    MenuBuilder& allowExit(bool allow);
    MenuBuilder& clearScreen(bool clear);
    
    // Items
    MenuBuilder& action(const std::string& id, const std::string& title, std::function<bool()> action, const std::string& description = "");
    MenuBuilder& submenu(const std::string& id, const std::string& title, std::function<void(MenuBuilder&)> builder, const std::string& description = "");
    MenuBuilder& toggle(const std::string& id, const std::string& title, bool initialState, std::function<void(bool)> callback, const std::string& description = "");
    MenuBuilder& input(const std::string& id, const std::string& title, const std::string& prompt, std::function<void(const std::string&)> callback, const std::string& description = "");
    MenuBuilder& choice(const std::string& id, const std::string& title, const std::vector<std::string>& choices, std::function<void(int)> callback, const std::string& description = "");
    MenuBuilder& multiChoice(const std::string& id, const std::string& title, const std::vector<std::string>& choices, std::function<void(const std::vector<int>&)> callback, const std::string& description = "");
    MenuBuilder& separator(const std::string& title = "");
    MenuBuilder& info(const std::string& id, const std::string& title, const std::string& info);
    
    // Build and show
    std::unique_ptr<InteractiveMenu> build();
    bool show();
    
private:
    MenuConfig m_config;
    std::vector<MenuItem> m_items;
};

// Predefined menu templates
namespace menus {
    // Configuration menu
    std::unique_ptr<InteractiveMenu> createConfigurationMenu();
    
    // Project creation menu
    std::unique_ptr<InteractiveMenu> createProjectMenu();
    
    // Template selection menu
    std::unique_ptr<InteractiveMenu> createTemplateMenu();
    
    // Framework selection menu
    std::unique_ptr<InteractiveMenu> createFrameworkMenu();
    
    // Settings menu
    std::unique_ptr<InteractiveMenu> createSettingsMenu();
    
    // Help menu
    std::unique_ptr<InteractiveMenu> createHelpMenu();
    
    // Main menu
    std::unique_ptr<InteractiveMenu> createMainMenu();
}

// Menu utilities
namespace menu_utils {
    // Input validation
    bool validateProjectName(const std::string& name);
    bool validateEmail(const std::string& email);
    bool validatePath(const std::string& path);
    bool validateURL(const std::string& url);
    
    // Choice helpers
    std::vector<std::string> getTemplateChoices();
    std::vector<std::string> getBuildSystemChoices();
    std::vector<std::string> getPackageManagerChoices();
    std::vector<std::string> getFrameworkChoices();
    
    // Formatting helpers
    std::string formatChoice(const std::string& choice, bool selected);
    std::string formatToggle(const std::string& title, bool state);
    std::string formatProgress(double progress);
    
    // Color themes
    struct ColorTheme {
        std::string primary;
        std::string secondary;
        std::string accent;
        std::string success;
        std::string warning;
        std::string error;
        std::string info;
        std::string muted;
    };
    
    ColorTheme getTheme(const std::string& themeName);
    void applyTheme(const ColorTheme& theme);
}

} // namespace utils
