#pragma once
#include <BWAPI.h>
#include <vector>

#include "UnitInfo.h"
#include "BaseInfo.h"

using namespace BWAPI;

namespace Boris
{
	class PlayerInfo
	{
	public:
		PlayerInfo(BWAPI::Player p);
		void update();
		std::string name = "[Unknown]";
		BWAPI::Color color = BWAPI::Text::Default;
		BWAPI::Race race = BWAPI::Races::Unknown;
		BWAPI::Player player = nullptr;
		BWAPI::Position mainBase;
		std::vector<UnitInfo*> units, structures;
		std::vector<BaseInfo*> bases;
		bool active = false;
	};
}