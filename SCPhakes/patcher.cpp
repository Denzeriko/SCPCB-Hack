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

class BaseWeapon
{
public:
    char pad_0000[16]; //0x0000
    int32_t m_iUnknownDef2; //0x0010
    int32_t m_iProjectileType; //0x0014  4 - rocket, 1 - mp5sd, 6 - grenade
    int32_t m_iPrimaryAmmo; //0x0018
    int32_t m_iUnknownDef30; //0x001C
    float m_fPunch; //0x0020
    float m_fRateOfFire; //0x0024
    float m_fIsReloading; //0x0028
    float m_fCanShoot; //0x002C
    int32_t m_iShot; //0x0030
    float m_fViewModel1; //0x0034
    int32_t m_iMagazines; //0x0038
    int32_t m_fFireSound; //0x003C
    int32_t m_fEmptySound; //0x0040
    int32_t m_fReloadSound; //0x0044
    char pad_0048[8]; //0x0048
    void* m_pUnknownBulletPtr; //0x0050
    char pad_0054[32]; //0x0054
    float m_fUnknownDef20; //0x0074
    float m_fUnknownDef10; //0x0078
    float m_fUnknownDef60; //0x007C
    float m_fUnknownDef70; //0x0080
    int32_t m_iIsInSightMode; //0x0084
    float m_fViewModel2; //0x0088
    char pad_008C[4]; //0x008C
    int32_t m_iUnknownDef10; //0x0090
}; //Size: 0x0094

std::stringstream msg;
void patchConsole(){
    //StaticPatcher::Miscellaneous::OpenConsole();
    DWORD scan = scanner();
    
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
    std::stringstream msg;
    msg << "SCP:CB console patched! " << Offset << "\n";
    addToDConsole(msg);
    StaticPatcher::Patching::WriteBytes(patch, Offset, sizeof(patch));

    //GetWeaponXD
    const char* WeaponPtrPattern[2] =
    {
        "\x8B\x00\x00\x00\x00\x00\x89\x04\x24\xE8\x00\x00\x00\x00\x89\x04\x24\xC7\x44\x24\x04\x70\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\x00\x69\xC0\x07\x00\x00\x00\x50\xDB\x04\x24\x58\xBB\x00\x00\x20\x41\x53",
        "x?????xxxx????xxxxxxxxxxxx????xxxxxxxxxxxxxxxxxxx"
    };

    //proper method
    DWORD* WeaponPtr = (DWORD*)(StaticPatcher::SearchOffset::FindPattern(scan, (DWORD)(scan + (DWORD)4194304), (PBYTE)WeaponPtrPattern[0], (char*)WeaponPtrPattern[1]) + 0x2);
    DWORD DerefWeaponPtr = **(DWORD**)WeaponPtr;
    FreeConsole();
    while (true) {
       
        if (DerefWeaponPtr != **(DWORD**)WeaponPtr) {
            DerefWeaponPtr = **(DWORD**)WeaponPtr;
            msg << "WeaponPtr: " << DerefWeaponPtr << "\n";
            addToDConsole(msg);
        }

        if (DerefWeaponPtr != NULL) {
            BaseWeapon* BWP = (BaseWeapon*)DerefWeaponPtr;
            BWP->m_fRateOfFire = 0;
            BWP->m_iPrimaryAmmo = 9999;
            BWP->m_iMagazines = 9999;
        }
        Sleep(5);
    }
  
    FreeConsole();
    return;
}