#include "BaseInfo.h"

using namespace Boris;

namespace Boris
{
	BaseInfo::BaseInfo() : spawn(false), owned(false), constructing(false), possible(false), scout(nullptr)
	{

	}

	bool BaseInfo::hasDepot()
	{
		/* for (auto* u : BWAPI::Broodwar->getUnitsOnTile((BWAPI::TilePosition)base->Center(), BWAPI::Filter::IsResourceDepot))
			if (u->getPlayer() == BWAPI::Broodwar->self())
				return true; */
		return false;
	}

	void BaseInfo::update()
	{

	}
}