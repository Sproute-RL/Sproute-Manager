#include <logger/logger.hpp>

bool Logger::SetConsoleColor(const ConsoleColors &color)
{
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!console)
    {
        // Replace with a custom error function
        std::cerr << "Failed to get handle to console.\n";
        return false;
    }

    WORD setColor = 0;
    switch (color)
    {
    case ConsoleColors::WHITE:
        setColor = 7;
        break;
    case ConsoleColors::RED:
        setColor = 4;
        break;
    case ConsoleColors::GREEN:
        setColor = 2;
        break;
    case ConsoleColors::GREY:
        setColor = 8;
        break;
    case ConsoleColors::YELLOW:
        setColor = 14;
        break;
    case ConsoleColors::BLUE:
        setColor = 9;
        break;
    case ConsoleColors::PURPLE:
        setColor = 5;
        break;
    case ConsoleColors::PINK:
        setColor = 13;
        break;
    case ConsoleColors::RESET:
        setColor = 15;
        break;
    default:
        setColor = 7;
    }

    return SetConsoleTextAttribute(console, setColor) != 0;
}

void Logger::ResetConsoleColor()
{
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    if (console != INVALID_HANDLE_VALUE)
    {
        SetConsoleTextAttribute(console, 7);
    }
}

bool Logger::SaveToFile(const std::string &output, bool saveTime)
{
    std::fstream outputFile("output.txt", std::ios::out | std::ios::app);
    if (!outputFile.is_open())
    {
        std::cerr << "Failed to open output.txt\n";
        return false;
    }

    std::stringstream outputString;
    if (saveTime)
    {
        std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::string timeString(30, '\0');
        std::strftime(&timeString[0], timeString.size(), "%Y-%m-%d %H:%M:%S", std::localtime(&currentTime));
        timeString = timeString.c_str();

        outputString << timeString << " | " << output;
        outputFile << outputString.str();
    }
    else
    {
        outputString << output;
        outputFile << outputString.str();
    }

    outputFile.close();
    return true;
}

void Logger::EmptyLine(bool saveToFile, int linesAmount)
{
    std::string outputFile;
    outputFile.reserve(linesAmount);
    for (int emptyLineCounter = 0; emptyLineCounter < linesAmount; emptyLineCounter++)
    {
        std::cout << "\n";
        outputFile.append("\n");
    }
    SaveToFile(outputFile, false);
}
