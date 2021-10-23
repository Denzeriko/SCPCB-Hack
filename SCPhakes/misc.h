#include <windows.h>
#include <iostream>
#include <sstream>

#include "nuklear.h"

namespace Hakes {
	extern nk_bool godMode;
	extern nk_bool infAmmo;
	extern nk_bool rapidFire;
	extern nk_bool stamina;
	extern nk_bool rocketAmmo;
	extern nk_bool noBlinking;
}

void menu();
void patchConsole();
void addToDConsole(std::stringstream&);

extern HWND MenuHWND;