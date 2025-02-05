add_rules("mode.debug", "mode.release")
set_languages("cxx20")

target("logger") 
    set_kind("static")
    add_includedirs("include")
    -- add_headerfiles("sproute-utils.h")
    add_files("**.cpp")