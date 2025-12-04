#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

int main(void) {
	HMODULE hModule = LoadLibraryW(L"Doorstop.dll");

	Sleep(1000);

	return hModule ? 0 : 1;
}