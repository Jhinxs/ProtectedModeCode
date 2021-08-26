#include <ntifs.h>
#include <ntddk.h>


typedef NTSTATUS(NTAPI* PspTerminateProcess)(PEPROCESS pEprocess, NTSTATUS ExitCode);


NTSTATUS DrvUnLoad(PDRIVER_OBJECT pDriver) 
{
    DbgPrint("[+] Driver Unload Success!");
    return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath) 
{

    pDriver->DriverUnload = DrvUnLoad;
    DbgPrint("[+] Driver load Success!");
    DbgPrint("pdrivre: %x\n", pDriver);

    UNICODE_STRING ntoskrnl;
    RtlInitUnicodeString(&ntoskrnl, L"ntoskrnl.exe");
    
	PVOID pLinkBase = pDriver->DriverSection;
    PVOID pLinkNext = pLinkBase;
    ULONG Ntosbase;
    ULONG imageSize;
    PULONG pspaddr =NULL;
    do
    {
        PUNICODE_STRING  pNtoskrnl = (UNICODE_STRING*)((ULONG)pLinkNext + 0x2c);
        if (RtlCompareUnicodeString(pNtoskrnl, &ntoskrnl, TRUE)==0)
        {
            Ntosbase = *(PULONG)((ULONG)pLinkNext + 0x018);
            imageSize = *(PULONG)((ULONG)pLinkNext + 0x020);
            DbgPrint("[+] Node: %x", pLinkNext);
            DbgPrint("[+] Found pNtoskrnl: %x", Ntosbase);
            DbgPrint("[+] Size of Image: %x", imageSize);
            break;
        } 
        pLinkNext = (PVOID)(*(PULONG)pLinkNext);
    } while (pLinkBase != pLinkNext);

    UINT32 bytecode[] = {
        0x0124a164, 0x758b0000, 0x44703b08, 0x0db80775,
        0xebc00000, 0xbe8d575a, 0x00000248, 0x200147f6,
        0x868d1274, 0x00000174
    };

    for (ULONG i = Ntosbase; i < Ntosbase+imageSize; i++)
    {
        if (memcmp((void*)i, (void*)bytecode, sizeof(bytecode)) == 0)
        {
            DbgPrint("[+] Found PspTerminateProcess address: %x", i-6);
            pspaddr = i - 6;
        }
    }
    PEPROCESS eprocess;
    PsLookupProcessByProcessId((HANDLE)1428,&eprocess);
    DbgPrint("[+] eprocess %x", eprocess);
    PspTerminateProcess pspt = (PspTerminateProcess)(pspaddr);
    pspt(eprocess, 0);
    DbgPrint("[+] 成功关闭!!!");
    return STATUS_SUCCESS;
}