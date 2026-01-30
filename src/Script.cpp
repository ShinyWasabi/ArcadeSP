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
    enum class eState
    {
        IDLE,
        FADE_OUT,
        CREATE_OBJECTS,
        REQUEST_AUDIO,
        LAUNCH_SCRIPTS,
        FADE_IN,
        READY,
        CLEANUP
    };

    int Location = 0;
    eState State = eState::IDLE;
    Blip Blips[NUM_ARCADE_LOCATIONS] = {};
    Object Objects[NUM_ARCADE_OBJECTS] = {};
} g_Arcade;

void SpoofGlobals()
{
    *getGlobalPtr(g_ScriptData.Glb.Gpbd.Index + (1 + (0 * g_ScriptData.Glb.Gpbd.Size))) = 4; // player state valid
    *getGlobalPtr(g_ScriptData.Glb.Gpbd.Index + (1 + (0 * g_ScriptData.Glb.Gpbd.Size)) + g_ScriptData.Glb.Gpbd.SimpleInteriorData + g_ScriptData.Glb.Gpbd.curSimpleInteriorOwner) = 0;
    *getGlobalPtr(g_ScriptData.Glb.GpbdFm.Index + (1 + (0 * g_ScriptData.Glb.GpbdFm.Size)) + g_ScriptData.Glb.GpbdFm.PropertyData + g_ScriptData.Glb.GpbdFm.ArcData + 1) |= (1 << 2); // arcade setup completed flag
    *getGlobalPtr(g_ScriptData.Glb.GpbdFm.Index + (1 + (0 * g_ScriptData.Glb.GpbdFm.Size)) + g_ScriptData.Glb.GpbdFm.PropertyData + g_ScriptData.Glb.GpbdFm.ArcCabFlags + 0) = -1;    // set all purchased flags
    *getGlobalPtr(g_ScriptData.Glb.GpbdFm.Index + (1 + (0 * g_ScriptData.Glb.GpbdFm.Size)) + g_ScriptData.Glb.GpbdFm.PropertyData + g_ScriptData.Glb.GpbdFm.ArcCabFlags + 1) = -1;    // set all delivered flags
    *getGlobalPtr(g_ScriptData.Glb.MissionObjectFlags) |= (1 << 0);
    *getGlobalPtr(g_ScriptData.Glb.NumReservedMissionObjects) = 80;

    // open management menu
    if (IsKeyJustUp(VK_F9) && SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("am_mp_arc_cab_manager"_J) > 0)
        *getGlobalPtr(g_ScriptData.Glb.AcmData) |= (1 << 1);
}

