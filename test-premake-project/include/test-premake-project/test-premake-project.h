#ifndef TEST-PREMAKE-PROJECT_H
#define TEST-PREMAKE-PROJECT_H

#include <string>

namespace test-premake-project {

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

} // namespace test-premake-project

#endif // TEST-PREMAKE-PROJECT_H
