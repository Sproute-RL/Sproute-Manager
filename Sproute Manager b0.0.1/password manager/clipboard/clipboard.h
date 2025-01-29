#pragma once
#include <Windows.h>
#include <sstream>

namespace password_manager
{
    class Clipboard
    {
    public:
        template <typename... Ty>
        static bool Copy(const Ty... args)
        {
            std::stringstream data;
            (data << ... << args);
            std::string strData = data.str();
            size_t len = strData.size() + 1;

            HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, len);
            if (!mem)
                return false;

            void* ptr = GlobalLock(mem);
            if (!ptr)
            {
                GlobalFree(mem);
                return false;
            }
            memcpy(ptr, strData.c_str(), len);
            GlobalUnlock(mem);

            if (!OpenClipboard(nullptr))
            {
                GlobalFree(mem);
                return false;
            }

            if (!EmptyClipboard())
            {
                CloseClipboard();
                GlobalFree(mem);
                return false;
            }

            if (!SetClipboardData(CF_TEXT, mem))
            {
                CloseClipboard();
                GlobalFree(mem);
                return false;
            }

            CloseClipboard();

            return true;
        }
    };
}
