#include "Hooks.hpp"
#include "Memory.hpp"
#include "rage/scrNativeRegistration.hpp"
#include "rage/scrProgram.hpp"
#include "rage/scrValue.hpp"
#include <MinHook.h>
#include <natives.h>

struct SyncedSceneInfo
{
    std::vector<Ped> Peds;
    std::vector<Entity> Entities;
};

std::unordered_map<int, SyncedSceneInfo> g_Scenes;

void* g_InitNativeTables = nullptr;
void (*g_InitNativeTablesOriginal)(rage::scrProgram* program) = nullptr;

void Return1Detour(rage::scrNativeCallContext* ctx)
{
    ctx->m_ReturnValue->Int = 1;
}

void Return0Detour(rage::scrNativeCallContext* ctx)
{
    ctx->m_ReturnValue->Int = 0;
}

void NullsubDetour(rage::scrNativeCallContext* ctx)
{
}

void NetworkIsParticipantActiveDetour(rage::scrNativeCallContext* ctx)
{
    ctx->m_ReturnValue->Int = ctx->m_Args[0].Int == 0 ? 1 : 0;
}

void GetNetworkTimeDetour(rage::scrNativeCallContext* ctx)
{
    ctx->m_ReturnValue->Int = MISC::GET_GAME_TIMER();
}

void StatGetIntDetour(rage::scrNativeCallContext* ctx)
{
    auto statHash = ctx->m_Args[0].Int;
    auto outValue = ctx->m_Args[1].Reference;
    auto p2 = ctx->m_Args[2].Int;

    if (statHash == "MP0_CAS_HEIST_FLOW"_J)
    {
        outValue->Int |= (1 << 4);
        ctx->m_ReturnValue->Int = 1;
        return;
    }

    BOOL value = STATS::STAT_GET_INT(statHash, &outValue->Int, p2);
    ctx->m_ReturnValue->Int = value;
}

void GetPackedStatBoolCodeDetour(rage::scrNativeCallContext* ctx)
{
    auto index = ctx->m_Args[0].Int;
    auto characterSlot = ctx->m_Args[1].Int;

    // Unlock grog mode
    if (index == 28253)
    {
        ctx->m_ReturnValue->Int = 1;
        return;
    }

    BOOL value = STATS::GET_PACKED_STAT_BOOL_CODE(index, characterSlot);
    ctx->m_ReturnValue->Int = value;
}

void NetworkCreateSynchronizedSceneDetour(rage::scrNativeCallContext* ctx)
{
    float x = ctx->m_Args[0].Float;
    float y = ctx->m_Args[1].Float;
    float z = ctx->m_Args[2].Float;
    float xRot = ctx->m_Args[3].Float;
    float yRot = ctx->m_Args[4].Float;
    float zRot = ctx->m_Args[5].Float;
    int RotOrder = ctx->m_Args[6].Int;
    BOOL holdLastFrame = ctx->m_Args[7].Int;
    BOOL looped = ctx->m_Args[8].Int;
    float phaseToStopScene = ctx->m_Args[9].Float; // no equivalent for this?
    float phaseToStartScene = ctx->m_Args[10].Float;
    float startRate = ctx->m_Args[11].Float;

    int scene = PED::CREATE_SYNCHRONIZED_SCENE(x, y, z, xRot, yRot, zRot, RotOrder);
    PED::SET_SYNCHRONIZED_SCENE_HOLD_LAST_FRAME(scene, holdLastFrame);
    PED::SET_SYNCHRONIZED_SCENE_LOOPED(scene, looped);
    PED::SET_SYNCHRONIZED_SCENE_PHASE(scene, phaseToStartScene);
    PED::SET_SYNCHRONIZED_SCENE_RATE(scene, startRate);
    g_Scenes.emplace(scene, SyncedSceneInfo{});

    ctx->m_ReturnValue->Int = scene;
}

void NetworkAddPedToSynchronizedSceneDetour(rage::scrNativeCallContext* ctx)
{
    auto ped = ctx->m_Args[0].Int;
    auto scene = ctx->m_Args[1].Int;
    auto dict = ctx->m_Args[2].String;
    auto anim = ctx->m_Args[3].String;
    auto blendIn = ctx->m_Args[4].Float;
    auto blendOut = ctx->m_Args[5].Float;
    auto flags = ctx->m_Args[6].Int;
    auto ragdollFlags = ctx->m_Args[7].Int;
    auto moverBlendInDelta = ctx->m_Args[8].Float;
    auto ikFlags = ctx->m_Args[9].Int;

    TASK::TASK_SYNCHRONIZED_SCENE(ped, scene, dict, anim, blendIn, blendOut, flags, ragdollFlags, moverBlendInDelta, ikFlags);
    g_Scenes[scene].Peds.push_back(ped);
}

void NetworkAddEntityToSynchronizedSceneDetour(rage::scrNativeCallContext* ctx)
{
    auto entity = ctx->m_Args[0].Int;
    auto scene = ctx->m_Args[1].Int;
    auto dict = ctx->m_Args[2].String;
    auto anim = ctx->m_Args[3].String;
    auto blendIn = ctx->m_Args[4].Float;
    auto blendOut = ctx->m_Args[5].Float;
    auto flags = ctx->m_Args[6].Int;

    ENTITY::PLAY_SYNCHRONIZED_ENTITY_ANIM(entity, scene, anim, dict, blendIn, blendOut, flags, 1000.0f);
    g_Scenes[scene].Entities.push_back(entity);
}

