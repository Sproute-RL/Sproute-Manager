#include <utilities/utilities.h>

std::wstring Utilities::StringToWString(const std::string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	std::wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
	return wstr;
}

std::string Utilities::WStringToString(const std::wstring& wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string str(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size_needed, nullptr, nullptr);
	return str;
}

std::string Utilities::PrintBool(const bool& boolean)
{
	return (boolean ? "True" : "False");
}

std::string Utilities::GetSpecialFolderPath(const std::string& folderName)
{
	char path[MAX_PATH];

	if (folderName == "LOCALAPPDATA") {
		if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path))) {
			return std::string(path);
		}
	}
	else if (folderName == "APPDATA") {
		if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path))) {
			return std::string(path);
		}
	}
	else if (folderName == "PROGRAMFILES") {
		if (GetEnvironmentVariableA("ProgramFiles", path, MAX_PATH)) {
			return std::string(path);
		}
	}
	else if (folderName == "PROGRAMFILESX86") {
		if (GetEnvironmentVariableA("ProgramFiles(x86)", path, MAX_PATH)) {
			return std::string(path);
		}
	}

	return "";
}

// bool Utilities::StartProgram(const std::string& exePath)
// {
// 	HINSTANCE result = ShellExecuteA(nullptr, "open", exePath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);

// 	if ((int)result <= 32) {
// 		Logger::Error("Failed to start the executable. Error code: ", (int)result);
// 		return false;
// 	}

// 	return true;
// }

fs::path Utilities::CreateFolder(const std::string& path, const std::string& folderName)
{
	try {
		// Combine the specified path and folder name
		fs::path fullPath = fs::path(path) / folderName;

		// Create the directory
		if (fs::create_directories(fullPath)) {
			return fullPath;
		}
		else {
			return fullPath;
		}
	}
	catch (const fs::filesystem_error& e) {
		return "";
	}
	catch (const std::exception& e) {
		return "";
	}
	return "";
}

fs::path Utilities::m_CreateFile(const std::string& directoryPath, const std::string& fileName, const std::string& content)
{
	try {
		fs::path filePath = fs::path(directoryPath) / fileName;

		if (!fs::exists(filePath.parent_path())) {
			fs::create_directories(filePath.parent_path());
		}

		std::ofstream outFile(filePath);
		if (outFile) {
			outFile << content;
			outFile.close();
			return filePath;
		}
		else {
			return "";
		}
	}
	catch (const fs::filesystem_error& e) {
		return "";
	}
	catch (const std::exception& e) {
		return "";
	}
	return "";
}

std::string Utilities::ReadFileContent(const std::string& filePath)
{
	std::ifstream file(filePath, std::ios::in | std::ios::binary); // Open file in binary mode for all content
	if (!file.is_open()) {
		std::cerr << "Error: Could not open file: " << filePath << std::endl;
		return "";
	}

	// Read entire content into a string
	std::string content;
	file.seekg(0, std::ios::end); // Move to the end of the file to get the size
	content.resize(file.tellg()); // Resize the string to fit the file size
	file.seekg(0, std::ios::beg); // Move back to the beginning of the file
	file.read(&content[0], content.size()); // Read the file into the string

	file.close();
	return content;
}

std::string Utilities::hexdump(const std::vector<uint8_t> &data, size_t bytes_per_line)
{
    std::ostringstream output;

    for (size_t i = 0; i < data.size(); i += bytes_per_line)
    {
        // Print the offset
        output << std::setw(8) << std::setfill('0') << std::hex << i << ": ";

        // Print the hexadecimal representation
        for (size_t j = 0; j < bytes_per_line; ++j)
        {
            if (i + j < data.size())
            {
                output << std::setw(2) << static_cast<int>(data[i + j]) << " ";
            }
            else
            {
                output << "   "; // Fill space for alignment
            }
        }

        // Print ASCII representation
        output << " | ";
        for (size_t j = 0; j < bytes_per_line; ++j)
        {
            if (i + j < data.size())
            {
                uint8_t byte = data[i + j];
                output << (isprint(byte) ? static_cast<char>(byte) : '.');
            }
            else
            {
                output << " ";
            }
        }
        output << "\n";
    }

    return output.str();
}

bool Utilities::WriteFileContent(const std::string& filePath, const std::string& content)
{
	std::ofstream file(filePath, std::ios::out | std::ios::binary); // Open file in write mode
	if (!file.is_open()) {
		return false;
	}

	file.write(content.c_str(), content.size()); // Write content to the file
	file.close(); // Close the file

	return true; // Indicate success
}

bool Utilities::FileOrFolderExists(const std::string& path)
{
	return std::filesystem::exists(path);
}

std::vector<std::string> Utilities::GetSubFolders(const std::string& directoryPath)
{
	std::vector<std::string> subFolders;

	// Iterate over the directory entries
	for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
		// Check if the entry is a directory
		if (entry.is_directory()) {
			subFolders.push_back(entry.path().string());
		}
	}

	return subFolders;
}

bool Utilities::DeleteFileOrFolder(const std::string& path)
{
	try
	{
		if (std::filesystem::exists(path))
		{
			if (std::filesystem::is_directory(path))
			{
				std::filesystem::remove_all(path);
			}
			else
			{
				std::filesystem::remove(path);
			}
			return true;
		}
		else
		{
			std::cerr << "Path does not exist: " << path << std::endl;
		}
	}
	catch (const std::filesystem::filesystem_error& e)
	{
	}
	return false;
}