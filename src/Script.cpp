#include "Script.hpp"
#include "Constants.hpp"
#include "Events.hpp"
#include "Hooks.hpp"
#include "Keyboard.hpp"
#include "ScriptData.hpp"
#include "rage/scrThread.hpp"
#include "rage/scrValue.hpp"
#include <natives.h>

struct Arcade
{
    enum class eLoadState
    {
        IDLE,
        FADE_OUT,
        ACTIVATE_ENTITY_SETS,
        CREATE_OBJECTS,
        CREATE_PEDS,
        REQUEST_AUDIO,
        LAUNCH_ACM_SCRIPT,
        FADE_IN,
        READY
    };

    struct ArcadePed
    {
        Ped PedIndex = NULL;
        Object ObjectIndex = NULL;
    };

    std::vector<Blip> Blips;
    std::vector<Object> Objects;
    std::vector<ArcadePed> Peds;
    eArcadeLocation Location = eArcadeLocation::NONE;
    eLoadState LoadState = eLoadState::IDLE;
} g_Arcade;

void CleanupScript()
{
    Interior interior = INTERIOR::GET_INTERIOR_AT_COORDS(2730.0f, -380.0f, -50.0f);
    if (INTERIOR::IS_VALID_INTERIOR(interior))
    {
        for (auto& set : g_ArcadeEntitySets)
            INTERIOR::DEACTIVATE_INTERIOR_ENTITY_SET(interior, set);

        INTERIOR::REFRESH_INTERIOR(interior);
    }

    for (auto& object : g_Arcade.Objects)
    {
        if (ENTITY::DOES_ENTITY_EXIST(object))
            ENTITY::DELETE_ENTITY(&object);
    }
    g_Arcade.Objects.clear();

    for (auto& ped : g_Arcade.Peds)
    {
        if (ENTITY::DOES_ENTITY_EXIST(ped.PedIndex))
            ENTITY::DELETE_ENTITY(&ped.PedIndex);

        if (ENTITY::DOES_ENTITY_EXIST(ped.ObjectIndex))
            ENTITY::DELETE_ENTITY(&ped.ObjectIndex);
    }
    g_Arcade.Peds.clear();

    AUDIO::RELEASE_NAMED_SCRIPT_AUDIO_BANK("DLC_HEIST3/ARCADE_GENERAL_01");
    AUDIO::RELEASE_NAMED_SCRIPT_AUDIO_BANK("DLC_HEIST3/ARCADE_GENERAL_02");
    AUDIO::RELEASE_NAMED_SCRIPT_AUDIO_BANK("DLC_TUNER/DLC_Tuner_Arcade_General");
    AUDIO::STOP_STREAM();

    *getGlobalPtr(g_ScriptData.Glb.AcmData) = (1 << 0);
    while (SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("am_mp_arc_cab_manager"_J) > 0)
        WAIT(0);

    *getGlobalPtr(g_ScriptData.Glb.AcmData) = 0;

    auto& coords = g_ArcadeMarkerCoords[static_cast<int>(g_Arcade.Location)];
    ENTITY::SET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), coords.x, coords.y, coords.z, FALSE, FALSE, FALSE, FALSE);

    g_Arcade.Location = eArcadeLocation::NONE;
    g_Arcade.LoadState = Arcade::eLoadState::IDLE;
}

void UpdateArcadeEntry()
{
    if (g_Arcade.LoadState != Arcade::eLoadState::IDLE)
        return;

    for (int i = 0; i < static_cast<int>(eArcadeLocation::NUM_LOCATIONS); i++)
    {
        auto& coords1 = g_ArcadeEntryAreaData[i].Coords1;
        auto& coords2 = g_ArcadeEntryAreaData[i].Coords2;
        auto& width = g_ArcadeEntryAreaData[i].Width;

        if (ENTITY::IS_ENTITY_IN_ANGLED_AREA(PLAYER::PLAYER_PED_ID(), coords1.x, coords1.y, coords1.z, coords2.x, coords2.y, coords2.z, width, FALSE, TRUE, 1))
        {
            g_Arcade.Location = static_cast<eArcadeLocation>(i);
            g_Arcade.LoadState = Arcade::eLoadState::FADE_OUT;
            break;
        }
    }
}

