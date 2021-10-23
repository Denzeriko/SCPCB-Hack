#include "patcher.h"
#include <vector>
#define NOT_FOUND -1
DWORD SpeedhackFactorJMP = 0x0;

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

void WeaponHacks(DWORD pWeapon)
{
    static std::vector<CBaseWeapon> Weapons;
    static bool bRapidFire = Hakes::rapidFire;
    static bool bInfiniteAmmo = Hakes::infAmmo;
    static bool bRocketAmmo = Hakes::rocketAmmo;


    int index = NOT_FOUND;
    if (Weapons.size() > 0)
    {
        for (int i = 0; i < Weapons.size(); i++)
        {
            if (Weapons.at(i).GetAddress() == pWeapon)
            {
                index = i;
            }
        }

        if (index == NOT_FOUND)
        {
            Weapons.push_back(CBaseWeapon((CBaseWeaponInfo*)pWeapon));
            index = Weapons.size() - 1;
        }

        if (Hakes::rapidFire)
        {
            Weapons.at(index).m_pBaseWeapon->m_fRateOfFire = 0;
        }

        else
        {
            Weapons.at(index).m_pBaseWeapon->m_fRateOfFire = Weapons.at(index).m_pDefaultInfo->m_fRateOfFire;
        }

        if (Hakes::infAmmo)
        {
            bInfiniteAmmo = Hakes::infAmmo;
            Weapons.at(index).m_pBaseWeapon->m_iPrimaryAmmo = 1337;
            Weapons.at(index).m_pBaseWeapon->m_iMagazines = 1337;
        }

        else
        {
            if (bInfiniteAmmo)
            {
                Weapons.at(index).m_pBaseWeapon->m_iPrimaryAmmo = Weapons.at(index).m_pDefaultInfo->m_iPrimaryAmmo;
                Weapons.at(index).m_pBaseWeapon->m_iMagazines = Weapons.at(index).m_pDefaultInfo->m_iMagazines;
            }
            bInfiniteAmmo = false;
        }

        if (Hakes::rocketAmmo)
        {
            bRocketAmmo = Hakes::rocketAmmo;
            Weapons.at(index).m_pBaseWeapon->m_iProjectileType = eProjectileType::ROCKET;
        }

        else
        {
            if (bRocketAmmo)
            {
                Weapons.at(index).Default();
            }

            bRocketAmmo = false;
        }

       // Weapons.at(index).m_pBaseWeapon->m_iProjectileType = Hakes::rocketAmmo ? eProjectileType::ROCKET : m_pDefaultInfo->m_iPrimaryAmmo;
    }

    else
    {
        Weapons.push_back(CBaseWeapon((CBaseWeaponInfo*)pWeapon));
    }
}

__declspec(naked) void ASM_Speedhack()
{
    __asm
    {
        add eax, Hakes::speedhack
        jmp SpeedhackFactorJMP
    }
}

void patchSpeed(DWORD scan)
{
    BYTE JNE[1] = { 0x85 };
    const char* ShakePtrPattern[2] =
    {
        "\x0F\x84\x00\x00\x00\x00\x8B\x00\x00\x00\x00\x00\x53\xD9\x04\x24\x5B\x8B\x00\x00\x00\x00\x00\x56\xD9\x04\x24\x5E\xDE\xC1\x53\xD9\x1C\x24\x5B\x89\x00\x00\x00\x00\x00\x81\xEC\x04\x00\x00\x00\x8B\x00\x00\x00\x00\x00\x50\xD9\x04\x24\x58\xBB\x00\x00\x00\x00\x53\xD9\x04\x24\x5B\xDE\xF9\x50\xD9\x1C\x24\x58\x89\x04\x24\xE8\x00\x00\x00\x00\x8B",
        "xx????x?????xxxxxx?????xxxxxxxxxxxxx?????xxxxxxx?????xxxxxx????xxxxxxxxxxxxxxxx????x"
    };

    const char* FastMovePtrPattern[2] =
    {
        "\x0F\x84\x00\x00\x00\x00\x81\xEC\x18\x00\x00\x00\x8B\x00\x00\x00\x00\x00\x89\x04\x24\xE8\x00\x00\x00\x00\x89\x04\x24\xC7\x44\x24\x04\x64\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\x00\xC1\xE0\x02\x03\x00\x00\x00\x00\x00\x8B\x00\x89\x04\x24\xE8\x00\x00\x00\x00\x89\x04\x24\xC7\x44\x24\x04\xB4\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\x00\x3D\x05\x00\x00\x00\x0F\x94\xD0\x0F\xB6\xC0\x69\xC0\x06\x00\x00\x00",
        "xx????xxxxxxx?????xxxx????xxxxxxxxxxxx????xxxxxx?????xxxxxx????xxxxxxxxxxxx????xxxxxxxxxxxxxxxxxxx"
    };

    const char* SpeedFactorPtrPattern[2] =
    {
        "\x05\x03\x00\x00\x00\x50\xDB\x04\x24\x58\x8B\x5D\xF8\x53\xD9\x04\x24\x5B\xDE\xC9\x53\xD9\x1C\x24\x5B\x89\x5D\xF8\x81\xEC\x18\x00\x00\x00\x8B\x00\x00\x00\x00\x00\x89\x04\x24\xE8\x00\x00\x00\x00\x89\x04\x24\xC7\x44\x24\x04\x64\x00\x00\x00",
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx?????xxxx????xxxxxxxxxxx"
    };

    DWORD ShakeOffset = StaticPatcher::SearchOffset::FindPattern(scan, (DWORD)(scan + (DWORD)4194304), (PBYTE)ShakePtrPattern[0], (char*)ShakePtrPattern[1]) + 0x1;
    DWORD FastMoveOffset = StaticPatcher::SearchOffset::FindPattern(scan, (DWORD)(scan + (DWORD)4194304), (PBYTE)FastMovePtrPattern[0], (char*)FastMovePtrPattern[1]) + 0x1;
    DWORD SpeedFactorOffset = StaticPatcher::SearchOffset::FindPattern(scan, (DWORD)(scan + (DWORD)4194304), (PBYTE)SpeedFactorPtrPattern[0], (char*)SpeedFactorPtrPattern[1]);
    SpeedhackFactorJMP = SpeedFactorOffset + 0x5;
    StaticPatcher::Patching::WriteBytes(JNE, ShakeOffset, sizeof(JNE));
    StaticPatcher::Patching::WriteBytes(JNE, FastMoveOffset, sizeof(JNE));
    StaticPatcher::Patching::CreateWarp((BYTE*)SpeedFactorOffset, (DWORD)ASM_Speedhack, 5);
}

