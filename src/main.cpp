#include <iostream>
#include <Windows.h>
using namespace std;

BOOL adjusttoken()
{
	HANDLE htoken;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &htoken))
	{
		size_t           s = sizeof(TOKEN_PRIVILEGES) + 2 * sizeof(LUID_AND_ATTRIBUTES);
		TOKEN_PRIVILEGES* p = (PTOKEN_PRIVILEGES)malloc(s);

		if (!LookupPrivilegeValue(NULL, SE_SYSTEM_ENVIRONMENT_NAME, &(p->Privileges[0].Luid)) ||
			!LookupPrivilegeValue(NULL, SE_BACKUP_NAME, &(p->Privileges[1].Luid)) ||
			!LookupPrivilegeValue(NULL, SE_RESTORE_NAME, &(p->Privileges[2].Luid)))
		{

			printf("failed to LookupPrivilegeValue error code : %d \r\n", GetLastError());
			free(p);
			return FALSE;
		}
		p->PrivilegeCount = 3;

		for (int i = 0; i < 3; ++i)
		{
			p->Privileges[i].Attributes = SE_PRIVILEGE_ENABLED;
		}

		if (!AdjustTokenPrivileges(htoken, FALSE, p, s, NULL, NULL) || GetLastError() != ERROR_SUCCESS)
		{
			printf("AdjustTokenPrivileges failed! error code : %d \r\n", GetLastError());
			free(p);
			return FALSE;
		}
		free(p);
	}
	else
	{
		printf("Open process token failed! error code : %d \r\n", GetLastError());
		return FALSE;
	}

	return TRUE;
}

void unlock()
{
	UCHAR c = 1;
	BOOL b = SetFirmwareEnvironmentVariableA("SystemAccess", "{E770BB69-BCB4-4D04-9E97-23FF9456FEAC}", &c, 1);
	if (b)
	{
		cout << "Change BIOS AccessLevel From User To Admin Success!" << endl;
	}
	else
	{
		cout << "Change BIOS AccessLevel From User To Admin Failed! Error Code: " << GetLastError() << endl;
	}

	b = SetFirmwareEnvironmentVariableA("AmiSetupFormSetVar", "{E102C8AA-0F10-4A0E-90A7-A6F816089BB7}", &c, 1);
	if (b)
	{
		cout << "Unlock BIOS Hidden Menu Success!" << endl;
	}
	else
	{
		cout << "Unlock BIOS Hidden Menu Failed! Error Code: " << GetLastError() << endl;
	}
}

UCHAR buffer[0x1000] = { 0 };

void readnvram()
{
	string name;
	string guid;

	cout << "Input Variable name:";
	cin >> name;
	cout << "Input Variable GUID in the format \"{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}\":";
	cin >> guid;
	DWORD size = GetFirmwareEnvironmentVariableA(name.c_str(), guid.c_str(), buffer, 0x1000);

	if (size == 0)
	{
		DWORD err = GetLastError();
		cout << "GetNVRam Variable Failed! Error Code: " << err << " ";
		if (err == ERROR_INVALID_PARAMETER) cout << "ERROR_INVALID_PARAMETER";
		else if (err == ERROR_ENVVAR_NOT_FOUND) cout << "ERROR_ENVVAR_NOT_FOUND";
		cout << endl;
	}
	else
	{
		cout << "Read " << size << " Bytes:" << endl;
		for (int i = 0; i < size; i++)
		{
			if (i % 16 == 0) printf("%04X: ", i);
			printf("%02X ", buffer[i]);
			if (i % 16 == 15) cout << endl;
		}
		cout << endl;
	}
}

int main()
{
	
	if(adjusttoken())
		cout << "Get Privilege Success!" << endl;
	else {
		cout << "Get Privilege Failed! Please run as Administrator!" << endl;
		system("pause");
		return 0;
	}
	
	// unlock();
	readnvram();

	system("pause");
	return 0;
}