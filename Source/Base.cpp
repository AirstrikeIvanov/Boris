#include "Base.h"

using namespace Boris;

namespace Boris
{
	Base::Base() : spawn(false), owned(false), constructing(false), possible(false), scout(nullptr)
	{

	}

	bool Base::hasDepot()
	{
		/* for (auto* u : BWAPI::Broodwar->getUnitsOnTile((BWAPI::TilePosition)base->Center(), BWAPI::Filter::IsResourceDepot))
			if (u->getPlayer() == BWAPI::Broodwar->self())
				return true; */
		return false;
	}

	void Base::update()
	{

	}
}