void NetworkGetLocalSceneFromNetworkIdDetour(rage::scrNativeCallContext* ctx)
{
    ctx->m_ReturnValue->Int = ctx->m_Args[0].Int;
}

void NetworkStopSynchronizedSceneDetour(rage::scrNativeCallContext* ctx)
{
    auto scene = ctx->m_Args[0].Int;

    auto it = g_Scenes.find(scene);
    if (it == g_Scenes.end())
        return;

    auto& info = it->second;

    for (auto ped : info.Peds)
    {
        if (ENTITY::DOES_ENTITY_EXIST(ped))
            TASK::CLEAR_PED_TASKS_IMMEDIATELY(ped);
    }

    for (auto ent : info.Entities)
    {
        if (ENTITY::DOES_ENTITY_EXIST(ent))
            ENTITY::STOP_SYNCHRONIZED_ENTITY_ANIM(ent, -4.0f, TRUE);
    }

    g_Scenes.erase(it);
}

void ApplyHook(rage::scrProgram* program, uint64_t hash, rage::scrNativeHandler detour)
{
    if (!program)
        return;

    auto handler = rage::scrNativeRegistration::GetHandler(hash);
    if (!handler)
        return;

    auto index = program->GetNativeIndex(handler);
    if (!index.has_value())
        return; // Already hooked

    program->m_Natives[index.value()] = detour;
}

void InitNativeTablesDetour(rage::scrProgram* program)
{
    g_InitNativeTablesOriginal(program);

    switch (program->m_NameHash)
    {
    case "ggsm_arcade"_J:
    case "gunslinger_arcade"_J:
    case "wizard_arcade"_J:
    case "road_arcade"_J:
    case "degenatron_games"_J:
    case "scroll_arcade_cabinet"_J:
    case "grid_arcade_cabinet"_J:
    case "am_mp_arcade_fortune_teller"_J:
    case "am_mp_arcade_claw_crane"_J:
    case "am_mp_arcade_love_meter"_J:
    case "am_mp_arcade_strength_test"_J:
    case "puzzle"_J:
    case "camhedz_arcade"_J:
    case "am_mp_arc_cab_manager"_J:
    {
        ApplyHook(program, 0x76CD105BCAC6EB9F, Return1Detour);                    // NETWORK_IS_GAME_IN_PROGRESS
        ApplyHook(program, 0xAE032CEDCF23C6D5, Return0Detour);                    // PARTICIPANT_ID_TO_INT
        ApplyHook(program, 0x95C7A22DBE7AEF4C, Return1Detour);                    // NETWORK_GET_MAX_NUM_PARTICIPANTS
        ApplyHook(program, 0x4470BE79F5771783, Return0Detour);                    // NETWORK_GET_PLAYER_INDEX
        ApplyHook(program, 0x7206AEB20960CCC8, NetworkIsParticipantActiveDetour); // NETWORK_IS_PARTICIPANT_ACTIVE
        ApplyHook(program, 0x762604C40829DB72, NetworkIsParticipantActiveDetour); // NETWORK_IS_PLAYER_ACTIVE
        ApplyHook(program, 0xCCD470854FB0E643, NetworkIsParticipantActiveDetour); // NETWORK_IS_PLAYER_A_PARTICIPANT
        ApplyHook(program, 0x7E3F74F641EE6B27, GetNetworkTimeDetour);
        ApplyHook(program, 0xDF7F16323520B858, StatGetIntDetour);
        ApplyHook(program, 0xA6D3C21763E25496, GetPackedStatBoolCodeDetour);
        ApplyHook(program, 0xBC5D9A293974F095, NetworkCreateSynchronizedSceneDetour);
        ApplyHook(program, 0xB94AB707B44E754, NetworkAddPedToSynchronizedSceneDetour);
        ApplyHook(program, 0xDEE175A01A05A2F7, NetworkAddEntityToSynchronizedSceneDetour);
        ApplyHook(program, 0xE7101255AD6F1952, NullsubDetour); // NETWORK_START_SYNCHRONISED_SCENE
        ApplyHook(program, 0x643DC062EE904FCA, NetworkGetLocalSceneFromNetworkIdDetour);
        ApplyHook(program, 0xF2E51EC84D76A2B6, NetworkStopSynchronizedSceneDetour); // NETWORK_STOP_SYNCHRONISED_SCENE
        ApplyHook(program, 0x78D35ABAF71764AD, Return1Detour);                      // CAN_REGISTER_MISSION_OBJECTS
        ApplyHook(program, 0xC8D49539708A80B4, Return1Detour);                      // NETWORK_GET_ENTITY_IS_NETWORKED
        ApplyHook(program, 0xF093E270C0B6B318, Return1Detour);                      // NETWORK_REQUEST_CONTROL_OF_ENTITY
        ApplyHook(program, 0x1B1A446EFA398EB5, Return1Detour);                      // NETWORK_HAS_CONTROL_OF_ENTITY
        break;
    }
    }
}

void InitHooks()
{
    if (auto addr = Memory::ScanPattern("EB 2A 0F 1F 40 00 48 8B 54 17 10"))
        g_InitNativeTables = addr->Sub(0x2A).As<PVOID>();

    if (g_InitNativeTables == nullptr)
        return;

    if (MH_Initialize() != MH_OK)
        return;

    MH_CreateHook(g_InitNativeTables, InitNativeTablesDetour, reinterpret_cast<void**>(&g_InitNativeTablesOriginal));
    MH_EnableHook(g_InitNativeTables);
}