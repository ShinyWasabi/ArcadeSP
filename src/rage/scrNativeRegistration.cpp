#include "scrNativeRegistration.hpp"
#include "Memory.hpp"

namespace rage
{
    scrNativeHandler scrNativeRegistration::GetHandler(uint64_t hash)
    {
        static bool init = [] {
            if (g_IsEnhanced)
            {
                if (auto addr = Memory::ScanPattern("4C 8D 0D ? ? ? ? 4C 8D 15 ? ? ? ? 45 31 F6"))
                    m_NativeRegistrationTable = addr->Add(3).Rip().As<decltype(m_NativeRegistrationTable)>();
            }
            else
            {
                if (auto addr = Memory::ScanPattern("48 8D 0D ? ? ? ? 48 8B 14 FA E8 ? ? ? ? 48 85 C0 75 0A")) // Works since 2019 or so
                    m_NativeRegistrationTable = addr->Add(3).Rip().As<decltype(m_NativeRegistrationTable)>();
            }

            return true;
        }();

        if (!m_NativeRegistrationTable)
            return nullptr;

        for (auto node = m_NativeRegistrationTable->m_Nodes[hash & 255]; node; node = node->Next.Decrypt())
        {
            const auto count = node->GetCount();
            for (uint32_t i = 0; i < count; i++)
            {
                if (node->Hashes[i].Decrypt() == hash)
                    return node->Handlers[i];
            }
        }

        return nullptr;
    }
}