#include <stdio.h>
#include "windows.h"
#include <iostream>

#include <conio.h>
#include <windows.h>
#include <thread>
#include <vector>
#include <string>
#include <winreg.h>
#include <sstream>
#include <string>

void menu();
void patchConsole();
void addToDConsole(std::stringstream&);

extern HWND MenuHWND;