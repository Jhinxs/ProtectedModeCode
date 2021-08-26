#include <Windows.h>
#include <stdio.h>

DWORD temp;
byte GDT[6] = { 0 };
DWORD Origineip;

void __declspec(naked) Getxyz()
{
	__asm 
	{
	    
		//int 3
		mov eax, 0x8003f000
		mov ebx,eax
		mov temp,ebx
		sgdt GDT
        
		mov eax,[esp]
        mov Origineip,eax

		mov eax,0x00401060
	    mov [esp],eax

	    retf 
	}
}
void succ() 
{
     
	printf("更改返回成功！！！\n");
	__asm
	{
       push Origineip
	   ret
	}
    
}

int main() {
	
	printf("%p\n", Getxyz);
	printf("%p\n", succ);
	char buffer[6];
	*((DWORD*)&buffer[0]) = 0x12345678;
	*((WORD*)&buffer[4]) = 0x48;
	__asm 
	{
	   
		 call fword ptr [buffer]

	}
	DWORD gdt_addr = *(DWORD*)(&GDT[2]);
    WORD gdt_limit =*(WORD*)(&GDT[0]);
	printf("gdt_addr:%x\n", gdt_addr);
	printf("gdt_limit:%x\n", gdt_limit);
	getchar();
}