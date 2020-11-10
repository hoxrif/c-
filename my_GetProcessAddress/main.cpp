// mustafa terzi (hoxrif)
// mustafaterzii@yandex.com
#include <windows.h>
#define PESIGNATURE_OFFSET			0x3C
#define EXPORT_TABLE_OFFSET			0x78
#define NUMBER_NAMES_OFFSET			0x18
#define ADDRESS_FUNCTION_OFFSET		0x1C
#define ADDRESS_NAMES_OFFSET		0x20
#define ADDRESS_NAMEORDINALS_OFFSET	0x24
#if !defined(_WINDEF_)
typedef unsigned long DWORD;
typedef unsigned short WORD;
#endif
void *my_GetProcessAddress(void *hModule,const char *strProcessName)
{
	DWORD dwModuleBase = (DWORD)hModule;
	DWORD dwPESignatureOff;
	DWORD dwPESignatureAddr;
	DWORD dwExportTableOff;
	DWORD dwExportTableAddr;
	DWORD dwNumberOfNames;
	DWORD dwAddressOfNames;
	DWORD dwAddressOfFunctions;
	DWORD dwAddressOfNameOrdinals;
	DWORD dwRetFuncAddr;
	char *currentProcessName;
	int len_process_name;
	int len_current_process_name;
	char cKey0,cKey1;
	bool bIsEqual;
	dwPESignatureOff 		= *(DWORD*)(dwModuleBase + PESIGNATURE_OFFSET);
	dwPESignatureAddr 		= dwModuleBase + dwPESignatureOff;
	dwExportTableOff 		= *(DWORD*)(dwPESignatureAddr + EXPORT_TABLE_OFFSET);
	dwExportTableAddr 		= dwModuleBase + dwExportTableOff;
	dwNumberOfNames 		= *(DWORD*)(dwExportTableAddr + NUMBER_NAMES_OFFSET);
	dwAddressOfFunctions 	= *(DWORD*)(dwExportTableAddr + ADDRESS_FUNCTION_OFFSET);
	dwAddressOfNames 		= *(DWORD*)(dwExportTableAddr + ADDRESS_NAMES_OFFSET);
	dwAddressOfNameOrdinals = *(DWORD*)(dwExportTableAddr + ADDRESS_NAMEORDINALS_OFFSET);
	for (len_process_name=0; strProcessName[len_process_name]; len_process_name++);
	for (DWORD i = 0; i < dwNumberOfNames; i++)
	{
		currentProcessName = (char*)(dwModuleBase + *(DWORD*)(dwModuleBase + dwAddressOfNames + (i * 4)));

		for (len_current_process_name=0;currentProcessName[len_current_process_name];len_current_process_name++);

		if (len_process_name != len_current_process_name)
			continue ;

		for (int j = 0; j < len_process_name; j++)
		{
			cKey0 = strProcessName[j];
			cKey1 = currentProcessName[j];
			///// case sensivity off :) ///////
			if (cKey0>= 'A' && cKey0<='Z')
				cKey0+=32;
			if (cKey1>= 'A' && cKey1<='Z')
				cKey1+=32;
			///////////////////////////////////
			bIsEqual = cKey0 == cKey1;
			if (!bIsEqual)
				break;
		}
		if (bIsEqual)
		{
			dwRetFuncAddr = dwModuleBase + *(DWORD*)(dwModuleBase +	dwAddressOfFunctions + (*(WORD*)(dwModuleBase + dwAddressOfNameOrdinals + (i * 2)) << 2));
			return (void*)dwRetFuncAddr;
		}
	}
	return (void*)0;
}
int main()
{
	// example
	// ornek
	void *hKernel =	GetModuleHandle("kernel32.dll");
	void *proc = my_GetProcessAddress(hKernel,"CreateFileA");

}