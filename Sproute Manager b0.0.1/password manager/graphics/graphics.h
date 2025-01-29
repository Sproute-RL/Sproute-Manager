#pragma once
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <d3d11.h>

#include "imgui/useful.h"
#include "fonts/fa.h"
#include "fonts/roboto.h"
#include "fonts/iconfontawesome.h"

#include "sproute-utils.h"

#include "../global/global.h"
#include "../functions/functions.h"
#include "../clipboard/clipboard.h"

namespace password_manager
{
    class Graphics
    {
    public:
        static bool Initialize(int newWidth, int newHeight, const char *newName);
        static void Render();
        static void Cleanup();

        static ImFont* normalFont;
        static ImFont* mediumFont;
        static ImFont* largeFont;

        static int width;
        static int height;
        static const char *name;

    private:
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

        static HWND hwnd;
        static ID3D11Device *device;
        static ID3D11DeviceContext *deviceContext;
        static IDXGISwapChain *swapChain;
        static ID3D11RenderTargetView *renderTargetView;

        static bool CreateDeviceD3D(HWND hWnd);
        static void CleanupDeviceD3D();
        static void CreateRenderTarget();
        static void CleanupRenderTarget();
    };
}