#include <memory-tool.h>
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <psapi.h>
#include <tchar.h>
#include <cstring>
#include <shlwapi.h>
#include <set>


std::vector<uint8_t> FileToBytes(std::string path) {
	std::ifstream file(path, std::ios::binary);

	if (!file.is_open())
		return {};

	return std::vector<uint8_t>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

uint32_t RVA(uint32_t rva, std::vector<IMAGE_SECTION_HEADER*> sectHdrs)
{
	if (sectHdrs.empty())
	{
		return -1;
	}

	for (auto sectHdr : sectHdrs) {
		uint32_t sectSize = sectHdr->Misc.VirtualSize ? sectHdr->Misc.VirtualSize : sectHdr->SizeOfRawData;

		if (rva >= sectHdr->VirtualAddress && rva <= sectHdr->VirtualAddress + sectSize)
			return rva - sectHdr->VirtualAddress + sectHdr->PointerToRawData;
	}
	return 0;
}
uintptr_t RVAVA(uintptr_t RVA, PIMAGE_NT_HEADERS NtHeaders, uint8_t* RawData)
{
	PIMAGE_SECTION_HEADER FirstSection = IMAGE_FIRST_SECTION(NtHeaders);

	for (PIMAGE_SECTION_HEADER Section = FirstSection; Section < FirstSection + NtHeaders->FileHeader.NumberOfSections; Section++)
		if (RVA >= Section->VirtualAddress && RVA < Section->VirtualAddress + Section->Misc.VirtualSize)
			return (uintptr_t)RawData + Section->PointerToRawData + (RVA - Section->VirtualAddress);

	return NULL;
}

BOOL RelocateImage(PVOID p_remote_img, PVOID p_local_img, PIMAGE_NT_HEADERS nt_head)
{
	struct reloc_entry
	{
		ULONG to_rva;
		ULONG size;
		struct
		{
			WORD offset : 12;
			WORD type : 4;
		} item[1];
	};

	uintptr_t delta_offset = (uintptr_t)p_remote_img - nt_head->OptionalHeader.ImageBase;
	if (!delta_offset) return true;
	else if (!(nt_head->OptionalHeader.DllCharacteristics & IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE))
		return false;

	// Cast p_local_img to uint8_t* before passing it to RVAVA
	reloc_entry* reloc_ent = (reloc_entry*)RVAVA(nt_head->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress, nt_head, (uint8_t*)p_local_img);

	uintptr_t reloc_end = (uintptr_t)reloc_ent + nt_head->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;

	if (reloc_ent == nullptr)
		return true;

	while ((uintptr_t)reloc_ent < reloc_end && reloc_ent->size)
	{
		DWORD records_count = (reloc_ent->size - 8) >> 1;
		for (DWORD i = 0; i < records_count; i++)
		{
			WORD fix_type = (reloc_ent->item[i].type);
			WORD shift_delta = (reloc_ent->item[i].offset) % 4096;

			if (fix_type == IMAGE_REL_BASED_ABSOLUTE)
				continue;

			if (fix_type == IMAGE_REL_BASED_HIGHLOW || fix_type == IMAGE_REL_BASED_DIR64)
			{
				uintptr_t fix_va = (uintptr_t)RVAVA(reloc_ent->to_rva, nt_head, (uint8_t*)p_local_img);

				if (!fix_va)
					fix_va = (uintptr_t)p_local_img;

				*(uintptr_t*)(fix_va + shift_delta) += delta_offset;
			}
		}

		reloc_ent = (reloc_entry*)((LPBYTE)reloc_ent + reloc_ent->size);
	}

	return true;
}

bool IsModuleLoaded(const std::string& moduleName) {
#if defined(UNICODE) || defined(_UNICODE) 
	HMODULE hMods[1024];
	DWORD cbNeeded;
	if (EnumProcessModules(memory->pHandle, hMods, sizeof(hMods), &cbNeeded))
	{
		for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			wchar_t szModName[MAX_PATH];
			if (GetModuleFileNameEx(memory->pHandle, hMods[i], szModName, sizeof(szModName) / sizeof(wchar_t)))
			{
				wchar_t* pFileName = PathFindFileName(szModName);

				std::wstring wideModuleName(moduleName.begin(), moduleName.end());

				if (CompareString(LOCALE_INVARIANT, NORM_IGNORECASE, wideModuleName.c_str(), -1, pFileName, -1) == CSTR_EQUAL)
				{
					return true;
				}
			}
		}
	}
#else
	HMODULE hMods[1024];
	DWORD cbNeeded;
	if (EnumProcessModules(memory->pHandle, hMods, sizeof(hMods), &cbNeeded))
	{
		for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			char szModName[MAX_PATH];
			if (GetModuleFileNameEx(memory->pHandle, hMods[i], szModName, sizeof(szModName) / sizeof(char)))
			{
				char* pFileName = PathFindFileNameA(szModName);

				if (_stricmp(moduleName.c_str(), pFileName) == 0)
				{
					return true;
				}
			}
		}
	}