void UpdateBlips()
{
    if (g_Arcade.State != Arcade::eState::IDLE)
        return;

    for (int i = 0; i < NUM_ARCADE_LOCATIONS; i++)
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

void UpdateMarkers()
{
    if (g_Arcade.State != Arcade::eState::IDLE)
        return;

    for (int i = 0; i < NUM_ARCADE_LOCATIONS; i++)
    {
        int r, g, b, a;
        HUD::GET_HUD_COLOUR(9, &r, &g, &b, &a);
        a = 100;

        auto& coords = g_ArcadeEntryCoords[i];
        GRAPHICS::DRAW_MARKER(1, coords.x, coords.y, coords.z, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, r, g, b, a, FALSE, FALSE, 2, FALSE, NULL, NULL, FALSE);
    }
}

void UpdateEntry()
{
    if (g_Arcade.State != Arcade::eState::IDLE)
        return;

    for (int i = 0; i < NUM_ARCADE_LOCATIONS; i++)
    {
        auto& coords = g_ArcadeEntryCoords[i];
        if (ENTITY::IS_ENTITY_AT_COORD(PLAYER::PLAYER_PED_ID(), coords.x, coords.y, coords.z, 1.0f, 1.0f, 1.0f, FALSE, TRUE, 0))
        {
            g_Arcade.Location = i;
            g_Arcade.State = Arcade::eState::FADE_OUT;
            LOG("Player is in angled area, fading out.");
            break;
        }
    }
}

void FadeOut()
{
    if (g_Arcade.State != Arcade::eState::FADE_OUT)
        return;

    PLAYER::SET_PLAYER_CONTROL(PLAYER::PLAYER_ID(), FALSE, 0);
    ENTITY::FREEZE_ENTITY_POSITION(PLAYER::PLAYER_PED_ID(), TRUE);

    CAM::DO_SCREEN_FADE_OUT(1000);
    while (!CAM::IS_SCREEN_FADED_OUT())
        WAIT(0);

    LOG("Faded out, creating objects.");
    g_Arcade.State = Arcade::eState::CREATE_OBJECTS;
}

void CreateObjects()
{
    if (g_Arcade.State != Arcade::eState::CREATE_OBJECTS)
        return;

    for (int i = 0; i < NUM_ARCADE_OBJECTS; i++)
    {
        auto& object = g_Arcade.Objects[i];
        if (ENTITY::DOES_ENTITY_EXIST(object))
            continue;

        auto& data = g_ArcadeObjects[i];
        while (!STREAMING::HAS_MODEL_LOADED(data.ObjectModel))
        {
            STREAMING::REQUEST_MODEL(data.ObjectModel);
            WAIT(0);
        }

        auto& coords = data.Coords;

        Object objectIndex = OBJECT::CREATE_OBJECT_NO_OFFSET(data.ObjectModel, coords.x, coords.y, coords.z, FALSE, FALSE, FALSE, 0);
        ENTITY::SET_ENTITY_COORDS_NO_OFFSET(objectIndex, coords.x, coords.y, coords.z, FALSE, FALSE, TRUE);
        ENTITY::SET_ENTITY_HEADING(objectIndex, data.Heading);
        ENTITY::FREEZE_ENTITY_POSITION(objectIndex, TRUE);
        ENTITY::SET_ENTITY_INVINCIBLE(objectIndex, TRUE, FALSE);
        ENTITY::SET_ENTITY_CAN_BE_DAMAGED(objectIndex, FALSE);
        STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(data.ObjectModel);

        g_Arcade.Objects[i] = objectIndex;
    }

    LOG("Objects created, requesting audio.");
    g_Arcade.State = Arcade::eState::REQUEST_AUDIO;
}

void RequestAudio()
{
    if (g_Arcade.State != Arcade::eState::REQUEST_AUDIO)
        return;

    bool loaded1 = false, loaded2 = false, loaded3 = false;
    do
    {
        loaded1 = AUDIO::REQUEST_SCRIPT_AUDIO_BANK("DLC_HEIST3/ARCADE_GENERAL_01", FALSE, -1);
        loaded2 = AUDIO::REQUEST_SCRIPT_AUDIO_BANK("DLC_HEIST3/ARCADE_GENERAL_02", FALSE, -1);
        loaded3 = AUDIO::REQUEST_SCRIPT_AUDIO_BANK("DLC_TUNER/DLC_Tuner_Arcade_General", FALSE, -1);
        WAIT(0);
    } while (!loaded1 || !loaded2 || !loaded3);

    LOG("Audio requested, launching scripts.");
    g_Arcade.State = Arcade::eState::LAUNCH_SCRIPTS;
}

void LaunchScripts()
{
    if (g_Arcade.State != Arcade::eState::LAUNCH_SCRIPTS)
        return;

    bool loaded1 = false, loaded2 = false, loaded3 = false, loaded4 = false;
    do
    {
        loaded1 = SCRIPT::HAS_SCRIPT_WITH_NAME_HASH_LOADED("am_mp_arcade_peds"_J);
        loaded2 = SCRIPT::HAS_SCRIPT_WITH_NAME_HASH_LOADED("arcade_seating"_J);
        loaded3 = SCRIPT::HAS_SCRIPT_WITH_NAME_HASH_LOADED("ob_jukebox"_J);
        loaded4 = SCRIPT::HAS_SCRIPT_WITH_NAME_HASH_LOADED("am_mp_arc_cab_manager"_J);
        SCRIPT::REQUEST_SCRIPT_WITH_NAME_HASH("am_mp_arcade"_J);
        SCRIPT::REQUEST_SCRIPT_WITH_NAME_HASH("am_mp_arcade_peds"_J);
        SCRIPT::REQUEST_SCRIPT_WITH_NAME_HASH("arcade_seating"_J);
        SCRIPT::REQUEST_SCRIPT_WITH_NAME_HASH("ob_jukebox"_J);
        SCRIPT::REQUEST_SCRIPT_WITH_NAME_HASH("am_mp_arc_cab_manager"_J);
        WAIT(0);
    } while (!loaded1 || !loaded2 || !loaded3 || !loaded4);

    int64_t pedArgs[3]{};
    int64_t acmArgs[87]{};

    acmArgs[1] = 41;
    acmArgs[43] = "mainw_rm"_J;
    acmArgs[44] = 41;
    for (int i = 0; i < 41; i++)
    {
        acmArgs[45 + i] = static_cast<int64_t>(g_ArcadeCabinets[i]);
        *getGlobalPtr(g_ScriptData.Glb.GpbdFm.Index + (1 + (0 * g_ScriptData.Glb.GpbdFm.Size)) + g_ScriptData.Glb.GpbdFm.PropertyData + g_ScriptData.Glb.GpbdFm.ArcCabSaveSlots + 1 + i) = static_cast<int64_t>(g_ArcadeSaveSlots[i]); // can be overriden by the management menu later
    }

    *getGlobalPtr(g_ScriptData.Glb.Gpbd.Index + (1 + (0 * g_ScriptData.Glb.Gpbd.Size)) + g_ScriptData.Glb.Gpbd.SimpleInteriorData + g_ScriptData.Glb.Gpbd.CurSimpleInterior) = 128 + g_Arcade.Location;
    *getGlobalPtr(g_ScriptData.Glb.GpbdFm.Index + (1 + (0 * g_ScriptData.Glb.GpbdFm.Size)) + g_ScriptData.Glb.GpbdFm.PropertyData + g_ScriptData.Glb.GpbdFm.ArcData + 0) = g_Arcade.Location + 1; // required for arcade_seating tv

    BUILTIN::START_NEW_SCRIPT_WITH_NAME_HASH_AND_ARGS("am_mp_arcade_peds"_J, reinterpret_cast<Any*>(&pedArgs[0]), 3, 5050);
    BUILTIN::START_NEW_SCRIPT_WITH_NAME_HASH("arcade_seating"_J, 5050);
    BUILTIN::START_NEW_SCRIPT_WITH_NAME_HASH("ob_jukebox"_J, 1424);
    BUILTIN::START_NEW_SCRIPT_WITH_NAME_HASH_AND_ARGS("am_mp_arc_cab_manager"_J, reinterpret_cast<Any*>(&acmArgs[0]), 87, 8344);

    SCRIPT::SET_SCRIPT_WITH_NAME_HASH_AS_NO_LONGER_NEEDED("am_mp_arcade_peds"_J);
    SCRIPT::SET_SCRIPT_WITH_NAME_HASH_AS_NO_LONGER_NEEDED("arcade_seating"_J);
    SCRIPT::SET_SCRIPT_WITH_NAME_HASH_AS_NO_LONGER_NEEDED("ob_jukebox"_J);
    SCRIPT::SET_SCRIPT_WITH_NAME_HASH_AS_NO_LONGER_NEEDED("am_mp_arc_cab_manager"_J);

    rage::scrThread::GetThread("arcade_seating"_J)->SetState(rage::scrThread::State::PAUSED); // terminates if the screen is faded out

    LOG("Scripts launched, fading in.");
    g_Arcade.State = Arcade::eState::FADE_IN;
}

void FadeIn()
{
    if (g_Arcade.State != Arcade::eState::FADE_IN)
        return;

    ENTITY::SET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 2737.91f, -374.29f, -48.0f, FALSE, FALSE, FALSE, FALSE);
    ENTITY::SET_ENTITY_HEADING(PLAYER::PLAYER_PED_ID(), 179.55f);
    CAM::SET_GAMEPLAY_CAM_RELATIVE_HEADING(0.0f);
    PLAYER::SET_PLAYER_CONTROL(PLAYER::PLAYER_ID(), TRUE, 0);
    ENTITY::FREEZE_ENTITY_POSITION(PLAYER::PLAYER_PED_ID(), FALSE);

    WAIT(2000);

    CAM::DO_SCREEN_FADE_IN(1000);

    rage::scrThread::GetThread("arcade_seating"_J)->SetState(rage::scrThread::State::RUNNING);

    LOG("Faded in, arcade is ready.");
    g_Arcade.State = Arcade::eState::READY;
}

