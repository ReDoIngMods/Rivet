#include "functionscanner.hpp"
#include <unordered_map>
#include <winternl.h>

#pragma warning(disable: 4200)

static std::unordered_map<DWORD64, std::vector<Rivet::FunctionData>> GCache;

#define UNW_FLAG_NHANDLER       0x0
#define UNW_FLAG_EHANDLER       0x1
#define UNW_FLAG_UHANDLER       0x2
#define UNW_FLAG_CHAININFO      0x4

typedef unsigned char UBYTE;
typedef unsigned short USHORT;

typedef union _UNWIND_CODE
{
	struct
	{
		UBYTE CodeOffset;
		UBYTE UnwindOp : 4;
		UBYTE OpInfo : 4;
	};
	USHORT FrameOffset;
} UNWIND_CODE, * PUNWIND_CODE;

typedef struct _UNWIND_INFO
{
	UBYTE Version : 3;
	UBYTE Flags : 5;
	UBYTE SizeOfProlog;
	UBYTE CountOfCodes;
	UBYTE FrameRegister : 4;
	UBYTE FrameOffset : 4;
	UNWIND_CODE UnwindCode[1];

	union {
		OPTIONAL ULONG ExceptionHandler;
		OPTIONAL ULONG FunctionEntry;
	};
	OPTIONAL ULONG ExceptionData[];
} UNWIND_INFO, * PUNWIND_INFO;

RIVET_LIB_API Rivet::FunctionScanner::FunctionScanner(const std::wstring& moduleName) {
	InitFunctionScanner(reinterpret_cast<DWORD64>(GetModuleHandle(moduleName.c_str())));
	Scan();
}

RIVET_LIB_API Rivet::FunctionScanner::FunctionScanner(const std::string& moduleName) {
	InitFunctionScanner(reinterpret_cast<DWORD64>(GetModuleHandleA(moduleName.c_str())));
	Scan();
}

RIVET_LIB_API const std::vector<Rivet::FunctionData> Rivet::FunctionScanner::getFunctions() const {
	if (pdataInfo.address == 0)
		return {};

	const auto iterator = GCache.find(pdataInfo.address);
	if (iterator == GCache.end())
		return {};

	return iterator->second;
}

void Rivet::FunctionScanner::Scan() {
	if (pdataInfo.address == 0 || pdataInfo.entryCount == 0)
		return;

	if (GCache.find(pdataInfo.address) != GCache.end())
		return;

	std::vector<FunctionData> functions;
	functions.reserve(pdataInfo.entryCount);

	DWORD64 pdataStart = pdataInfo.address;
	DWORD64 pdataEnd = pdataInfo.address + pdataInfo.entryCount * sizeof(RUNTIME_FUNCTION);

	RUNTIME_FUNCTION* runtimeFunctions = reinterpret_cast<RUNTIME_FUNCTION*>(pdataInfo.address);
	for (size_t index = 0; index < pdataInfo.entryCount; ++index) {
		RUNTIME_FUNCTION& func = runtimeFunctions[index];
		if (!func.BeginAddress || !func.EndAddress)
			continue;

		DWORD64 begin  = baseAddress_ + func.BeginAddress;
		DWORD64 end    = baseAddress_ + func.EndAddress;
		DWORD64 unwind = baseAddress_ + func.UnwindData;

		auto* unwindInfo = reinterpret_cast<UNWIND_INFO*>(unwind);
		if (unwindInfo->Flags & UNW_FLAG_CHAININFO) {
			DWORD64 chainedVa = baseAddress_ + unwindInfo->FunctionEntry;

			if (chainedVa >= pdataStart && chainedVa + sizeof(RUNTIME_FUNCTION) <= pdataEnd) {
				RUNTIME_FUNCTION* chained = reinterpret_cast<RUNTIME_FUNCTION*>(chainedVa);
				begin = baseAddress_ + chained->BeginAddress;
				end = baseAddress_ + chained->EndAddress;
				unwind = baseAddress_ + chained->UnwindData;
			} else {
				continue;
			}
		}

		functions.emplace_back(FunctionData{ begin, end });
	}

	GCache[pdataInfo.address] = std::move(functions);
}

void Rivet::FunctionScanner::InitFunctionScanner(DWORD64 baseAddress) {
	baseAddress_ = baseAddress;

	IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(baseAddress);
	IMAGE_NT_HEADERS64* ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS64*>(baseAddress + dosHeader->e_lfanew);
	IMAGE_DATA_DIRECTORY& dataDir = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION];
	
	PDataInfo output;
	output.address = baseAddress + dataDir.VirtualAddress;
	output.entryCount = dataDir.Size / sizeof(RUNTIME_FUNCTION);

	pdataInfo = output;
}
