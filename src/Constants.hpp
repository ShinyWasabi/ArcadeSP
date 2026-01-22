#pragma once

constexpr int NUM_ARCADE_OBJECTS = 6;
constexpr int NUM_ARCADE_PEDS = 14;

enum class eArcadeLocation
{
    NONE = -1,
    PALETO_BAY,
    GRAPESEED,
    DAVIS,
    WEST_VINEWOOD,
    ROCKFORD_HILLS,
    LA_MESA,
    NUM_LOCATIONS
};

enum class eArcadePedAction
{
    WENDY_BAR,
    HANG_OUT,
    MALE_STAND_WITHDRINK,
    MALE_STAND_WITHDRINK_2,
    FEMALE_STAND,
    FEMALE_STAND_2,
    FEMALE_STAND_IDLE,
    FEMALE_STAND_WITHDRINK,
    MALE_STANDING_CALL,
    MALE_STAND_MOBILE_TEXT,
    MALE_ELBOWS_ON_BAR
};

enum class eArcadeCabinet
{
    INVALID = -1,
    GG_SPACE_MONKEY,
    BADLANDS_REVENGE,
    WIZARDS_RUIN,
    RACE_N_CHASE_STREET,
    RACE_N_CHASE_TRUCKIN,
    RACE_N_CHASE_ROCKET,
    DEFENDER_OF_THE_FAITH,
    MONKEYS_PARADISE,
    PENETRATOR,
    INVADE_AND_PERSUADE,
    STREET_CRIMES,
    NAZAR_SPEAKS,
    SWK_CLAW,
    LOVE_PROFESSOR,
    AXE_OF_FURY,
    QUB3D,
    CAMHEDZ,
    NUM_CABINETS
};

enum class eArcadeSaveSlot
{
    INVALID,
    STREET_CRIMES_0,
    STREET_CRIMES_1,
    STREET_CRIMES_2,
    STREET_CRIMES_3,
    GG_SPACE_MONKEY,
    BADLANDS_REVENGE,
    RACE_N_CHASE_ROCKET,
    RACE_N_CHASE_STREET,
    RACE_N_CHASE_TRUCKIN,
    WIZARDS_RUIN,
    DEFENDER_OF_THE_FAITH,
    MONKEYS_PARADISE,
    PENETRATOR,
    SWK_CLAW,
    NAZAR_SPEAKS,
    LOVE_PROFESSOR,
    INVADE_AND_PERSUADE,
    AXE_OF_FURY = 21,
    QUB3D,
    CAMHEDZ,
    NUM_SAVE_SLOTS
};

struct ArcadeObjectPlacementData
{
    Hash ObjectModel;
    Vector3 Coords;
    float Heading;
};

struct ArcadePedPlacementData
{
    Hash PedModel;
    eArcadePedAction Action;
    Vector3 Coords;
    float Heading;
};

struct ArcadeEntryAreaData
{
    Vector3 Coords1;
    Vector3 Coords2;
    float Width;
};

struct ArcadePedActionData
{
    Hash ObjectModel;
    int BoneId;
    int AnimCount;
    const char* const* AnimNames;
    const char* BaseAnimName;
};

constexpr const char* g_ArcadeEntitySets[] = {
    "entity_set_constant_geometry",
    "entity_set_arcade_set_ceiling_beams",
    "entity_set_mural_option_07",
    "entity_set_floor_option_05",
    "entity_set_mural_neon_option_03",
    "entity_set_arcade_set_trophy_claw",
    "entity_set_arcade_set_trophy_love",
    "entity_set_arcade_set_trophy_teller",
    "entity_set_arcade_set_trophy_gunner",
    "entity_set_arcade_set_trophy_king",
    "entity_set_arcade_set_trophy_racer",
    "entity_set_arcade_set_trophy_patriot",
    "entity_set_arcade_set_trophy_monkey",
    "entity_set_arcade_set_trophy_brawler",
    "entity_set_arcade_set_trophy_retro",
    "entity_set_arcade_set_trophy_cabs",
    "entity_set_arcade_set_trophy_strife",
    "entity_set_plushie_01",
    "entity_set_plushie_02",
    "entity_set_plushie_03",
    "entity_set_plushie_04",
    "entity_set_plushie_05",
    "entity_set_plushie_06",
    "entity_set_plushie_07",
    "entity_set_plushie_08",
    "entity_set_plushie_09",
    "entity_set_big_screen",
    "entity_set_screens"};