void UpdatePlayerVisibility()
{
    if (g_Arcade.State != Arcade::eState::READY)
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

void UpdateControls()
{
    if (g_Arcade.State != Arcade::eState::READY)
        return;

    WEAPON::SET_CURRENT_PED_WEAPON(PLAYER::PLAYER_PED_ID(), "WEAPON_UNARMED"_J, TRUE);

    for (auto& control : g_ControlsToDisable)
        PAD::DISABLE_CONTROL_ACTION(0, control, TRUE);
}

void UpdateExit()
{
    if (g_Arcade.State != Arcade::eState::READY)
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

            g_Arcade.State = Arcade::eState::CLEANUP;
        }
    }
    else
    {
        HUD::BEGIN_TEXT_COMMAND_IS_THIS_HELP_MESSAGE_BEING_DISPLAYED("ILH_H_PORT2");
        if (HUD::END_TEXT_COMMAND_IS_THIS_HELP_MESSAGE_BEING_DISPLAYED(0))
            HUD::CLEAR_HELP(TRUE);
    }
}

void Cleanup()
{
    CAM::DO_SCREEN_FADE_OUT(1000);
    while (!CAM::IS_SCREEN_FADED_OUT())
        WAIT(0);

    for (auto& object : g_Arcade.Objects)
    {
        if (ENTITY::DOES_ENTITY_EXIST(object))
        {
            ENTITY::DELETE_ENTITY(&object);
            object = NULL;
        }
    }

    AUDIO::RELEASE_NAMED_SCRIPT_AUDIO_BANK("DLC_HEIST3/ARCADE_GENERAL_01");
    AUDIO::RELEASE_NAMED_SCRIPT_AUDIO_BANK("DLC_HEIST3/ARCADE_GENERAL_02");
    AUDIO::RELEASE_NAMED_SCRIPT_AUDIO_BANK("DLC_TUNER/DLC_Tuner_Arcade_General");

    *getGlobalPtr(g_ScriptData.Glb.AcmData) |= (1 << 0); // kill ACM
    *getGlobalPtr(g_ScriptData.Glb.Gpbd.Index + (1 + (0 * g_ScriptData.Glb.Gpbd.Size)) + g_ScriptData.Glb.Gpbd.SimpleInteriorData + g_ScriptData.Glb.Gpbd.CurSimpleInterior) = -1;
    *getGlobalPtr(g_ScriptData.Glb.GpbdFm.Index + (1 + (0 * g_ScriptData.Glb.GpbdFm.Size)) + g_ScriptData.Glb.GpbdFm.PropertyData + g_ScriptData.Glb.GpbdFm.ArcData + 0) = 0;

    int startTime = MISC::GET_GAME_TIMER();

    bool running1 = true, running2 = true, running3 = true, running4 = true;
    do
    {
        running1 = SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("am_mp_arcade_peds"_J) > 0;
        running2 = SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("arcade_seating"_J) > 0;
        running3 = SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("ob_jukebox"_J) > 0;
        running4 = SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("am_mp_arc_cab_manager"_J) > 0;
        if ((MISC::GET_GAME_TIMER() - startTime) >= 5000)
        {
            MISC::TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("am_mp_arcade_peds");
            MISC::TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("arcade_seating");
            MISC::TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("ob_jukebox");
            MISC::TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("am_mp_arc_cab_manager");
            LOG("Timed out killing scripts, did it manually.");
            break;
        }

        WAIT(0);
    } while (running1 || running2 || running3 || running4);

    *getGlobalPtr(g_ScriptData.Glb.AcmData) = 0;

    auto& coords = g_ArcadeExitCoords[g_Arcade.Location].first;
    auto& heading = g_ArcadeExitCoords[g_Arcade.Location].second;
    ENTITY::SET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), coords.x, coords.y, coords.z, FALSE, FALSE, FALSE, FALSE);
    ENTITY::SET_ENTITY_HEADING(PLAYER::PLAYER_PED_ID(), heading);
    CAM::SET_GAMEPLAY_CAM_RELATIVE_HEADING(0.0f);
    GRAPHICS::CLEAR_TIMECYCLE_MODIFIER(); // fix conflict with simple trainer

    g_Arcade.Location = -1;
    g_Arcade.State = Arcade::eState::IDLE;

    CAM::DO_SCREEN_FADE_IN(1000);

    LOG("Cleaned up script.");
}

