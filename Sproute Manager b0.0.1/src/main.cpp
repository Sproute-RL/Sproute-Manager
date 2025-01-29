#include <Windows.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>

#include "sproute-utils.h"
#include "../password manager/graphics/graphics.h"
#include "../password manager/functions/functions.h"

int main(int argc, char *argv[])
{
    if (!password_manager::functions::initialize())
    {
        Logger::Error(true, "Failed to initialize files!");
        return 1;
    }

    if (argc >= 2)
    {
        // Logging in
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (arg.find("--login=") == 0) // Check if the argument starts with --login=
            {
                strncpy_s(password_manager::global::loginCredentials, arg.substr(8).c_str(), sizeof(password_manager::global::loginCredentials) - 1);
                break;
            }
        }

        if (password_manager::functions::hasMasterPassword())
        {
            if (password_manager::functions::checkCredentials())
            {
                Logger::Info(false, "Logged in");
            }
        }
        else
        {
            Logger::Warn(false, "First create a master password using the GUI");
        }
    }
    else
    {
        if (!password_manager::Graphics::Initialize(1200, 700, "Sproute Manager"))
        {
            Logger::Error(true, "Failed to initialize graphics window.");
            return 1;
        }

        password_manager::Graphics::Render();
    }
}