constexpr eControl g_ControlsToDisable[] = {
    eControl::ControlWeaponWheelNext,
    eControl::ControlWeaponWheelPrev,
    eControl::ControlSelectNextWeapon,
    eControl::ControlSelectPrevWeapon,
    eControl::ControlJump,
    eControl::ControlAttack,
    eControl::ControlAim,
    eControl::ControlDuck,
    eControl::ControlSelectWeapon,
    eControl::ControlCover,
    eControl::ControlTalk,
    eControl::ControlDetonate,
    eControl::ControlWeaponSpecial,
    eControl::ControlWeaponSpecial2,
    eControl::ControlMeleeAttackLight,
    eControl::ControlMeleeAttackHeavy,
    eControl::ControlMeleeAttackAlternate,
    eControl::ControlMeleeBlock,
    eControl::ControlSelectWeaponUnarmed,
    eControl::ControlSelectWeaponMelee,
    eControl::ControlSelectWeaponHandgun,
    eControl::ControlSelectWeaponShotgun,
    eControl::ControlSelectWeaponSmg,
    eControl::ControlSelectWeaponAutoRifle,
    eControl::ControlSelectWeaponSniper,
    eControl::ControlSelectWeaponHeavy,
    eControl::ControlSelectWeaponSpecial};

constexpr Vector3 g_ArcadeBlipCoords[] = {
    {-247.6898f, 6212.915f, 30.944f},
    {1695.1714f, 4785.1177f, 40.9847f},
    {-116.3816f, -1772.1368f, 28.8592f},
    {-599.5152f, 279.6308f, 81.074f},
    {-1273.2231f, -304.1054f, 37.2289f},
    {758.3455f, -815.9312f, 25.2905f}};

constexpr Vector3 g_ArcadeMarkerCoords[] = {
    {-245.64f, 6210.96f, 30.94f},
    {1695.85f, 4783.85f, 41.02f},
    {-115.17f, -1771.64f, 28.86f},
    {-601.11f, 280.47f, 81.04f},
    {-1269.72f, -304.09f, 36.0f},
    {758.46f, -814.57f, 25.3f}};

constexpr ArcadeEntryAreaData g_ArcadeEntryAreaData[] = {
    {{-247.65437f, 6214.0815f, 30.94425f},
        {-246.62204f, 6213.045f, 33.56925f},
        1.4375f},
    {{1696.583f, 4786.322f, 41.07819f},
        {1696.583f, 4784.135f, 43.68791f},
        1.9375f},
    {{-114.92545f, -1772.4277f, 28.903496f},
        {-115.87838f, -1773.5585f, 31.45869f},
        2.0625f},
    {{-601.77313f, 278.98578f, 80.95472f},
        {-601.70166f, 280.0712f, 83.01721f},
        1.6875f},
    {{-1271.3613f, -305.91278f, 35.944458f},
        {-1270.6467f, -303.98718f, 38.77862f},
        1.9375f},
    {{758.11774f, -816.8457f, 25.270824f},
        {758.0814f, -815.3637f, 28.145824f},
        1.8125f}};

