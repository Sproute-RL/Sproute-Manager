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
            throw std::exception("Write failed: Invalid handle");
        }
        if (size > sizeof(Ty))
        {
            throw std::runtime_error("Oversized write");
        }

        // Key fix: Handle pointer types correctly
        const void *src = std::is_pointer_v<Ty> ? reinterpret_cast<const void *>(data) : // Use pointer value directly
                              reinterpret_cast<const void *>(&data);                     // Use address of non-pointer data

        return WriteProcessMemory(
            pHandle,
            reinterpret_cast<LPVOID>(address),
            src,
            size,
            nullptr);
    }

    template <typename Ty>
    Ty Read(uintptr_t address)
    {
        if (pHandle == INVALID_HANDLE_VALUE)
        {
            throw std::exception("Read failed: Invalid handle");
            std::cout << "Read failed: " << GetLastError() << "\n";
            return {};
        }
        Ty buffer;
        ReadProcessMemory(pHandle, (LPVOID)address, &buffer, sizeof(Ty), nullptr);
        return buffer;
    }

    bool Read(uintptr_t address, void* buffer, size_t size)
{
    if (pHandle == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Read failed: Invalid handle\n";
        return false;
    }

    // Attempt to read the memory
    if (!ReadProcessMemory(pHandle, (LPCVOID)address, buffer, size, nullptr))
    {
        DWORD error = GetLastError();
        std::cerr << "ReadProcessMemory failed! Error Code: " << error << "\n";
        return false;
    }

    return true;
}

    uintptr_t AllocateMemory(size_t size);
    bool FreeMemory(uintptr_t address);

    bool ChangePageProtection(uintptr_t page, DWORD newProtection = PAGE_READWRITE);
    MEMORY_BASIC_INFORMATION GetPageInformation(uintptr_t page);

protected:
    std::vector<uintptr_t> allocatedAddys;
};
inline std::unique_ptr<Memory> memory;