#pragma once
#include <string>
#include <iostream>
#include <fstream>

class Credentials
{
private:
    static std::string credentialsFileName;

public:
    static char loginCredentials[256]; // Just premaking it for imgui
    static char registerCredentials[256]; // Just premaking it for imgui

    static bool loggedIn;

    static bool SaveCredentials(); // also replaces, use at risk. or something
    static bool AuthCredentials();
    static bool HasCredentials();

    static bool Initialize(std::string newCredentialsFileName);
};