constexpr eArcadeCabinet g_ArcadeCabinetPlacementData[] = {
    eArcadeCabinet::SWK_CLAW,
    eArcadeCabinet::AXE_OF_FURY,
    eArcadeCabinet::LOVE_PROFESSOR,
    eArcadeCabinet::STREET_CRIMES,
    eArcadeCabinet::STREET_CRIMES,
    eArcadeCabinet::STREET_CRIMES,
    eArcadeCabinet::STREET_CRIMES,
    eArcadeCabinet::BADLANDS_REVENGE,
    eArcadeCabinet::INVALID,
    eArcadeCabinet::NAZAR_SPEAKS,
    eArcadeCabinet::GG_SPACE_MONKEY,
    eArcadeCabinet::INVADE_AND_PERSUADE,
    eArcadeCabinet::WIZARDS_RUIN,
    eArcadeCabinet::CAMHEDZ,
    eArcadeCabinet::INVALID,
    eArcadeCabinet::RACE_N_CHASE_ROCKET,
    eArcadeCabinet::INVALID,
    eArcadeCabinet::STREET_CRIMES,
    eArcadeCabinet::STREET_CRIMES,
    eArcadeCabinet::STREET_CRIMES,
    eArcadeCabinet::STREET_CRIMES,
    eArcadeCabinet::PENETRATOR,
    eArcadeCabinet::DEFENDER_OF_THE_FAITH,
    eArcadeCabinet::MONKEYS_PARADISE,
    eArcadeCabinet::DEFENDER_OF_THE_FAITH,
    eArcadeCabinet::PENETRATOR,
    eArcadeCabinet::GG_SPACE_MONKEY,
    eArcadeCabinet::WIZARDS_RUIN,
    eArcadeCabinet::INVADE_AND_PERSUADE,
    eArcadeCabinet::QUB3D,
    eArcadeCabinet::RACE_N_CHASE_TRUCKIN,
    eArcadeCabinet::INVALID,
    eArcadeCabinet::QUB3D,
    eArcadeCabinet::WIZARDS_RUIN,
    eArcadeCabinet::GG_SPACE_MONKEY,
    eArcadeCabinet::INVALID,
    eArcadeCabinet::INVALID,
    eArcadeCabinet::INVALID,
    eArcadeCabinet::INVALID,
    eArcadeCabinet::INVALID,
    eArcadeCabinet::INVALID};

constexpr eArcadeSaveSlot g_ArcadeSaveSlotData[] = {
    eArcadeSaveSlot::SWK_CLAW,
    eArcadeSaveSlot::AXE_OF_FURY,
    eArcadeSaveSlot::LOVE_PROFESSOR,
    eArcadeSaveSlot::STREET_CRIMES_0,
    eArcadeSaveSlot::STREET_CRIMES_1,
    eArcadeSaveSlot::STREET_CRIMES_2,
    eArcadeSaveSlot::STREET_CRIMES_3,
    eArcadeSaveSlot::BADLANDS_REVENGE,
    eArcadeSaveSlot::INVALID,
    eArcadeSaveSlot::NAZAR_SPEAKS,
    eArcadeSaveSlot::GG_SPACE_MONKEY,
    eArcadeSaveSlot::INVADE_AND_PERSUADE,
    eArcadeSaveSlot::WIZARDS_RUIN,
    eArcadeSaveSlot::CAMHEDZ,
    eArcadeSaveSlot::INVALID,
    eArcadeSaveSlot::RACE_N_CHASE_ROCKET,
    eArcadeSaveSlot::INVALID,
    eArcadeSaveSlot::STREET_CRIMES_0,
    eArcadeSaveSlot::STREET_CRIMES_1,
    eArcadeSaveSlot::STREET_CRIMES_2,
    eArcadeSaveSlot::STREET_CRIMES_3,
    eArcadeSaveSlot::PENETRATOR,
    eArcadeSaveSlot::DEFENDER_OF_THE_FAITH,
    eArcadeSaveSlot::MONKEYS_PARADISE,
    eArcadeSaveSlot::DEFENDER_OF_THE_FAITH,
    eArcadeSaveSlot::PENETRATOR,
    eArcadeSaveSlot::GG_SPACE_MONKEY,
    eArcadeSaveSlot::WIZARDS_RUIN,
    eArcadeSaveSlot::INVADE_AND_PERSUADE,
    eArcadeSaveSlot::QUB3D,
    eArcadeSaveSlot::RACE_N_CHASE_TRUCKIN,
    eArcadeSaveSlot::INVALID,
    eArcadeSaveSlot::QUB3D,
    eArcadeSaveSlot::WIZARDS_RUIN,
    eArcadeSaveSlot::GG_SPACE_MONKEY,
    eArcadeSaveSlot::INVALID,
    eArcadeSaveSlot::INVALID,
    eArcadeSaveSlot::INVALID,
    eArcadeSaveSlot::INVALID,
    eArcadeSaveSlot::INVALID,
    eArcadeSaveSlot::INVALID};

