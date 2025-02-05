{
    files = {
        [[build\.objs\Target\windows\x64\release\Target\main.cpp.obj]]
    },
    values = {
        [[C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.42.34433\bin\HostX64\x64\link.exe]],
        {
            "-nologo",
            "-dynamicbase",
            "-nxcompat",
            "-machine:x64",
            [[-libpath:D:\Programming\VS Code\c++\Calling External\build\.packages\m\memory-tool-priv\latest\b1caf5aecf3447d1bfaf66081135de32\lib]],
            [[-libpath:D:\Programming\VS Code\c++\Calling External\build\.packages\l\logger\latest\b1caf5aecf3447d1bfaf66081135de32\lib]],
            [[-libpath:D:\Programming\VS Code\c++\Calling External\build\.packages\s\sproute-utils\latest\b1caf5aecf3447d1bfaf66081135de32\lib]],
            [[-libpath:D:\Programming\VS Code\c++\Calling External\build\.packages\s\shellcode-utils\latest\b1caf5aecf3447d1bfaf66081135de32\lib]],
            "/opt:ref",
            "/opt:icf",
            "user32.lib",
            "Shell32.lib",
            "Shlwapi.lib",
            "memory-tool-priv.lib",
            "logger.lib",
            "sproute-utils.lib",
            "shellcode-utils.lib"
        }
    }
}