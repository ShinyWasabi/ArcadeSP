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

    switch (statHash)
    {
    case "MP0_CAS_HEIST_FLOW"_J:
        outValue->Int |= (1 << 4);
        goto exit;
    case "MP0_AWD_SHARPSHOOTER"_J: // Badlands Revenge II - Sharpshooter
        outValue->Int = 40;
        goto exit;
    case "MP0_AWD_RACECHAMP"_J: // Race and Chase - Race Champion
        outValue->Int = 40;
        goto exit;
    case "MP0_AWD_BATSWORD"_J: // The Wizard's Ruin - Platinum Sword
        outValue->Int = 1000000;
        goto exit;
    case "MP0_AWD_COINPURSE"_J: // The Wizard's Ruin - Platinum Sword - Coin Purse
        outValue->Int = 950000;
        goto exit;
    case "MP0_AWD_ASTROCHIMP"_J: // Space Monkey 3: Bananas Gone Bad - Astrochimp
        outValue->Int = 3000000;
        goto exit;
    case "MP0_AWD_MASTERFUL"_J: // Penetrator - Masterful
        outValue->Int = 40000;
        goto exit;
    case "MP0_SCGW_NUM_WINS_GANG_0"_J: // Street Crimes: Gang Wars Edition - Win 20 games with character 1
    case "MP0_SCGW_NUM_WINS_GANG_1"_J: // Street Crimes: Gang Wars Edition - Win 20 games with character 2
    case "MP0_SCGW_NUM_WINS_GANG_2"_J: // Street Crimes: Gang Wars Edition - Win 20 games with character 3
    case "MP0_SCGW_NUM_WINS_GANG_3"_J: // Street Crimes: Gang Wars Edition - Win 20 games with character 4
        outValue->Int = 20;
        goto exit;
    case "MP0_IAP_MAX_MOON_DIST"_J: // Invade and Persuade II - Travel 3,474,000km on the moon
        outValue->Int = 3474;       // as per IVC_IAP_MOON_DISTANCE_CHALLENGE_REQUIREMENT
        goto exit;
    case "MP0_LAST_ANIMAL"_J: // Invade and Persuade II - Kill 100 animals in a single playthrough
        outValue->Int = 100;
        goto exit;
    case "MP0_CH_ARC_CAB_CLAW_TROPHY"_J: // Kitty Claw Trophy
        outValue->Int = -1;
        goto exit;
    case "MP0_CH_ARC_CAB_LOVE_TROPHY"_J: // The Love Professor Trophy
        outValue->Int = -1;
        goto exit;
    case "MP0_AWD_FACES_OF_DEATH"_J: // Camhedz - Faces Of Death
        outValue->Int = 30;
        goto exit;
    exit:
        ctx->m_ReturnValue->Int = 1;
        return;
    }

    BOOL value = STATS::STAT_GET_INT(statHash, &outValue->Int, p2);
    ctx->m_ReturnValue->Int = value;
}