void ArcadeLoadStateFadeOut()
{
    if (g_Arcade.LoadState != Arcade::eLoadState::FADE_OUT)
        return;

    PLAYER::SET_PLAYER_CONTROL(PLAYER::PLAYER_ID(), FALSE, 0);
    ENTITY::FREEZE_ENTITY_POSITION(PLAYER::PLAYER_PED_ID(), TRUE);

    CAM::DO_SCREEN_FADE_OUT(1000);
    while (!CAM::IS_SCREEN_FADED_OUT())
        WAIT(0);

    g_Arcade.LoadState = Arcade::eLoadState::ACTIVATE_ENTITY_SETS;
}

void ArcadeLoadStateActivateEntitySets()
{
    if (g_Arcade.LoadState != Arcade::eLoadState::ACTIVATE_ENTITY_SETS)
        return;

    Interior interior = INTERIOR::GET_INTERIOR_AT_COORDS(2730.0f, -380.0f, -50.0f);
    if (INTERIOR::IS_VALID_INTERIOR(interior))
    {
        for (auto& entitySet : g_ArcadeEntitySets)
            INTERIOR::ACTIVATE_INTERIOR_ENTITY_SET(interior, entitySet);

        INTERIOR::REFRESH_INTERIOR(interior);
    }

    g_Arcade.LoadState = Arcade::eLoadState::CREATE_OBJECTS;
}

void ArcadeLoadStateCreateObjects()
{
    if (g_Arcade.LoadState != Arcade::eLoadState::CREATE_OBJECTS)
        return;

    for (auto& placementData : g_ArcadeObjectPlacementData)
    {
        while (!STREAMING::HAS_MODEL_LOADED(placementData.ObjectModel))
        {
            STREAMING::REQUEST_MODEL(placementData.ObjectModel);
            WAIT(0);
        }

        auto& coords = placementData.Coords;

        Object objectIndex = OBJECT::CREATE_OBJECT_NO_OFFSET(placementData.ObjectModel, coords.x, coords.y, coords.z, FALSE, FALSE, FALSE, 0);
        ENTITY::SET_ENTITY_COORDS_NO_OFFSET(objectIndex, coords.x, coords.y, coords.z, FALSE, FALSE, TRUE);
        ENTITY::SET_ENTITY_HEADING(objectIndex, placementData.Heading);
        ENTITY::FREEZE_ENTITY_POSITION(objectIndex, TRUE);
        ENTITY::SET_ENTITY_INVINCIBLE(objectIndex, TRUE, FALSE);
        ENTITY::SET_ENTITY_CAN_BE_DAMAGED(objectIndex, FALSE);
        STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(placementData.ObjectModel);

        g_Arcade.Objects.push_back(objectIndex);
    }

    g_Arcade.LoadState = Arcade::eLoadState::CREATE_PEDS;
}

