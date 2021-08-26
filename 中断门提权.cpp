#include <stdio.h>
#include <Windows.h>

DWORD dwCs;
void __declspec(naked) Interrupt()  //0x00401020
{

	__asm
	{
       
        mov eax,[0x8003f00C]
		mov EBX,[eax]
		mov dwCs,EBX
		iretd

	}
}

int main()
{
	__asm
	{
        int 32;   //0040ee00`00081020 
	}


    printf("hello world\n");
	printf("%p\n",Interrupt);
	printf("%p\n",dwCs);
	getchar();
	return 0;
}