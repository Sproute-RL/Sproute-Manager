#include "graphics.h"

#pragma region Static members
// Define static members
int password_manager::Graphics::width;
int password_manager::Graphics::height;
const char *password_manager::Graphics::name;
HWND password_manager::Graphics::hwnd;
ID3D11Device *password_manager::Graphics::device = nullptr;
ID3D11DeviceContext *password_manager::Graphics::deviceContext = nullptr;
IDXGISwapChain *password_manager::Graphics::swapChain = nullptr;
ID3D11RenderTargetView *password_manager::Graphics::renderTargetView = nullptr;
ImFont *password_manager::Graphics::largeFont = nullptr;
ImFont *password_manager::Graphics::mediumFont = nullptr;
ImFont *password_manager::Graphics::normalFont = nullptr;

#pragma endregion

#pragma region Style
void SetupImGuiStyle()
{
    ImGuiStyle &style = ImGui::GetStyle();
    ImVec4 *colors = style.Colors;

    // Base colors for a pleasant and modern dark theme with dark accents
    colors[ImGuiCol_Text] = ImVec4(0.92f, 0.93f, 0.94f, 1.00f);                  // Light grey text for readability
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.52f, 0.54f, 1.00f);          // Subtle grey for disabled text
    colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);              // Dark background with a hint of blue
    colors[ImGuiCol_ChildBg] = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);               // Slightly lighter for child elements
    colors[ImGuiCol_PopupBg] = ImVec4(0.18f, 0.18f, 0.20f, 1.00f);               // Popup background
    colors[ImGuiCol_Border] = ImVec4(0.28f, 0.29f, 0.30f, 0.60f);                // Soft border color
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);          // No border shadow
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);               // Frame background
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.24f, 0.26f, 1.00f);        // Frame hover effect
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.26f, 0.28f, 1.00f);         // Active frame background
    colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);               // Title background
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);         // Active title background
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);      // Collapsed title background
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);             // Menu bar background
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);           // Scrollbar background
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.24f, 0.26f, 0.28f, 1.00f);         // Dark accent for scrollbar grab
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.28f, 0.30f, 0.32f, 1.00f);  // Scrollbar grab hover
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.32f, 0.34f, 0.36f, 1.00f);   // Scrollbar grab active
    colors[ImGuiCol_CheckMark] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f);             // Dark blue checkmark
    colors[ImGuiCol_SliderGrab] = ImVec4(0.36f, 0.46f, 0.56f, 1.00f);            // Dark blue slider grab
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.40f, 0.50f, 0.60f, 1.00f);      // Active slider grab
    colors[ImGuiCol_Button] = ImVec4(0.24f, 0.34f, 0.44f, 1.00f);                // Dark blue button
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.38f, 0.48f, 1.00f);         // Button hover effect
    colors[ImGuiCol_ButtonActive] = ImVec4(0.32f, 0.42f, 0.52f, 1.00f);          // Active button
    colors[ImGuiCol_Header] = ImVec4(0.24f, 0.34f, 0.44f, 1.00f);                // Header color similar to button
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.28f, 0.38f, 0.48f, 1.00f);         // Header hover effect
    colors[ImGuiCol_HeaderActive] = ImVec4(0.32f, 0.42f, 0.52f, 1.00f);          // Active header
    colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.29f, 0.30f, 1.00f);             // Separator color
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f);      // Hover effect for separator
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f);       // Active separator
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.36f, 0.46f, 0.56f, 1.00f);            // Resize grip
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.40f, 0.50f, 0.60f, 1.00f);     // Hover effect for resize grip
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.44f, 0.54f, 0.64f, 1.00f);      // Active resize grip
    colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);                   // Inactive tab
    colors[ImGuiCol_TabHovered] = ImVec4(0.28f, 0.38f, 0.48f, 1.00f);            // Hover effect for tab
    colors[ImGuiCol_TabActive] = ImVec4(0.24f, 0.34f, 0.44f, 1.00f);             // Active tab color
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);          // Unfocused tab
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.24f, 0.34f, 0.44f, 1.00f);    // Active but unfocused tab
    colors[ImGuiCol_PlotLines] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f);             // Plot lines
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f);      // Hover effect for plot lines
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.36f, 0.46f, 0.56f, 1.00f);         // Histogram color
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.40f, 0.50f, 0.60f, 1.00f);  // Hover effect for histogram
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);         // Table header background
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.28f, 0.29f, 0.30f, 1.00f);     // Strong border for tables
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.24f, 0.25f, 0.26f, 1.00f);      // Light border for tables
    colors[ImGuiCol_TableRowBg] = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);            // Table row background
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.22f, 0.24f, 0.26f, 1.00f);         // Alternate row background
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.24f, 0.34f, 0.44f, 0.35f);        // Selected text background
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.46f, 0.56f, 0.66f, 0.90f);        // Drag and drop target
    colors[ImGuiCol_NavHighlight] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f);          // Navigation highlight
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f); // Windowing highlight
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);     // Dim background for windowing
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);      // Dim background for modal windows

    // Style adjustments
    style.WindowRounding = 8.0f;    // Softer rounded corners for windows
    style.FrameRounding = 4.0f;     // Rounded corners for frames
    style.ScrollbarRounding = 6.0f; // Rounded corners for scrollbars
    style.GrabRounding = 4.0f;      // Rounded corners for grab elements
    style.ChildRounding = 4.0f;     // Rounded corners for child windows

    style.WindowTitleAlign = ImVec2(0.50f, 0.50f); // Centered window title
    style.WindowPadding = ImVec2(10.0f, 10.0f);    // Comfortable padding
    style.FramePadding = ImVec2(6.0f, 4.0f);       // Frame padding
    style.ItemSpacing = ImVec2(8.0f, 8.0f);        // Item spacing
    style.ItemInnerSpacing = ImVec2(8.0f, 6.0f);   // Inner item spacing
    style.IndentSpacing = 22.0f;                   // Indentation spacing

    style.ScrollbarSize = 16.0f; // Scrollbar size
    style.GrabMinSize = 10.0f;   // Minimum grab size

    style.AntiAliasedLines = true; // Enable anti-aliased lines
    style.AntiAliasedFill = true;  // Enable anti-aliased fill
}
#pragma endregion

