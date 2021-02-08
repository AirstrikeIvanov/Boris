#include "Fighter.h"

using namespace BWAPI;
using namespace Boris;

namespace Boris
{
	void UnitPriorities::onStart()
	{
		structures.clear();
		for (UnitType t : BWAPI::UnitTypes::allUnitTypes())
		{
			structures[t] = 1;
			if (t == UnitTypes::Terran_Supply_Depot || t == UnitTypes::Zerg_Overlord || t == UnitTypes::Protoss_Pylon)
				structures[t] = 2;
			if (t.isResourceDepot()) structures[t] = 3;
			if (t.isWorker()) structures[t] = 4;
			if (t.canProduce()) structures[t] = 5;
			if (t == UnitTypes::Zerg_Hydralisk_Den || t == UnitTypes::Zerg_Spawning_Pool || t == UnitTypes::Zerg_Spire || t == UnitTypes::Zerg_Greater_Spire ||
				t == UnitTypes::Protoss_Forge || t == UnitTypes::Protoss_Cybernetics_Core || t == UnitTypes::Terran_Academy)
				structures[t] = 6;
			if (t == UnitTypes::Terran_Bunker || t == UnitTypes::Protoss_Photon_Cannon || t == UnitTypes::Zerg_Spore_Colony || t == UnitTypes::Zerg_Creep_Colony || t == UnitTypes::Zerg_Sunken_Colony)
				structures[t] = 7;
		}
	}

	Squad::Squad(UnitInfo* u)
	{
		units.clear();
		strength = 0;
		order = 0;
		addToSquad(u);
	}

	bool Squad::containsUnit(UnitInfo* u)
	{
		for (auto& i : units)
			if (i == u)
				return true;
		return false;
	}

	void Squad::addToSquad(UnitInfo* u)
	{
		units.emplace_back(u);
	}

	bool Squad::removeFromSquad(UnitInfo* u)
	{
		auto itr = units.begin();
		while (itr != units.end())
			if (*itr == u)
			{
				units.erase(itr);
				return true;
			}
			else itr++;
		return false;
	}

	void Fighter::onStart()
	{
		squads.clear();
		priorities.onStart();
	}

	void Fighter::drawInfo()
	{
		Broodwar->drawTextScreen(2, 80, "tmp: %d squads managed", squads.size());
		for(auto& s : squads)
		{
			Broodwar->drawTextScreen(2, 90, "Type %s, unitcount %d", ((UnitType)s.first).c_str(), s.second->units.size());
			for (int i = 0; i > s.second->units.size(); i++)
				if (i == 0)
					Broodwar->drawCircleMap(s.second->units[0]->position, 5, Broodwar->self()->getColor());
					//Broodwar->drawBoxMap(s.second.units[i]->text(), s.second.units[i]->text(s.second.units[i]->type.size()), Broodwar->self()->getColor());
				else Broodwar->drawLineMap(s.second->units[i]->position, s.second->units[0]->position, BWAPI::Colors::Teal);
		}
	}

	Unit Fighter::getTarget(UnitInfo* u)
	{
		return nullptr;
	}

	Unit Fighter::getTarget(Squad* squad)
	{
		return nullptr;
	}
	
	Position Fighter::getTargetPos(UnitInfo* u)
	{
		return BWAPI::Positions::Invalid;
	}

	Position Fighter::getTargetPos(Squad* squad)
	{
		return BWAPI::Positions::Invalid;
	}

	Position Fighter::getTargetFar(UnitInfo* u)
	{
		return BWAPI::Positions::Invalid;
	}

	Position Fighter::getTargetFar(Squad* squad)
	{
		return BWAPI::Positions::Invalid;
	}

	Squad* Fighter::getSquad(UnitInfo* u)
	{
		for (auto& s : squads)
			if (s.first == u->type)
				return s.second;
		return nullptr;
	}

	bool Fighter::assignSquad(UnitInfo* u)
	{
		auto s = getSquad(u);
		if (s)
		{
			s->addToSquad(u);
			return true;
		}
		squads.emplace(u->type, new Squad(u));
		
		return false;
	}

	void Fighter::onUnitDestroy(UnitInfo* u)
	{
		auto s = getSquad(u);
		if (s)
			s->removeFromSquad(u);
		//else Broodwar << "Fighter: Error removing unit " << u->id << " of type " << u->type.c_str() << std::endl;
	}
}