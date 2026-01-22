#pragma once
#include "scrNativeCallContext.hpp"

namespace rage
{
    class scrNativeRegistration
    {
    public:
        struct RegistrationNode
        {
            template <typename T>
            struct NodeEnc
            {
                uint32_t A;
                uint32_t B;
                uint32_t C;
                char Pad[0x04];

                T Decrypt() const
                {
                    const auto nonce = reinterpret_cast<uintptr_t>(this) ^ C;
                    const auto value = static_cast<uint32_t>(nonce ^ A) | (nonce ^ B) << 32U;
                    if constexpr (std::is_integral_v<T>)
                        return value;
                    else
                        return reinterpret_cast<T>(value);
                }
            };

            NodeEnc<RegistrationNode*> Next;
            scrNativeHandler Handlers[7];
            uint32_t numEntries1;
            uint32_t numEntries2;
            char Pad[0x04];
            NodeEnc<uint64_t> Hashes[7];

            uint32_t GetCount() const
            {
                return numEntries1 ^ numEntries2 ^ static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&numEntries1));
            }
        };

        static scrNativeHandler GetHandler(uint64_t hash);

        RegistrationNode* m_Nodes[256];

        static inline scrNativeRegistration* m_NativeRegistrationTable;
    };
}