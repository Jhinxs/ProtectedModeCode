#include <stdio.h>
#include <Windows.h>

DWORD funcaddr = (DWORD)0x003a0000;

int* funcpte;
int* zeropte;
void __declspec(naked) func()
{

	*zeropte = *funcpte;
    __asm
	{
       // int 3
        retf
	}
 
}

int main()
{

    DWORD* PageAddr = (DWORD *)VirtualAlloc(NULL,0x1000,MEM_COMMIT,PAGE_READWRITE);
    *PageAddr = 0x87654321;

	printf("PageAddr:%X\n",PageAddr);

	printf("%08x\n",func);

	int* zeroaddr = (int*)0;
    char buff[6];
	*(WORD*)&buff[4] = 0x48;
    
	
	DWORD PDPT = (DWORD)funcaddr >> 30;
	DWORD PDT =  (DWORD)funcaddr >> 21&0x1ff;
	DWORD PTT = (DWORD)funcaddr >> 12&0x1ff;
	DWORD offset = (DWORD)funcaddr&0xfff;
    
	printf("%x\n",funcaddr);
	printf("PDPT:%x\n",PDPT);
	printf("PDT:%x\n",PDT);
	printf("PTT:%x\n",PTT);
	printf("PAGE_OFFSET:%x\n",offset);

	funcpte = (int*)(0xc0000000 + ((funcaddr >> 9) & 0x7ffff8));
    zeropte = (int*)(0xc0000000 + ((0 >> 9) & 0x7ffff8));

	printf("funcpte:%08x\n",funcpte);
    printf("zeropte:%08x\n",zeropte);

	system("pause");
	__asm
	{
		call fword ptr [buff]
	}
    printf("%08x",*zeroaddr);
	system("pause");
	return 0;
}