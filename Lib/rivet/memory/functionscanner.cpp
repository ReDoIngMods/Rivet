#include "functionscanner.hpp"
#include <unordered_map>
#include <algorithm>

#include <Zydis/Zydis.h>

static std::unordered_map<DWORD64, std::vector<Rivet::FunctionData>> GCache;

RIVET_LIB_API Rivet::FunctionScanner::FunctionScanner(std::wstring_view moduleName) {
	baseAddress_ = reinterpret_cast<DWORD64>(GetModuleHandle(moduleName.data()));
	Scan();
}

RIVET_LIB_API Rivet::FunctionScanner::FunctionScanner(std::string_view moduleName) {
	baseAddress_ = reinterpret_cast<DWORD64>(GetModuleHandleA(moduleName.data()));
	Scan();
}

RIVET_LIB_API const std::vector<Rivet::FunctionData> Rivet::FunctionScanner::getFunctions() const {
	if (baseAddress_ == 0)
		return {};

	const auto iterator = GCache.find(baseAddress_);
	if (iterator == GCache.end())
		return {};

	return iterator->second;
}

void Rivet::FunctionScanner::Scan() {
    IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(baseAddress_);
    IMAGE_NT_HEADERS* ntHeader = reinterpret_cast<IMAGE_NT_HEADERS*>(baseAddress_ + dosHeader->e_lfanew);

    uint64_t startAddress = 0, endAddress = 0;
    IMAGE_SECTION_HEADER* pSection = IMAGE_FIRST_SECTION(ntHeader);
    for (int i = 0; i < ntHeader->FileHeader.NumberOfSections; ++i, ++pSection) {
        if (memcmp(pSection->Name, ".text", 5) != 0)
            continue;

        startAddress = baseAddress_ + pSection->VirtualAddress;
        endAddress   = startAddress + pSection->Misc.VirtualSize;
        break;
    }

    uint8_t* code = reinterpret_cast<uint8_t*>(startAddress);
    size_t size = endAddress - startAddress;

    ZydisDecoder decoder;
    ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);
    ZydisDecoderContext ctx = {};

    std::vector<uint64_t> functionStarts;
    functionStarts.reserve(size / 4);
    functionStarts.push_back(startAddress);

    uint64_t offset = 0;
    while (offset < size) {
        ZydisDecodedInstruction instr;
        if (!ZYAN_SUCCESS(ZydisDecoderDecodeInstruction(&decoder, &ctx, code + offset, size - offset, &instr))) {
            ++offset;
            continue;
        }

        if (instr.mnemonic == ZYDIS_MNEMONIC_CALL && instr.raw.imm[0].is_signed) {
            int64_t rel = instr.raw.imm[0].value.s;
            uint64_t target = startAddress + offset + instr.length + rel;

            if (target >= startAddress && target < endAddress)
                functionStarts.push_back(target);
        }

        offset += instr.length;
    }

    std::sort(functionStarts.begin(), functionStarts.end());
    functionStarts.erase(std::unique(functionStarts.begin(), functionStarts.end()), functionStarts.end());

    std::vector<FunctionData> functions;
    for (size_t i = 0; i < functionStarts.size(); ++i) {
        uint64_t funcStart = functionStarts[i];
        uint64_t funcEnd = (i + 1 < functionStarts.size()) ? functionStarts[i + 1] : endAddress;

        uint64_t localOffset = funcStart - startAddress;

        while (localOffset < funcEnd - startAddress) {
            ZydisDecodedInstruction instr;
            if (!ZYAN_SUCCESS(ZydisDecoderDecodeInstruction(&decoder, &ctx, code + localOffset, funcEnd - startAddress - localOffset, &instr))) {
                funcEnd = funcStart + localOffset;
                break;
            }

            if (instr.mnemonic == ZYDIS_MNEMONIC_RET) {
                funcEnd = funcStart + localOffset + instr.length;
                break;
            } else if (instr.mnemonic == ZYDIS_MNEMONIC_JMP) {
                uint64_t target = instr.raw.imm[0].is_signed ? funcStart + localOffset + instr.length + instr.raw.imm[0].value.s : instr.raw.imm[0].value.u;

                if (target < funcStart || target >= funcEnd) {
                    funcEnd = funcStart + localOffset + instr.length;
                    break;
                }
            }

            localOffset += instr.length;
        }

        functions.emplace_back(FunctionData{ funcStart, funcEnd });
    }

    GCache[baseAddress_] = std::move(functions);
}