#pragma once

constexpr int NUM_ARCADE_LOCATIONS = 6;
constexpr int NUM_ARCADE_OBJECTS = 6;

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
    CAMHEDZ
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
    CAMHEDZ
};

struct ArcadeObjectData
{
    Hash ObjectModel;
    Vector3 Coords;
    float Heading;
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
    "entity_set_screens"
};

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
    eControl::ControlSelectWeaponSpecial
};

constexpr Vector3 g_ArcadeBlipCoords[] = {
    {-247.6898f, 6212.915f, 30.944f},
    {1695.1714f, 4785.1177f, 40.9847f},
    {-116.3816f, -1772.1368f, 28.8592f},
    {-599.5152f, 279.6308f, 81.074f},
    {-1273.2231f, -304.1054f, 37.2289f},
    {758.3455f, -815.9312f, 25.2905f}
};

constexpr Vector3 g_ArcadeEntryCoords[] = {
    {-245.64f, 6210.96f, 30.94f},
    {1695.85f, 4783.85f, 41.02f},
    {-115.17f, -1771.64f, 28.86f},
    {-601.11f, 280.47f, 81.04f},
    {-1269.72f, -304.09f, 36.0f},
    {758.46f, -814.57f, 25.3f}
};

constexpr std::pair<Vector3, float> g_ArcadeExitCoords[] = {
    {
        {-248.1222f, 6212.625f, 30.944f},
        136.405f
    },
    {
        {1695.4316f, 4785.1533f, 40.9945f},
        89.645f
    },
    {
        {-116.2575f, -1772.1418f, 28.8594f},
        34.56f
    },
    {
        {-599.5079f, 279.6345f, 81.0742f},
        173.975f
    },
    {
        {-1269.7025f, -305.4233f, 35.9952f},
        250.2f
    },
    {
        {759.0959f, -816.0111f, 25.2974f},
        268.95f
    }
};

constexpr eArcadeCabinet g_ArcadeCabinets[] = {
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
    eArcadeCabinet::INVALID
};

constexpr eArcadeSaveSlot g_ArcadeSaveSlots[] = {
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
    eArcadeSaveSlot::INVALID
};

constexpr ArcadeObjectData g_ArcadeObjects[] = {
    {
        "ch_prop_arcade_fortune_door_01a"_J,
        {2727.9114f, -371.9659f, -48.3982f},
        0.0f
    },
    {
        "ch_prop_laptop_01a"_J,
        {2726.5452f, -376.4341f, -47.6f},
        0.0f
    },
    {
        "sm_prop_smug_offchair_01a"_J,
        {2726.55f, -377.3595f, -47.8f},
        -175.0f
    },
    {
        "sum_prop_sum_trophy_ripped_01a"_J,
        {2726.85f, -376.332f, -47.6f},
        330.0f
    },
    {
        "sum_prop_sum_trophy_qub3d_01a"_J,
        {2726.14f, -376.372f, -47.6f},
        30.0f
    },
    {
        "tr_prop_tr_trophy_camhedz_01a"_J,
        {2726.735f, -375.982f, -47.6f},
        0.0f
    }
};