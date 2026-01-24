#include "scrThread.hpp"
#include "Memory.hpp"
#include "atArray.hpp"

namespace rage
{
    scrThread* scrThread::GetThread(uint32_t hash)
    {
        static bool init = [] {
            if (g_IsEnhanced)
            {
                if (auto addr = Memory::ScanPattern("48 8B 05 ? ? ? ? 48 89 34 F8 48 FF C7 48 39 FB 75 97"))
                    m_Threads = addr->Add(3).Rip().As<decltype(m_Threads)>();
            }
            else
            {
                if (auto addr = Memory::ScanPattern("45 33 F6 8B E9 85 C9 B8")) // Works since b323
                    m_Threads = addr->Sub(4).Rip().Sub(8).As<decltype(m_Threads)>();
            }

            return true;
        }();

        if (!m_Threads)
            return nullptr;

        for (auto& thread : *m_Threads)
        {
            if (thread && thread->GetProgramHash() == hash)
                return thread;
        }

        return nullptr;
    }
}