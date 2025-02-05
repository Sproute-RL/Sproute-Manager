set_languages("c++20")
set_rules("mode.debug", "mode.release")

add_links("user32", "Shell32", "Shlwapi")      

target("Hacker")
    set_kind("binary")
    add_files("Hacker/**.cpp")
    
target("Target")
    set_kind("binary")
    add_files("Target/**.cpp")