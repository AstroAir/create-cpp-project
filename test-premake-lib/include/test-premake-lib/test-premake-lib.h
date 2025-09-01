#ifndef TEST-PREMAKE-LIB_H
#define TEST-PREMAKE-LIB_H

#include <string>

namespace test-premake-lib {

// 示例类
class Example {
public:
    explicit Example(int value = 0);

    int getValue() const;
    void setValue(int value);

private:
    int value_;
};

// 示例函数
int add(int a, int b);

// 获取库版本
std::string getVersion();

} // namespace test-premake-lib

#endif // TEST-PREMAKE-LIB_H
