#include "scrProgram.hpp"
#include "Memory.hpp"

namespace rage
{
    scrProgram* scrProgram::GetProgram(uint32_t hash)
    {
        static bool init = [] {
            if (auto addr = Memory::ScanPattern("48 C7 84 C8 D8 00 00 00 00 00 00 00"))
                m_Programs = addr->Add(0x13).Add(3).Rip().Add(0xD8).As<decltype(m_Programs)>();

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