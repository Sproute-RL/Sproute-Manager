{
    files = {
        [[build\.objs\Hacker\windows\x64\debug\Hacker\main.cpp.obj]]
    },
    values = {
        [[C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.42.34433\bin\HostX64\x64\link.exe]],
        {
            "-nologo",
            "-dynamicbase",
            "-nxcompat",
            "-machine:x64",
            "-debug",
            [[-pdb:build\windows\x64\debug\Hacker.pdb]],
            "user32.lib",
            "Shell32.lib",
            "Shlwapi.lib"
        }
    }
}