#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <exception>

#include <nlohmann/json.hpp>

struct Account
{
    std::string username;
    std::string password;
    std::string website;

    int id;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Account, id, password, website)
};

class Manager
{
public:
    static bool SaveAccount(Account data);
    static bool DeleteAccount(std::string username, int counter);
    static bool EditAccount(std::string username, int counter, Account data);

    static bool Initialize(std::string newDBFileName);

private:
    static nlohmann::json GetDatabase();
    static std::string dbFileName;
};
