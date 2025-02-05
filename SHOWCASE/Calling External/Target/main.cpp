#include <Windows.h>
#include <string>
#include <iostream>

void CallMeExternally(int number)
{
    if (number == 69)
    {
        std::cout << "Call Me Externally" << "\n";
    }
    else
    {
        std::cout << "Bouncer: invalid number" << "\n";
        return;
    }
}

int main()
{
    SetConsoleTitle("Target");

    while (true)
    {
        SleepEx(1000, TRUE);
    }
}