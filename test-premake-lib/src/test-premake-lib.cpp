#include "test-premake-lib/test-premake-lib.h"
#include "test-premake-lib/version.h"

namespace test-premake-lib {

// Example类实现
Example::Example(int value) : value_(value) {}

int Example::getValue() const {
    return value_;
}

void Example::setValue(int value) {
    value_ = value;
}

// 示例函数实现
int add(int a, int b) {
    return a + b;
}

// 获取库版本
std::string getVersion() {
    return VERSION_STR;
}

} // namespace test-premake-lib