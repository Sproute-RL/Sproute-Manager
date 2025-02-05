set_languages("c++20")
set_rules("mode.debug", "mode.release")

add_packages("nlohmann_json")
add_requires("nlohmann_json")

target("manager")
    set_kind("binary")

    add_files("Source Code/**.cpp")