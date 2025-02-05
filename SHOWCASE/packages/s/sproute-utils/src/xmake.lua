add_rules("mode.debug", "mode.release")
set_languages("cxx20")

add_links("Shell32")

target("sproute-utils") 
    set_kind("static")
    add_includedirs("include")
    -- add_headerfiles("sproute-utils.h")
    add_files("**.cpp")
