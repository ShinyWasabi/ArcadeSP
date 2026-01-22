#pragma once

namespace rage
{
    union scrValue;

    struct scrNativeCallContext
    {
        scrValue* m_ReturnValue;
        uint32_t m_ArgCount;
        scrValue* m_Args;
        char m_Pad1[0x68];
    };
    static_assert(sizeof(scrNativeCallContext) == 0x80);

    using scrNativeHandler = void (*)(scrNativeCallContext*);
}