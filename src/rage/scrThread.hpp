#pragma once

namespace rage
{
    template <typename T>
    class atArray;
    union scrValue;

    class scrThread
    {
    public:
        enum class State : uint32_t
        {
            RUNNING,
            IDLE,
            KILLED,
            PAUSED
        };

        enum class Priority : uint32_t
        {
            HIGHEST,
            NORMAL,
            LOWEST,
            MANUAL_UPDATE = 100
        };

        uint32_t GetProgramHash() const
        {
            return g_IsEnhanced ? reinterpret_cast<const scrThread_GEN9*>(this)->m_Context.m_ProgramHash : reinterpret_cast<const scrThread_GEN8*>(this)->m_Context.m_ProgramHash;
        }

        State GetState() const
        {
            return g_IsEnhanced ? reinterpret_cast<const scrThread_GEN9*>(this)->m_Context.m_State : reinterpret_cast<const scrThread_GEN8*>(this)->m_Context.m_State;
        }

        void SetState(State state)
        {
            g_IsEnhanced ? reinterpret_cast<scrThread_GEN9*>(this)->m_Context.m_State = state : reinterpret_cast<scrThread_GEN8*>(this)->m_Context.m_State = state;
        }

        scrValue* GetStack() const
        {
            return g_IsEnhanced ? reinterpret_cast<const scrThread_GEN9*>(this)->m_Stack : reinterpret_cast<const scrThread_GEN8*>(this)->m_Stack;
        }

        static scrThread* GetThread(uint32_t hash);

    private:
        struct scrThread_GEN8
        {
            struct Context
            {
                uint32_t m_Id;
                uint32_t m_ProgramHash;
                State m_State;
                uint32_t m_ProgramCounter;
                uint32_t m_FramePointer;
                uint32_t m_StackPointer;
                float m_TimerA;
                float m_TimerB;
                float m_WaitTimer;
                char m_Pad1[0x2C];
                uint32_t m_StackSize;
                uint32_t m_CatchProgramCounter;
                uint32_t m_CatchFramePointer;
                uint32_t m_CatchStackPointer;
                Priority m_Priority;
                uint8_t m_CallDepth;
                uint32_t m_Callstack[16];
            };
            static_assert(sizeof(Context) == 0xA8);

            virtual ~scrThread_GEN8() = default;
            virtual void Reset(uint32_t programHash, void* args, uint32_t argCount) = 0;
            virtual State Run() = 0;
            virtual State Update() = 0;
            virtual void Kill() = 0;

            Context m_Context;
            scrValue* m_Stack;
            char m_Pad2[0x04];
            uint32_t m_ArgSize;
            uint32_t m_ArgLoc;
            char m_Pad3[0x04];
            const char* m_ErrorMessage;
            uint32_t m_ScriptHash;
            char m_ScriptName[64];
        };
        static_assert(sizeof(scrThread_GEN8) == 0x118);

        struct scrThread_GEN9
        {
            struct Context
            {
                uint32_t m_Id;
                char m_Pad1[0x04];
                uint32_t m_ProgramHash;
                char m_Pad2[0x04];
                State m_State;
                uint32_t m_ProgramCounter;
                uint32_t m_FramePointer;
                uint32_t m_StackPointer;
                float m_TimerA;
                float m_TimerB;
                float m_WaitTimer;
                char m_Pad3[0x2C];
                uint32_t m_StackSize;
                uint32_t m_CatchProgramCounter;
                uint32_t m_CatchFramePointer;
                uint32_t m_CatchStackPointer;
                Priority m_Priority;
                uint8_t m_CallDepth;
                uint32_t m_Callstack[16];
            };
            static_assert(sizeof(Context) == 0xB0);

            virtual ~scrThread_GEN9() = default;
            virtual void Reset(uint32_t programHash, void* args, uint32_t argCount) = 0;
            virtual State Run() = 0;
            virtual State Update() = 0;
            virtual void Kill() = 0;
            virtual void GetInfo(void* info) = 0;

            Context m_Context;
            scrValue* m_Stack;
            char m_Pad2[0x04];
            uint32_t m_ArgSize;
            uint32_t m_ArgLoc;
            char m_Pad3[0x04];
            char m_ErrorMessage[128];
            uint32_t m_ScriptHash;
            char m_ScriptName[64];
        };
        static_assert(sizeof(scrThread_GEN9) == 0x198);

        static inline atArray<scrThread*>* m_Threads;
    };
}