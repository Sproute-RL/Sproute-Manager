#pragma once
#include <Windows.h>
#include <string>

namespace password_manager
{
    namespace global
    {
        inline char newCredentials[256] = "";
        inline char loginCredentials[256] = "";

        inline bool hasLoggedIn = false;
        inline int passwordsSize = 6;
    }
}