void ArcadeLoadStateCreatePeds()
{
    if (g_Arcade.LoadState != Arcade::eLoadState::CREATE_PEDS)
        return;

    for (int i = 0; i < NUM_ARCADE_PEDS; i++)
    {
        auto& arcadePed = g_Arcade.Peds[i];
        if (ENTITY::DOES_ENTITY_EXIST(arcadePed.PedIndex))
            continue;

        auto& placementData = g_ArcadePedPlacementData[i];
        auto& actionData = g_ArcadePedActionData[static_cast<int>(placementData.Action)];

        const char* animDict = placementData.Action == eArcadePedAction::WENDY_BAR ? "anim_heist@arcade_property@wendy@bar@" : "anim_heist@arcade_combined@";

        while (!STREAMING::HAS_MODEL_LOADED(placementData.PedModel))
        {
            STREAMING::REQUEST_MODEL(placementData.PedModel);
            WAIT(0);
        }

        while (!STREAMING::HAS_ANIM_DICT_LOADED(animDict))
        {
            STREAMING::REQUEST_ANIM_DICT(animDict);
            WAIT(0);
        }

        auto& coords = placementData.Coords;

        Ped pedIndex = PED::CREATE_PED(26, placementData.PedModel, coords.x, coords.y, coords.z, placementData.Heading, FALSE, FALSE);
        ENTITY::SET_ENTITY_CAN_BE_DAMAGED(pedIndex, FALSE);
        PED::SET_PED_AS_ENEMY(pedIndex, FALSE);
        WEAPON::SET_CURRENT_PED_WEAPON(pedIndex, "WEAPON_UNARMED"_J, TRUE);
        PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(pedIndex, TRUE);
        PED::SET_PED_RESET_FLAG(pedIndex, 249, TRUE);
        PED::SET_PED_CONFIG_FLAG(pedIndex, 185, TRUE);
        PED::SET_PED_CONFIG_FLAG(pedIndex, 108, TRUE);
        PED::SET_PED_CONFIG_FLAG(pedIndex, 106, TRUE);
        PED::SET_PED_CAN_EVASIVE_DIVE(pedIndex, FALSE);
        PED::SET_TREAT_AS_AMBIENT_PED_FOR_DRIVER_LOCKON(pedIndex, TRUE);
        PED::SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(pedIndex, FALSE);
        PED::SET_PED_CAN_RAGDOLL(pedIndex, FALSE);
        PED::SET_PED_CONFIG_FLAG(pedIndex, 208, TRUE);
        TASK::CLEAR_PED_TASKS(pedIndex);

        if (placementData.Action == eArcadePedAction::WENDY_BAR)
        {
            PED::SET_PED_DEFAULT_COMPONENT_VARIATION(pedIndex);
            PED::SET_PED_COMPONENT_VARIATION(pedIndex, 0, 0, 0, 0);
            PED::SET_PED_COMPONENT_VARIATION(pedIndex, 2, 0, 0, 0);
            PED::SET_PED_COMPONENT_VARIATION(pedIndex, 3, 0, 0, 0);
            PED::SET_PED_COMPONENT_VARIATION(pedIndex, 4, 0, 0, 0);
            PED::SET_PED_COMPONENT_VARIATION(pedIndex, 7, 0, 0, 0);
            PED::SET_PED_COMPONENT_VARIATION(pedIndex, 8, 0, 0, 0);
            PED::SET_PED_PROP_INDEX(pedIndex, 0, 0, 0, FALSE, 1);
            PED::SET_PED_PROP_INDEX(pedIndex, 1, 0, 0, FALSE, 1);
        }

        Object objectIndex = NULL;
        if (actionData.ObjectModel != NULL)
        {
            while (!STREAMING::HAS_MODEL_LOADED(actionData.ObjectModel))
            {
                STREAMING::REQUEST_MODEL(actionData.ObjectModel);
                WAIT(0);
            }

            Vector3 boneCoords = PED::GET_PED_BONE_COORDS(pedIndex, actionData.BoneId, 0.0f, 0.0f, 0.0f);
            int boneIndex = PED::GET_PED_BONE_INDEX(pedIndex, actionData.BoneId);

            objectIndex = OBJECT::CREATE_OBJECT(actionData.ObjectModel, boneCoords.x, boneCoords.y, boneCoords.z, FALSE, FALSE, TRUE);
            ENTITY::ATTACH_ENTITY_TO_ENTITY(objectIndex, pedIndex, boneIndex, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, TRUE, TRUE, FALSE, FALSE, 2, TRUE, 0);
        }

        if (actionData.AnimCount > 1)
            PED::SET_PED_ALTERNATE_MOVEMENT_ANIM(pedIndex, 0, animDict, actionData.BaseAnimName, 8.0f, TRUE);

        arcadePed = {pedIndex, objectIndex};
    }

    g_Arcade.LoadState = Arcade::eLoadState::REQUEST_AUDIO;
}

