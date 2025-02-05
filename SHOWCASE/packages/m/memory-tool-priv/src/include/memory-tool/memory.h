#pragma once
#include <Windows.h>
#include <exception>
#include <TlHelp32.h>
#include <vector>
#include <memory>

// those are standard includes

/*
Getting process id
Getting main thread id

Reading, Writing
Allocating and Freeing when Memory goes out of scope
Change page protection
*/

/*
Memory: a class to modify a program externally with ease
Memory(const char* pName, const char* wName, bool waitForProcess)
*/
class Memory
{
public:
    DWORD pID;
    DWORD tID;

    HANDLE pHandle;

    const char *pName;
    const char *wName;
    bool attached;

public:
    Memory(const char *newPName = "", bool waitForProcess = false, const char *newWName = "");
    ~Memory();

    bool Detach();
    bool Attach(bool waitForProcess = false);

public:
    static DWORD GetIDByName(const char *name);
    MODULEENTRY32 GetModuleByName(const char *moduleName);

    template <typename Ty>
    bool Write(uintptr_t address, const Ty &data, size_t size = sizeof(Ty))
    {
        if (pHandle == INVALID_HANDLE_VALUE)
        {
            throw std::exception("Read failed: Invalid handle");
        }
        if (size > sizeof(Ty))
        {
            throw std::exception("Oversized write");
        }
        return WriteProcessMemory(pHandle, reinterpret_cast<LPVOID>(address), &data, size, nullptr);
    }

    template <typename Ty>
    Ty Read(uintptr_t address)
    {
        if (pHandle == INVALID_HANDLE_VALUE)
        {
            throw std::exception("Read failed: Invalid handle");
        }
        Ty buffer;
        ReadProcessMemory(pHandle, (LPVOID)address, &buffer, sizeof(Ty), nullptr);
        return buffer;
    }

    uintptr_t AllocateMemory(size_t size);
    bool FreeMemory(uintptr_t address);

    bool ChangePageProtection(uintptr_t page, DWORD newProtection = PAGE_READWRITE);
    MEMORY_BASIC_INFORMATION GetPageInformation(uintptr_t page);

protected:
    std::vector<uintptr_t> allocatedAddys;
};
inline std::unique_ptr<Memory> memory;