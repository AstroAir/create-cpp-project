#include <iostream>
#include "test-premake-lib/test-premake-lib.h"

int main() {
    // Create an Example object
    test-premake-lib::Example example(42);

    // Get value
    int value = example.getValue();
    std::cout << "Example value: " << value << std::endl;

    // Modify value
    example.setValue(100);
    std::cout << "New value: " << example.getValue() << std::endl;

    // Use free function
    int result = test-premake-lib::add(3, 4);
    std::cout << "3 + 4 = " << result << std::endl;

    // Get library version
    std::cout << "Library version: " << test-premake-lib::getVersion() << std::endl;

    return 0;
}
