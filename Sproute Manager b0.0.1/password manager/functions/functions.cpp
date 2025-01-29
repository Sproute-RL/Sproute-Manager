#include "functions.h"

namespace password_manager
{
    namespace functions
    {
        bool saveCredentials()
        {
            std::fstream credentialsFile("credentials.txt", std::ofstream::out | std::ofstream::trunc);
            if (!credentialsFile.is_open())
            {
                Logger::Error(true, "Failed to open credentials file.");
                return false;
            }

            credentialsFile << global::newCredentials;
            credentialsFile.close();
            global::hasLoggedIn = true;
            return true;
        }

        bool hasMasterPassword()
        {
            path credentialsFile = current_path() / "credentials.txt";
            if (!exists(credentialsFile))
            {
                return false;
            }

            std::string readLine;
            std::fstream file(credentialsFile.string());
            if (!file.good())
            {
                return false;
            }
            std::getline(file, readLine);
            file.close();
            return !readLine.empty();
        }

        bool checkCredentials()
        {
            std::string readLine;
            std::fstream file("credentials.txt");
            if (!file.good())
            {
                return false;
            }
            std::getline(file, readLine);
            file.close();
            password_manager::global::hasLoggedIn = readLine == global::loginCredentials;
            return password_manager::global::hasLoggedIn;
        }

        bool initialize()
        {
            path credentialsFile = current_path() / "credentials.txt";
            if (!exists(credentialsFile))
            {
                std::ofstream(credentialsFile.string()).close();
            }

            const char *passwordsFileName = "passwords.json";
            std::ifstream passwordsFileRead(passwordsFileName);
            if (!passwordsFileRead)
            {
                std::ofstream passwordsFileWrite(passwordsFileName);
                if (passwordsFileWrite)
                {
                    passwordsFileWrite << "{}";
                    passwordsFileWrite.close();
                }
                else
                {
                    Logger::Error(true, "Failed to create the passwords file.");
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

                    std::ofstream passwordsFileWrite(passwordsFileName);
                    if (passwordsFileWrite)
                    {
                        passwordsFileWrite << content;
                        passwordsFileWrite.close();
                    }
                    else
                    {
                        Logger::Error(true, "Failed to rewrite the passwords file.");
                        return false;
                    }
                }
            }

            return true;
        }
        void showAvailableCommands(bool forCMD)
        {
            if (forCMD)
            {
            }
            else
            {
                Logger::Info(false, "Commands: ");
                Logger::Info(false, "1. exit: Exit the program nigga you really need an explaination?");
                Logger::Info(false, "2. newpass: Create a new password for logging in");
                Logger::Info(false, "3. help: Show this help menu");
                Logger::Info(false, "4. list: List all passwords");
                Logger::Info(false, "5. add: Add a new password to the database");
                Logger::Info(false, "5. delete: Delete an password to the database");
                Logger::EmptyLine(false);
            }
        }

        std::string randomString(int minLength, int maxLength)
        {
            const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
            std::random_device random_device;
            std::mt19937 generator(random_device());
            std::uniform_int_distribution<> length_distribution(minLength, maxLength);
            std::uniform_int_distribution<> char_distribution(0, CHARACTERS.size() - 1);

            std::size_t password_length = length_distribution(generator);
            std::string random_string;

            for (std::size_t i = 0; i < password_length; ++i)
            {
                random_string += CHARACTERS[char_distribution(generator)];
            }

            return random_string;
        }

