#pragma once
#include <cstdint>
#include <vector>
#include <iostream>
#include <string>

// struct is vector { value = false}
template <typename T>
struct is_vector : std::false_type
{
};

// struct is vector { value = true}
template <typename T>
struct is_vector<std::vector<T>> : std::true_type
{
};

template <typename T>
struct is_vector_of_u8 : std::false_type
{
};

template <>
struct is_vector_of_u8<std::vector<std::uint8_t>> : std::true_type
{
};

template <typename T>
struct is_string_type : std::false_type
{
};

template <>
struct is_string_type<std::string> : std::true_type
{
};

class ShellCode
{
protected:
    std::vector<std::uint8_t> core;

public:
    ShellCode() : core({}) {}
    ShellCode(unsigned char *shellcode, size_t size = 0)
    {
        core = std::vector<std::uint8_t>(shellcode, shellcode + size);
    }

    template <typename T>
    void set(size_t index, T value, const char *debugname = "")
    {
        if (index >= core.size())
        {
            throw std::runtime_error("Index out of bounds");
        }
        // convert value to bytes
        std::vector<std::uint8_t> bytes = to_bytes(value);

        // replace bytes
        if (LittleEndian())
        {
            for (size_t i = bytes.size(); i > 0; i--)
            {
                // so i is gonna start at 8 for example , but we want to start at index then increase
                core[index + (bytes.size() - i)] = bytes[i - 1]; // this is gonna be little endian
                // core[0] = bytes[7]
            }
        }
        else
        {
            for (size_t i = 0; i < bytes.size(); i++)
            {
                core[index + i] = bytes[i];
            }
        }
    }

    // complete shellcode
    std::vector<std::uint8_t> get()
    {
        return core;
    }

    static bool &LittleEndian()
    {
        static bool littleEndian = false; // default value change once ;p
        return littleEndian;
    }

private:
template <typename T>
std::vector<uint8_t> to_bytes(T value)
{
    if constexpr (std::is_integral_v<T>) // is number
    {
        std::vector<uint8_t> bytes(sizeof(T));
        for (size_t i = 0; i < sizeof(T); ++i)
        {
            bytes[sizeof(T) - 1 - i] = static_cast<uint8_t>((value >> (8 * i)) & 0xFF);
        }
        return bytes;
    }
    else if constexpr (is_vector<T>::value) // is vector
    {
        if constexpr (is_vector_of_u8<T>::value)
        {
            return value; // nothing to do
        }
        else
        {
            std::vector<uint8_t> bytes;
            try
            {
                for (auto &val : value)
                {
                    bytes.push_back(to_bytes(val));
                }
                return bytes;
            }
            catch (const std::runtime_error &e)
            {
                std::cerr << "Failed to convert type to array of bytes" << std::endl;
            }
        }
    }
    else if constexpr (is_string_type<T>::value) // is string
    {
        std::vector<uint8_t> bytes;
        for (int i = 0; i < value.size(); i++)
        {
            bytes.push_back(value.data()[i]);
        }
        return bytes;
    }
    else
    {
        throw std::exception("Invalid call to to_bytes");
    }
}
};