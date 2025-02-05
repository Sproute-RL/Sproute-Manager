#include "credentials.h"

std::string Credentials::credentialsFileName = "";
char Credentials::loginCredentials[256] = {};
char Credentials::registerCredentials[256] = {};
bool Credentials::loggedIn = false;

bool Credentials::SaveCredentials()
{
    std::ofstream file(credentialsFileName, std::ios::out | std::ios::trunc);
    if (!file.good())
    {
        throw std::exception("Failed to open the credentials file for saving!");
        return false;
    }

    file << registerCredentials;
    file.close();
    return true;
}

bool Credentials::AuthCredentials()
{
    std::ifstream file(credentialsFileName);
    if (!file.is_open())
    {
        return false;
    }

    std::string storedCredentials(Credentials::registerCredentials);
    std::getline(file, storedCredentials);
    file.close();

    if (storedCredentials == loginCredentials)
    {
        loggedIn = true;
        return true;
    }

    return false;
}

bool Credentials::HasCredentials()
{
    std::ifstream file(credentialsFileName, std::ios::in);

    if (!file)
    {
        return false;
    }

    file.seekg(0, std::ios::end);
    bool isEmpty = (file.tellg() == 0);

    file.close();

    return !isEmpty;
}

bool Credentials::Initialize(std::string newCredentialsFileName)
{
    loggedIn = false;
    credentialsFileName = newCredentialsFileName;

    std::ifstream file(credentialsFileName);

    if (!file)
    {
        std::ofstream createFile(credentialsFileName);
        if (!createFile)
        {
            std::cerr << "Failed to create the credentials file!" << std::endl;
            return false;
        }
        createFile.close();
    }

    return true;
}