void patchConsole() 
{
    DWORD scan = scanner();
    std::stringstream msg;

    msg << "VM: 0x" << std::hex << scan << "\n";
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

    msg << "SCP:CB console patched! 0x" << std::hex << Offset << "\n";
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

    const char* GodmodePtrPattern[2] =
    {
        "\x81\x3D\x00\x00\x00\x00\x00\x00\x00\x00\x89\xF0\x0F\x94\xD0\x0F\xB6\xC0\x89\xC6\x8B\x85\xDC\xFE\xFF\xFF\x21\xF3\x21\xC3\x21\xDB\x0F\x84\x00\x00\x00\x00\x81\xEC\x24\x00\x00\x00\x8B\x00\x00\x00\x00\x00\x89\x04\x24\xE8",
        "xx????????xxxxxxxxxxxxxxxxxxxxxxxx????xxxxxxx?????xxxx"
    };

    FreeConsole();

    DWORD* WeaponPtr = (DWORD*)(StaticPatcher::SearchOffset::FindPattern(scan, (DWORD)(scan + (DWORD)4194304), (PBYTE)WeaponPtrPattern[0], (char*)WeaponPtrPattern[1]) + 0x2);
    DWORD* LocalPlayerPtr = (DWORD*)(StaticPatcher::SearchOffset::FindPattern(scan, (DWORD)(scan + (DWORD)4194304), (PBYTE)LocalPlayerPtrPattern[0], (char*)LocalPlayerPtrPattern[1]) + 0x2);
    DWORD* CameraFogFarPtr = (DWORD*)(StaticPatcher::SearchOffset::FindPattern(scan, (DWORD)(scan + (DWORD)4194304), (PBYTE)CameraFogFarPtrPattern[0], (char*)CameraFogFarPtrPattern[1]) + 0x2);
    DWORD* NoclipPtr = (DWORD*)(StaticPatcher::SearchOffset::FindPattern(scan, (DWORD)(scan + (DWORD)4194304), (PBYTE)NoclipPtrPattern[0], (char*)NoclipPtrPattern[1]) + 0x2);
    DWORD* GodmodePtr = (DWORD*)(StaticPatcher::SearchOffset::FindPattern(scan, (DWORD)(scan + (DWORD)4194304), (PBYTE)GodmodePtrPattern[0], (char*)GodmodePtrPattern[1]) + 0x2);
    float* DerefCameraFogFarPtr = *(float**)CameraFogFarPtr;
    int* DerefNoclipPtr = *(int**)NoclipPtr;
    int* DerefGodmodePtr = *(int**)GodmodePtr;
    DWORD* DerefLocalPlayerPtr = *(DWORD**)LocalPlayerPtr;
    DWORD DerefWeaponPtr = **(DWORD**)WeaponPtr;

    msg << "WeaponPtr: 0x" << std::hex << DerefWeaponPtr << "\n";
    msg << "LocalPlayerPtr: 0x" << std::hex << DerefLocalPlayerPtr << "\n";
    addToDConsole(msg);

    bool tglNoclip = false; //fix later this cum omg pls kill me AW(D(AWD(W
    bool tglNoclip2 = false;
    *DerefNoclipPtr = 0;
    
    patchSpeed(scan);
    while (true)
    {
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
            msg << "WeaponPtr: 0x" << std::hex << (DWORD)DerefWeaponPtr << "\n";
            addToDConsole(msg);
        }

        if (DerefLocalPlayerPtr != *(DWORD**)LocalPlayerPtr)
        {
            DerefLocalPlayerPtr = *(DWORD**)LocalPlayerPtr;
            msg << "LocalPlayerPtr: 0x" << std::hex << DerefLocalPlayerPtr << "\n";
            addToDConsole(msg);
        }

        if (DerefLocalPlayerPtr != NULL)
        {
            LocalPlayerInfo* LPDP = (LocalPlayerInfo*)DerefLocalPlayerPtr;
            *DerefGodmodePtr = Hakes::godMode ? 1 : 0;

            if (Hakes::godMode)
            {
                LPDP->m_fBloodloss = 0.0f;
                LPDP->m_fInjuries = 0.0f;
            }

            if (Hakes::noBlinking)
                LPDP->m_fBlinkTimer = 600.0f;

            if (Hakes::stamina)
                LPDP->m_fStamina = 100.0f;
        }

        if (DerefWeaponPtr != NULL)
        {
            WeaponHacks(DerefWeaponPtr);
        }
        Sleep(5);
    }

    return;
}