        void restart()
        {
            TCHAR szFileName[MAX_PATH];
            GetModuleFileName(NULL, szFileName, MAX_PATH);
            STARTUPINFO si = {sizeof(si)};
            PROCESS_INFORMATION pi;
            if (!CreateProcess(szFileName, GetCommandLine(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
            {
                Logger::Error(true, "Failed to restart the program.");
            }
            else
            {
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
                // Graphics::Cleanup();
                exit(0); // Terminate the current process
            }
        }

        nlohmann::json getAccounts()
        {
            nlohmann::json accounts = {};
            std::ifstream passwords("passwords.json");
            if (!passwords.good())
            {
                Logger::Error(true, "Failed to locate passwords.json file for returning the accounts");
                return accounts;
            }

            passwords >> accounts;
            return accounts;
        }

        nlohmann::json getAccount(std::string accountName)
        {
            nlohmann::json account = {};
            std::ifstream passwords("passwords.json");
            if (!passwords.good())
            {
                Logger::Error(true, "Failed to locate passwords.json file for returning the accounts!");
                return account;
            }

            passwords >> account;
            return account[accountName];
        }

        nlohmann::json getAccountsCached()
        {
            static nlohmann::json cachedAccounts;
            static auto lastUpdate = std::chrono::steady_clock::now();

            auto now = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdate);

            if (duration.count() >= 1)
            {
                cachedAccounts = getAccounts();
                lastUpdate = now;
            }

            return cachedAccounts;
        }

        bool editAccount(nlohmann::json newAccount, int counter, std::string name)
        {
            // Open the passwords file
            std::ifstream passwords("passwords.json");
            if (!passwords.good())
            {
                std::stringstream ss;
                ss << "Failed to locate passwords.json file!";
                Logger::Error(true, ss.str());
                return false;
            }

            nlohmann::json storedAccounts;
            passwords >> storedAccounts;
            passwords.close();

            // Check if the account name exists
            if (storedAccounts.contains(name))
            {
                for (auto &acc : storedAccounts[name])
                {
                    if (acc["counter"] == counter)
                    {
                        // Replace only the edited fields
                        for (auto &[key, value] : newAccount.items())
                        {
                            if (!value.is_null() && value != "Empty")
                            {
                                acc[key] = value;
                            }
                        }

                        // Write the updated accounts back to the file
                        std::ofstream outPasswords("passwords.json");
                        if (!outPasswords.good())
                        {
                            std::stringstream ss;
                            ss << "Failed to open passwords.json for writing!";
                            Logger::Error(true, ss.str());
                            return false;
                        }

                        outPasswords << storedAccounts.dump(4); // Write with pretty formatting
                        outPasswords.close();

                        return true;
                    }
                }
            }

            return false; // Account or counter not found
        }

        bool deleteAccount(int counter, std::string name)
        {
            // Open the passwords file
            std::ifstream passwords("passwords.json");
            if (!passwords.good())
            {
                std::stringstream ss;
                ss << "Failed to locate passwords.json file!";
                Logger::Error(true, ss.str());
                return false;
            }

            nlohmann::json storedAccounts;
            passwords >> storedAccounts;
            passwords.close();

            // Check if the account name exists
            if (storedAccounts.contains(name))
            {
                for (auto it = storedAccounts[name].begin(); it != storedAccounts[name].end(); ++it)
                {
                    if ((*it)["counter"] == counter)
                    {
                        // Remove the account
                        storedAccounts[name].erase(it);

                        // If no accounts are left for the name, remove the name itself
                        if (storedAccounts[name].empty())
                        {
                            storedAccounts.erase(name);
                        }

                        // Write the updated accounts back to the file
                        std::ofstream outPasswords("passwords.json");
                        if (!outPasswords.good())
                        {
                            std::stringstream ss;
                            ss << "Failed to open passwords.json for writing!";
                            Logger::Error(true, ss.str());
                            return false;
                        }

                        outPasswords << storedAccounts.dump(4); // Write with pretty formatting
                        outPasswords.close();

                        return true;
                    }
                }
            }

            return false; // Account or counter not found
        }

        bool saveAccount(nlohmann::json account)
        {
            std::ifstream passwords("passwords.json");

            if (!passwords.good())
            {
                std::stringstream ss;
                ss << "Failed to locate passwords.json file for returning the accounts!";
                Logger::Error(true, ss.str());
                return false;
            }

            nlohmann::json storedAccounts;
            passwords >> storedAccounts;
            passwords.close();

            if (!storedAccounts.is_object())
            {
                std::stringstream ss;
                ss << "Stored accounts are not in the expected object format.";
                Logger::Error(true, ss.str());
                return false;
            }

            std::string username = account.begin().key();

            nlohmann::json newEntry = account[username];

            if (storedAccounts.contains(username))
            {
                if (!storedAccounts[username].is_array())
                {
                    nlohmann::json oldEntry = storedAccounts[username];
                    storedAccounts[username] = nlohmann::json::array();
                    storedAccounts[username].push_back(oldEntry);
                }

                int currentSize = storedAccounts[username].size();
                newEntry["counter"] = currentSize + 1;
                storedAccounts[username].push_back(newEntry);
            }
            else
            {
                newEntry["counter"] = 1;
                storedAccounts[username] = {newEntry};
            }

            std::ofstream outFile("passwords.json", std::ios::out | std::ios::trunc);
            if (!outFile.is_open())
            {
                std::stringstream ss;
                ss << "Failed to open passwords.json file for saving!";
                Logger::Error(true, ss.str());
                return false;
            }
            outFile << storedAccounts.dump(4);
            outFile.close();

            return true;
        }
        bool deleteAccount(std::string name)
        {
            std::ifstream passwords("passwords.json");

            if (!passwords.good())
            {
                std::stringstream ss;
                ss << "Failed to locate passwords.json file for returning the accounts!";
                Logger::Error(true, ss.str());
                return false;
            }

            nlohmann::json storedAccounts;
            passwords >> storedAccounts;
            passwords.close();

            if (storedAccounts.contains(name))
            {
                storedAccounts.erase(name);
            }

            std::ofstream outFile("passwords.json", std::ios::out | std::ios::trunc);
            if (!outFile.is_open())
            {
                std::stringstream ss;
                ss << "Failed to open passwords.json file for saving!";
                Logger::Error(true, ss.str());
                return false;
            }
            outFile << storedAccounts.dump(4);
            outFile.close();
            return true;
        }
        bool deleteSubAccount(std::string name, int counter)
        {
            std::ifstream passwords("passwords.json");

            if (!passwords.good())
            {
                std::stringstream ss;
                ss << "Failed to locate passwords.json file for returning the accounts!";
                Logger::Error(true, ss.str());
                return false;
            }

            nlohmann::json storedAccounts;
            passwords >> storedAccounts;
            passwords.close();

            if (storedAccounts.contains(name))
            {
                auto &accounts = storedAccounts[name]; // Get the array of sub-accounts for this account
                for (auto it = accounts.begin(); it != accounts.end(); ++it)
                {
                    // Find the sub-account where the 'counter' matches
                    if ((*it)["counter"] == counter)
                    {
                        accounts.erase(it); // Erase the sub-account from the array
                        break;              // Exit the loop after deleting the sub-account
                    }
                }
            }

            std::ofstream outFile("passwords.json", std::ios::out | std::ios::trunc);
            if (!outFile.is_open())
            {
                std::stringstream ss;
                ss << "Failed to open passwords.json file for saving!";
                Logger::Error(true, ss.str());
                return false;
            }
            outFile << storedAccounts.dump(4);
            outFile.close();
            return true;
        }
    }
}
