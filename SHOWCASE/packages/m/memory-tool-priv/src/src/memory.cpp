#include "memory-tool/memory.h"

// INLINED FUNCTIONS BEGIN
#include <string>
#include <stdexcept>
#include "memory.h"

std::wstring ConvertToWide(const std::string &str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
    return wstr;
}

// INLINED FUNCTIONS END

Memory::Memory(const char *newPName, bool waitForProcess, const char *newWName)
{
    pName = newPName;
    wName = newWName;

    attached = false;
    Attach(waitForProcess);
}

Memory::~Memory()
{
    Detach();
}

bool Memory::Detach()
{
    bool returnValue = false;

    if (pHandle == INVALID_HANDLE_VALUE)
    {
        return false;
    }
    for (const uintptr_t &memory : allocatedAddys)
    {
        if (!FreeMemory(memory))
        {
            returnValue = false;
            break;
        }
    }

    if (!CloseHandle(pHandle))
    {
        returnValue = false;
    }

    return returnValue;
}

bool Memory::Attach(bool waitForProcess)
{
    if (strlen(pName) != 0)
    {
        do
        {
            pID = GetIDByName(pName);
            if (pID == 0 && waitForProcess)
            {
                Sleep(100);
            }
        } while (waitForProcess && pID == 0);

        if (pID == 0)
        {
            throw std::exception((std::string("Process ") + pName + " wasn't found. 1").c_str());
            return false;
        }
    }
    else
    {
        if (strlen(wName) == 0)
        {
            throw std::exception((std::string("Process ") + pName + " wasn't found. 2").c_str());
            return false;
        }

        HWND hwnd = nullptr;
        do
        {
            hwnd = FindWindowA(nullptr, wName);
            GetWindowThreadProcessId(hwnd, &pID);
            if (pID == 0 && waitForProcess)
            {
                Sleep(100);
            }
        } while (waitForProcess && pID == 0);

        if (pID == 0)
        {
            throw std::exception((std::string("Process ") + pName + " wasn't found. 2").c_str());
            return false;
        }
    }

    pHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, pID);
    if (pHandle == INVALID_HANDLE_VALUE)
    {
        throw std::exception((std::string("Failed to open process ") + pName).c_str());
        return false;
    }

    // Find the earliest created thread
    tID = 0;
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap != INVALID_HANDLE_VALUE)
    {
        THREADENTRY32 te32;
        te32.dwSize = sizeof(THREADENTRY32);
        FILETIME earliestTime = {0xFFFFFFFF, 0xFFFFFFFF}; // Max FILETIME value

        if (Thread32First(hThreadSnap, &te32))
        {
            do
            {
                if (te32.th32OwnerProcessID == pID)
                {
                    HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, te32.th32ThreadID);
                    if (hThread)
                    {
                        FILETIME creationTime, exitTime, kernelTime, userTime;
                        if (GetThreadTimes(hThread, &creationTime, &exitTime, &kernelTime, &userTime))
                        {
                            if (CompareFileTime(&creationTime, &earliestTime) == -1)
                            {
                                earliestTime = creationTime;
                                tID = te32.th32ThreadID;
                            }
                        }
                        CloseHandle(hThread);
                    }
                }
            } while (Thread32Next(hThreadSnap, &te32));
        }
        CloseHandle(hThreadSnap);
    }

    if (tID == 0)
    {
        throw std::exception((std::string("Failed to retrieve main thread for process ") + pName).c_str());
        return false;
    }

    return true;
}

DWORD Memory::GetIDByName(const char *name)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
    {
        throw std::exception("CreateToolhelp32Snapshot failed due to invalid handle.");
        return 0;
    }

    PROCESSENTRY32 pe = {sizeof(pe)};

    if (Process32First(snapshot, &pe))
    {
        do
        {
            if (wcscmp(ConvertToWide(pe.szExeFile).c_str(), ConvertToWide(name).c_str()) == 0)
            {
                CloseHandle(snapshot);
                return pe.th32ProcessID;
            }

        } while (Process32Next(snapshot, &pe));
    }
    CloseHandle(snapshot);
    return 0;
}

MODULEENTRY32 Memory::GetModuleByName(const char *moduleName)
{
    MODULEENTRY32 modEntry = {0};
    modEntry.dwSize = sizeof(MODULEENTRY32);

    HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pID);
    if (hModuleSnap == INVALID_HANDLE_VALUE)
    {
        throw std::exception("Failed to create module snapshot");
    }

    if (Module32First(hModuleSnap, &modEntry))
    {
        do
        {
#ifdef UNICODE
            if (_wcsicmp(modEntry.szModule, std::wstring(moduleName, moduleName + strlen(moduleName)).c_str()) == 0) // Convert char* to wchar_t*
#else
            if (_stricmp(modEntry.szModule, moduleName) == 0) // ANSI mode
#endif
            {
                CloseHandle(hModuleSnap);
                return modEntry;
            }
        } while (Module32Next(hModuleSnap, &modEntry));
    }

    CloseHandle(hModuleSnap);
    throw std::exception((std::string("Module ") + moduleName + " not found").c_str());
}
uintptr_t Memory::AllocateMemory(size_t size)
{
    if (!pHandle)
    {
        throw std::runtime_error("Invalid process handle");
    }

    LPVOID allocatedMemory = VirtualAllocEx(pHandle, nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    if (!allocatedMemory)
    {
        DWORD error = GetLastError();
        throw std::runtime_error("VirtualAllocEx failed with error: " + std::to_string(error));
    }

    return reinterpret_cast<uintptr_t>(allocatedMemory);
}


bool Memory::FreeMemory(uintptr_t address)
{
    if (pHandle == INVALID_HANDLE_VALUE)
    {
        throw std::exception("Free failed: Invalid handle");
        return false;
    }

    if (VirtualFreeEx(pHandle, (LPVOID)address, 0, MEM_RELEASE))
    {
        auto it = std::find(allocatedAddys.begin(), allocatedAddys.end(), address);
        if (it != allocatedAddys.end())
        {
            allocatedAddys.erase(it);
        }
        return true;
    }
    else
    {
        return false;
    }
}

bool Memory::ChangePageProtection(uintptr_t page, DWORD newProtection)
{
    if (pHandle == INVALID_HANDLE_VALUE)
    {
        throw std::exception("ChangePageProtection failed: Invalid handle");
    }

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    SIZE_T pageSize = sysInfo.dwPageSize;

    DWORD oldProtection;
    if (!VirtualProtectEx(pHandle, (LPVOID)page, pageSize, newProtection, &oldProtection))
    {
        throw std::exception("ChangePageProtection failed: VirtualProtectEx returned 0");
    }

    return true;
}

MEMORY_BASIC_INFORMATION Memory::GetPageInformation(uintptr_t page)
{
    if (pHandle == INVALID_HANDLE_VALUE)
    {
        throw std::exception("QueryPageProtection failed: Invalid handle");
    }

    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQueryEx(pHandle, (LPCVOID)page, &mbi, sizeof(mbi)) == 0)
    {
        throw std::exception("QueryPageProtection failed: VirtualQueryEx returned 0");
    }

    return mbi;
}
