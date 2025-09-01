set_project("verify-xmake-console")
set_version("1.0.0")

set_languages("cxx17")

add_requires("spdlog")

target("verify-xmake-console")
    set_kind("binary")
    add_files("src/main.cpp")
    add_packages("spdlog")

add_requires("gtest")

target("verify-xmake-console_tests")
    set_kind("binary")
    add_files("tests/test_main.cpp")
    add_packages("gtest")
    add_deps("verify-xmake-console")
