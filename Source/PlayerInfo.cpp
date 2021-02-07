#include "PlayerInfo.h"

using namespace BWAPI;
using namespace Boris;

namespace Boris
{
	PlayerInfo::PlayerInfo(Player p)
	{
		if (!p)
			return;
		player = p;
		color = p->getTextColor();
		name = p->getName();
		active = !p->isDefeated();
		units.clear();
		structures.clear();
		bases.clear();
		mainBase = BWAPI::Positions::Unknown;
	}

	void PlayerInfo::update()
	{

	}
}