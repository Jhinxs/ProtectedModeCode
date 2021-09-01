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

DWORD g_out;
DWORD old_pte[2];

VOID JmpFunc();
char* p;
//00401030
VOID __declspec(naked) IntEntry() 
{
    p = (char*)0x8003f120;
	for (int i = 0; i < 256; i++)
	{
		*p = ((char*)JmpFunc)[i];
		p++;
	}

	__asm
	{
		mov eax, 0xffffffff
		mov ds:[Target_CR3],eax

		mov eax,cr0
		and eax,~0x10000
		mov cr0,eax


		mov eax,0x03f12068
		mov ds:[0x80541450],eax
		mov ax,0xc380
		mov ds:[0x80541454],ax

		xor eax,eax
		mov ds:[IDTERRORCODE],eax
		mov ds:[IDTEIP],eax
		mov ds:[K_CR2],eax


		mov eax, cr0
		or eax, 0x10000
		mov cr0, eax
	
	    iretd
	}
}

VOID __declspec(naked) JmpFunc() 
{
	__asm
	{
		pushad
		xor eax, eax
		mov eax, cr3
		cmp eax, ds: [Target_CR3]
		jnz PASS

		mov eax, cr2
		shr eax, 0xc
		cmp eax, 0x477
		jnz PASS

		mov eax, ss: [esp + 0x20]
		test eax, 0x10
		jnz EXEC
		jmp RW

EXEC :
	    }
		PTE(0x477000)[0] = *(DWORD*)REAL_PTE0;
		PTE(0x477000)[1] = *(DWORD*)REAL_PTE1;

		__asm 
		{
		   mov eax,0x0477020
		   call eax
		}
		PTE(0x477000)[0] = PTE(0x477000)[1] = 0;
		__asm
		{
			popad
			add esp, 4
			iretd
RW : 
		}
		

		PTE(0x477000)[0] = *(DWORD*)FAKE_PTE0;
		PTE(0x477000)[1] = *(DWORD*)FAKE_PTE1;
	
		__asm
		{
		    mov eax,ds:[0x477000]
		}
		PTE(0x477000)[0] = PTE(0x477000)[1] = 0;
		__asm{
		popad
		add esp, 4
		iretd
PASS:
		popad
		mov word ptr[esp + 2], 0
        push 0x80541457
	    ret
	}

}

VOID go() 
{

	__asm 
	{
		int 0X20;
	}

}



VOID main()
{
	printf("intry :%08x",(DWORD)IntEntry);
	if ((DWORD)IntEntry != 0x401030)
	{
		printf("intry :%08x != 0x401030", (DWORD)IntEntry);
		exit(0);
	}
	go();
	//printf("%x\n", g_out);
	system("pause");
}
