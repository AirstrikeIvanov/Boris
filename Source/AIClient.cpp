#include <BWAPI.h>
#include <BWAPI/Client.h>

#include <iostream>
#include <thread>
#include <chrono>
#include <string>

#include "Boris.h"

using namespace BWAPI;

void reconnect()
{
    while(!BWAPIClient.connect())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds{ 1000 });
    }
}

int main(int argc, const char* argv[])
{
    Boris bot;
    std::cout << "Connecting..." << std::endl;;
    reconnect();
    while (true)
    {
        std::cout << "waiting to enter match" << std::endl;
        while (!Broodwar->isInGame())
        {
            BWAPI::BWAPIClient.update();
            if (!BWAPI::BWAPIClient.isConnected())
            {
                std::cout << "Reconnecting..." << std::endl;;
                reconnect();
            }
        }
        while (Broodwar->isInGame())
        {
            for (const auto& e : Broodwar->getEvents())
            {
                switch (e.getType())
                {
                    case BWAPI::EventType::MatchStart:
                        bot.onStart();
                        break;
                    case BWAPI::EventType::UnitComplete:
                        bot.onUnitComplete(e.getUnit());
                        break;
                    case BWAPI::EventType::UnitDestroy:
                        bot.onUnitDestroy(e.getUnit());
                        break;
                    case BWAPI::EventType::MatchEnd:
                        bot.onEnd(e.isWinner());
                        break;
                    case BWAPI::EventType::UnitMorph:
                        bot.onMorph(e.getUnit());
                        break;
                    case BWAPI::EventType::UnitDiscover:
                        bot.onDiscover(e.getUnit());
                        break;
                    default:
                        break;
                }
            }
            bot.onFrame();
            BWAPI::BWAPIClient.update();
        }
    }
    return 0;
}
