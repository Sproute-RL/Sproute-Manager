{
    files = {
        [[build\.objs\Hacker\windows\x64\release\Hacker\main.cpp.obj]]
    },
    values = {
        [[C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.42.34433\bin\HostX64\x64\link.exe]],
        {
            "-nologo",
            "-dynamicbase",
            "-nxcompat",
            "-machine:x64",
            "/opt:ref",
            "/opt:icf",
            "user32.lib",
            "Shell32.lib",
            "Shlwapi.lib"
        }
    }
}