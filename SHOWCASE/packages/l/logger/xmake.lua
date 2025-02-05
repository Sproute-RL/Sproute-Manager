package("logger")

-- set_homepage("https://github.com/SprouteRL/memory-tool-priv")
set_license("MIT")
set_description("hacker man") -- miaww was here

set_sourcedir("src")
-- add_deps("nlohman_json")

-- MIAW. hehe

set_sourcedir(os.scriptdir().."/src")
on_install(function(package)
    import("package.tools.xmake").install(package)
    os.cp(os.scriptdir().."/src/include/", package:installdir())
end)

package_end()