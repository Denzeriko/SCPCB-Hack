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

typedef NTSTATUS(NTAPI* pNtQueryVirtualMemory)(IN HANDLE ProcessHandle, IN PVOID BaseAddress, IN MEMORY_INFORMATION_CLASS MemoryInformationClass,
    OUT PVOID MemoryInformation, IN ULONG MemoryInformationLength, OUT PULONG ReturnLength OPTIONAL);

DWORD scanner();