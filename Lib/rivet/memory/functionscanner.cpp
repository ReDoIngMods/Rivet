#include "functionscanner.hpp"
#include "peheaders.hpp"

#include <unordered_map>
#include <algorithm>

#include <Zydis/Zydis.h>

static std::unordered_map<DWORD64, std::vector<Rivet::FunctionData>> GCache;

RIVET_LIB_API Rivet::FunctionScanner::FunctionScanner(std::string_view moduleName) {
    PEHeaderManager& peHeaderMgr = PEHeaderManager::getInstance();

    PEHeadersMap headers;
    if (!peHeaderMgr.queryModuleHeaders("ScrapMechanic.exe", headers))
        return;

    PEHeader header = headers[".text"];
    startAddress_ = header.startAddress;
    endAddress_ = header.endAddress;

	Scan();
}

RIVET_LIB_API const std::vector<Rivet::FunctionData> Rivet::FunctionScanner::getFunctions() const {
	if (startAddress_ == 0 || endAddress_ == 0)
		return {};

	const auto iterator = GCache.find(startAddress_);
	if (iterator == GCache.end())
		return {};

	return iterator->second;
}

void Rivet::FunctionScanner::Scan() const {
    if (startAddress_ == 0 || endAddress_ == 0)
        return;

    if (GCache.contains(startAddress_))
        return;

    IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(startAddress_);
    IMAGE_NT_HEADERS* ntHeader = reinterpret_cast<IMAGE_NT_HEADERS*>(startAddress_ + dosHeader->e_lfanew);

    uint8_t* code = reinterpret_cast<uint8_t*>(startAddress_);
    size_t size = endAddress_ - startAddress_;

    ZydisDecoder decoder;
    ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);
    ZydisDecoderContext ctx = {};

    std::vector<uint64_t> functionStarts;
    functionStarts.reserve(size / 4);
    functionStarts.push_back(startAddress_);

    uint64_t offset = 0;
    while (offset < size) {
        ZydisDecodedInstruction instr;
        if (!ZYAN_SUCCESS(ZydisDecoderDecodeInstruction(&decoder, &ctx, code + offset, size - offset, &instr))) {
            ++offset;
            continue;
        }

        if (instr.mnemonic == ZYDIS_MNEMONIC_CALL && instr.raw.imm[0].is_signed) {
            int64_t rel = instr.raw.imm[0].value.s;
            uint64_t target = startAddress_ + offset + instr.length + rel;

            if (target >= startAddress_ && target < endAddress_)
                functionStarts.push_back(target);
        }

        offset += instr.length;
    }

    std::sort(functionStarts.begin(), functionStarts.end());
    functionStarts.erase(std::unique(functionStarts.begin(), functionStarts.end()), functionStarts.end());

    std::vector<FunctionData> functions;
    for (size_t i = 0; i < functionStarts.size(); ++i) {
        uint64_t funcStart = functionStarts[i];
        uint64_t funcEnd = (i + 1 < functionStarts.size()) ? functionStarts[i + 1] : endAddress_;

        uint64_t localOffset = funcStart - startAddress_;
        
        while (localOffset < funcEnd - startAddress_) {
            ZydisDecodedInstruction instr;
            if (!ZYAN_SUCCESS(ZydisDecoderDecodeInstruction(&decoder, &ctx, code + localOffset, funcEnd - startAddress_ - localOffset, &instr))) {
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

    GCache[startAddress_] = std::move(functions);
}