void Run()
{
    SpoofGlobals();
    ProcessScriptEvents();

    switch (g_Arcade.State)
    {
    case Arcade::eState::IDLE:
        UpdateBlips();
        UpdateMarkers();
        UpdateEntry();
        break;
    case Arcade::eState::FADE_OUT:
        FadeOut();
        break;
    case Arcade::eState::CREATE_OBJECTS:
        CreateObjects();
        break;
    case Arcade::eState::REQUEST_AUDIO:
        RequestAudio();
        break;
    case Arcade::eState::LAUNCH_SCRIPTS:
        LaunchScripts();
        break;
    case Arcade::eState::FADE_IN:
        FadeIn();
        break;
    case Arcade::eState::READY:
        UpdatePlayerVisibility();
        UpdateControls();
        UpdateExit();
        break;
    case Arcade::eState::CLEANUP:
        Cleanup();
        break;
    }
}

bool Init()
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

    DLC::ON_ENTER_MP();
    MISC::SET_INSTANCE_PRIORITY_MODE(1);

    Interior interior = INTERIOR::GET_INTERIOR_AT_COORDS(2730.0f, -380.0f, -50.0f);
    if (INTERIOR::IS_VALID_INTERIOR(interior))
    {
        for (auto& entitySet : g_ArcadeEntitySets)
            INTERIOR::ACTIVATE_INTERIOR_ENTITY_SET(interior, entitySet);

        INTERIOR::REFRESH_INTERIOR(interior);
    }

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

    if (!Init())
        return;

    while (true)
    {
        Run();
        WAIT(0);
    }
}