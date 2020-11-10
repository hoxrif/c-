// mustafa terzi (hoxrif)
// mustafaterzii@yandex.com
#define PESIGNATURE_OFFSET		0x3C
#define EXPORT_TABLE_OFFSET		0x78
#if !defined(_WINDEF_)
typedef unsigned long DWORD;
#endif
void *my_GetModuleHandle(const char *strModuleName)
{

//bu fonksiyon yalnizca kendi içerisinde bulunan modullerin handle degerlerini bulabilir.
//this function can find only included library handle

DWORD dwPESignatureOff;
DWORD dwPESignatureAddr;
DWORD dwExportTableOff;
DWORD dwExportTableAddr;
DWORD dwBase;
DWORD dwModuleBase;
char *currentModuleNameAddr;
int len_module_name;
int len_current_module_name;
char cKey0,cKey1;
bool bIsEqual;
	__asm
	{
		//Find Modules Base
		mov eax,fs:[0x30]
		mov eax,[eax+0x0C]
		mov eax,[eax+0x1C]
		mov dwBase,eax
	}
	for (len_module_name=0;strModuleName[len_module_name] && strModuleName[len_module_name]!='.';len_module_name++);

	for (dwModuleBase = *(DWORD*)(dwBase + 0x08); dwModuleBase ; dwBase = *(DWORD*)dwBase,dwModuleBase = *(DWORD*)(dwBase + 0x08))
	{
		dwPESignatureOff = *(DWORD*)(dwModuleBase + PESIGNATURE_OFFSET);
		dwPESignatureAddr = dwModuleBase + dwPESignatureOff;
		dwExportTableOff = *(DWORD*)(dwPESignatureAddr + EXPORT_TABLE_OFFSET);
		dwExportTableAddr = dwModuleBase + dwExportTableOff;
		currentModuleNameAddr = (char*)(dwModuleBase + *(DWORD*)(dwExportTableAddr + 0x0C));

		for (len_current_module_name=0;currentModuleNameAddr[len_current_module_name] && currentModuleNameAddr[len_current_module_name]!='.'; len_current_module_name++);
		if (len_module_name != len_current_module_name)
			continue;
		for (int i = 0; i < len_module_name; i++)
		{
			cKey0 = strModuleName[i];
			cKey1 = currentModuleNameAddr[i];
			///// case sensivity off :) ///////
			if (cKey0>= 'A' && cKey0<='Z')
				cKey0+=32;
			if (cKey1>= 'A' && cKey1<='Z')
				cKey1+=32;
			////////////

			bIsEqual = cKey0 == cKey1;
			if (!bIsEqual)
				break;
		}
		if (bIsEqual)
		{
			return (void*)dwModuleBase;
		}
	}
	return (void*)0;
}


int main()
{
	// example
	// ornek
	void *hKernel =	my_GetModuleHandle("kernel32");
}
