#include <stdio.h>
#include <Windows.h>

char buffer2[6];
DWORD STACK[10];
DWORD TSS[0X68];
DWORD dwCS;

void __declspec(naked) TssFunc() //0x00401020
{ 

	__asm													
	{	
		mov ax,cs
	    mov word PTR [dwCS],ax;									
		iret
												
	}													
	
}

int main()
{
	
	DWORD CR3;
	printf("INPUT CR3: ");
	scanf("%x",&CR3);
	DWORD TSS[0x68]=
	{
		    0x00000000,//link
			0x00000000,//esp0
			0x00000000,//ss0
			0x00000000,//esp1
			0x00000000,//ss1
			0x00000000,//esp2
			0x00000000,//ss2
			(DWORD)CR3,//CR3--TSS[7]
			(DWORD)TssFunc,//eip--TSS[8]
			0x00000000,//eflags
			0x11111111,//eax
			0x22222222,//ecx
			0x33333333,//edx
			0x44444444,//ebx
			(DWORD)(STACK),//esp
			0x00000000,//ebp
			0x55555555,//esi
			0x66666666,//edi
			0x00000023,//es
			0x00000008,//cs
			0x00000010,//ss
			0x00000023,//ds
			0x00000030,//fs
			0x00000000,//gs
			0x00000000,//ldt
			0x20AC0000,//iomap
	};

	printf("eq 8003f048 %02X00E9%02X`%04X0068\n", (DWORD)TSS>>24, ((DWORD)TSS >> 16) & 0xFFFF, (WORD)TSS);
	char buff[6];
    *(DWORD*)&buff[4]= 0x48;
	
	printf("CR3:0x%08x\n",CR3);
	printf("TssFunc:0x%08x\n",TssFunc);
    printf("TSS:0x%08x\n",TSS);
	system("pause");
	WORD tr;
	__asm
	{
        int 0x20                 //eq 8003f500 0000e500`00480000
	}
	//printf("%x\n",tr);
	printf("cs: %x",dwCS);
    printf("success !!!\n");
	system("pause");
	return 0;
}