void StatGetBoolDetour(rage::scrNativeCallContext* ctx)
{
    auto statHash = ctx->m_Args[0].Int;
    auto outValue = ctx->m_Args[1].Reference;
    auto p2 = ctx->m_Args[2].Int;

    switch (statHash)
    {
    case "MP0_AWD_DEADEYE"_J:            // Badlands Revenge II - Dead Eye
    case "MP0_AWD_PISTOLSATDAWN"_J:      // Badlands Revenge II - Pistols At Dawn
    case "MP0_AWD_TRAFFICAVOI"_J:        // Race and Chase - Beat the Traffic
    case "MP0_AWD_CANTCATCHBRA"_J:       // Race and Chase - All Wheels
    case "MP0_AWD_WIZHARD"_J:            // The Wizard's Ruin - Feelin' Grogy
    case "MP0_AWD_APEESCAPE"_J:          // Space Monkey 3: Bananas Gone Bad - Ape Escape
    case "MP0_AWD_MONKEYKIND"_J:         // Space Monkey 3: Bananas Gone Bad - Monkey Mind (I hate this)
    case "MP0_AWD_AQUAAPE"_J:            // Monkey Paradise - Aquatic Ape
    case "MP0_AWD_KEEPFAITH"_J:          // Defender of the Faith - Keeping The Faith
    case "MP0_AWD_TRUELOVE"_J:           // The Love Professor - True Love
    case "MP0_AWD_NEMESIS"_J:            // The Love Professor - Nemesis
    case "MP0_AWD_FRIENDZONED"_J:        // The Love Professor - Friendzoned
    case "MP0_SCGW_WON_NO_DEATHS"_J:     // Street Crimes: Gang Wars Edition - Win a game without taking any damage
    case "MP0_IAP_CHALLENGE_0"_J:        // Invade and Persuade II - Score over 2,000,000 in a single playthrough
    case "MP0_IAP_CHALLENGE_1"_J:        // Invade and Persuade II - Collect 88 barrels in a single playthrough
    case "MP0_IAP_CHALLENGE_2"_J:        // Invade and Persuade II - Kill 100 animals in a single playthrough
    case "MP0_IAP_CHALLENGE_3"_J:        // Invade and Persuade II - Travel 3,474,000km on the moon
    case "MP0_IAP_CHALLENGE_4"_J:        // Invade and Persuade II - Finish any level of Invade and persuade with over 7 lives
    case "MP0_AWD_KINGOFQUB3D"_J:        // QUB3D - King Of QUB3D
    case "MP0_AWD_QUBISM"_J:             // QUB3D - Qubism
    case "MP0_AWD_GODOFQUB3D"_J:         // QUB3D - God Of QUB3D
    case "MP0_AWD_QUIBITS"_J:            // QUB3D - Qubits
    case "MP0_AWD_ELEVENELEVEN"_J:       // Axe Of Fury - 11 11
    case "MP0_AWD_GOFOR11TH"_J:          // Axe Of Fury - Crank It To 11
    case "MP0_AWD_STRAIGHT_TO_VIDEO"_J:  // Camhedz - Straight To Video
    case "MP0_AWD_MONKEY_C_MONKEY_DO"_J: // Camhedz - Monkey See Monkey Do
    case "MP0_AWD_TRAINED_TO_KILL"_J:    // Camhedz - Trained to Kill
    case "MP0_AWD_DIRECTOR"_J:           // Camhedz - The Director
        outValue->Int = 1;
        ctx->m_ReturnValue->Int = 1;
        return;
    }

    BOOL value = STATS::STAT_GET_BOOL(statHash, &outValue->Int, p2);
    ctx->m_ReturnValue->Int = value;
}

void GetPackedStatBoolCodeDetour(rage::scrNativeCallContext* ctx)
{
    auto index = ctx->m_Args[0].Int;
    auto characterSlot = ctx->m_Args[1].Int;

    switch (index)
    {
    case 27184: // Oil Barrels Trophy
    case 27185: // Oil Barrels Trophy
    case 27186: // Oil Barrels Trophy
    case 27187: // Oil Barrels Trophy
    case 27188: // Oil Barrels Trophy
    case 27189: // Spray Paint Trophy
    case 27190: // Spray Paint Trophy
    case 27191: // Spray Paint Trophy
    case 27192: // Spray Paint Trophy
    case 27193: // Spray Paint Trophy
    case 27247: // Madam Nazar Trophy
    case 28176: // Plushie Grindy Tee (incl. Arcade & Penthouse Decoration)
    case 28177: // Plushie Saki Tee (incl. Arcade & Penthouse Decoration)
    case 28178: // Plushie Humpy Tee (incl. Arcade & Penthouse Decoration)
    case 28179: // Plushie Smoker Tee (incl. Arcade & Penthouse Decoration)
    case 28180: // Plushie Poopie Tee (incl. Arcade & Penthouse Decoration)
    case 28181: // Plushie Muffy Tee (incl. Arcade & Penthouse Decoration)
    case 28182: // Plushie Wasabi Kitty Tee (incl. Arcade & Penthouse Decoration)
    case 28183: // Plushie Princess Tee (incl. Arcade & Penthouse Decoration)
    case 28184: // Plushie Master Tee (incl. Arcade & Penthouse Decoration)
    case 28253: // Grog Mode
        ctx->m_ReturnValue->Int = 1;
        return;
    }

    BOOL value = STATS::GET_PACKED_STAT_BOOL_CODE(index, characterSlot);
    ctx->m_ReturnValue->Int = value;
}

