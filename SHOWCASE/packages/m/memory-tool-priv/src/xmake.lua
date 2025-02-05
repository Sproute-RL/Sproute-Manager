add_rules("mode.debug", "mode.release")
set_languages("cxx20")

local shared_build = false; 

add_links("user32")
add_links("Shell32")
add_links("Shlwapi")

target("memory-tool-priv") 
    set_kind("static")
    add_includedirs("include")
    add_files("**.cpp")

    if not shared_build then
        add_files("DO NOT INCLUDE/main.cpp")
    end