#pragma region Initialization
bool password_manager::Graphics::Initialize(int newWidth, int newHeight, const char *newName)
{
    width = newWidth;
    height = newHeight;
    name = newName;

    // Register window class
    WNDCLASSEX wc = {sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, name, NULL};
    RegisterClassEx(&wc);

    // Create the window without borders and title bar
    hwnd = CreateWindow(wc.lpszClassName, name, WS_POPUP, 100, 100, width, height, NULL, NULL, wc.hInstance, NULL);

    // Set window style for transparency and no decorations
    SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return false;
    }

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    io.IniFilename = nullptr;

    SetupImGuiStyle();

    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device, deviceContext);

    static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    ImFontConfig icons_config;
    ImFontConfig font_config;
    icons_config.MergeMode = true;
    largeFont = io.Fonts->AddFontFromMemoryTTF((void *)Roboto_Medium, sizeof(Roboto_Medium), 32);
    mediumFont = io.Fonts->AddFontFromMemoryTTF((void *)Roboto_Medium, sizeof(Roboto_Medium), 24);
    normalFont = io.Fonts->AddFontFromMemoryTTF((void *)Roboto_Medium, sizeof(Roboto_Medium), 18);
    io.Fonts->AddFontFromMemoryCompressedTTF(FA_compressed_data, FA_compressed_size, 16.0f, &icons_config, icons_ranges);
    io.FontDefault = normalFont;
    io.Fonts->Build();

    return true;
}
#pragma endregion