constexpr ArcadeObjectPlacementData g_ArcadeObjectPlacementData[] = {
    {"ch_prop_arcade_fortune_door_01a"_J,
        {2727.9114f, -371.9659f, -48.3982f},
        0.0f},
    {"ch_prop_laptop_01a"_J,
        {2726.5452f, -376.4341f, -47.6f},
        0.0f},
    {"sm_prop_smug_offchair_01a"_J,
        {2726.55f, -377.3595f, -47.8f},
        -175.0f},
    {"sum_prop_sum_trophy_ripped_01a"_J,
        {2726.85f, -376.332f, -47.6f},
        330.0f},
    {"sum_prop_sum_trophy_qub3d_01a"_J,
        {2726.14f, -376.372f, -47.6f},
        30.0f},
    {"tr_prop_tr_trophy_camhedz_01a"_J,
        {2726.735f, -375.982f, -47.6f},
        0.0f}};

constexpr ArcadePedPlacementData g_ArcadePedPlacementData[] = {
    {"ig_jimmydisanto2"_J,
        eArcadePedAction::MALE_STAND_MOBILE_TEXT,
        {2722.576f, -382.3125f, -49.005f},
        -74.5f},
    {"ig_wendy"_J,
        eArcadePedAction::WENDY_BAR,
        {2723.0735f, -385.5764f, -48.9676f},
        -88.11f},
    {"A_M_Y_BeachVesp_01"_J,
        eArcadePedAction::MALE_STAND_WITHDRINK_2,
        {2725.3936f, -382.9775f, -49.9124f},
        34.9f},
    {"A_F_Y_Hipster_04"_J,
        eArcadePedAction::FEMALE_STAND_IDLE,
        {2724.9746f, -382.305f, -49.9999f},
        217.665f},
    {"A_M_Y_Skater_01"_J,
        eArcadePedAction::MALE_STANDING_CALL,
        {2729.8884f, -388.92f, -49.3124f},
        300.0f},
    {"A_M_Y_Skater_02"_J,
        eArcadePedAction::MALE_ELBOWS_ON_BAR,
        {2725.3784f, -386.4875f, -49.2124f},
        90.0f},
    {"A_M_Y_Hipster_01"_J,
        eArcadePedAction::MALE_STAND_MOBILE_TEXT,
        {2734.2324f, -387.2575f, -49.3799f},
        134.565f},
    {"A_M_Y_Skater_02"_J,
        eArcadePedAction::MALE_STAND_MOBILE_TEXT,
        {2733.6694f, -387.7975f, -49.3624f},
        -47.575f},
    {"A_F_Y_Hipster_02"_J,
        eArcadePedAction::FEMALE_STAND,
        {2725.3135f, -387.165f, -49.9525f},
        38.64f},
    {"A_M_Y_BeachVesp_01"_J,
        eArcadePedAction::MALE_ELBOWS_ON_BAR,
        {2728.1426f, -387.5425f, -49.1974f},
        180.0f},
    {"A_F_Y_Skater_01"_J,
        eArcadePedAction::FEMALE_STAND_WITHDRINK,
        {2736.2815f, -374.2325f, -49.9999f},
        6.48f},
    {"A_M_Y_Hipster_01"_J,
        eArcadePedAction::HANG_OUT,
        {2736.2366f, -373.4775f, -50.0024f},
        190.165f},
    {"A_M_Y_StWhi_02"_J,
        eArcadePedAction::MALE_STAND_WITHDRINK,
        {2736.4185f, -379.0775f, -49.3649f},
        100.64f},
    {"A_F_Y_Hipster_04"_J,
        eArcadePedAction::FEMALE_STAND_2,
        {2735.6836f, -379.1575f, -49.3649f},
        280.63f}};

constexpr const char* g_WendyBarAnimNames[] = {
    "back_bar_base"};

constexpr const char* g_HangOutAnimNames[] = {
    "amb_world_human_hang_out_street_male_c_base"};

constexpr const char* g_MaleStandWithDrinkAnimNames[] = {
    "ped_male@_stand_withdrink@_01a@_idles_idle_a",
    "ped_male@_stand_withdrink@_01a@_idles_idle_b",
    "ped_male@_stand_withdrink@_01a@_idles_idle_c",
    "ped_male@_stand_withdrink@_01a@_idles_idle_d"};

