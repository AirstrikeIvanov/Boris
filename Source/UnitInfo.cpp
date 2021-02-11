#include "UnitInfo.h"

using namespace BWAPI;
using namespace Boris;

namespace Boris
{
		UnitInfo::UnitInfo(Unit u)
		{
			if (!u) Broodwar << "Error creating unitinfo" << std::endl;
			unit = u;
			type = u->getType();
			health = u->getHitPoints();
			shields = u->getShields();
			energy = u->getEnergy();
			position = u->getPosition();
			if (type.isResourceContainer())
				resources = unit->getResources();
			owner = unit->getPlayer();
			id = u->getID();
		}

		void UnitInfo::update()
		{
			if (!unit) Broodwar << "Oi" << std::endl;
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
			auto i = position - ((Position)type.tileSize() / 2);
			i.x += x;
			i.y += y;
			return i;
		}

		Position UnitInfo::text(UnitType t, int y)
		{
			auto i = position + ((Position)type.tileSize() / 2);
			i.x -= t.width();
			i.y -= y;
			return i;
		}

		Position UnitInfo::box()
		{
			auto i = position + ((Position)type.tileSize() / 2);
			return i;
		}

		bool UnitInfo::canSee(Position t)
		{
			if (position.getDistance(t) <= type.sightRange())
				return true;
			else return false;
		}

		void UnitInfo::drawBars(bool unit)
		{
			if (unit)
			{
				//health
				if (type.maxShields() > 0)
				{} //shields
				if (type.maxEnergy() > 0)
				{} //energy
			}
			else
			{
				if (type == UnitTypes::Terran_Refinery || type == UnitTypes::Protoss_Assimilator || type == UnitTypes::Zerg_Extractor || type == UnitTypes::Resource_Vespene_Geyser)
				{}//gas
				else
				{} //minerals
			}
		}
}