void ArcadeLoadStateRequestAudio()
{
    if (g_Arcade.LoadState != Arcade::eLoadState::REQUEST_AUDIO)
        return;

    while (!AUDIO::LOAD_STREAM("Walla_Normal", "DLC_H3_Arcade_Walla_Sounds"))
        WAIT(0);

    AUDIO::PLAY_STREAM_FROM_POSITION(2729.5894f, -383.9195f, -48.9951f);

    while (!AUDIO::IS_STREAM_PLAYING())
        WAIT(0);

    AUDIO::SET_VARIABLE_ON_STREAM("PedDensity", 1.0f);

    bool loaded1 = false, loaded2 = false, loaded3 = false;
    do
    {
        loaded1 = AUDIO::REQUEST_SCRIPT_AUDIO_BANK("DLC_HEIST3/ARCADE_GENERAL_01", FALSE, -1);
        loaded2 = AUDIO::REQUEST_SCRIPT_AUDIO_BANK("DLC_HEIST3/ARCADE_GENERAL_02", FALSE, -1);
        loaded3 = AUDIO::REQUEST_SCRIPT_AUDIO_BANK("DLC_TUNER/DLC_Tuner_Arcade_General", FALSE, -1);
        WAIT(0);
    } while (!loaded1 || !loaded2 || !loaded3);

    g_Arcade.LoadState = Arcade::eLoadState::LAUNCH_ACM_SCRIPT;
}

void ArcadeLoadStateLaunchACMScript()
{
    if (g_Arcade.LoadState != Arcade::eLoadState::LAUNCH_ACM_SCRIPT)
        return;

    if (SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("am_mp_arc_cab_manager"_J) > 0)
    {
        g_Arcade.LoadState = Arcade::eLoadState::FADE_IN;
        return;
    }

    while (!SCRIPT::HAS_SCRIPT_WITH_NAME_HASH_LOADED("am_mp_arc_cab_manager"_J))
    {
        SCRIPT::REQUEST_SCRIPT_WITH_NAME_HASH("am_mp_arc_cab_manager"_J);
        WAIT(0);
    }

    int64_t args[87]{};

    args[1] = 41;
    args[43] = "mainw_rm"_J;
    args[44] = 41;

    for (int i = 0; i < 41; i++)
    {
        args[45 + i] = static_cast<int64_t>(g_ArcadeCabinetPlacementData[i]);
        *getGlobalPtr(g_ScriptData.Glb.GpbdFm.Index + (1 + (0 * g_ScriptData.Glb.GpbdFm.Size)) + g_ScriptData.Glb.GpbdFm.PropertyData + g_ScriptData.Glb.GpbdFm.ArcCabSaveSlots + 1 + i) = static_cast<int64_t>(g_ArcadeSaveSlotData[i]); // can be overriden by the management menu later
    }

    BUILTIN::START_NEW_SCRIPT_WITH_NAME_HASH_AND_ARGS("am_mp_arc_cab_manager"_J, reinterpret_cast<Any*>(&args[0]), 87, 8344);
    SCRIPT::SET_SCRIPT_WITH_NAME_HASH_AS_NO_LONGER_NEEDED("am_mp_arc_cab_manager"_J);

    g_Arcade.LoadState = Arcade::eLoadState::FADE_IN;
}

void ArcadeLoadStateFadeIn()
{
    if (g_Arcade.LoadState != Arcade::eLoadState::FADE_IN)
        return;

    ENTITY::SET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 2737.91f, -374.29f, -48.0f, FALSE, FALSE, FALSE, FALSE);
    ENTITY::SET_ENTITY_HEADING(PLAYER::PLAYER_PED_ID(), 179.55f);
    PLAYER::SET_PLAYER_CONTROL(PLAYER::PLAYER_ID(), TRUE, 0);
    ENTITY::FREEZE_ENTITY_POSITION(PLAYER::PLAYER_PED_ID(), FALSE);

    CAM::DO_SCREEN_FADE_IN(1000);

    g_Arcade.LoadState = Arcade::eLoadState::READY;
}

