#include "PlayerInfo.h"

using namespace BWAPI;
using namespace Boris;

namespace Boris
{
	PlayerInfo::PlayerInfo(Player p)
	{
		player = p;
		color = p->getTextColor();
		name = p->getName();
		race = p->getRace();
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