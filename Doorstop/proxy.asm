OPTION CASEMAP:NONE ; Don't change the case of any identifiers (e.g. `GetFileVersionInfoA` stays the same)

EXTERN PGetFileVersionInfoW:QWORD
EXTERN PGetFileVersionInfoA:QWORD
EXTERN PGetFileVersionInfoSizeW:QWORD
EXTERN PVerQueryValueA:QWORD
EXTERN PVerQueryValueW:QWORD
EXTERN PGetFileVersionInfoByHandle:QWORD
EXTERN PGetFileVersionInfoExA:QWORD
EXTERN PGetFileVersionInfoExW:QWORD
EXTERN PGetFileVersionInfoSizeA:QWORD
EXTERN PGetFileVersionInfoSizeExA:QWORD
EXTERN PGetFileVersionInfoSizeExW:QWORD
EXTERN PVerFindFileA:QWORD
EXTERN PVerFindFileW:QWORD
EXTERN PVerInstallFileA:QWORD
EXTERN PVerInstallFileW:QWORD

.code

; Thunks to the real functions in version.dll

PUBLIC GetFileVersionInfoW
GetFileVersionInfoW PROC
	jmp qword ptr [PGetFileVersionInfoW]
GetFileVersionInfoW ENDP

PUBLIC GetFileVersionInfoA
GetFileVersionInfoA PROC
	jmp qword ptr [PGetFileVersionInfoA]
GetFileVersionInfoA ENDP

PUBLIC GetFileVersionInfoSizeW
GetFileVersionInfoSizeW PROC
	jmp qword ptr [PGetFileVersionInfoSizeW]
GetFileVersionInfoSizeW ENDP

PUBLIC VerQueryValueA
VerQueryValueA PROC
	jmp qword ptr [PVerQueryValueA]
VerQueryValueA ENDP

PUBLIC VerQueryValueW
VerQueryValueW PROC
	jmp qword ptr [PVerQueryValueW]
VerQueryValueW ENDP

PUBLIC GetFileVersionInfoByHandle
GetFileVersionInfoByHandle PROC
	jmp qword ptr [PGetFileVersionInfoByHandle]
GetFileVersionInfoByHandle ENDP

PUBLIC GetFileVersionInfoExA
GetFileVersionInfoExA PROC
	jmp qword ptr [PGetFileVersionInfoExA]
GetFileVersionInfoExA ENDP

PUBLIC GetFileVersionInfoExW
GetFileVersionInfoExW PROC
	jmp qword ptr [PGetFileVersionInfoExW]
GetFileVersionInfoExW ENDP

PUBLIC GetFileVersionInfoSizeA
GetFileVersionInfoSizeA PROC
	jmp qword ptr [PGetFileVersionInfoSizeA]
GetFileVersionInfoSizeA ENDP

PUBLIC GetFileVersionInfoSizeExA
GetFileVersionInfoSizeExA PROC
	jmp qword ptr [PGetFileVersionInfoSizeExA]
GetFileVersionInfoSizeExA ENDP

PUBLIC GetFileVersionInfoSizeExW
GetFileVersionInfoSizeExW PROC
	jmp qword ptr [PGetFileVersionInfoSizeExW]
GetFileVersionInfoSizeExW ENDP

PUBLIC VerFindFileA
VerFindFileA PROC
	jmp qword ptr [PVerFindFileA]
VerFindFileA ENDP

PUBLIC VerFindFileW
VerFindFileW PROC
	jmp qword ptr [PVerFindFileW]
VerFindFileW ENDP

PUBLIC VerInstallFileA
VerInstallFileA PROC
	jmp qword ptr [PVerInstallFileA]
VerInstallFileA ENDP

PUBLIC VerInstallFileW
VerInstallFileW PROC
	jmp qword ptr [PVerInstallFileW]
VerInstallFileW ENDP

END