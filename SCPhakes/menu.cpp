#include "misc.h"

#define WIN32_LEAN_AND_MEAN
//#define NK_INCLUDE_FIXED_TYPES
//#define NK_INCLUDE_STANDARD_IO
//#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_GDI_IMPLEMENTATION

#include "nuklear.h"
#include "nuklear_gdi.h"


//#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "Msimg32.lib")

static LRESULT CALLBACK WindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    if (nk_gdi_handle_event(wnd, msg, wparam, lparam))
        return 0;
    return DefWindowProcW(wnd, msg, wparam, lparam);
}

namespace Hakes {
    extern nk_bool godMode = false;
    extern nk_bool infAmmo = false;
    extern nk_bool rapidFire = false;
    extern nk_bool stamina = false;
    extern nk_bool rocketAmmo = false;
    extern nk_bool noBlinking = false;
}

bool consolePatched = false;
HWND MenuHWND;

char DconsoleBuffer[2048] = "Console INIT!\n-------------\nLoaded SCP:CB Hakes\n";
bool DConsoleInit = false;

void addToDConsole(std::stringstream &what) {
    //if (strlen(DconsoleBuffer) > 2000)
    //    memset(DconsoleBuffer, 0, sizeof(DconsoleBuffer)); //clear to prevent overflow xd:XDXDAUSDA*WD)

    //if (DConsoleInit == false) {
    //    ZeroMemory(&DconsoleBuffer, sizeof(DconsoleBuffer));
    //    char DconsoleBuffer[2048] = "Console INIT!\n-------------\nLoaded SCP:CB Hakes\n";
    //    DConsoleInit = true;
    //}
    //if(number_of_lines > 2)
    //    buf.erase(0, buf.find("\n") + 1); //remove first line

    strcat(DconsoleBuffer, what.str().c_str());
    what.str("");
}

void clearToDConsole() {
    ZeroMemory(&DconsoleBuffer, sizeof(DconsoleBuffer));
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

    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandleW(0);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"SCP:CB Hakes";
    RegisterClassW(&wc);

    AdjustWindowRectEx(&rect, style, FALSE, exstyle);
    MenuHWND = CreateWindowExW(exstyle, wc.lpszClassName, L"DWARE v.0.2a SCP:CB", style | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, wc.hInstance, NULL);

    dc = GetDC(MenuHWND);
    font = nk_gdifont_create("Consolas", 16);
    ctx = nk_gdi_init(font, dc, 600, 540);

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
            nk_label(ctx, u8"█▄▀ ▀▄▀▄▀ █▀█ █▀▄ ██▄ v.0.2a SCP:CB", NK_TEXT_LEFT);
            nk_label(ctx, "", NK_TEXT_LEFT);

            nk_layout_row_static(ctx, 30, 120, 1);
            if (nk_button_label(ctx, "Patch Console")) {
                if (consolePatched == false) {
                    consolePatched = true;
                    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)patchConsole, NULL, NULL, NULL);
                }
                else {
                    std::stringstream asd;
                    asd << "Console already patched!\n";
                    addToDConsole(asd);
                }
            }

            nk_layout_row_static(ctx, 20, 120, 2);
            nk_checkbox_label(ctx, "Inf. ammo", &Hakes::infAmmo);
            nk_checkbox_label(ctx, "Inf. stamina", &Hakes::stamina);
            nk_checkbox_label(ctx, "God Mode", &Hakes::godMode);
            nk_checkbox_label(ctx, "Rapid Fire", &Hakes::rapidFire);
            nk_checkbox_label(ctx, "No blinking", &Hakes::noBlinking);

            nk_layout_row_static(ctx, 20, 120, 1);
            nk_label(ctx, "Experemental:", NK_TEXT_LEFT);

            nk_layout_row_static(ctx, 20, 120, 2);
            nk_checkbox_label(ctx, "Rocket Ammo", &Hakes::rocketAmmo);
            

            nk_layout_row_static(ctx, 30, 120, 1);
            nk_label(ctx, "Console:\n", NK_TEXT_LEFT);

            nk_layout_row_static(ctx, 220, 380, 1);
            int sz = (int)sizeof(DconsoleBuffer) - 1;
            nk_edit_string(ctx, NK_EDIT_SIMPLE | NK_EDIT_NO_CURSOR | NK_EDIT_CLIPBOARD | NK_EDIT_MULTILINE | NK_EDIT_READ_ONLY, DconsoleBuffer, &sz, 2048, nk_filter_default); //nuklear.h 27084 fix \ rewrite this shit pls.
            //ctx->current->edit.scrollbar.y = (nk_uint)25;

            ctx->current->edit.active = nk_true; //fucking moron.
            if (ctx->text_edit.scrollbar.y > 7000) // prevent leak shit fix, but work
                clearToDConsole();
        }

        nk_end(ctx);
        nk_gdi_render(nk_rgb(30, 30, 130));
    }
    nk_gdifont_del(font);
    ReleaseDC(MenuHWND, dc);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
}