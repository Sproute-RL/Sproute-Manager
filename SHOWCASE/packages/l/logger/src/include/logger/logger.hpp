#pragma once
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>    
#include <chrono>    

class Logger
{
public:
    enum ConsoleColors
    {
        WHITE,  // Done
        RED,    // Done
        GREEN,  // Done
        BLUE,   // Done
        PURPLE, // Done
        GREY,   // Done
        BLACK,  // Doesn't seem to exist
        PINK,   // Done
        YELLOW, // Done
        RESET   // Done
    };

    static bool SetConsoleColor(const ConsoleColors &color);
    static void ResetConsoleColor();

    static bool SaveToFile(const std::string& output, bool saveTime = true);

    template <typename... Ty>
    static void PrintWithColor(const ConsoleColors &color, Ty... args)
    {
        std::stringstream output;
        (output << ... << args);

        SetConsoleColor(color);
        std::cout << output.str() << "\n";
        ResetConsoleColor();
    }
    template <typename... Ty>
    static void PrintWithColorAndPrefix(const ConsoleColors &textColor, const ConsoleColors &prefixColor, const std::string &prefix, Ty... args)
    {
        std::stringstream output;

        SetConsoleColor(prefixColor);
        std::cout << prefix << " ";

        SetConsoleColor(textColor);
        (output << ... << args);

        std::cout << output.str() << "\n";
        ResetConsoleColor();
    }

    template <typename... Ty>
    static void Info(bool saveToFile = false, Ty... args)
    {
        PrintWithColorAndPrefix(ConsoleColors::WHITE, ConsoleColors::GREY, "[Info]", args...);
        if (saveToFile)
        {
            std::stringstream output;
            (output << ... << args);
            SaveToFile(output.str());
        }
    }
    template <typename... Ty>
    static void Prefixless(bool saveToFile = false, Ty... args)
    {
        PrintWithColor(ConsoleColors::WHITE, ConsoleColors::GREY, args...);
        if (saveToFile)
        {
            std::stringstream output;
            (output << ... << args);
            SaveToFile(output.str());
        }
    }
    template <typename... Ty>
    static void Error(bool saveToFile = false, Ty... args)
    {
        PrintWithColorAndPrefix(ConsoleColors::WHITE, ConsoleColors::RED, "[Error]", args...);
        if (saveToFile)
        {
            std::stringstream output;
            (output << ... << args);
            SaveToFile(output.str());
        }
    }
    template <typename... Ty>
    static void Warn(bool saveToFile = false, Ty... args)
    {
        PrintWithColorAndPrefix(ConsoleColors::WHITE, ConsoleColors::YELLOW, "[Warning]", args...);
        if (saveToFile)
        {
            std::stringstream output;
            (output << ... << args);
            SaveToFile(output.str());
        }
    }
    template <typename... Ty>
    static void Debug(bool saveToFile = false, Ty... args)
    {
        PrintWithColorAndPrefix(ConsoleColors::WHITE, ConsoleColors::PURPLE, "[Debug]", args...);
        if (saveToFile)
        {
            std::stringstream output;
            (output << ... << args);
            SaveToFile(output.str());
        }
    }
    static void EmptyLine(bool saveToFile = false, int linesAmount=1);
};