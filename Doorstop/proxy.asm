OPTION CASEMAP:NONE ; Don't change the case of any identifiers (e.g. `GetFileVersionInfoA` stays the same)

EXTERN pGetFileVersionInfoW:QWORD
EXTERN pGetFileVersionInfoA:QWORD
EXTERN pGetFileVersionInfoSizeW:QWORD
EXTERN pVerQueryValueA:QWORD
EXTERN pVerQueryValueW:QWORD
EXTERN pGetFileVersionInfoByHandle:QWORD
EXTERN pGetFileVersionInfoExA:QWORD
EXTERN pGetFileVersionInfoExW:QWORD
EXTERN pGetFileVersionInfoSizeA:QWORD
EXTERN pGetFileVersionInfoSizeExA:QWORD
EXTERN pGetFileVersionInfoSizeExW:QWORD
EXTERN pVerFindFileA:QWORD
EXTERN pVerFindFileW:QWORD
EXTERN pVerInstallFileA:QWORD
EXTERN pVerInstallFileW:QWORD

.code

; Thunks to the real functions in version.dll

PUBLIC GetFileVersionInfoW
GetFileVersionInfoW PROC
	jmp qword ptr [pGetFileVersionInfoW]
GetFileVersionInfoW ENDP

PUBLIC GetFileVersionInfoA
GetFileVersionInfoA PROC
	jmp qword ptr [pGetFileVersionInfoA]
GetFileVersionInfoA ENDP

PUBLIC GetFileVersionInfoSizeW
GetFileVersionInfoSizeW PROC
	jmp qword ptr [pGetFileVersionInfoSizeW]
GetFileVersionInfoSizeW ENDP

PUBLIC VerQueryValueA
VerQueryValueA PROC
	jmp qword ptr [pVerQueryValueA]
VerQueryValueA ENDP

PUBLIC VerQueryValueW
VerQueryValueW PROC
	jmp qword ptr [pVerQueryValueW]
VerQueryValueW ENDP

PUBLIC GetFileVersionInfoByHandle
GetFileVersionInfoByHandle PROC
	jmp qword ptr [pGetFileVersionInfoByHandle]
GetFileVersionInfoByHandle ENDP

PUBLIC GetFileVersionInfoExA
GetFileVersionInfoExA PROC
	jmp qword ptr [pGetFileVersionInfoExA]
GetFileVersionInfoExA ENDP

PUBLIC GetFileVersionInfoExW
GetFileVersionInfoExW PROC
	jmp qword ptr [pGetFileVersionInfoExW]
GetFileVersionInfoExW ENDP

PUBLIC GetFileVersionInfoSizeA
GetFileVersionInfoSizeA PROC
	jmp qword ptr [pGetFileVersionInfoSizeA]
GetFileVersionInfoSizeA ENDP

PUBLIC GetFileVersionInfoSizeExA
GetFileVersionInfoSizeExA PROC
	jmp qword ptr [pGetFileVersionInfoSizeExA]
GetFileVersionInfoSizeExA ENDP

PUBLIC GetFileVersionInfoSizeExW
GetFileVersionInfoSizeExW PROC
	jmp qword ptr [pGetFileVersionInfoSizeExW]
GetFileVersionInfoSizeExW ENDP

PUBLIC VerFindFileA
VerFindFileA PROC
	jmp qword ptr [pVerFindFileA]
VerFindFileA ENDP

PUBLIC VerFindFileW
VerFindFileW PROC
	jmp qword ptr [pVerFindFileW]
VerFindFileW ENDP

PUBLIC VerInstallFileA
VerInstallFileA PROC
	jmp qword ptr [pVerInstallFileA]
VerInstallFileA ENDP

PUBLIC VerInstallFileW
VerInstallFileW PROC
	jmp qword ptr [pVerInstallFileW]
VerInstallFileW ENDP

END