constexpr const char* g_MaleStandWithDrink2AnimNames[] = {
    "ped_male@_stand_withdrink@_01b@_idles_idle_a",
    "ped_male@_stand_withdrink@_01b@_idles_idle_b",
    "ped_male@_stand_withdrink@_01b@_idles_idle_c",
    "ped_male@_stand_withdrink@_01b@_idles_idle_d"};

constexpr const char* g_FemaleStandAnimNames[] = {
    "ped_female@_stand@_03a@_idles_idle_a",
    "ped_female@_stand@_03a@_idles_idle_b",
    "ped_female@_stand@_03a@_idles_idle_c",
    "ped_female@_stand@_03a@_idles_idle_d"};

constexpr const char* g_FemaleStand2AnimNames[] = {
    "ped_female@_stand@_03b@_idles_idle_a",
    "ped_female@_stand@_03b@_idles_idle_b",
    "ped_female@_stand@_03b@_idles_idle_c",
    "ped_female@_stand@_03b@_idles_idle_d"};

constexpr const char* g_FemaleStandIdleAnimNames[] = {
    "ped_female@_stand@_02a@_idles_convo_idle_a",
    "ped_female@_stand@_02a@_idles_convo_idle_b",
    "ped_female@_stand@_02a@_idles_convo_idle_c",
    "ped_female@_stand@_02a@_idles_convo_idle_d"};

constexpr const char* g_FemaleStandWithDrinkAnimNames[] = {
    "ped_female@_stand_withdrink@_01b@_idles_idle_a",
    "ped_female@_stand_withdrink@_01b@_idles_idle_b",
    "ped_female@_stand_withdrink@_01b@_idles_idle_c",
    "ped_female@_stand_withdrink@_01b@_idles_idle_d"};

constexpr const char* g_MaleStandingCallAnimNames[] = {
    "male@_standing@_call@_idle_a"};

constexpr const char* g_MaleStandMobileTextAnimNames[] = {
    "world_human_stand_mobile@_male@_text@_idle_a"};

constexpr const char* g_MaleElbowsOnBarAnimNames[] = {
    "amb@prop_human_seat_bar@male@elbows_on_bar@idle_a"};

constexpr ArcadePedActionData g_ArcadePedActionData[] = {
    {NULL,
        0,
        1,
        g_WendyBarAnimNames,
        nullptr},
    {NULL,
        0,
        1,
        g_HangOutAnimNames,
        nullptr},
    {"prop_amb_beer_bottle"_J,
        60309,
        4,
        g_MaleStandWithDrinkAnimNames,
        "PED_MALE@_STAND_WITHDRINK@_01A@_BASE_BASE"},
    {"prop_amb_beer_bottle"_J,
        28422,
        4,
        g_MaleStandWithDrink2AnimNames,
        "PED_MALE@_STAND_WITHDRINK@_01B@_BASE_BASE"},
    {NULL,
        0,
        4,
        g_FemaleStandAnimNames,
        "PED_FEMALE@_STAND@_03A@_BASE_BASE"},
    {NULL,
        0,
        4,
        g_FemaleStand2AnimNames,
        "PED_FEMALE@_STAND@_03B@_BASE_BASE"},
    {NULL,
        0,
        4,
        g_FemaleStandIdleAnimNames,
        "PED_FEMALE@_STAND@_02A@_BASE_BASE"},
    {"prop_amb_beer_bottle"_J,
        28422,
        4,
        g_FemaleStandWithDrinkAnimNames,
        "PED_FEMALE@_STAND_WITHDRINK@_01B@_BASE_BASE"},
    {"prop_amb_phone"_J,
        28422,
        1,
        g_MaleStandingCallAnimNames,
        nullptr},
    {"prop_amb_phone"_J,
        28422,
        1,
        g_MaleStandMobileTextAnimNames,
        nullptr},
    {NULL,
        0,
        1,
        g_MaleElbowsOnBarAnimNames,
        "AMB@PROP_HUMAN_SEAT_BAR@MALE@ELBOWS_ON_BAR@BASE"},
};