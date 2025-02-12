#pragma once
#include <Windows.h>
#include <exception>
#include <TlHelp32.h>
#include <vector>
#include <functional>
#include <memory>

// INLINED FUNCTIONS BEGIN
#include <string>
#include <stdexcept>

inline std::wstring ConvertToWide(const std::string& str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
    return wstr;
}

inline std::string ConvertToNarrow(const std::wstring& wstr)
{
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size_needed, nullptr, nullptr);
    return str;
}

// INLINED FUNCTIONS END

class Thread {
public:
    HANDLE handle = 0;
    bool suspended = false;

    Thread(HANDLE newhandle)
    {
        handle = newhandle;
        Resume();
    }

    void Suspend() {
        if (!suspended)
            if (SuspendThread(handle) == ERROR_SUCCESS)
                suspended = true;
    }

    void Resume() {
        if (suspended)
            if (ResumeThread(handle) == ERROR_SUCCESS)
                suspended = false;
    }

    void SuspendResume(std::function<void()> func) {
        this->Suspend();
        func();
        this->Resume();
    }

    void GetContext(CONTEXT* ctx, uint32_t flags) {
        ctx->ContextFlags = flags;
        GetThreadContext(handle, ctx);
    }

    void SetContext(CONTEXT* ctx) {
        SetThreadContext(handle, ctx);
    }

    uint32_t GetExitCode() {
        DWORD exitCode = 0;
        GetExitCodeThread(handle, &exitCode);
        return exitCode;
    }
};

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

    uintptr_t AllocateMemory(size_t size, uintptr_t baseAddress = 0);
    bool FreeMemory(uintptr_t address);

    bool ChangePageProtection(uintptr_t address, size_t size, DWORD &oldProtection, DWORD newProtection = PAGE_READWRITE);
    MEMORY_BASIC_INFORMATION GetPageInformation(uintptr_t page);

protected:
    std::vector<uintptr_t> allocatedAddys;
};
inline std::unique_ptr<Memory> memory;