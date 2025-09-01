#include <iostream>
#include "test-premake-project/test-premake-project.h"

int main() {
    // Create an Example object
    test-premake-project::Example example(42);

    // Get value
    int value = example.getValue();
    std::cout << "Example value: " << value << std::endl;

    // Modify value
    example.setValue(100);
    std::cout << "New value: " << example.getValue() << std::endl;

    // Use free function
    int result = test-premake-project::add(3, 4);
    std::cout << "3 + 4 = " << result << std::endl;

    // Get library version
    std::cout << "Library version: " << test-premake-project::getVersion() << std::endl;

    return 0;
}
