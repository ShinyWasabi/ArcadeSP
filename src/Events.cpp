#include "Events.hpp"
#include <natives.h>

struct ScriptEvent
{
    int64_t Index = 0;
    int64_t Sender = 0;
    int64_t Bits = 0;
};

struct TSE_457347143 : ScriptEvent
{
    int64_t GameType = 0;
};

// Generate random leaderboard names and scores
void HandleTSE_457347143(int index)
{
    TSE_457347143 data;
    if (SCRIPT::GET_EVENT_DATA(1, index, reinterpret_cast<Any*>(&data), sizeof(data) / 8))
    {
        int64_t dataToSend[26]{};

        dataToSend[0] = -2031658982;
        dataToSend[1] = PLAYER::PLAYER_ID();
        dataToSend[3] = data.GameType;
        dataToSend[4] = 10;
        dataToSend[15] = 10;

        int scores[10];
        for (int i = 0; i < 10; i++)
            scores[i] = MISC::GET_RANDOM_INT_IN_RANGE(1000, 100000);

        std::sort(scores, scores + 10, std::greater<int>());

        for (int i = 0; i < 10; i++)
        {
            int a = MISC::GET_RANDOM_INT_IN_RANGE(0, 35);
            int b = MISC::GET_RANDOM_INT_IN_RANGE(0, 35);
            int c = MISC::GET_RANDOM_INT_IN_RANGE(0, 35);
            dataToSend[5 + i] = (int64_t)(a & 0x3F) | ((int64_t)(b & 0x3F) << 6) | ((int64_t)(c & 0x3F) << 12);
            dataToSend[16 + i] = scores[i];
        }

        // Send the data back to whichever arcade script requested it
        SCRIPT::_SEND_TU_SCRIPT_EVENT_NEW(1, reinterpret_cast<Any*>(&dataToSend), 26, -1, -2031658982);
    }
}

void ProcessScriptEvents()
{
    int numEvents = SCRIPT::GET_NUMBER_OF_EVENTS(1);

    for (int i = 0; i < numEvents; i++)
    {
        int event = SCRIPT::GET_EVENT_AT_INDEX(1, i);
        if (event != 174)
            continue;

        ScriptEvent data;
        SCRIPT::GET_EVENT_DATA(1, i, reinterpret_cast<Any*>(&data), sizeof(data) / 8);

        switch (data.Index)
        {
        case 457347143:
            HandleTSE_457347143(i);
            break;
        }
    }
}