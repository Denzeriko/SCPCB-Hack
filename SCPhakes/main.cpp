#include "misc.h"

/*TODO*/
/*
    1. Fix fucking scrollbar Y \ edit nuklear.h ffs shit code without api moron.
    2. Make overlay on dx7 :( le pook
*/

void checkButtons(){
    bool isOpened = true;
    bool tglConsole = false;

    while (true) {
        if ((GetAsyncKeyState(VK_PRIOR) & 0x8000) && !tglConsole) {
            isOpened = !isOpened;
            ShowWindow(MenuHWND, isOpened ? SW_HIDE : SW_SHOW);
        }
        tglConsole = (GetAsyncKeyState(VK_PRIOR) & 0x8000);
        Sleep(20);
    }
}

int main() {
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)menu, NULL, NULL, NULL);
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)checkButtons, NULL, NULL, NULL); //useless?
    return 1;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)main, NULL, NULL, NULL);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}