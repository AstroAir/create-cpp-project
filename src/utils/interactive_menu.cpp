#include "interactive_menu.h"
#include "terminal_utils.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <sstream>
#include <limits>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

namespace utils {

// InteractiveMenu Implementation
InteractiveMenu::InteractiveMenu(const MenuConfig& config) : m_config(config) {
}

void InteractiveMenu::setConfig(const MenuConfig& config) {
    m_config = config;
}

void InteractiveMenu::setTitle(const std::string& title) {
    m_config.title = title;
}

void InteractiveMenu::setSubtitle(const std::string& subtitle) {
    m_config.subtitle = subtitle;
}

void InteractiveMenu::setFooter(const std::string& footer) {
    m_config.footer = footer;
}

void InteractiveMenu::addItem(const MenuItem& item) {
    m_items.push_back(item);
}

void InteractiveMenu::addAction(const std::string& id, const std::string& title, std::function<bool()> action, const std::string& description) {
    MenuItem item;
    item.id = id;
    item.title = title;
    item.description = description;
    item.type = MenuItemType::Action;
    item.action = action;
    item.icon = "▶";
    m_items.push_back(item);
}

void InteractiveMenu::addSubmenu(const std::string& id, const std::string& title, const std::vector<MenuItem>& items, const std::string& description) {
    MenuItem item;
    item.id = id;
    item.title = title;
    item.description = description;
    item.type = MenuItemType::Submenu;
    item.submenuItems = items;
    item.icon = "📁";
    m_items.push_back(item);
}

void InteractiveMenu::addToggle(const std::string& id, const std::string& title, bool initialState, std::function<void(bool)> callback, const std::string& description) {
    MenuItem item;
    item.id = id;
    item.title = title;
    item.description = description;
    item.type = MenuItemType::Toggle;
    item.toggleState = initialState;
    item.toggleCallback = callback;
    item.icon = initialState ? "☑" : "☐";
    m_items.push_back(item);
}

void InteractiveMenu::addInput(const std::string& id, const std::string& title, const std::string& prompt, std::function<void(const std::string&)> callback, const std::string& description) {
    MenuItem item;
    item.id = id;
    item.title = title;
    item.description = description;
    item.type = MenuItemType::Input;
    item.inputPrompt = prompt;
    item.inputCallback = callback;
    item.icon = "✏";
    m_items.push_back(item);
}

void InteractiveMenu::addChoice(const std::string& id, const std::string& title, const std::vector<std::string>& choices, std::function<void(int)> callback, const std::string& description) {
    MenuItem item;
    item.id = id;
    item.title = title;
    item.description = description;
    item.type = MenuItemType::Choice;
    item.choices = choices;
    item.choiceCallback = [callback](const std::vector<int>& selected) {
        if (!selected.empty()) {
            callback(selected[0]);
        }
    };
    item.icon = "📋";
    m_items.push_back(item);
}

void InteractiveMenu::addMultiChoice(const std::string& id, const std::string& title, const std::vector<std::string>& choices, std::function<void(const std::vector<int>&)> callback, const std::string& description) {
    MenuItem item;
    item.id = id;
    item.title = title;
    item.description = description;
    item.type = MenuItemType::MultiChoice;
    item.choices = choices;
    item.choiceCallback = callback;
    item.icon = "☰";
    m_items.push_back(item);
}

void InteractiveMenu::addSeparator(const std::string& title) {
    MenuItem item;
    item.id = "separator_" + std::to_string(m_items.size());
    item.title = title;
    item.type = MenuItemType::Separator;
    item.icon = "─";
    m_items.push_back(item);
}

void InteractiveMenu::addInfo(const std::string& id, const std::string& title, const std::string& info) {
    MenuItem item;
    item.id = id;
    item.title = title;
    item.description = info;
    item.type = MenuItemType::Info;
    item.icon = "ℹ";
    m_items.push_back(item);
}

bool InteractiveMenu::show() {
    m_running = true;
    
    while (m_running) {
        if (m_config.clearScreen) {
            clearScreen();
        }
        
        render();
        
        int selection = getSelection();
        if (selection == -1) {
            // Exit requested
            break;
        }
        
        if (!handleSelection(selection)) {
            break;
        }
    }
    
    return true;
}

void InteractiveMenu::close() {
    m_running = false;
}

void InteractiveMenu::render() {
    renderHeader();
    renderItems();
    renderFooter();
}

void InteractiveMenu::renderHeader() {
    if (!m_config.title.empty()) {
        std::cout << TerminalUtils::colorize(m_config.title, utils::Color::BrightCyan) << std::endl;
        
        if (!m_config.subtitle.empty()) {
            std::cout << TerminalUtils::colorize(m_config.subtitle, utils::Color::White) << std::endl;
        }
        
        std::cout << std::endl;
    }
}

void InteractiveMenu::renderItems() {
    int visibleIndex = 1;
    
    for (size_t i = 0; i < m_items.size(); ++i) {
        const auto& item = m_items[i];
        
        if (!item.visible) {
            continue;
        }
        
        if (item.type == MenuItemType::Separator) {
            if (!item.title.empty()) {
                std::cout << TerminalUtils::colorize("-- " + item.title + " ", utils::Color::BrightBlack) << std::string(40, '-') << std::endl;
            } else {
                std::cout << TerminalUtils::colorize(std::string(60, '-'), utils::Color::BrightBlack) << std::endl;
            }
            continue;
        }
        
        std::string formattedItem = formatItem(item, visibleIndex);
        
        if (item.enabled) {
            std::cout << formattedItem << std::endl;
        } else {
            std::cout << TerminalUtils::colorize(formattedItem, utils::Color::BrightBlack) << std::endl;
        }
        
        if (item.type != MenuItemType::Info) {
            visibleIndex++;
        }
    }
    
    std::cout << std::endl;
}

void InteractiveMenu::renderFooter() {
    if (m_config.allowBack && !m_menuStack.empty()) {
        std::cout << TerminalUtils::colorize("b) Back", utils::Color::BrightYellow) << "  ";
    }
    
    if (m_config.allowExit) {
        std::cout << TerminalUtils::colorize("q) Quit", utils::Color::BrightRed);
    }
    
    if (!m_config.footer.empty()) {
        std::cout << std::endl << std::endl;
        std::cout << TerminalUtils::colorize(m_config.footer, utils::Color::BrightBlack) << std::endl;
    }
    
    std::cout << std::endl;
}

int InteractiveMenu::getSelection() {
    std::cout << "Enter your choice: ";
    
    std::string input;
    std::getline(std::cin, input);
    
    if (input.empty()) {
        return -2; // Invalid input
    }
    
    // Handle special commands
    if (input == "q" || input == "quit" || input == "exit") {
        return -1; // Exit
    }
    
    if (input == "b" || input == "back") {
        if (m_config.allowBack && !m_menuStack.empty()) {
            goBack();
            return -2; // Continue
        }
    }
    
    // Try to parse as number
    try {
        int selection = std::stoi(input);
        return selection;
    } catch (...) {
        std::cout << TerminalUtils::colorize("Invalid input. Please try again.", utils::Color::BrightRed) << std::endl;
        waitForEnter();
        return -2; // Invalid input
    }
}

bool InteractiveMenu::handleSelection(int selection) {
    if (selection <= 0) {
        return true; // Continue
    }
    
    // Find the corresponding menu item
    int visibleIndex = 1;
    for (auto& item : m_items) {
        if (!item.visible || item.type == MenuItemType::Separator || item.type == MenuItemType::Info) {
            continue;
        }
        
        if (visibleIndex == selection) {
            if (!item.enabled) {
                std::cout << TerminalUtils::colorize("This option is currently disabled.", utils::Color::BrightRed) << std::endl;
                waitForEnter();
                return true;
            }
            
            switch (item.type) {
                case MenuItemType::Action:
                    return executeAction(item);
                case MenuItemType::Submenu:
                    enterSubmenu(item);
                    return true;
                case MenuItemType::Toggle:
                    return handleToggle(item);
                case MenuItemType::Input:
                    return handleInput(item);
                case MenuItemType::Choice:
                    return handleChoice(item);
                case MenuItemType::MultiChoice:
                    return handleMultiChoice(item);
                default:
                    break;
            }
        }
        
        visibleIndex++;
    }
    
    std::cout << TerminalUtils::colorize("Invalid selection. Please try again.", utils::Color::BrightRed) << std::endl;
    waitForEnter();
    return true;
}

bool InteractiveMenu::executeAction(const MenuItem& item) {
    if (item.action) {
        return item.action();
    }
    return true;
}

bool InteractiveMenu::handleToggle(MenuItem& item) {
    item.toggleState = !item.toggleState;
    item.icon = item.toggleState ? "☑" : "☐";
    
    if (item.toggleCallback) {
        item.toggleCallback(item.toggleState);
    }
    
    return true;
}

bool InteractiveMenu::handleInput(MenuItem& item) {
    std::cout << std::endl;
    std::cout << item.inputPrompt << ": ";
    
    std::string input;
    std::getline(std::cin, input);
    
    if (item.inputValidator && !item.inputValidator(input)) {
        std::cout << TerminalUtils::colorize("Invalid input. Please try again.", utils::Color::BrightRed) << std::endl;
        waitForEnter();
        return true;
    }
    
    item.inputValue = input;
    
    if (item.inputCallback) {
        item.inputCallback(input);
    }
    
    return true;
}

std::string InteractiveMenu::formatItem(const MenuItem& item, int index) const {
    std::ostringstream formatted;
    
    // Number
    if (m_config.showNumbers && item.type != MenuItemType::Info) {
        formatted << std::setw(2) << index << ") ";
    } else {
        formatted << "   ";
    }
    
    // Icon
    if (m_config.showIcons && !item.icon.empty()) {
        formatted << item.icon << " ";
    }
    
    // Title
    std::string color = getItemColor(item);
    if (!color.empty()) {
        formatted << TerminalUtils::colorize(item.title, utils::Color::BrightWhite);
    } else {
        formatted << item.title;
    }
    
    // Description
    if (m_config.showDescriptions && !item.description.empty()) {
        formatted << " - " << TerminalUtils::colorize(item.description, utils::Color::BrightBlack);
    }
    
    return formatted.str();
}

std::string InteractiveMenu::getItemColor(const MenuItem& item) const {
    if (item.highlighted) {
        return "highlight";
    }
    
    switch (item.type) {
        case MenuItemType::Action:
            return "action";
        case MenuItemType::Submenu:
            return "submenu";
        case MenuItemType::Toggle:
            return item.toggleState ? "toggle_on" : "toggle_off";
        case MenuItemType::Input:
            return "input";
        case MenuItemType::Choice:
        case MenuItemType::MultiChoice:
            return "choice";
        case MenuItemType::Info:
            return "info";
        default:
            return "";
    }
}

void InteractiveMenu::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void InteractiveMenu::waitForEnter() {
    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void InteractiveMenu::goBack() {
    if (!m_menuStack.empty()) {
        m_items = m_menuStack.back();
        m_menuStack.pop_back();
    }
}

void InteractiveMenu::enterSubmenu(const MenuItem& item) {
    m_menuStack.push_back(m_items);
    m_items = item.submenuItems;
}

MenuItem* InteractiveMenu::findItem(const std::string& id) {
    for (auto& item : m_items) {
        if (item.id == id) {
            return &item;
        }
    }
    return nullptr;
}

bool InteractiveMenu::handleChoice(MenuItem& item) {
    if (item.choices.empty()) {
        std::cout << TerminalUtils::colorize("No choices available for this item.", utils::Color::BrightRed) << std::endl;
        return false;
    }

    std::cout << "\n" << TerminalUtils::colorize("Select an option:", utils::Color::BrightCyan) << std::endl;
    for (size_t i = 0; i < item.choices.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << item.choices[i] << std::endl;
    }

    std::cout << "\nEnter your choice (1-" << item.choices.size() << "): ";
    std::string input;
    std::getline(std::cin, input);

    try {
        int choice = std::stoi(input);
        if (choice >= 1 && choice <= static_cast<int>(item.choices.size())) {
            item.selectedChoices = {choice - 1};
            std::cout << TerminalUtils::colorize("Selected: " + item.choices[choice - 1], utils::Color::BrightGreen) << std::endl;

            // Call callback if available
            if (item.choiceCallback) {
                item.choiceCallback(item.selectedChoices);
            }
            return true;
        }
    } catch (const std::exception&) {
        // Invalid input
    }

    std::cout << TerminalUtils::colorize("Invalid choice. Please try again.", utils::Color::BrightRed) << std::endl;
    return false;
}

bool InteractiveMenu::handleMultiChoice(MenuItem& item) {
    if (item.choices.empty()) {
        std::cout << TerminalUtils::colorize("No choices available for this item.", utils::Color::BrightRed) << std::endl;
        return false;
    }

    std::cout << "\n" << TerminalUtils::colorize("Select multiple options (comma-separated):", utils::Color::BrightCyan) << std::endl;
    for (size_t i = 0; i < item.choices.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << item.choices[i] << std::endl;
    }

    std::cout << "\nEnter your choices (e.g., 1,3,5): ";
    std::string input;
    std::getline(std::cin, input);

    std::vector<int> selectedIndices;
    std::stringstream ss(input);
    std::string token;

    while (std::getline(ss, token, ',')) {
        // Trim whitespace
        token.erase(0, token.find_first_not_of(" \t"));
        token.erase(token.find_last_not_of(" \t") + 1);

        try {
            int choice = std::stoi(token);
            if (choice >= 1 && choice <= static_cast<int>(item.choices.size())) {
                selectedIndices.push_back(choice - 1);
            }
        } catch (const std::exception&) {
            // Invalid input, skip
        }
    }

    if (!selectedIndices.empty()) {
        item.selectedChoices = selectedIndices;

        // Build result string for display
        std::string result;
        for (size_t i = 0; i < selectedIndices.size(); ++i) {
            if (i > 0) result += ", ";
            result += item.choices[selectedIndices[i]];
        }

        std::cout << TerminalUtils::colorize("Selected: " + result, utils::Color::BrightGreen) << std::endl;

        // Call callback if available
        if (item.choiceCallback) {
            item.choiceCallback(item.selectedChoices);
        }
        return true;
    }

    std::cout << TerminalUtils::colorize("No valid choices selected.", utils::Color::BrightRed) << std::endl;
    return false;
}

} // namespace utils
