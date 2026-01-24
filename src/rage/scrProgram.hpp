#pragma once
#include "scrNativeCalLContext.hpp"

namespace rage
{
    union scrValue;

    class scrProgram
    {
    public:
        char m_Pad1[0x10];
        uint8_t** m_CodeBlocks;
        uint32_t m_GlobalVersion;
        uint32_t m_CodeSize;
        uint32_t m_ArgCount;
        uint32_t m_StaticCount;
        uint32_t m_GlobalCount;
        uint32_t m_NativeCount;
        scrValue* m_Statics;
        scrValue** m_Globals;
        scrNativeHandler* m_Natives;
        uint32_t m_ProcCount;
        char m_Pad2[0x04];
        const char** m_ProcNames;
        uint32_t m_NameHash;
        uint32_t m_RefCount;
        const char* m_Name;
        const char** m_Strings;
        uint32_t m_StringsCount;
        char m_Pad3[0x0C];

        uint8_t* GetCode(uint32_t address) const
        {
            if (address < m_CodeSize)
                return &m_CodeBlocks[address >> 14][address & 0x3FFF];

            return nullptr;
        }

        std::optional<uint32_t> GetNativeIndex(scrNativeHandler handler) const
        {
            for (uint32_t i = 0; i < m_NativeCount; ++i)
            {
                if (m_Natives[i] == handler)
                    return i;
            }

            return std::nullopt;
        }

        std::optional<uint32_t> ScanPattern(const char* pattern) const;

        static scrProgram* GetProgram(uint32_t hash);

        static inline scrProgram** m_Programs;
    };
    static_assert(sizeof(scrProgram) == 0x80);
}