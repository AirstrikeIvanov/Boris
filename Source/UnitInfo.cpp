#include "UnitInfo.h"

using namespace BWAPI;
using namespace Boris;

namespace Boris
{
		UnitInfo::UnitInfo(Unit u)
		{
			unit = u;
			type = u->getType();
			health = u->getHitPoints();
			shields = u->getShields();
			energy = u->getEnergy();
			position = u->getPosition();
			if (type.isResourceContainer())
				resources = unit->getResources();
			owner = unit->getPlayer();
		}

		void UnitInfo::update()
		{
			health = unit->getHitPoints();
			if (unit->getType() != type)
				type = unit->getType();
			shields = unit->getShields();
			energy = unit->getEnergy();
			position = unit->getPosition();
			if (!unit->isIdle())
				target = unit->getTargetPosition();
			if (unit->getPlayer() != owner)
				owner = unit->getPlayer();
		}

		Position UnitInfo::text(int x, int y)
		{
			auto i = unit->getPosition() - ((Position)unit->getType().tileSize() / 2);
			i.x += x;
			i.y += y;
			return i;
		}

		Position UnitInfo::text(UnitType t, int y)
		{
			auto i = unit->getPosition() + ((Position)unit->getType().tileSize() / 2);
			i.x -= t.width();
			i.y -= y;
			return i;
		}

		bool UnitInfo::canSee(Position t)
		{
			if (position.getDistance(t) <= type.sightRange())
				return true;
			else return false;
		}
}