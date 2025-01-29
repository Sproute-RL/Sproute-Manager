#pragma once
#include "sproute-utils.h"
#include "../global/global.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <map>
#include <random>

#include <filesystem>
using namespace std::filesystem;

namespace password_manager
{
    namespace functions
    {
        bool saveCredentials();
        bool hasMasterPassword();
        bool checkCredentials();

        nlohmann::json getAccounts();
        nlohmann::json getAccount(std::string name);
        nlohmann::json getAccountsCached();
        bool editAccount(nlohmann::json newAccount, int counter, std::string name);
        bool deleteAccount(int counter, std::string name);

        bool saveAccount(nlohmann::json account);
        bool deleteSubAccount(std::string name, int counter);

        bool initialize();
        void showAvailableCommands(bool forCMD);
        std::string randomString(int minLength = 12, int maxLength = 15);
        void restart();
    }
}