void UpdateArcadeLoadState()
{
    switch (g_Arcade.LoadState)
    {
    case Arcade::eLoadState::FADE_OUT:
        ArcadeLoadStateFadeOut();
        break;
    case Arcade::eLoadState::ACTIVATE_ENTITY_SETS:
        ArcadeLoadStateActivateEntitySets();
        break;
    case Arcade::eLoadState::CREATE_OBJECTS:
        ArcadeLoadStateCreateObjects();
        break;
    case Arcade::eLoadState::CREATE_PEDS:
        ArcadeLoadStateCreatePeds();
        break;
    case Arcade::eLoadState::REQUEST_AUDIO:
        ArcadeLoadStateRequestAudio();
        break;
    case Arcade::eLoadState::LAUNCH_ACM_SCRIPT:
        ArcadeLoadStateLaunchACMScript();
        break;
    case Arcade::eLoadState::FADE_IN:
        ArcadeLoadStateFadeIn();
        break;
    }
}

void UpdateArcadePedAnimations()
{
    if (g_Arcade.LoadState != Arcade::eLoadState::READY)
        return;

    for (int i = 0; i < NUM_ARCADE_PEDS; i++)
    {
        auto& arcadePed = g_Arcade.Peds[i];

        if (!ENTITY::DOES_ENTITY_EXIST(arcadePed.PedIndex))
            continue;

        int status = TASK::GET_SCRIPT_TASK_STATUS(arcadePed.PedIndex, "script_task_perform_sequence"_J);
        if (status == 0 || status == 1)
            continue;

        auto& placementData = g_ArcadePedPlacementData[i];
        auto& actionData = g_ArcadePedActionData[static_cast<int>(placementData.Action)];

        const char* animDict = placementData.Action == eArcadePedAction::WENDY_BAR ? "anim_heist@arcade_property@wendy@bar@" : "anim_heist@arcade_combined@";

        int sequence = 0;
        TASK::OPEN_SEQUENCE_TASK(&sequence);

        if (actionData.AnimCount == 1)
        {
            auto& animName = actionData.AnimNames[0];
            float animTime = MISC::GET_RANDOM_FLOAT_IN_RANGE(0.0f, 0.7f);

            if (placementData.Action == eArcadePedAction::MALE_ELBOWS_ON_BAR)
                TASK::TASK_PLAY_ANIM_ADVANCED(NULL, animDict, animName, placementData.Coords.x, placementData.Coords.y, placementData.Coords.z, 0.0f, 0.0f, placementData.Heading, 8.0f, -8.0f, -1, 790529, animTime, 2, 1);
            else
                TASK::TASK_PLAY_ANIM(NULL, animDict, animName, 8.0f, -8.0f, -1, 1, animTime, FALSE, FALSE, FALSE);
        }
        else
        {
            std::vector<int> clipVar;
            clipVar.reserve(actionData.AnimCount);

            for (int i = 0; i < actionData.AnimCount; i++)
                clipVar.push_back(i);

            for (int i = 0; i < 10; i++)
            {
                int value1 = MISC::GET_RANDOM_INT_IN_RANGE(0, actionData.AnimCount);
                int value2 = MISC::GET_RANDOM_INT_IN_RANGE(0, actionData.AnimCount);
                int value3 = clipVar[value1];
                clipVar[value1] = clipVar[value2];
                clipVar[value2] = value3;
            }

            for (int i = 0; i < 4; i++)
            {
                if (clipVar[i] < 0)
                    continue;

                auto& animName = actionData.AnimNames[clipVar[i]];
                TASK::TASK_PLAY_ANIM(NULL, animDict, animName, 8.0f, -8.0f, -1, 0, 0.0f, FALSE, FALSE, FALSE);
            }
        }

        TASK::SET_SEQUENCE_TO_REPEAT(sequence, 1);
        TASK::CLOSE_SEQUENCE_TASK(sequence);
        TASK::TASK_PERFORM_SEQUENCE(arcadePed.PedIndex, sequence);
        TASK::CLEAR_SEQUENCE_TASK(&sequence);
    }
}

