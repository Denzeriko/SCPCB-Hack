#include "patcher.h"

DWORD scanner()
{
    pNtQueryVirtualMemory ntQVM;
    MEMORY_BASIC_INFORMATION MBI = { 0 };
    ntQVM = (pNtQueryVirtualMemory)GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "NtQueryVirtualMemory");
    DWORD CurrentAddress = 0x00000000;
    SIZE_T Length = 0L;
    while (CurrentAddress < 0x10000000)
    {
        if (NT_SUCCESS(ntQVM(GetCurrentProcess(), (PVOID*)CurrentAddress, MemoryBasicInformation, &MBI, sizeof(MEMORY_BASIC_INFORMATION), &Length)))
        {
            if (MBI.Type == MEM_PRIVATE)
            {
                std::cout << "Address: " << MBI.AllocationBase << "\t" << "Size: " << MBI.RegionSize
                    << "\tVirtualProtect: " << MBI.Protect << std::endl;
            }
        }
        if (MBI.RegionSize == 4194304)
            return (DWORD)MBI.AllocationBase;
        CurrentAddress += MBI.RegionSize;
    }
    return (DWORD)0x0;
}


void patchConsole() {
    //StaticPatcher::Miscellaneous::OpenConsole();
    DWORD scan = scanner();
    std::stringstream msg;

    msg << "VM: " << scan << "\n";
    addToDConsole(msg);
    const char* ConsolePattern[2] =
    { "\x0F\x85\x00\x00\x00\x00\x81\xEC\x0C\x00\x00\x00\x8B\x00\x00\x00\x00\x00\x89\x04\x24\xE8\x00\x00\x00\x00\x89\x04\x24\xC7\x44\x24\x04\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\x00\x21\xC0\x0F\x84",
        "xx????xxxxxxx?????xxxx????xxxxxxxxxxxx????xxxxxx" };

    DWORD Offset = StaticPatcher::SearchOffset::FindPattern(scan, (DWORD)(scan + (DWORD)4194304), (PBYTE)ConsolePattern[0], (char*)ConsolePattern[1]);

    if (!Offset) {
        msg << "Couldn't patch SCP:CB console admin check!\n";
        addToDConsole(msg);
        return;
    }
    BYTE patch[3] = { 0x0F, 0x84, 0x95 };

    msg << "SCP:CB console patched! " << Offset << "\n";
    addToDConsole(msg);
    StaticPatcher::Patching::WriteBytes(patch, Offset, sizeof(patch));

    const char* WeaponPtrPattern[2] =
    {
        "\x8B\x00\x00\x00\x00\x00\x89\x04\x24\xE8\x00\x00\x00\x00\x89\x04\x24\xC7\x44\x24\x04\x70\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\x00\x69\xC0\x07\x00\x00\x00\x50\xDB\x04\x24\x58\xBB\x00\x00\x20\x41\x53",
        "x?????xxxx????xxxxxxxxxxxx????xxxxxxxxxxxxxxxxxxx"
    };

    const char* LocalPlayerPtrPattern[2] =
    {
        "\x89\x00\x00\x00\x00\x00\x81\xEC\x04\x00\x00\x00\x8B\x00\x00\x00\x00\x00\x89\x04\x24\xE8\x00\x00\x00\x00\x81\xEC\x1C\x00\x00\x00\x8B\x00\x00\x00\x00\x00\x89\x04\x24\xC7\x44\x24\x04\x01\x00\x00\x00\xE8\x00\x00\x00\x00\x50\xD9\x1C\x24\x58\x89\x44\x24\x08\x89\x45\xF4",
        "x?????xxxxxxx?????xxxx????xxxxxxx?????xxxxxxxxxxxx????xxxxxxxxxxxx"
    };

    const char* CameraFogFarPtrPattern[2] =
    {
        "\x8B\x00\x00\x00\x00\x00\x56\xD9\x04\x24\x5E\x8B\x00\x00\x00\x00\x00\x57\xD9\x04\x24\x5F\xDE\xC9\x56\xD9\x1C\x24\x5E\x89\x74\x24\x08\x8B\x00\x00\x00\x00\x00\x89\x04\x24\xE8",
        "x?????xxxxxx?????xxxxxxxxxxxxxxxxx?????xxxx"
    };

    const char* NoclipPtrPattern[2] =
    {
        "\x8B\x00\x00\x00\x00\x00\x21\xDB\x0F\x84\x00\x00\x00\x00\x81\x7D\x14\x00\x00\x00\x00\x0F\x85\x00\x00\x00\x00\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\x81\xEC\x20\x00\x00\x00",
        "x?????xxxx????xxxxxxxxx????xx????????xx????????xx????????xx????????xxxxxx"
    };

    FreeConsole();

    DWORD* WeaponPtr = (DWORD*)(StaticPatcher::SearchOffset::FindPattern(scan, (DWORD)(scan + (DWORD)4194304), (PBYTE)WeaponPtrPattern[0], (char*)WeaponPtrPattern[1]) + 0x2);
    DWORD* LocalPlayerPtr = (DWORD*)(StaticPatcher::SearchOffset::FindPattern(scan, (DWORD)(scan + (DWORD)4194304), (PBYTE)LocalPlayerPtrPattern[0], (char*)LocalPlayerPtrPattern[1]) + 0x2);
    DWORD* CameraFogFarPtr = (DWORD*)(StaticPatcher::SearchOffset::FindPattern(scan, (DWORD)(scan + (DWORD)4194304), (PBYTE)CameraFogFarPtrPattern[0], (char*)CameraFogFarPtrPattern[1]) + 0x2);
    DWORD* NoclipPtr = (DWORD*)(StaticPatcher::SearchOffset::FindPattern(scan, (DWORD)(scan + (DWORD)4194304), (PBYTE)NoclipPtrPattern[0], (char*)NoclipPtrPattern[1]) + 0x2);
    float* DerefCameraFogFarPtr = *(float**)CameraFogFarPtr;
    int* DerefNoclipPtr = *(int**)NoclipPtr;
    DWORD* DerefLocalPlayerPtr = *(DWORD**)LocalPlayerPtr;
    DWORD DerefWeaponPtr = **(DWORD**)WeaponPtr;

    msg << "WeaponPtr: " << DerefWeaponPtr << "\n";
    msg << "LocalPlayerPtr: " << DerefLocalPlayerPtr << "\n";
    addToDConsole(msg);

    bool tglNoclip = false; //fix later this cum omg pls kill me AW(D(AWD(W
    bool tglNoclip2 = false;
    *DerefNoclipPtr = 0;
    while (true)
    {
        //
        if ((GetAsyncKeyState(0x56) & 0x8000) && !tglNoclip) {
            tglNoclip2 = !tglNoclip2;
            //msg << "NOCLIP: " << (tglNoclip2 ? "ON" : "OFF") << "\n"; //uncomment then ill fix fucking addToDConsole overflow :( and scrollbar
            //addToDConsole(msg);
            *DerefNoclipPtr = tglNoclip2 ? 0 : 1;
        }
        tglNoclip = (GetAsyncKeyState(0x56) & 0x8000);

        *DerefCameraFogFarPtr = 1000.0f;
        if (DerefWeaponPtr != **(DWORD**)WeaponPtr)
        {
            DerefWeaponPtr = **(DWORD**)WeaponPtr;
            msg << "WeaponPtr: " << (DWORD)DerefWeaponPtr << "\n";
            addToDConsole(msg);
        }

        if (DerefLocalPlayerPtr != *(DWORD**)LocalPlayerPtr)
        {
            DerefLocalPlayerPtr = *(DWORD**)LocalPlayerPtr;
            msg << "LocalPlayerPtr: " << DerefLocalPlayerPtr << "\n";
            addToDConsole(msg);
        }

        if (DerefLocalPlayerPtr != NULL)
        {
            LocalPlayer* LPDP = (LocalPlayer*)DerefLocalPlayerPtr;
            if (Hakes::godMode)
                LPDP->m_fBloodloss = 0.0f;
                LPDP->m_fInjuries = 0.0f;
            if (Hakes::noBlinking)
                LPDP->m_fBlinkTimer = 600.0f;
            if (Hakes::stamina)
                LPDP->m_fStamina = 100.0f;
        }

        if (DerefWeaponPtr != NULL)
        {
            BaseWeapon* BWP = (BaseWeapon*)DerefWeaponPtr;
            if(Hakes::rapidFire)
                BWP->m_fRateOfFire = 0;
            if (Hakes::infAmmo) 
                BWP->m_iPrimaryAmmo = 9999;
            if(Hakes::infAmmo)
                BWP->m_iMagazines = 9999;

            BWP->m_iProjectileType = Hakes::rocketAmmo ? 4 : 1; //bruh
            //if (Hakes::rocketAmmo)
            //    BWP->m_iProjectileType = 4; //found how to gather default value bruh
                 //found how to gather default value bruh
        }
        Sleep(5);
    }

    return;
}