#endif
	return false;
}

bool ManualMap(std::string dllPath)
{
	std::ifstream dllFile(dllPath, std::ios::binary | std::ios::ate);
	if (!dllFile.good())
	{
		std::cerr << "Failed to find " << (std::filesystem::path(dllPath).filename()) << "\n";
		return false;
	}

	std::streampos dllSize = dllFile.tellg();
	dllFile.seekg(std::ios::beg);

	BYTE* dllBuffer = new BYTE[dllSize];
	dllFile.read(reinterpret_cast<char*>(dllBuffer), dllSize);
	dllFile.close();

	auto fileBytes = FileToBytes(dllPath);

	PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(dllBuffer);
	PIMAGE_NT_HEADERS ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(dllBuffer + dosHeader->e_lfanew);

	uintptr_t remoteBase = memory->AllocateMemory(ntHeaders->OptionalHeader.SizeOfImage);
	if (!remoteBase)
	{
		std::cerr << "Failed to allocate remote base" << "\n";
		return false;
	}
	std::cout << "Allocated remote base: 0x" << std::hex << remoteBase << std::dec << "\n";

	PIMAGE_SECTION_HEADER sectionHeader = IMAGE_FIRST_SECTION(ntHeaders);
	if (!WriteProcessMemory(memory->pHandle, (PVOID)remoteBase, dllBuffer, ntHeaders->OptionalHeader.SizeOfHeaders, 0))
	{
		std::cerr << "Failed to write size of headers" << "\n";
		return false;
	}

	std::vector<IMAGE_SECTION_HEADER*> sectHdrsVector;
	IMAGE_SECTION_HEADER* sectionHeaders = IMAGE_FIRST_SECTION(ntHeaders);
	for (int sectionHeaderC = 0; sectionHeaderC < ntHeaders->FileHeader.NumberOfSections; sectionHeaderC++)
	{
		IMAGE_SECTION_HEADER* sectionHeader = &sectionHeaders[sectionHeaderC];

		if (!WriteProcessMemory(memory->pHandle, (PVOID)(remoteBase + sectionHeaders->VirtualAddress), (PVOID)(dllBuffer + sectionHeaders->PointerToRawData), sectionHeader->SizeOfRawData, nullptr))
		{
			std::cerr << "Failed to write section header: " << sectionHeader->Name << "\n";
			return false;
		}
		sectHdrsVector.push_back(sectionHeader);
	}
	std::cout << "Wrote sections\n";

	if (!RelocateImage((PVOID)remoteBase, dllBuffer, ntHeaders))
	{
		std::cerr << "Failed to relocate image";
		return false;
	}
	std::cout << "Relocated image\n";

	PIMAGE_IMPORT_DESCRIPTOR ImportDescriptor = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(RVAVA(ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress, ntHeaders, dllBuffer));

	if (ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress || ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size)
	{
		// Loop through all the import descriptors
		while (ImportDescriptor->Name != 0)
		{
			LPSTR ModuleName = (LPSTR)RVAVA(ImportDescriptor->Name, ntHeaders, dllBuffer);

			if (ModuleName)
			{
				PIMAGE_THUNK_DATA ThunkData = reinterpret_cast<PIMAGE_THUNK_DATA>(RVAVA(ImportDescriptor->FirstThunk, ntHeaders, dllBuffer));

				if (IsModuleLoaded(ModuleName))
				{

					// Resolve the imports
					while (ThunkData->u1.Function)
					{
						int size_needed = MultiByteToWideChar(CP_ACP, 0, ModuleName, -1, NULL, 0);
						wchar_t* wideModuleName = new wchar_t[size_needed];
						MultiByteToWideChar(CP_ACP, 0, ModuleName, -1, wideModuleName, size_needed);
						FARPROC functionAddress = GetProcAddress(GetModuleHandle(wideModuleName), (LPCSTR)ThunkData->u1.Function);

						if (functionAddress)
						{
							if (!WriteProcessMemory(memory->pHandle, (LPVOID)(ThunkData->u1.Function), &functionAddress, sizeof(functionAddress), nullptr))
							{
								std::cerr << "Failed to resolve import for function at address 0x" << std::hex << ThunkData->u1.Function;
								return false;
							}
						}
						ThunkData++;
					}
					//std::cout << "Resolved: " << ModuleName << "\n";
				}
				else
				{
					void* sectionsMemory = VirtualAllocEx(memory->pHandle, 0, ntHeaders->OptionalHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
					if (!sectionsMemory)
					{
						std::cerr << "Failed to allocate memory for manual map" << " import: " << ModuleName << "\n";
						return false;
					}

					if (!WriteProcessMemory(memory->pHandle, sectionsMemory, dllBuffer, ntHeaders->OptionalHeader.SizeOfHeaders, 0))
					{
						std::cerr << "Failed to write headers to allocated memory" << " import: " << ModuleName << "\n";
						VirtualFreeEx(memory->pHandle, sectionsMemory, 0, MEM_RELEASE);
						return false;
					}

					IMAGE_SECTION_HEADER* sectionHeaders = IMAGE_FIRST_SECTION(ntHeaders);
					for (size_t i = 0; i < ntHeaders->FileHeader.NumberOfSections; ++i)
					{
						IMAGE_SECTION_HEADER* section = &sectionHeaders[i];
						LPVOID sectionAddr = (LPBYTE)sectionsMemory + section->VirtualAddress;
						LPVOID rawData = dllBuffer + section->PointerToRawData;

						if (!WriteProcessMemory(memory->pHandle, sectionAddr, rawData, section->SizeOfRawData, nullptr))
						{
							std::cerr << "Failed to write section: " << section->Name << " import: " << ModuleName << "\n";
							VirtualFreeEx(memory->pHandle, sectionsMemory, 0, MEM_RELEASE);
							return false;
						}
					}

					uintptr_t relocationOffset = (uintptr_t)sectionsMemory - ntHeaders->OptionalHeader.ImageBase;
					if (relocationOffset != 0)
					{
						IMAGE_DATA_DIRECTORY relocationDir = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
						if (relocationDir.Size == 0 || relocationDir.VirtualAddress == 0)
						{
							std::cout << "No relocation entries found" << " import: " << ModuleName << "\n";
						}
						else
						{
							PIMAGE_BASE_RELOCATION relocationData = (PIMAGE_BASE_RELOCATION)RVAVA(relocationDir.VirtualAddress, ntHeaders, dllBuffer);
							if (!relocationData)
							{
								std::cerr << "Failed to locate relocation data" << " import: " << ModuleName << "\n";
								VirtualFreeEx(memory->pHandle, sectionsMemory, 0, MEM_RELEASE);
								return false;
							}

							bool is64Bit = ntHeaders->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64;

							while (relocationData->VirtualAddress != 0 && relocationData->SizeOfBlock > 0)
							{
								DWORD numEntries = (relocationData->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
								WORD* entries = (WORD*)(relocationData + 1);

								for (DWORD i = 0; i < numEntries; i++)
								{
									WORD entry = entries[i];
									BYTE type = entry >> 12;
									WORD offset = entry & 0xFFF;

									if ((is64Bit && type != IMAGE_REL_BASED_DIR64) || (!is64Bit && type != IMAGE_REL_BASED_HIGHLOW))
										continue;

									uintptr_t targetAddr = (uintptr_t)sectionsMemory + relocationData->VirtualAddress + offset;

									DWORD_PTR value = 0;
									SIZE_T bytesRead;
									if (!ReadProcessMemory(memory->pHandle, (LPCVOID)targetAddr, &value, is64Bit ? sizeof(ULONGLONG) : sizeof(DWORD), &bytesRead))
									{
										std::cerr << "Failed to read relocation target at 0x" << std::hex << targetAddr << std::dec << " import: " << ModuleName << "\n";
										VirtualFreeEx(memory->pHandle, sectionsMemory, 0, MEM_RELEASE);
										return false;
									}

									DWORD_PTR newValue = value + relocationOffset;

									if (!WriteProcessMemory(memory->pHandle, (LPVOID)targetAddr, &newValue, is64Bit ? sizeof(ULONGLONG) : sizeof(DWORD), nullptr))
									{
										std::cerr << "Failed to write relocation target at 0x" << std::hex << targetAddr << std::dec << " import: " << ModuleName << "\n";
										VirtualFreeEx(memory->pHandle, sectionsMemory, 0, MEM_RELEASE);
										return false;
									}
								}

								relocationData = (PIMAGE_BASE_RELOCATION)((BYTE*)relocationData + relocationData->SizeOfBlock);
							}
						}
					}

					PIMAGE_IMPORT_DESCRIPTOR ImportDescriptor = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(RVAVA(ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress, ntHeaders, dllBuffer));

					if (ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress || ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size)
					{
						while (ImportDescriptor->Name != 0)
						{
							LPSTR ModuleName = (LPSTR)RVAVA(ImportDescriptor->Name, ntHeaders, dllBuffer);

							PIMAGE_THUNK_DATA ThunkData = reinterpret_cast<PIMAGE_THUNK_DATA>(RVAVA(ImportDescriptor->FirstThunk, ntHeaders, dllBuffer));

							// Resolve the imports in the manual-mapped module
							while (ThunkData->u1.Function)
							{
								int size_needed = MultiByteToWideChar(CP_ACP, 0, ModuleName, -1, NULL, 0);
								wchar_t* wideModuleName = new wchar_t[size_needed];
								MultiByteToWideChar(CP_ACP, 0, ModuleName, -1, wideModuleName, size_needed);
								FARPROC functionAddress = GetProcAddress(GetModuleHandle(wideModuleName), (LPCSTR)ThunkData->u1.Function);

								if (functionAddress)
								{
									if (!WriteProcessMemory(memory->pHandle, (LPVOID)(ThunkData->u1.Function), &functionAddress, sizeof(functionAddress), nullptr))
									{
										std::cerr << "Failed to resolve import for function at address 0x" << std::hex << ThunkData->u1.Function << "\n";
										return false;
									}
								}
								ThunkData++;
							}

							ImportDescriptor++;
						}
						//std::cout << "Resolved: " << ModuleName << "\n";
					}
				}
			}

			ImportDescriptor++;
		}
	}
	std::cout << "Resolved imports\n";

	HANDLE threadHandle = OpenThread(THREAD_ALL_ACCESS, 0, memory->tID);
	if (!threadHandle)
	{
		std::cerr << "Failed to open handle to target thread\n";
		return false;
	}
	Thread mainT(threadHandle);
	mainT.Suspend();

	CONTEXT tCtx{};
	mainT.GetContext(&tCtx, CONTEXT_CONTROL);

	uintptr_t oldStack = memory->Read<uintptr_t>(tCtx.Rsp);
	uintptr_t dllMain = remoteBase + ntHeaders->OptionalHeader.AddressOfEntryPoint;

	std::cout << "DllMain at: 0x" << std::hex << dllMain << std::dec << "\n";

	unsigned char shellcode[] = {
	0x48, 0xb8, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01,                      // mov rax, 0x111111111111111 | 10
	0x50,                                                                            // push rax | 1
	0xb8, 0x01, 0x00, 0x00, 0x00,                                                    // mov eax, 1 | 5
	0x50,                                                                            // push rax | 1
	0x48, 0x31, 0xc0,                                                                // xor rax, rax | 3
	0x50,                                                                            // push rax | 1
	0x48, 0xb8, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x02,                      // mov rax, 0x222222222222222 | 10
	0xff, 0xd0,                                                                      // call rax | 2
	0x48, 0xb8, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x03,                      // mov rax, 0x333333333333333 | 10
	0xff, 0xe0                                                                       // jmp rax | 2
	};

	*(uintptr_t*)(&shellcode[2]) = (uint64_t)remoteBase;
	*(uintptr_t*)(&shellcode[23]) = (uint64_t)dllMain;
	*(uintptr_t*)(&shellcode[sizeof(shellcode) - 10]) = (uint64_t)oldStack;

	uintptr_t codeCave = memory->AllocateMemory(0x1000);
	std::cout << "Code cave at: 0x" << std::hex << codeCave << std::dec << "\n";

	memory->Write(codeCave, shellcode);

	if (!memory->Write(tCtx.Rsp, codeCave))
	{
		std::cerr << "Failed to set rsp pointer to code cave\n";
		return false;
	}

	mainT.Resume();

	CloseHandle(threadHandle);
	delete[] dllBuffer;
	return true;
}

int main()
{
	memory = std::make_unique<Memory>("Target.exe", false, "Target");
	if (!memory->pHandle)
	{
		std::cerr << "Couldn't find " << memory->pName << "\n";
		return 1;
	}
	std::cout << "Found " << memory->pName << "\n\n";

	std::string dllName("hello-world-x64.dll");

#ifdef DEBUG
	bool injected = ManualMap((std::filesystem::current_path() / dllName).string());
#else
	bool injected = ManualMap((std::filesystem::path("D:\\Programming\\Visual Studio\\C++\\Manual Map\\build") / dllName).string());
#endif // DEBUG

	std::cout << (injected ? "\nInjected" : "\nFailed to inject") << "\n";
}