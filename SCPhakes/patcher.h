#include "misc.h"
#include "rtm.h"

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
typedef enum _MEMORY_INFORMATION_CLASS
{
    MemoryBasicInformation,
    MemoryWorkingSetList,
    MemorySectionName,
    MemoryBasicVlmInformation
} MEMORY_INFORMATION_CLASS;

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

class LocalPlayer
{
public:
    float m_fDeathTimer; //0x0000
    char pad_0004[32]; //0x0004
    float m_fPreviousBlink; //0x0024
    float m_fBlinkTimer; //0x0028
    char pad_002C[16]; //0x002C
    float m_fStamina; //0x003C
    char pad_0040[84]; //0x0040
    float m_fInjuries; //0x0094
    float m_fBloodloss; //0x0098
    char pad_009C[40]; //0x009C
};

typedef NTSTATUS(NTAPI* pNtQueryVirtualMemory)(IN HANDLE ProcessHandle, IN PVOID BaseAddress, IN MEMORY_INFORMATION_CLASS MemoryInformationClass,
    OUT PVOID MemoryInformation, IN ULONG MemoryInformationLength, OUT PULONG ReturnLength OPTIONAL);

DWORD scanner();
