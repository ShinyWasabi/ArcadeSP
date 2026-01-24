#pragma once

struct ScriptData
{
    struct
    {
        int AcmData = 0;

        struct
        {
            int Index = 0;
            int Size = 0;
            int PropertyData = 0;
            int ArcData = 0;
            int ArcCabFlags = 0;
            int ArcCabSaveSlots = 0;
        } GpbdFm;

        struct
        {
            int Index = 0;
            int Size = 0;
            int SimpleInteriorData = 0;
            int CurSimpleInterior = 0;
            int curSimpleInteriorOwner = 0;
        } Gpbd;

        int MissionObjectFlags = 0;
        int NumReservedMissionObjects = 0;
    } Glb;

    struct
    {
        struct
        {
            int ClawCraneData = 0;
            int ClawCraneCamera = 0;
        } ArcClawCrane;
    } Stc;

    bool Init();
} inline g_ScriptData;