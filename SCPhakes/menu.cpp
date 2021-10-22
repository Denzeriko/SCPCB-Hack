#include "misc.h"

#define WIN32_LEAN_AND_MEAN
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_GDI_IMPLEMENTATION


#include "nuklear.h"
#include "nuklear_gdi.h"

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "Msimg32.lib")

static LRESULT CALLBACK WindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    if (nk_gdi_handle_event(wnd, msg, wparam, lparam))
        return 0;
    return DefWindowProcW(wnd, msg, wparam, lparam);
}


nk_bool InfAmmo = false;
bool consolePatched = false;
HWND MenuHWND;

char DconsoleBuffer[2048] = "Console INIT!\n-------------\nLoaded SCP:CB Hakes\n";

void addToDConsole(std::stringstream &what) {
    //if (sizeof(DconsoleBuffer) / sizeof(*DconsoleBuffer) > 1900)
    //    memset(DconsoleBuffer, 0, sizeof(DconsoleBuffer)); //clear to prevent overflow xd:XDXDAUSDA*WD)
    //strcat(DconsoleBuffer, what.str().c_str());
    //what << sizeof(DconsoleBuffer) / sizeof(*DconsoleBuffer);
    strcat(DconsoleBuffer, what.str().c_str());
    memset(&what, 0, sizeof(what));
}

void menu() {
    GdiFont* font;
    struct nk_context* ctx;

    WNDCLASSW wc;
    RECT rect = { 0, 0, 600, 460 };
    DWORD style = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);//WS_OVERLAPPEDWINDOW;
    DWORD exstyle = WS_EX_APPWINDOW;
    HDC dc;
    int running = 1;
    int needs_refresh = 1;

    /* Win32 */
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandleW(0);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"SCP:CB Hakes";
    RegisterClassW(&wc);

    AdjustWindowRectEx(&rect, style, FALSE, exstyle);
    MenuHWND = CreateWindowExW(exstyle, wc.lpszClassName, L"DWARE v.0.1 SCP:CB",
        style | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, wc.hInstance, NULL);
    dc = GetDC(MenuHWND);

    /* GUI */
    font = nk_gdifont_create("Consolas", 16);
    ctx = nk_gdi_init(font, dc, 600, 540);

    ctx->style.slider.show_buttons = true;
    ctx->style.slider.cursor_size.x = 14;
    ctx->style.slider.cursor_size.y = 14;

    while (running)
    {
        /* Input */
        MSG msg;
        nk_input_begin(ctx);
        if (needs_refresh == 0) {
            if (GetMessageW(&msg, NULL, 0, 0) <= 0)
                running = 0;
            else {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            needs_refresh = 1;
        }
        else needs_refresh = 0;

        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                running = 0;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
            needs_refresh = 1;
        }
        nk_input_end(ctx);

        /* GUI */
        if (nk_begin(ctx, "", nk_rect(0, 0, 600, 540), 0x0))
        {
            nk_layout_row_static(ctx, 10, 380, 1);
            nk_label(ctx, "", NK_TEXT_LEFT);
            nk_label(ctx, u8"█▀▄ █░█░█ ▄▀█ █▀█ █▀▀", NK_TEXT_LEFT);
            nk_label(ctx, u8"█▄▀ ▀▄▀▄▀ █▀█ █▀▄ ██▄ v.0.1 SCP:CB", NK_TEXT_LEFT);
            nk_label(ctx, "", NK_TEXT_LEFT);

            nk_layout_row_static(ctx, 30, 120, 1);
            if (nk_button_label(ctx, "Patch Console") && consolePatched == false) {
                consolePatched = true;
                CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)patchConsole, NULL, NULL, NULL);
            }

            nk_layout_row_static(ctx, 20, 120, 2);
            nk_checkbox_label(ctx, "Inf. ammo", &InfAmmo);
            nk_checkbox_label(ctx, "God Mode", &InfAmmo);
            nk_checkbox_label(ctx, "Noclip", &InfAmmo);

            nk_layout_row_static(ctx, 30, 120, 1);
            nk_label(ctx, "Console:\n", NK_TEXT_LEFT);

            nk_layout_row_static(ctx, 260, 380, 1);
            int sz = (int)sizeof(DconsoleBuffer) - 1;
            nk_edit_string(ctx, NK_EDIT_BOX, DconsoleBuffer, &sz, 2048, nk_filter_default);
        }

        nk_end(ctx);

        /* Draw */
        nk_gdi_render(nk_rgb(30, 30, 130));
    }
    nk_gdifont_del(font);
    ReleaseDC(MenuHWND, dc);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
}