#pragma region Rendering
bool done = false;
void password_manager::Graphics::Render()
{
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));

    while (!done)
    {
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }

        if (done)
            break;

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        static bool open1 = true;
        if (!open1)
        {
            done = true;
            return;
        }

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(width, height));
        ImGui::Begin(name, &open1, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
        if (!global::hasLoggedIn)
        {
            if (functions::hasMasterPassword())
            {
                static ImVec2 availSize = ImGui::GetContentRegionAvail();
                static float inputWidth = (availSize.x / 1.5) + 50;
                static float buttonWidth = inputWidth;

                ImGui::Dummy({0, (availSize.y / 7) - 50});
                ImGui::PushFont(largeFont);
                ImGui::TextCentered("Login - Let's get it!");
                ImGui::PopFont();

                ImGui::SetNextItemWidth(inputWidth);
                ImGui::SetCursorPos(ImVec2((availSize.x / 2) - inputWidth / 2, (availSize.y / 1.5) - 180));
                ImGui::SetCursorPosX((availSize.x / 2) - inputWidth / 2);
                ImGui::InputTextWithHint("##LOGINPASSWORD", "Enter master password", global::loginCredentials, sizeof(global::loginCredentials), ImGuiInputTextFlags_Password);

                ImGui::SetCursorPosX((availSize.x / 2) - buttonWidth / 2);
                if (ImGui::Button("Login", ImVec2(buttonWidth, 0)))
                {
                    if (functions::checkCredentials())
                    {
                        global::hasLoggedIn = true;
                        MessageBoxA(0, "Logged in successfully!", "Success", MB_OK | MB_ICONINFORMATION);
                    }
                    else
                    {
                        MessageBoxA(0, "Incorrect password. Try again!", "Error", MB_OK | MB_ICONERROR);
                        strcpy(global::loginCredentials, "\0");
                        ImGui::End();
                        continue;
                    }
                }
                ImGui::SetCursorPosX((availSize.x / 2) - buttonWidth / 2);
                if (ImGui::Button("Reset Password", ImVec2(buttonWidth, 0)))
                {
                    if (MessageBoxA(0, "Are you sure you want to reset the password?\nYou will lose all accounts.", "Reset Password", MB_YESNO | MB_ICONWARNING) == IDYES)
                    {
                        remove("passwords.json");
                        remove("credentials.txt");
                        functions::restart();
                    }
                }
            }
            else
            {
                static ImVec2 availSize = ImGui::GetContentRegionAvail();
                static float inputWidth = (availSize.x / 1.5) + 50;
                static float buttonWidth = inputWidth;

                ImGui::Dummy({0, (availSize.y / 7) - 50});
                ImGui::PushFont(largeFont);
                ImGui::TextCentered("Sign up - <Insert quote here> ahh momement 😭");
                ImGui::PopFont();

                ImGui::SetNextItemWidth(inputWidth);
                ImGui::SetCursorPos(ImVec2((availSize.x / 2) - inputWidth / 2, (availSize.y / 1.5) - 180));
                ImGui::SetCursorPosX((availSize.x / 2) - inputWidth / 2);
                ImGui::InputTextWithHint("##MASTERPASSWORD", "Enter new master password", global::newCredentials, sizeof(global::newCredentials), ImGuiInputTextFlags_Password);

                ImGui::SetCursorPosX((availSize.x / 2) - buttonWidth / 2);
                if (ImGui::Button("Sign up", ImVec2(buttonWidth, 0)))
                {
                    if (strlen(global::newCredentials) <= 4)
                    {
                        MessageBoxA(0, "Size of password is less than 5 characters. Try again!", "Error", MB_OK | MB_ICONERROR);
                        ImGui::End();
                        continue;
                    }

                    if (functions::saveCredentials())
                    {
                        MessageBoxA(0, "Saved new password!", "Success", MB_OK | MB_ICONINFORMATION);
                        functions::restart();
                    }
                    else
                    {
                        ImGui::End();
                        continue;
                    }
                }
                ImGui::SetCursorPosX((availSize.x / 2) - buttonWidth / 2);
                if (ImGui::Button("Clear password", ImVec2(buttonWidth, 0)))
                {
                    strcpy(global::newCredentials, "\0");
                }
            }
        }
        else
        {
            static bool creatingAccount = false;
            static bool showingAccountInfo = false;
            static std::string selectedAccount = "";

            if (ImGui::BeginTabBar("Main"))
            {
                if (ImGui::BeginTabItem("Account Management"))
                {
                    ImVec2 windowSize = ImGui::GetWindowSize();
                    if (ImGui::BeginChild("Accounts List", ImVec2(0, windowSize.y - ((35) * 3.2) - 3), ImGuiChildFlags_Border))
                    {
                        if (ImGui::BeginTable("Account view", 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders))
                        {
                            ImGui::TableSetupColumn("Username");
                            ImGui::TableHeadersRow();

                            static int idCounter = 0;
                            auto accounts = functions::getAccountsCached();

                            for (const auto &account : accounts.items())
                            {
                                ImGui::TableNextRow();
                                ImGui::PushID(idCounter);

                                ImGui::TableSetColumnIndex(0);
                                if (ImGui::Selectable(account.key().c_str(), selectedAccount == account.key()))
                                {
                                    selectedAccount = account.key();
                                    Logger::Info(false, selectedAccount, "");
                                }

                                ImGui::PopID();
                            }

                            ImGui::EndTable();
                        }

                        ImGui::EndChild();
                    }

                    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - ImGui::GetFrameHeightWithSpacing() - 3);
                    if (ImGui::Button("Add new account"))
                    {
                        creatingAccount = true;
                        showingAccountInfo = false;
                    }
                    if (selectedAccount.size() != 0)
                    {
                        ImGui::SameLine();
                        if (ImGui::Button("Extend account"))
                        {
                            showingAccountInfo = true;
                            creatingAccount = false;
                        }
                    }

#pragma region Test
                    if (creatingAccount)
                    {
                        static bool open2 = true;
                        if (!open2)
                        {
                            creatingAccount = false;
                            open2 = true;
                        }

                        ImGui::MakeWindowThingy(1.25, 1.4);
                        if (ImGui::Begin("Create an account", &open2, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
                        {
                            ImGui::SetWindowFocus();

                            // Variables
                            static bool showAdditionalInput = false;

                            static char username[256] = "\0";
                            static char password[256] = "\0";
                            static char reason[256] = "\0";
                            static char phoneNumber[256] = "\0";
                            static char websiteURL[256] = "\0";
                            static char extraNote[256] = "\0";

                            // Draw input for new account
                            float windowWidth = ImGui::GetContentRegionAvail().x;
                            ImGui::SetNextItemWidth(windowWidth);
                            ImGui::InputTextWithHint("##MAIL/NAME", "Enter here the username / email of the account", username, sizeof(username));

                            ImGui::SetNextItemWidth(windowWidth);
                            ImGui::InputTextWithHint("##PASSWORD", "Enter here the password of the account", password, sizeof(password));

                            ImGui::SetNextItemWidth(windowWidth);
                            ImGui::InputTextWithHint("##REASON", "Enter here the reason of the account or the service it's used in", reason, sizeof(reason));

                            // Additional input
                            if (showAdditionalInput)
                            {
                                ImGui::Dummy({0, 2});

                                ImGui::SetNextItemWidth(windowWidth);
                                ImGui::InputTextWithHint("##PHONE", "Enter here the phone number*", phoneNumber, sizeof(phoneNumber));

                                ImGui::SetNextItemWidth(windowWidth);
                                ImGui::InputTextWithHint("##WEBSITEURL", "Enter here the base url of the website (e.g. https://www.google.com/)*", websiteURL, sizeof(websiteURL));

                                ImGui::SetNextItemWidth(windowWidth);
                                ImGui::InputTextWithHint("##EXTRANOTE", "Enter here an additional note*", extraNote, sizeof(extraNote));
                            }

                            ImGui::Checkbox("Show extra information", &showAdditionalInput);

                            if (ImGui::Button("Generate a random password"))
                            {
                                std::string random_string = functions::randomString();
                                strncpy(password, random_string.c_str(), sizeof(password) - 1);
                            }
                            ImGui::SameLine();
                            if (ImGui::Button("Clear all params"))
                            {
                                strcpy(username, "\0");
                                strcpy(password, "\0");
                                strcpy(reason, "\0");
                                strcpy(phoneNumber, "\0");
                                strcpy(websiteURL, "\0");
                                strcpy(extraNote, "\0");
                                showAdditionalInput = false;
                            }

                            if (ImGui::Button("Save new account"))
                            {
                                nlohmann::json newAccount;
                                newAccount[username] = {
                                    {"password", strlen(password) == 0 ? "Empty" : password},
                                    {"reason", strlen(reason) == 0 ? "Empty" : reason},
                                    {"phoneNumber", strlen(phoneNumber) == 0 ? "Empty" : phoneNumber},
                                    {"websiteURL", strlen(websiteURL) == 0 ? "Empty" : websiteURL},
                                    {"extraNote", strlen(extraNote) == 0 ? "Empty" : extraNote}};

                                if (functions::saveAccount(newAccount))
                                {
                                    MessageBoxA(0, "Account saved successfully!", "Success", MB_OK | MB_ICONINFORMATION);
                                    creatingAccount = false;

                                    strcpy(username, "\0");
                                    strcpy(password, "\0");
                                    strcpy(reason, "\0");
                                    strcpy(phoneNumber, "\0");
                                    strcpy(websiteURL, "\0");
                                    strcpy(extraNote, "\0");
                                    showAdditionalInput = false;
                                }
                                else
                                {
                                    MessageBoxA(0, "Failed to save the account. Please try again.", "Error", MB_OK | MB_ICONERROR);
                                }
                            }

                            ImGui::End();
                        }
                    }
                    if (showingAccountInfo)
                    {
                        static bool open2 = true;
                        if (!open2)
                        {
                            showingAccountInfo = false;
                            open2 = true;
                        }

                        ImGui::MakeWindowThingy(1.25, 1.1);
                        if (ImGui::Begin(Utilities::Stringify("Extended info - ", selectedAccount).c_str(), &open2, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
                        {
                            static bool editingAccount;
                            static int selectedCounter;
                            if (!editingAccount)
                            {
                                ImGui::SetWindowFocus();
                            }

                            windowSize = ImGui::GetWindowSize();
                            if (ImGui::BeginChild("Account Info", ImVec2(0, windowSize.y - ((35) * 3.2) - 3), ImGuiChildFlags_Border))
                            {
                                enum FilterType
                                {
                                    COUNTER,
                                    PASSWORD,
                                    PHONE,
                                    REASON,
                                    URL,
                                    EXTRA
                                };
                                const char *filterNames[] = {
                                    "Counter",
                                    "Password",
                                    "Phone",
                                    "Reason",
                                    "Url",
                                    "Extra"};
                                static int filterType = COUNTER;
                                const char *filterName = (filterType >= 0 && filterType <= EXTRA) ? filterNames[filterType] : "Unknown";
                                static char FilterText[256] = "";
                                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.6);
                                ImGui::InputTextWithHint("##filter1", Utilities::Stringify("Enter the filter of ", filterName).c_str(), FilterText, 256, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue);
                                ImGui::SameLine();
                                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                                ImGui::SliderInt("Filter Type", &filterType, 0, EXTRA, filterName);
                                if (ImGui::BeginTable("Account Infos", global::passwordsSize, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders))
                                {
                                    /*
                                    We need a way to get the size of the accounts "procuderaly" in case we add more stuff
                                    We need a way to display the columns "procuderaly" in case we add more stuff

                                    Aka parse json and make classes ig
                                    */
                                    ImGui::TableSetupColumn("Counter");
                                    ImGui::TableSetupColumn("Password");
                                    ImGui::TableSetupColumn("Phone Number");
                                    ImGui::TableSetupColumn("Reason");
                                    ImGui::TableSetupColumn("Website URL");
                                    ImGui::TableSetupColumn("Extra Note");
                                    ImGui::TableHeadersRow();

                                    auto account = functions::getAccount(selectedAccount);

                                    auto processAccount = [](const auto &acc, int id)
                                    {
                                        switch (filterType)
                                        {
                                        case FilterType::COUNTER:
                                            if (Utilities::Stringify(acc["counter"]).find(FilterText) == std::string::npos)
                                                return;
                                            break;
                                        case FilterType::PASSWORD:
                                            if (Utilities::Stringify(acc["password"]).find(FilterText) == std::string::npos)
                                                return;
                                            break;
                                        case FilterType::PHONE:
                                            if (Utilities::Stringify(acc["phoneNumber"]).find(FilterText) == std::string::npos)
                                                return;
                                            break;
                                        case FilterType::REASON:
                                            if (Utilities::Stringify(acc["reason"]).find(FilterText) == std::string::npos)
                                                return;
                                            break;
                                        case FilterType::URL:
                                            if (Utilities::Stringify(acc["websiteURL"]).find(FilterText) == std::string::npos)
                                                return;
                                            break;
                                        case FilterType::EXTRA:
                                            if (Utilities::Stringify(acc["extraNote"]).find(FilterText) == std::string::npos)
                                                return;
                                            break;

                                        default:
                                            break;
                                        }
                                        auto StripQuotes = [](const std::string &str) -> std::string
                                        {
                                            if (!str.empty() && str.front() == '"' && str.back() == '"')
                                            {
                                                return str.substr(1, str.length() - 2);
                                            }
                                            return str;
                                        };

                                        ImGuiStyle &style = ImGui::GetStyle();
                                        ImVec4 oldButtonColor = style.Colors[ImGuiCol_Button];
                                        ImVec4 oldButtonHoveredColor = style.Colors[ImGuiCol_ButtonHovered];
                                        ImVec4 oldButtonActiveColor = style.Colors[ImGuiCol_ButtonActive];

                                        // Set button colors to fully transparent
                                        style.Colors[ImGuiCol_Button] = ImVec4(0, 0, 0, 0);        // Normal state
                                        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0, 0, 0, 0); // Hovered state
                                        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0, 0, 0, 0);  // Active state

                                        ImGui::PushID(id);
                                        ImGui::TableSetColumnIndex(0);
                                        if (ImGui::Button(StripQuotes(Utilities::Stringify(acc["counter"])).c_str()))
                                        {
                                            selectedCounter = acc["counter"];
                                            Logger::Debug(false, "Selected counter: ", selectedCounter);
                                        }

                                        ImGui::TableSetColumnIndex(1);
                                        ImGui::PushID(1);
                                        if (ImGui::Button(StripQuotes(Utilities::Stringify(acc["password"])).c_str()))
                                        {
                                            if (Clipboard::Copy(StripQuotes(acc["password"])))
                                                Logger::Debug(false, "Copied password");
                                        }
                                        ImGui::PopID();

                                        ImGui::TableSetColumnIndex(2);
                                        ImGui::PushID(2);
                                        if (ImGui::Button(StripQuotes(Utilities::Stringify(acc["phoneNumber"])).c_str()))
                                        {
                                            if (Clipboard::Copy(StripQuotes(acc["phoneNumber"])))
                                                Logger::Debug(false, "Copied phone number");
                                        }
                                        ImGui::PopID();

                                        ImGui::TableSetColumnIndex(4);
                                        ImGui::PushID(4);
                                        if (ImGui::Button(StripQuotes(Utilities::Stringify(acc["websiteURL"])).c_str()))
                                        {
                                            if (Clipboard::Copy(StripQuotes(acc["websiteURL"])))
                                                Logger::Debug(false, "Copied website URL");
                                        }
                                        ImGui::PopID();

                                        ImGui::TableSetColumnIndex(3);
                                        ImGui::Text(StripQuotes(Utilities::Stringify(acc["reason"])).c_str());

                                        ImGui::TableSetColumnIndex(5);
                                        ImGui::Text(StripQuotes(Utilities::Stringify(acc["extraNote"])).c_str());

                                        ImGui::PopID();

                                        style.Colors[ImGuiCol_Button] = oldButtonColor;
                                        style.Colors[ImGuiCol_ButtonHovered] = oldButtonHoveredColor;
                                        style.Colors[ImGuiCol_ButtonActive] = oldButtonActiveColor;
                                    };

                                    // account is always an array // no hwere run again when i tell you
                                    if (account.is_array())
                                    {
                                        int id = 0;
                                        for (auto &acc : account)
                                        {
                                            ImGui::TableNextRow();
                                            processAccount(acc, id++);
                                        }
                                    }
                                    else
                                    {
                                        ImGui::TableNextRow();
                                        processAccount(account, 0);
                                    }

                                    ImGui::EndTable();
                                }

                                ImGui::EndChild();
                            }

                            if (ImGui::Button("Delete account"))
                            {
                                if (MessageBoxA(0, Utilities::Stringify("Are you sure you want to remove ", selectedAccount, " ", selectedCounter, "?").c_str(), "Confirmation", MB_OKCANCEL | MB_ICONINFORMATION) == IDOK)
                                {
                                    if (functions::deleteAccount(selectedCounter, selectedAccount))
                                    {
                                        MessageBoxA(0, "Account deleted successfully!", "Success", MB_OK | MB_ICONINFORMATION);
                                    }
                                    else
                                    {
                                        MessageBoxA(0, "Failed to delete the account. Please try again.", "Error", MB_OK | MB_ICONERROR);
                                    }
                                }
                            }
                            ImGui::SameLine();
                            if (ImGui::Button("Edit account"))
                            {
                                editingAccount = true;
                            }

                            if (editingAccount)
                            {
                                static bool open3 = true;
                                if (!open3)
                                {
                                    open3 = true;
                                    editingAccount = false;
                                }

                                ImGui::MakeWindowThingy(1.25, 1.4);
                                if (ImGui::Begin(Utilities::Stringify("Editing account - ", selectedAccount, " | ", selectedCounter).c_str(), &open3, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
                                {
                                    // Variables
                                    static bool showAdditionalInput = false;

                                    static char password[256] = "\0";
                                    static char reason[256] = "\0";
                                    static char phoneNumber[256] = "\0";
                                    static char websiteURL[256] = "\0";
                                    static char extraNote[256] = "\0";

                                    // Draw input for new account
                                    float windowWidth = ImGui::GetContentRegionAvail().x;

                                    ImGui::SetNextItemWidth(windowWidth);
                                    ImGui::InputTextWithHint("##PASSWORD", "Enter here the new password of the account", password, sizeof(password));

                                    ImGui::SetNextItemWidth(windowWidth);
                                    ImGui::InputTextWithHint("##REASON", "Enter here the new reason of the account or the service it's used in", reason, sizeof(reason));

                                    // Additional input
                                    if (showAdditionalInput)
                                    {
                                        ImGui::Dummy({0, 2});

                                        ImGui::SetNextItemWidth(windowWidth);
                                        ImGui::InputTextWithHint("##PHONE", "Enter here the new phone number*", phoneNumber, sizeof(phoneNumber));

                                        ImGui::SetNextItemWidth(windowWidth);
                                        ImGui::InputTextWithHint("##WEBSITEURL", "Enter here the new base url of the website (e.g. https://www.google.com/)*", websiteURL, sizeof(websiteURL));

                                        ImGui::SetNextItemWidth(windowWidth);
                                        ImGui::InputTextWithHint("##EXTRANOTE", "Enter here a new additional note*", extraNote, sizeof(extraNote));
                                    }

                                    ImGui::Checkbox("Show extra information", &showAdditionalInput);

                                    if (ImGui::Button("Generate a random password"))
                                    {
                                        std::string random_string = functions::randomString();
                                        strncpy(password, random_string.c_str(), sizeof(password) - 1);
                                    }
                                    ImGui::SameLine();
                                    if (ImGui::Button("Clear all params"))
                                    {
                                        strcpy(password, "\0");
                                        strcpy(reason, "\0");
                                        strcpy(phoneNumber, "\0");
                                        strcpy(websiteURL, "\0");
                                        strcpy(extraNote, "\0");
                                        showAdditionalInput = false;
                                    }

                                    if (ImGui::Button("Edit account"))
                                    {
                                        nlohmann::json newAccount;
                                        newAccount = {
                                            {"password", strlen(password) == 0 ? "Empty" : password},
                                            {"reason", strlen(reason) == 0 ? "Empty" : reason},
                                            {"phoneNumber", strlen(phoneNumber) == 0 ? "Empty" : phoneNumber},
                                            {"websiteURL", strlen(websiteURL) == 0 ? "Empty" : websiteURL},
                                            {"extraNote", strlen(extraNote) == 0 ? "Empty" : extraNote}};

                                        // Call editAccount function and check the result
                                        if (functions::editAccount(newAccount, selectedCounter, selectedAccount))
                                        {
                                            MessageBoxA(0, "Account edited successfully!", "Success", MB_OK | MB_ICONINFORMATION);
                                            editingAccount = false; // Exit editing state
                                            strcpy(password, "\0");
                                            strcpy(reason, "\0");
                                            strcpy(phoneNumber, "\0");
                                            strcpy(websiteURL, "\0");
                                            strcpy(extraNote, "\0");
                                            showAdditionalInput = false;
                                        }
                                        else
                                        {
                                            MessageBoxA(0, "Failed to edit the account. Please try again.", "Error", MB_OK | MB_ICONERROR);
                                        }
                                    }

                                    ImGui::End();
                                }
                            }

                            ImGui::End();
                        }
                    }
#pragma endregion

                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Developer"))
                {
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
        }

        ImGui::End();

        // Rendering
        const float clear_color[4] = {0.0f, 0.0f, 0.0f, 0.0f}; // Transparent background
        deviceContext->OMSetRenderTargets(1, &renderTargetView, NULL);
        deviceContext->ClearRenderTargetView(renderTargetView, clear_color);
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        swapChain->Present(1, 0); // Present with vsync
    }
    Cleanup();
}
#pragma endregion

void password_manager::Graphics::Cleanup()
{
    done = true;

    CleanupDeviceD3D();
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (hwnd != nullptr && !DestroyWindow(hwnd))
    {
        // Logger::Error(false, "Failed to destroy window.\n");
    }

    // Unregister the class
    if (!UnregisterClass(name, GetModuleHandle(NULL)))
    {
        // Logger::Error(false, "Failed to unregister window class.\n");
    }
    // Logger::Info(false, "hey\n");
}

bool password_manager::Graphics::CreateDeviceD3D(HWND hWnd)
{
    // Create the device and swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0,
    };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &swapChain, &device, &featureLevel, &deviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void password_manager::Graphics::CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (swapChain)
    {
        swapChain->Release();
        swapChain = nullptr;
    }
    if (deviceContext)
    {
        deviceContext->Release();
        deviceContext = nullptr;
    }
    if (device)
    {
        device->Release();
        device = nullptr;
    }
}

void password_manager::Graphics::CreateRenderTarget()
{
    ID3D11Texture2D *pBackBuffer;
    swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    device->CreateRenderTargetView(pBackBuffer, NULL, &renderTargetView);
    pBackBuffer->Release();
}

void password_manager::Graphics::CleanupRenderTarget()
{
    if (renderTargetView)
    {
        renderTargetView->Release();
        renderTargetView = nullptr;
    }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall password_manager::Graphics::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static bool dragging = false;
    static POINT dragStartPoint = {};
    static POINT windowStartPoint = {};

    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_LBUTTONDOWN:
    {
        POINT currentCursorPos;
        GetCursorPos(&currentCursorPos);

        RECT windowRect;
        GetWindowRect(hWnd, &windowRect);

        if (currentCursorPos.y < windowRect.top + 30)
        {
            dragging = true;
            SetCapture(hWnd);

            dragStartPoint = currentCursorPos;

            windowStartPoint.x = windowRect.left;
            windowStartPoint.y = windowRect.top;
        }
        else
        {
            if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
                return true;
        }
    }
    break;

    case WM_MOUSEMOVE:
        if (dragging)
        {
            POINT currentCursorPos;
            GetCursorPos(&currentCursorPos);

            int deltaX = currentCursorPos.x - dragStartPoint.x;
            int deltaY = currentCursorPos.y - dragStartPoint.y;

            SetWindowPos(hWnd, nullptr, windowStartPoint.x + deltaX, windowStartPoint.y + deltaY, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
        }
        break;

    case WM_LBUTTONUP:
        if (dragging)
        {
            dragging = false;
            ReleaseCapture();
        }
        break;

    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED)
        {
            password_manager::Graphics::width = LOWORD(lParam);
            password_manager::Graphics::height = HIWORD(lParam);
        }
        return 0;

    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;

    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}