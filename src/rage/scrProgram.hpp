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

        std::optional<uint32_t> GetNativeIndex(scrNativeHandler handler) const
        {
            for (uint32_t i = 0; i < m_NativeCount; ++i)
            {
                if (m_Natives[i] == handler)
                    return i;
            }

            return std::nullopt;
        }

        static scrProgram* GetProgram(uint32_t hash);

        static inline scrProgram** m_Programs;
    };
    static_assert(sizeof(scrProgram) == 0x80);
}