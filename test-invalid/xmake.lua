set_project("test-invalid")
set_version("1.0.0")

set_languages("cxx17")

add_requires("spdlog")

target("test-invalid")
    set_kind("binary")
    add_files("src/main.cpp")
    add_packages("spdlog")
