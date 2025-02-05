#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

#include <Shlwapi.h>

DWORD GetPidByName(const char *targetName) // because im not sure how to convert char to wchar_t
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    if (!snapshot)
    {
        return 0;
    }

    PROCESSENTRY32 pe32 = {sizeof(pe32)};

    if (Process32First(snapshot, &pe32))
    {
        do
        {
            if (strcmp(pe32.szExeFile, targetName) == 0)
            {
                CloseHandle(snapshot);
                return pe32.th32ProcessID;
            }

        } while (Process32Next(snapshot, &pe32));
    }

    CloseHandle(snapshot);
    return 0;
}

DWORD GetMainThreadId(DWORD pID)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (!snapshot)
    {
        return 0;
    }

    THREADENTRY32 te32 = {sizeof(te32)};
    FILETIME earliestTime = {0xFFFFFFFF, 0xFFFFFFFF}; // Max FILETIME value
    DWORD returnID = 0;

    if (Thread32First(snapshot, &te32))
    {
        do
        {
            if (te32.th32OwnerProcessID == pID)
            {
                FILETIME one, two, three, four{};

                HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, te32.th32ThreadID);
                if (GetThreadTimes(hThread, &one, &two, &three, &four))
                {
                    if (CompareFileTime(&one, &earliestTime) == -1) // aka >
                    {
                        earliestTime = one;
                        returnID = te32.th32ThreadID;
                    }
                }
                CloseHandle(hThread);
            }

        } while (Thread32Next(snapshot, &te32));
    }

    CloseHandle(snapshot);
    return returnID;
}

MODULEENTRY32 GetModuleByName(const char *name, DWORD pID)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);
    if (!snapshot)
    {
        return {};
    }

    MODULEENTRY32 pe32 = {sizeof(pe32)};

    if (Module32First(snapshot, &pe32))
    {
        do
        {
            if (strcmp(PathFindFileNameA(pe32.szExePath), name) == 0)
            {
                CloseHandle(snapshot);
                return pe32;
            }

        } while (Module32Next(snapshot, &pe32));
    }

    CloseHandle(snapshot);
    return {};
}

int main()
{
    DWORD pID = GetPidByName("Target.exe");
    DWORD mainTID = GetMainThreadId(pID);
    MODULEENTRY32 baseModule = GetModuleByName("Target.exe", pID);

    std::cout << "Target pID: " << pID << "\n";
    std::cout << "Main TID: " << mainTID << "\n";
    std::cout << "Base Address: " << std::hex << (uintptr_t)baseModule.hModule << std::dec << "\n";

    HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, pID);
    HANDLE tHandle = OpenThread(THREAD_ALL_ACCESS, 0, mainTID);

    // Hijacking
    DWORD fail = -1;
    if (SuspendThread(tHandle) == fail)
    {
        CloseHandle(pHandle);
        CloseHandle(tHandle);
    }

    // Now we need to get the "old" stack pointer
    CONTEXT tCtx = {};
    tCtx.ContextFlags = CONTEXT_CONTROL;

    if (!GetThreadContext(tHandle, &tCtx))
    {
        std::cout << "Failed to get thread context\n";
        ResumeThread(tHandle);
        CloseHandle(pHandle);
        CloseHandle(tHandle);
    }

    uintptr_t oldStackPointer = 0;
    ReadProcessMemory(pHandle, (LPVOID)tCtx.Rsp, &oldStackPointer, sizeof(oldStackPointer), 0); // sorry i mostly use the memory class i made
    std::cout << "Old stack pointer: 0x" << std::hex << oldStackPointer << std::dec << "\n";

    // Now we need to create our shellcode
    unsigned char shellcode[] = {
        0x48, 0x31, 0xc0,                                           // xor rax, rax
        0x48, 0x31, 0xc9,                                           // xor rcx, rcx
        0x48, 0xb8, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, // movabs rax, 0xbbbbbbbbbbbbbbbb
        0xb9, 0x46, 0x00, 0x00, 0x00,                               // mov ecx, 0x45
        0xff, 0xd0,                                                 // call rax
        0x48, 0x31, 0xc0,                                           // xor rax, rax
        0x48, 0xb8, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, // movabs rax, 0xaaaaaaaaaaaaaaaa
        0xff, 0xe0                                                  // jmp rax
    };

    uintptr_t targetFunction = (uintptr_t)baseModule.hModule + 0xFBF0;
    std::cout << "Target Function located at: 0x" << std::hex << targetFunction << std::dec << "\n";

    *(std::uintptr_t *)(&shellcode[8]) = targetFunction;
    *(std::uintptr_t *)(&shellcode[28]) = oldStackPointer;

    PVOID codeCave = VirtualAllocEx(pHandle, 0, sizeof(shellcode), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!WriteProcessMemory(pHandle, (LPVOID)codeCave, &shellcode, sizeof(shellcode), 0))
    {
        std::cout << "Failed to write shellcode\n";
        ResumeThread(tHandle);
        CloseHandle(pHandle);
        CloseHandle(tHandle);
    }
    std::cout << "Allocated code cave: 0x" << std::hex << codeCave << std::dec << "\n";

    if (!WriteProcessMemory(pHandle, (LPVOID)tCtx.Rsp, &codeCave, sizeof(codeCave), 0))
    {
        std::cout << "Failed to set stack pointer to code cave\n";
        ResumeThread(tHandle);
        CloseHandle(pHandle);
        CloseHandle(tHandle);
    }

    ResumeThread(tHandle); // No bother to check here (idk if you must but i dont lol)
    CloseHandle(pHandle);
    CloseHandle(tHandle);
}