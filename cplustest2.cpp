#include <Windows.h>
#include <stdio.h>

#define PTE(x) ((DWORD*)(0xc0000000+((x>>12)<<3)))
#define IDTERRORCODE 0x8003f3e4
#define IDTEIP       0x8003f3e0
#define Target_CR3   0x8003f3e8
#define K_CR2        0x8003f3f0
#define REAL_PTE0    0x8003f3d4
#define REAL_PTE1    0x8003f3d0
#define FAKE_PTE0    0x8003f3dc 
#define FAKE_PTE1    0x8003f3d8

DWORD g_cr2;
DWORD g_eip;
DWORD g_idterrorcode;
DWORD currentCR3;
#pragma section("data_seg",read,write)
__declspec(allocate("data_seg")) DWORD FakePage[1024];//498000

VOID __declspec(naked) IntEntry()
{
	*(DWORD*)REAL_PTE0 = PTE(0x477000)[0];
	*(DWORD*)REAL_PTE1 = PTE(0x477000)[1];
	*(DWORD*)FAKE_PTE0 = PTE(0x498000)[0];
	*(DWORD*)FAKE_PTE1 = PTE(0x498000)[1];
	PTE(0x477000)[0] = PTE(0x477000)[1] = 0;
	__asm
	{
	    mov eax,cr3
		mov ds:[Target_CR3],eax

		iretd

	}

}


VOID go()
{

	__asm
	{
		int 0X20;
	}

}

#pragma code_seg(".mycode") __decpspec(allocate(".mycode")) void main();
#pragma code_seg(".mycode") __decpspec(allocate(".mycode")) void go();

VOID main()  //477000
{
	__asm 
	{
	   jmp L
	   ret
L:
	}
	printf("intry :%08x\n", (DWORD)IntEntry);
	if ((DWORD)IntEntry != 0x401030)
	{
		printf("intry :%08x != 0x401030", (DWORD)IntEntry);
		exit(0);
	}
	FakePage[0] = 1;
	int i = 0;
	go();
	while (true)
	{   
		printf("%d\n", i++);
		Sleep(1000);
	}
	system("pause");

}