void NetworkCreateSynchronizedSceneDetour(rage::scrNativeCallContext* ctx)
{
    auto x = ctx->m_Args[0].Float;
    auto y = ctx->m_Args[1].Float;
    auto z = ctx->m_Args[2].Float;
    auto xRot = ctx->m_Args[3].Float;
    auto yRot = ctx->m_Args[4].Float;
    auto zRot = ctx->m_Args[5].Float;
    auto RotOrder = ctx->m_Args[6].Int;
    auto holdLastFrame = ctx->m_Args[7].Int;
    auto looped = ctx->m_Args[8].Int;
    auto phaseToStopScene = ctx->m_Args[9].Float; // no equivalent for this?
    auto phaseToStartScene = ctx->m_Args[10].Float;
    auto startRate = ctx->m_Args[11].Float;

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

uint32_t GetTotalCashStatHash()
{
    Hash model = ENTITY::GET_ENTITY_MODEL(PLAYER::PLAYER_PED_ID());
    return model == "player_zero"_J ? "SP0_TOTAL_CASH"_J : model == "player_one"_J ? "SP1_TOTAL_CASH"_J : model == "player_two"_J   ? "SP2_TOTAL_CASH"_J : 0;
}

void NetworkCanSpendMoneyDetour(rage::scrNativeCallContext* ctx)
{
    auto amount = ctx->m_Args[0].Int;

    uint32_t statHash = GetTotalCashStatHash();

    int value = 0;
    if (STATS::STAT_GET_INT(statHash, &value, -1))
    {
        ctx->m_ReturnValue->Int = value >= amount;
        return;
    }

    ctx->m_ReturnValue->Int = 0;
}

void NetworkSpentJukeboxDetour(rage::scrNativeCallContext* ctx)
{
    auto amount = ctx->m_Args[0].Int;

    uint32_t statHash = GetTotalCashStatHash();

    int value = 0;
    STATS::STAT_GET_INT(statHash, &value, -1);
    STATS::STAT_SET_INT(statHash, value - amount, TRUE);
}

void GetNumberOfThreadsRunningTheScriptWithThisHashDetour(rage::scrNativeCallContext* ctx)
{
    auto scriptHash = ctx->m_Args[0].Uns;

    if (scriptHash == "am_mp_arcade"_J)
    {
        ctx->m_ReturnValue->Int = 1;
        return;
    }

    BOOL value = SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(scriptHash);
    ctx->m_ReturnValue->Int = value;
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
        return; // already hooked

    program->m_Natives[index.value()] = detour;
}

void InitNativeTablesDetour(rage::scrProgram* program)
{
    g_InitNativeTablesOriginal(program);

    switch (program->m_NameHash)
    {
    case "am_mp_arcade_peds"_J:
    case "arcade_seating"_J:
    case "ob_jukebox"_J:
    case "am_mp_arc_cab_manager"_J:
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
    {
        ApplyHook(program, 0x76CD105BCAC6EB9F, Return1Detour);                                        // NETWORK_IS_GAME_IN_PROGRESS
        ApplyHook(program, 0xAE032CEDCF23C6D5, Return0Detour);                                        // PARTICIPANT_ID_TO_INT
        ApplyHook(program, 0x95C7A22DBE7AEF4C, Return1Detour);                                        // NETWORK_GET_MAX_NUM_PARTICIPANTS
        ApplyHook(program, 0x4470BE79F5771783, Return0Detour);                                        // NETWORK_GET_PLAYER_INDEX
        ApplyHook(program, 0x1C1C92A1CBAE364B, Return0Detour);                                        // NETWORK_GET_PLAYER_INDEX_FROM_PED
        ApplyHook(program, 0x7206AEB20960CCC8, NetworkIsParticipantActiveDetour);                     // NETWORK_IS_PARTICIPANT_ACTIVE
        ApplyHook(program, 0x762604C40829DB72, NetworkIsParticipantActiveDetour);                     // NETWORK_IS_PLAYER_ACTIVE
        ApplyHook(program, 0xCCD470854FB0E643, NetworkIsParticipantActiveDetour);                     // NETWORK_IS_PLAYER_A_PARTICIPANT
        ApplyHook(program, 0x7E3F74F641EE6B27, GetNetworkTimeDetour);                                 // GET_NETWORK_TIME
        ApplyHook(program, 0xDF7F16323520B858, StatGetIntDetour);                                     // STAT_GET_INT
        ApplyHook(program, 0xF249567F2E83E093, StatGetBoolDetour);                                    // STAT_GET_BOOL
        ApplyHook(program, 0xA6D3C21763E25496, GetPackedStatBoolCodeDetour);                          // GET_PACKED_STAT_BOOL_CODE
        ApplyHook(program, 0xBC5D9A293974F095, NetworkCreateSynchronizedSceneDetour);                 // NETWORK_CREATE_SYNCHRONISED_SCENE
        ApplyHook(program, 0x0B94AB707B44E754, NetworkAddPedToSynchronizedSceneDetour);               // NETWORK_ADD_PED_TO_SYNCHRONISED_SCENE
        ApplyHook(program, 0xDEE175A01A05A2F7, NetworkAddEntityToSynchronizedSceneDetour);            // NETWORK_ADD_ENTITY_TO_SYNCHRONISED_SCENE
        ApplyHook(program, 0xE7101255AD6F1952, NullsubDetour);                                        // NETWORK_START_SYNCHRONISED_SCENE
        ApplyHook(program, 0x643DC062EE904FCA, NetworkGetLocalSceneFromNetworkIdDetour);              // NETWORK_GET_LOCAL_SCENE_FROM_NETWORK_ID
        ApplyHook(program, 0xF2E51EC84D76A2B6, NetworkStopSynchronizedSceneDetour);                   // NETWORK_STOP_SYNCHRONISED_SCENE
        ApplyHook(program, 0x78D35ABAF71764AD, Return1Detour);                                        // CAN_REGISTER_MISSION_OBJECTS
        ApplyHook(program, 0xC8D49539708A80B4, Return1Detour);                                        // NETWORK_GET_ENTITY_IS_NETWORKED
        ApplyHook(program, 0xF093E270C0B6B318, Return1Detour);                                        // NETWORK_REQUEST_CONTROL_OF_ENTITY
        ApplyHook(program, 0x1B1A446EFA398EB5, Return1Detour);                                        // NETWORK_HAS_CONTROL_OF_ENTITY
        ApplyHook(program, 0xC18CB5D7A27A2E00, Return0Detour);                                        // NET_GAMESERVER_USE_SERVER_TRANSACTIONS (don't trigger any transactions)
        ApplyHook(program, 0x0AF5E4A6C74DC312, NetworkCanSpendMoneyDetour);                           // NETWORK_CAN_SPEND_MONEY
        ApplyHook(program, 0x4EFA5A2F877A4580, NetworkSpentJukeboxDetour);                            // NETWORK_SPENT_JUKEBOX
        ApplyHook(program, 0x486FF5D06E9659F1, GetNumberOfThreadsRunningTheScriptWithThisHashDetour); // GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH
        break;
    }
    }
}

bool InitHooks()
{
    if (g_IsEnhanced)
    {
        if (auto addr = Memory::ScanPattern("EB 2A 0F 1F 40 00 48 8B 54 17 10"))
            g_InitNativeTables = addr->Sub(0x2A).As<PVOID>();
    }
    else
    {
        if (auto addr = Memory::ScanPattern("48 8D 0D ? ? ? ? 48 8B 14 FA E8 ? ? ? ? 48 85 C0 75 0A")) // same as m_NativeRegistrationTable
            g_InitNativeTables = addr->Sub(0x25).As<PVOID>();
    }

    if (!g_InitNativeTables)
        return false;

    if (MH_Initialize() != MH_OK)
        return false;

    if (MH_CreateHook(g_InitNativeTables, InitNativeTablesDetour, reinterpret_cast<void**>(&g_InitNativeTablesOriginal)) != MH_OK)
        return false;

    if (MH_EnableHook(g_InitNativeTables) != MH_OK)
        return false;

    return true;
}