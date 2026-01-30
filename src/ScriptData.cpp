#include "ScriptData.hpp"
#include "rage/scrProgram.hpp"
#include <natives.h>

bool readFailed = false;

void ReadScript(rage::scrProgram* program, const char* pattern, const std::vector<std::pair<uint32_t, bool>>& offsetAndRip, void* out, uint32_t size)
{
    auto addressOpt = program->ScanPattern(pattern);
    if (!addressOpt.has_value())
    {
        readFailed = true;
        LOGF("Failed to find script pattern '%s' in script program %s.", pattern, program->m_Name);
        return;
    }

    uint32_t address = addressOpt.value();

    for (const auto& [offset, rip] : offsetAndRip)
    {
        address += offset;
        if (rip)
        {
            auto code = program->GetCode(address);
            address = code[0] | (code[1] << 8) | (code[2] << 16);
        }
    }

    auto code = program->GetCode(address);
    std::memcpy(out, code, size);

    int32_t value = 0;
    std::memcpy(&value, out, size);
    LOGF("Read script code at 0x%08X in script program %s, value is %d.", address, program->m_Name, value);
}

bool ScriptData::Init()
{
    while (!SCRIPT::HAS_SCRIPT_WITH_NAME_HASH_LOADED("am_mp_arc_cab_manager"_J) || !SCRIPT::HAS_SCRIPT_WITH_NAME_HASH_LOADED("am_mp_arcade_claw_crane"_J))
    {
        SCRIPT::REQUEST_SCRIPT_WITH_NAME_HASH("am_mp_arc_cab_manager"_J);
        SCRIPT::REQUEST_SCRIPT_WITH_NAME_HASH("am_mp_arcade_claw_crane"_J);
        WAIT(0);
    }

    auto program1 = rage::scrProgram::GetProgram("am_mp_arc_cab_manager"_J);
    auto program2 = rage::scrProgram::GetProgram("am_mp_arcade_claw_crane"_J);
    if (!program1 || !program2)
        return false;

    // Globals

    ReadScript(program1, "61 ? ? ? 40 03 35 01 37 0C", {{1, false}}, &Glb.AcmData, 3);

    // this has two occurrences, but the scan should always find the first one, which is what we need
    ReadScript(program1, "38 01 38 00 2C 01 ? ? 61 ? ? ? 49 ? ? 46 ? ? 46 ? ? 36", {{9, false}}, &Glb.GpbdFm.Index, 3);
    ReadScript(program1, "38 01 38 00 2C 01 ? ? 61 ? ? ? 49 ? ? 46 ? ? 46 ? ? 36", {{13, false}}, &Glb.GpbdFm.Size, 2);
    ReadScript(program1, "38 01 38 00 2C 01 ? ? 61 ? ? ? 49 ? ? 46 ? ? 46 ? ? 36", {{16, false}}, &Glb.GpbdFm.PropertyData, 2);
    ReadScript(program1, "38 01 38 00 2C 01 ? ? 61 ? ? ? 49 ? ? 46 ? ? 46 ? ? 36", {{19, false}}, &Glb.GpbdFm.ArcCabSaveSlots, 2);
    Glb.GpbdFm.ArcData = Glb.GpbdFm.ArcCabSaveSlots - 8;
    Glb.GpbdFm.ArcCabFlags = Glb.GpbdFm.ArcCabSaveSlots - 2;

    ReadScript(program1, "61 ? ? ? 49 ? ? 46 ? ? 41 ? 5D ? ? ? 25 11", {{1, false}}, &Glb.Gpbd.Index, 3);
    ReadScript(program1, "61 ? ? ? 49 ? ? 46 ? ? 41 ? 5D ? ? ? 25 11", {{5, false}}, &Glb.Gpbd.Size, 2);
    ReadScript(program1, "61 ? ? ? 49 ? ? 46 ? ? 41 ? 5D ? ? ? 25 11", {{8, false}}, &Glb.Gpbd.SimpleInteriorData, 2);
    ReadScript(program1, "61 ? ? ? 49 ? ? 46 ? ? 41 ? 5D ? ? ? 25 11", {{11, false}}, &Glb.Gpbd.CurSimpleInterior, 1);
    Glb.Gpbd.curSimpleInteriorOwner = Glb.Gpbd.CurSimpleInterior + 3;

    ReadScript(program1, "2D 05 09 00 00 62", {{6, false}}, &Glb.MissionObjectFlags, 3);
    Glb.NumReservedMissionObjects = Glb.MissionObjectFlags + 10;

    // Statics

    ReadScript(program2, "4F ? ? 41 ? 2C 05 ? ? 2A", {{1, false}}, &Stc.ArcClawCrane.ClawCraneData, 2);
    ReadScript(program2, "4F ? ? 41 ? 2C 05 ? ? 2A", {{4, false}}, &Stc.ArcClawCrane.ClawCraneCamera, 1);

    SCRIPT::SET_SCRIPT_WITH_NAME_HASH_AS_NO_LONGER_NEEDED("am_mp_arc_cab_manager"_J);
    SCRIPT::SET_SCRIPT_WITH_NAME_HASH_AS_NO_LONGER_NEEDED("am_mp_arcade_claw_crane"_J);

    if (readFailed)
    {
        *this = {};
        readFailed = false;
        return false;
    }

    return true;
}