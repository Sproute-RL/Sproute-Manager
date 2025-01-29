set_rules("mode.debug", "mode.release")
set_languages("cxx20")
add_cxxflags("/Zm1000") -- wtf is this?

add_repositories("Sproute-Repo D:\\xrepos ")

-- package("clipboardxx")
--     on_install("windows", function (package)
--         os.cp(os.scriptdir().."dependencies/ClipboardXX/clipboardxx.hpp", package:installdir("include"))
--         os.cp(os.scriptdir().."dependencies/ClipboardXX/detail", package:installdir("include"))
--     end)
-- package_end()

add_requires("nlohmann_json", "sproute-utils", "logger")
add_requires("imgui", {configs = {dx11=true,win32=true}})
add_links("d3d11", "d3dcompiler")

target("Sproute Manager")
    set_kind("binary")
    add_files("**.cpp")

    add_packages("nlohmann_json", "imgui", "sproute-utils", "logger")