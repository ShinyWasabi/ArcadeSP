#include "scrProgram.hpp"
#include "Memory.hpp"

namespace rage
{
    std::optional<uint32_t> scrProgram::ScanPattern(const char* pattern) const
    {
        auto parsed = Memory::ParsePattern(pattern);

        for (uint32_t i = 0; i < (m_CodeSize - parsed.size()); i++)
        {
            for (uint32_t j = 0; j < parsed.size(); j++)
            {
                if (parsed[j].has_value())
                {
                    if (parsed[j].value() != *GetCode(i + j))
                        goto incorrect;
                }
            }

            return i;

        incorrect:
            continue;
        }

        return std::nullopt;
    }

    scrProgram* scrProgram::GetProgram(uint32_t hash)
    {
        static bool init = [] {
            if (g_IsEnhanced)
            {
                if (auto addr = Memory::ScanPattern("48 C7 84 C8 D8 00 00 00 00 00 00 00"))
                    m_Programs = addr->Add(0x13).Add(3).Rip().Add(0xD8).As<decltype(m_Programs)>();
            }
            else
            {
                if (auto addr = Memory::ScanPattern("48 8D 0D ? ? ? ? E8 ? ? ? ? 33 FF 48 85 C0 74")) // Works since b323
                    m_Programs = addr->Add(3).Rip().Add(0xD8).As<decltype(m_Programs)>();
            }

            return true;
        }();

        if (!m_Programs)
            return nullptr;

        for (int i = 0; i < 176; i++)
        {
            if (m_Programs[i] && m_Programs[i]->m_NameHash == hash)
                return m_Programs[i];
        }

        return nullptr;
    }
}