void UpdateArcadePlayerVisibility()
{
    if (g_Arcade.LoadState != Arcade::eLoadState::READY)
        return;

    BOOL visible = TRUE;

    // workaround for SET_ENTITY_LOCALLY_INVISIBLE, which doesn't work in SP

    if (*getGlobalPtr(g_ScriptData.Glb.AcmData) & (1 << 1))
    {
        visible = FALSE;
    }
    else
    {
        if (auto thread = rage::scrThread::GetThread("am_mp_arcade_claw_crane"_J))
        {
            if (auto stack = thread->GetStack())
                visible = !CAM::DOES_CAM_EXIST(stack[g_ScriptData.Stc.ArcClawCrane.ClawCraneData + g_ScriptData.Stc.ArcClawCrane.ClawCraneCamera].Int);
        }
    }

    ENTITY::SET_ENTITY_VISIBLE(PLAYER::PLAYER_PED_ID(), visible, FALSE);
}

void UpdateArcadeControls()
{
    if (g_Arcade.LoadState != Arcade::eLoadState::READY)
        return;

    WEAPON::SET_CURRENT_PED_WEAPON(PLAYER::PLAYER_PED_ID(), "WEAPON_UNARMED"_J, TRUE);

    for (auto& control : g_ControlsToDisable)
        PAD::DISABLE_CONTROL_ACTION(0, control, TRUE);
}

void UpdateArcadeExit()
{
    if (g_Arcade.LoadState != Arcade::eLoadState::READY)
        return;

    if (ENTITY::IS_ENTITY_IN_ANGLED_AREA(PLAYER::PLAYER_PED_ID(), 2737.304f, -373.593f, -48.979f, 2738.514f, -373.562f, -46.750f, 1.500f, FALSE, TRUE, 1))
    {
        if (!HUD::IS_HELP_MESSAGE_BEING_DISPLAYED())
        {
            HUD::BEGIN_TEXT_COMMAND_DISPLAY_HELP("ILH_H_PORT2"); // Press ~INPUT_CONTEXT~ to exit.
            HUD::END_TEXT_COMMAND_DISPLAY_HELP(0, TRUE, TRUE, -1);
        }

        if (PAD::IS_CONTROL_JUST_PRESSED(2, eControl::ControlContext))
        {
            HUD::CLEAR_HELP(TRUE);

            CAM::DO_SCREEN_FADE_OUT(1000);
            while (!CAM::IS_SCREEN_FADED_OUT())
                WAIT(0);

            CleanupScript();

            CAM::DO_SCREEN_FADE_IN(1000);
        }
    }
    else
    {
        HUD::BEGIN_TEXT_COMMAND_IS_THIS_HELP_MESSAGE_BEING_DISPLAYED("ILH_H_PORT2");
        if (HUD::END_TEXT_COMMAND_IS_THIS_HELP_MESSAGE_BEING_DISPLAYED(0))
            HUD::CLEAR_HELP(TRUE);
    }
}

void UpdateArcadeBlips()
{
    for (int i = 0; i < static_cast<int>(eArcadeLocation::NUM_LOCATIONS); i++)
    {
        auto& blip = g_Arcade.Blips[i];
        if (HUD::DOES_BLIP_EXIST(blip))
            continue;

        auto& coords = g_ArcadeBlipCoords[i];

        blip = HUD::ADD_BLIP_FOR_COORD(coords.x, coords.y, coords.z);
        HUD::SET_BLIP_ROUTE(blip, FALSE);
        HUD::SET_BLIP_SPRITE(blip, 740);
        HUD::SET_BLIP_SCALE(blip, 1.0f);
        HUD::SET_BLIP_PRIORITY(blip, 7);
        HUD::SET_BLIP_COLOUR(blip, 4);
        HUD::SET_BLIP_AS_SHORT_RANGE(blip, TRUE);
    }
}

void UpdateArcadeMarkers()
{
    if (g_Arcade.LoadState != Arcade::eLoadState::IDLE)
        return;

    for (int i = 0; i < static_cast<int>(eArcadeLocation::NUM_LOCATIONS); i++)
    {
        auto& coords = g_ArcadeMarkerCoords[i];

        int r, g, b, a;
        HUD::GET_HUD_COLOUR(9, &r, &g, &b, &a);
        a = 100;

        GRAPHICS::DRAW_MARKER(1, coords.x, coords.y, coords.z, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, r, g, b, a, FALSE, FALSE, 2, FALSE, NULL, NULL, FALSE);
    }
}

