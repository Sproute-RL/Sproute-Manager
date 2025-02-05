#include "manager.h"

std::string Manager::dbFileName = "";

bool Manager::SaveAccount(Account data)
{
    nlohmann::json database = GetDatabase();

    if (database.contains(data.username))
    {
        if (!database[data.username].is_array())
        {
            nlohmann::json old = database[data.username];
            old["id"] = 0;
            database[data.username] = nlohmann::json::array();
            database[data.username].push_back(old);

            data.id = 1;
            database[data.username].push_back(data);
        }
        else
        {
            int accountSize = database[data.username].size();
            data.id = accountSize;
            database[data.username].push_back(data);
        }
    }
    else
    {
        data.id = 0;
        database[data.username] = nlohmann::json::array();
        database[data.username].push_back(data);
    }

    std::ofstream outFile(dbFileName.c_str(), std::ios::out | std::ios::trunc);
    if (!outFile.is_open())
    {
        throw std::exception("Failed to open passwords file for saving!");
        return false;
    }

    outFile << database.dump(4);
    outFile.close();

    return true;
}

bool Manager::DeleteAccount(std::string username, int counter)
{
    nlohmann::json database = GetDatabase();

    if (!database.contains(username))
    {
        return false;
    }

    if (counter == -1)
    {
        database.erase(username);
    }
    else
    {
        nlohmann::json &userEntry = database[username];

        if (!userEntry.is_array())
        {
            nlohmann::json old = userEntry;
            userEntry = nlohmann::json::array();
            userEntry.push_back(old);
        }

        if (counter < 0 || counter >= userEntry.size())
        {
            return false;
        }

        userEntry.erase(userEntry.begin() + counter);

        for (size_t i = 0; i < userEntry.size(); ++i)
        {
            userEntry[i]["id"] = i;
        }

        if (userEntry.empty())
        {
            database.erase(username);
        }
    }

    std::ofstream outFile(dbFileName.c_str(), std::ios::out | std::ios::trunc);
    if (!outFile.is_open())
    {
        throw std::exception("Failed to open passwords file for saving!");
    }

    outFile << database.dump(4);
    outFile.close();

    return true;
}

bool Manager::EditAccount(std::string username, int counter, Account data)
{
    nlohmann::json database = GetDatabase();

    if (!database.contains(username))
    {
        return false;
    }

    nlohmann::json &userEntry = database[username];

    if (!userEntry.is_array())
    {
        nlohmann::json old = userEntry;
        userEntry = nlohmann::json::array();
        userEntry.push_back(old);
    }

    if (counter < 0 || counter >= userEntry.size())
    {
        return false;
    }

    nlohmann::json &entryToEdit = userEntry[counter];

    int id = entryToEdit["id"];

    entryToEdit["password"] = data.password;
    entryToEdit["website"] = data.website;

    entryToEdit["id"] = id;

    std::ofstream outFile(dbFileName.c_str(), std::ios::out | std::ios::trunc);
    if (!outFile.is_open())
    {
        throw std::exception("Failed to open passwords file for saving!");
    }

    outFile << database.dump(4);
    outFile.close();

    return true;
}

bool Manager::Initialize(std::string newDBFileName)
{
    dbFileName = newDBFileName;

    std::ifstream passwordsFileRead(dbFileName);
    if (!passwordsFileRead)
    {
        std::ofstream passwordsFileWrite(dbFileName);
        if (passwordsFileWrite)
        {
            passwordsFileWrite << "{}";
            passwordsFileWrite.close();
        }
        else
        {
            throw std::exception("Failed to create the passwords file.");
            return false;
        }
    }
    else
    {
        std::string content((std::istreambuf_iterator<char>(passwordsFileRead)), std::istreambuf_iterator<char>());
        passwordsFileRead.close();
        if (content.empty() || content.front() != '{' || content.back() != '}')
        {
            if (content.empty())
                content = "{}";
            else
                content = "{" + content.substr(1, content.size() - 2) + "}";

            std::ofstream passwordsFileWrite(dbFileName);
            if (passwordsFileWrite)
            {
                passwordsFileWrite << content;
                passwordsFileWrite.close();
            }
            else
            {
                throw std::exception("Failed to rewrite the passwords file.");
                return false;
            }
        }

        return true;
    }
}

nlohmann::json Manager::GetDatabase()
{
    std::ifstream dbFile(dbFileName);
    if (!dbFile.good())
    {
        return nlohmann::json::array();
    }

    nlohmann::json database;
    dbFile >> database;
    dbFile.close();
    return database;
}