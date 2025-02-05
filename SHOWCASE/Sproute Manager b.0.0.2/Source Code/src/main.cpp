#include "../manager/manager.h"
#include "../credentials/credentials.h"

int main()
{
    if (!Manager::Initialize("passwords.json"))
    {
        std::cerr << "Failed to initialize Manager.\n";
        return 1;
    }
    if (!Credentials::Initialize("credentials.txt"))
    {
        std::cerr << "Failed to initialize Credentials.\n";
        return 1;
    }

    if (Credentials::HasCredentials())
    {
        while (true)
        {
            static std::string tempCredentials;
            std::cout << "Enter auth credentials: ";
            std::getline(std::cin, tempCredentials);

            strncpy(Credentials::loginCredentials, tempCredentials.c_str(), sizeof(Credentials::loginCredentials) - 1);

            if (Credentials::AuthCredentials())
            {
                std::cout << "Logged in!\n";
                break;
            }
        }
    }
    else
    {
        while (true)
        {
            static std::string tempCredentials;
            std::cout << "Enter new auth credentials: ";
            std::getline(std::cin, tempCredentials);

            strncpy(Credentials::registerCredentials, tempCredentials.c_str(), sizeof(Credentials::registerCredentials) - 1);

            if (Credentials::SaveCredentials())
            {
                std::cout << "Saved your credentials!\n";
                break;
            }
        }
    }

    Account testingPurpose;
    testingPurpose.username = "Hello world";
    testingPurpose.password = "fake";
    testingPurpose.website = ".com";

    Manager::EditAccount("Hello world", 0, testingPurpose);
}