void SpoofGlobals()
{
    *getGlobalPtr(g_ScriptData.Glb.Gpbd.Index + (1 + (0 * g_ScriptData.Glb.Gpbd.Size))) = 4;                                                                                        // player state valid
    *getGlobalPtr(g_ScriptData.Glb.Gpbd.Index + (1 + (0 * g_ScriptData.Glb.Gpbd.Size)) + g_ScriptData.Glb.Gpbd.SimpleInteriorData + g_ScriptData.Glb.Gpbd.CurSimpleInterior) = 133; // SIMPLE_INTERIOR_ARCADE_LA_MESA
    *getGlobalPtr(g_ScriptData.Glb.Gpbd.Index + (1 + (0 * g_ScriptData.Glb.Gpbd.Size)) + g_ScriptData.Glb.Gpbd.SimpleInteriorData + g_ScriptData.Glb.Gpbd.curSimpleInteriorOwner) = 0;
    *getGlobalPtr(g_ScriptData.Glb.GpbdFm.Index + (1 + (0 * g_ScriptData.Glb.GpbdFm.Size)) + g_ScriptData.Glb.GpbdFm.PropertyData + g_ScriptData.Glb.GpbdFm.ArcData + 1) = (1 << 2); // casino scope out completed flag
    *getGlobalPtr(g_ScriptData.Glb.GpbdFm.Index + (1 + (0 * g_ScriptData.Glb.GpbdFm.Size)) + g_ScriptData.Glb.GpbdFm.PropertyData + g_ScriptData.Glb.GpbdFm.ArcCabFlags + 0) = -1;   // set all purchased flags
    *getGlobalPtr(g_ScriptData.Glb.GpbdFm.Index + (1 + (0 * g_ScriptData.Glb.GpbdFm.Size)) + g_ScriptData.Glb.GpbdFm.PropertyData + g_ScriptData.Glb.GpbdFm.ArcCabFlags + 1) = -1;   // set all delivered flags
    *getGlobalPtr(g_ScriptData.Glb.MissionObjectFlags) = (1 << 0);
    *getGlobalPtr(g_ScriptData.Glb.NumReservedMissionObjects) = 80;

    // open management menu
    if (IsKeyJustUp(VK_F9) && SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("am_mp_arc_cab_manager"_J) > 0)
        *getGlobalPtr(g_ScriptData.Glb.AcmData) = (1 << 1);
}

void RunScript()
{
    SpoofGlobals();
    ProcessScriptEvents();

    UpdateArcadeEntry();
    UpdateArcadeLoadState();
    UpdateArcadePedAnimations();
    UpdateArcadePlayerVisibility();
    UpdateArcadeControls();
    UpdateArcadeExit();
    UpdateArcadeMarkers();
    UpdateArcadeBlips();
}

bool InitScript()
{
    if (!InitHooks())
    {
        LOG("Failed to initialize hooks.");
        return false;
    }

    if (!g_ScriptData.Init())
    {
        LOG("Failed to initialize script data.");
        return false;
    }

    MISC::SET_INSTANCE_PRIORITY_MODE(1);
    DLC::ON_ENTER_MP();

    g_Arcade.Blips.reserve(static_cast<size_t>(eArcadeLocation::NUM_LOCATIONS));
    g_Arcade.Objects.reserve(NUM_ARCADE_OBJECTS);
    g_Arcade.Peds.reserve(NUM_ARCADE_PEDS);

    LOG("Script initialized successfully.");
    return true;
}

void ScriptMain()
{
    auto gameVersion = getGameVersion();
    if (gameVersion < eGameVersion::VER_1_0_2944_0)
    {
        LOGF("Unsupported game version: %d", static_cast<int>(gameVersion));
        return;
    }

    if (gameVersion > 1000)
        g_IsEnhanced = true;

    LOGF("Game type is %s. Game version is %d.", g_IsEnhanced ? "Enhanced" : "Legacy", static_cast<int>(gameVersion));

    if (!InitScript())
        return;

    while (true)
    {
        RunScript();
        WAIT(0);
    }
}