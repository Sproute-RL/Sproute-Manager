add_rules("mode.debug", "mode.release")
set_languages("cxx20")


target("shellcode-utils") 
    set_kind("static")
    add_includedirs("include")
    -- add_headerfiles("sproute-utils.h")
    add_files("**.cpp")
