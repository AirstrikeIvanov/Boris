#include "Boris.h"

using namespace BWAPI;

void Boris::onFrame()
{
	drawInfo();

	//ZLING CODE
	for (auto u : zerglings)
	{
		auto pos = u->getTargetPosition();
		if (u->getPosition().getDistance(u->getTargetPosition()) <= supply.sightRange())
		{
			auto possible = u->getClosestUnit(BWAPI::Filter::IsEnemy && BWAPI::Filter::IsBuilding, supply.sightRange());
			if (!possible)
				removePossibleEnemyLocation(u->getTargetPosition());
		}

		processAttack(u);
	}
		

	//automine
	for (auto& base : baseList)
	{
		if (base.second.owned)
		{
			for (auto u : base.second.mineralWorkers)
				if (u->isIdle())
					if (base.second.minerals.size())
						u->gather(base.second.minerals[0]);
			for (auto u : spawnBase->gasWorkers)
				if (u->isIdle() && base.second.geysers.size())
					u->gather(base.second.geysers[0]);
		}

		//scouts
		if (base.second.possible)
		{
			if (base.second.scout == nullptr)
			{
				if (availScouts.size())
				{
					base.second.scout = availScouts[0];
					removeFromAvailable(base.second.scout);
					if (base.second.scout->isIdle())
						base.second.scout->move(base.second.loc);
				}
			}
			else
				if (base.second.scout->isIdle())
					base.second.scout->move(base.second.loc);

		}
	}

	for (auto u : Broodwar->self()->getUnits())
	{
		if (u->getType() == supply)
			checkBaseForEnemy(u);
		if (u->getType() == hatch)
		{
			if (!hasPool)
			{
				if ((getNumOfUnitType(drone) + prodDrones) < 9 && Broodwar->self()->minerals() >= 50)
				{

					u->train(drone);
					break;
				}
				else
				{
					if (!startPool)
					{
						buildPool = Broodwar->getBuildLocation(producer, spawnBase->mineralWorkers[0]->getTilePosition(), 64, true);
						startPool = true;
					}
					if (Broodwar->self()->minerals() >= 200)
						spawnBase->mineralWorkers[0]->build(producer, buildPool);
					break;
					}
				}
			//SUPPLY
			if (Broodwar->self()->supplyUsed() + 8 >= Broodwar->self()->supplyTotal() &&
				Broodwar->self()->minerals() >= supply.mineralPrice())
			{
				if (u->getType().isResourceDepot() && prodSupply < desHatch && hasPool)
					u->train(supply);
				break;
			}

			//UNITS
			if (Broodwar->self()->minerals() >= 50 && drone.supplyRequired())
			{
				if (prodDrones < desHatch)
				{
					if (findAssignedBase(u, false)->mineralWorkers.size() < (spawnBase->minerals.size() * 4) && totalMinWorkers < minWorkersDesired)
						u->train(drone);
				}
				if (hasPool)
					u->train(soldier);
			}

			//POOL
			if (findAssignedBase(u, false)->mineralWorkers.size())
			{
				auto worker = findAssignedBase(u, false)->mineralWorkers[0];
				if (Broodwar->self()->supplyTotal() >= 18 && !hasPool && Broodwar->self()->minerals() >= producer.mineralPrice())
				{
					if (!startPool)
					{
						buildPool = Broodwar->getBuildLocation(producer, findAssignedBase(u, false)->mineralWorkers[0]->getTilePosition(), 64, true);
						startPool = true;
					}
					worker->build(producer, buildPool);
				}
			}
		}

		//HATCH
		auto b = findAssignedBase(u, false);
		if (b && b->mineralWorkers.size() && !spawnBase->constructing && desHatch > curHatch&& Broodwar->self()->minerals() >= hatch.mineralPrice() && !addingProduction && hasPool)
		{
			if (!startHatch)
			{
				buildHatch = Broodwar->getBuildLocation(hatch, findAssignedBase(u, false)->mineralWorkers[1]->getTilePosition(), 64, true);
				startHatch = true;
			}

			findAssignedBase(u, false)->mineralWorkers[1]->build(hatch, buildHatch);
		}
	}

	return;
}

void Boris::onEnd(bool won)
{
	if (won)
	{
		std::cout << "Victory" << std::endl;
		Broodwar->sendText("Fear for the swarm!");
	}
		
	else
	{
		std::cout << "Defeat" << std::endl;
		Broodwar->sendText("Well then....more zerglings next time?");
	}
}



void Boris::onStart()
{
	if (Broodwar->enemy())
		enemyRace = Broodwar->enemy()->getRace();
	found = false;
	possible = 0;
	zerglings.clear();
	minWorkersDesired = 9;
	gasWorkersDesired = 4;
	totalMinWorkers = 0;
	totalGasWorkers = 0;
	baseList.clear();
	hasPool = false;
	desHatch = 1;
	curHatch = 1;
	prodDrones = 4;
	prodLings = 0;
	allLings = 0;
	addingProduction = false;
	startPool = false;
	startHatch = false;
	availScouts.clear();
	prodSupply = 0; 
	//hatcheries.clear();
	//mineralWorkers.clear();
	for (const auto& u : Broodwar->getStaticNeutralUnits())
	{
		auto& base = baseList[u->getResourceGroup()];
		if (u->getType().isMineralField())
			base.minerals.push_back(u);
		else if (u->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser)
			base.geysers.push_back(u);
	}
	auto u = Broodwar->getClosestUnit(static_cast<BWAPI::Position>(Broodwar->self()->getStartLocation()), BWAPI::Filter::IsMineralField);

	spawnBase = &baseList[u->getResourceGroup()];
	spawnBase->spawn = true;
	spawnBase->owned = true;
	for (auto p : Broodwar->getStartLocations())
		if (p != Broodwar->self()->getStartLocation())
		{
			auto m = Broodwar->getClosestUnit(BWAPI::Position(p), BWAPI::Filter::IsMineralField);
			if (m)
			{
				auto b = findAssignedBase(m, false);
				b->possible = true;
				b->loc = BWAPI::Position(p);
				possible++;
			}
		}
}

void Boris::onMorph(BWAPI::Unit u)
{
	if (u->getBuildType() == supply)
		prodSupply++;
	else if (u->getBuildType() == hatch)
	{
		addingProduction = true;
		curHatch++;
		startHatch = false;
		removeWorker(u->getClosestUnit(BWAPI::Filter::IsMineralField));
	}
	else if (u->getBuildType() == producer)
	{
		hasPool = true;
		startPool = false;
	}
	else if (u->getBuildType() == drone)
		prodDrones++;
	else if (u->getBuildType() == soldier)
		prodLings += 2;

	if (u->getBuildType().isBuilding() && u->getBuildType() != hatch)
		removeWorker(u);
}

void Boris::onUnitComplete(BWAPI::Unit u)
{
	if (u->getPlayer() != Broodwar->self())
		return;
	if (u->getType().isWorker())
	{
		prodDrones--;
		auto m = Broodwar->getClosestUnit(u->getPosition(), BWAPI::Filter::IsMineralField);
		if (m)
		{
			auto& base = baseList[m->getResourceGroup()];
			base.mineralWorkers.push_back(u);
			totalMinWorkers++;
		}
	}
	else if (u->getType().isBuilding())
	{
		if (u->getType().isResourceDepot())
		{
			if (u->getType() == hatch)
				addingProduction = false;
			auto m = Broodwar->getClosestUnit(u->getPosition(), BWAPI::Filter::IsMineralField);
			if (m)
			{
				auto base = findAssignedBase(m, false);
				if (base)
				{
					base->constructing = false;
					base->buildings.push_back(u);
					if (!base->owned)
					{
						base->owned = true;
						base->mainDepot = &u;
					}
				}
			}
		}
		else
		{
			auto b = Broodwar->getClosestUnit(u->getPosition(), BWAPI::Filter::IsResourceDepot);
			if (b)
			{
				auto base = findAssignedBase(b, false);
				if (base)
				{
					base->constructing = false;
					base->buildings.push_back(u);
				}
			}
		}
	}
	else if (u->getType() == soldier)
	{
		zerglings.push_back(u);
		allLings++;
		prodLings--;
	}
	
	if (u->getType() == supply)
	{
		availScouts.push_back(u);
		prodSupply--;
		if (Broodwar->self()->supplyTotal() >= 200)
			desHatch = 12;
		else if (Broodwar->self()->supplyTotal() >= 175)
			desHatch = 11;
		else if (Broodwar->self()->supplyTotal() >= 150)
			desHatch = 10;
		else if (Broodwar->self()->supplyTotal() >= 125)
			desHatch = 9;
		else if (Broodwar->self()->supplyTotal() >= 100)
			desHatch = 8;
		if (Broodwar->self()->supplyTotal() >= 90)
		{
			desHatch = 7;
			minWorkersDesired = 32;
			gasWorkersDesired = 16;
		}
		else if (Broodwar->self()->supplyTotal() >= 75)
			desHatch = 6;
		else if (Broodwar->self()->supplyTotal() >= 60)
		{
			desHatch = 5;
			minWorkersDesired = 24;
			gasWorkersDesired = 12;
		}
		else if (Broodwar->self()->supplyTotal() >= 50)
			desHatch = 4;
		else if (Broodwar->self()->supplyTotal() >= 40)
		{
			desHatch = 3;
			minWorkersDesired = 16;
			gasWorkersDesired = 8;
		}
		else if (Broodwar->self()->supplyTotal() >= 25)
			desHatch = 2;
		else
		{
			desHatch = 1;
			minWorkersDesired = 9;
			gasWorkersDesired = 4;
		}
	}
}
void Boris::onUnitDestroy(BWAPI::Unit u)
{
	if (u->getPlayer() != Broodwar->self())
		return;
	if (u->getType().isMineralField())
	{
		auto& base = baseList[u->getResourceGroup()];
		for (auto itr = base.minerals.begin(); itr != base.minerals.end(); itr++)
			if (u == *itr)
			{
				base.minerals.erase(itr);
				return;
			}
	}
	else if (u->getType() == soldier)
	{
		allLings--;

		auto itr = zerglings.begin();
		while (itr != zerglings.end())
		{
			if (*itr == u)
				itr = zerglings.erase(itr);
			else
				itr++;
		}

	}
	else if (u->getType().isWorker())
		removeWorker(u);
	else if (u->getType().isBuilding())
	{
		if (u->getType() == producer)
		{
			hasPool = false;
		}
		for (auto& base : baseList)
			for (auto itr = base.second.buildings.begin(); itr != base.second.buildings.end(); itr++)
				if (u == *itr)
				{
					base.second.buildings.erase(itr);
					return;
				}
	}

	else if (u->getType() == supply)
	{
		if (Broodwar->self()->supplyTotal() >= 175)
			desHatch = 8;
		else if (Broodwar->self()->supplyTotal() >= 150)
			desHatch = 7;
		else if (Broodwar->self()->supplyTotal() >= 125)
			desHatch = 6;
		else if (Broodwar->self()->supplyTotal() >= 100)
			desHatch = 5;
		if (Broodwar->self()->supplyTotal() >= 90)
		{
			minWorkersDesired = 32;
			gasWorkersDesired = 16;
		}
		else if (Broodwar->self()->supplyTotal() >= 75)
			desHatch = 4;
		else if (Broodwar->self()->supplyTotal() >= 60)
		{
			minWorkersDesired = 24;
			gasWorkersDesired = 12;
		}
		else if (Broodwar->self()->supplyTotal() >= 50)
			desHatch = 3;
		else if (Broodwar->self()->supplyTotal() >= 40)
		{
			minWorkersDesired = 16;
			gasWorkersDesired = 8;
		}
		else if (Broodwar->self()->supplyTotal() >= 25)
			desHatch = 2;
		else
		{
			desHatch = 1;
			minWorkersDesired = 8;
			gasWorkersDesired = 4;
		}
		for (auto itr = availScouts.begin(); itr != availScouts.end(); itr++)
			if (u == *itr)
			{
				availScouts.erase(itr);
				return;
			}
	}
}

void Boris::onDiscover(BWAPI::Unit u)
{
	if (u->getPlayer() == Broodwar->self())
		return;
	if (u->getType().isBuilding() && Broodwar->self()->isEnemy(u->getPlayer()))
	{
		auto m = u->getClosestUnit(BWAPI::Filter::IsMineralField);
		if (m)
		{
			auto b = findAssignedBase(m, false);
			for (auto a : baseList)
				b->possible = false;
			b->possible = true;
			if (!found)
			{
				mainEnemyBase = b->loc;
				found = true;
			}
		}
	}
}

void Boris::drawInfo()
{
	Broodwar->drawText(CoordinateType::Enum::Screen, 2, 0, "Boris v3");
	Broodwar->drawText(CoordinateType::Enum::Screen, 82, 0, "FPS: %d", Broodwar->getFPS());
	Broodwar->drawText(CoordinateType::Enum::Screen, 2, 10, "Playing on %s", Broodwar->mapName().c_str());
	if (hasPool)
		Broodwar->drawText(CoordinateType::Enum::Screen, 2, 20, "Producing %d zerglings, %d active", prodLings, allLings);
	else Broodwar->drawText(CoordinateType::Enum::Screen, 2, 20, "No spawning pool");
	Broodwar->drawText(CoordinateType::Enum::Screen, 2, 30, "%d hatcheries, producing %d drones", curHatch, prodDrones);
	Broodwar->drawText(CoordinateType::Enum::Screen, 2, 40, "%d/%d drones on minerals"/*, %d/%d on gas"*/, totalMinWorkers, minWorkersDesired);// , totalGasWorkers, gasWorkersDesired);
	if (found)
		Broodwar->drawText(CoordinateType::Enum::Screen, 2, 50, "Enemy main base located");
	else Broodwar->drawText(CoordinateType::Enum::Screen, 2, 50, "Possible enemy base locations: %d", possible);
	Broodwar->drawText(CoordinateType::Enum::Screen, 2, 60, "Enemy race: %s", enemyRace.c_str());
	if (prodSupply > 0)
		Broodwar->drawText(CoordinateType::Enum::Screen, 2, 70, "Increasing supply");
	if (addingProduction)
		Broodwar->drawText(CoordinateType::Enum::Screen, 2, 80, "Increasing production");

	//Broodwar->drawText(CoordinateType::Enum::Screen, 2, 70, "%d/%d supply", Broodwar->self()->supplyUsed(), Broodwar->self()->supplyTotal());
}

Base* Boris::findAssignedBase(BWAPI::Unit r, bool ownedOnly = false)
{	
	auto u = Broodwar->getClosestUnit(r->getPosition(), BWAPI::Filter::IsMineralField);
	if (!u)
		u = Broodwar->getClosestUnit(r->getPosition(), BWAPI::Filter::IsOwned && BWAPI::Filter::IsBuilding);
	if (!u)
		u = Broodwar->getClosestUnit(r->getPosition());

	for (auto &base : baseList)
	{
		//Broodwar << u->getType() << std::endl;
		for (auto w : base.second.mineralWorkers)
			if (u == w)
				if (ownedOnly && base.second.owned)
					return &(base.second);
				else return &(base.second);
		for (auto w : base.second.gasWorkers)
			if (u == w)
				if (ownedOnly && base.second.owned)
					return &(base.second);
				else return &(base.second);
		for (auto b : base.second.buildings)
			if (u == b)
				if (ownedOnly && base.second.owned)
					return &(base.second);
				else return &(base.second);
		for (auto m : base.second.minerals)
			if (u == m)
				if (ownedOnly && base.second.owned)
					return &(base.second);
				else return &(base.second);
	}
	Broodwar << "NULLBASEREF" << std::endl;
	return nullptr;
}

/* int Boris::getRefineries(Base* b)
{
	int r = 0;
	for (auto c : b->buildings)
		if (c->getType().isRefinery())
			r++;
	return r;
} */

int Boris::getNumOfUnitType(BWAPI::UnitType type)
{
	int r = 0;
	for (auto u : Broodwar->self()->getUnits())
		if (u->getType() == type)
			r++;
	return r;
}

int Boris::getNumOfOwnedBases()
{
	int r = 0;
	for (auto& base : baseList)
		if (base.second.owned)
			r++;
	return r;
}

void Boris::removeWorker(BWAPI::Unit u)
{
	for (auto& base : baseList)
	{
		for (auto itr = base.second.mineralWorkers.begin(); itr != base.second.mineralWorkers.end(); itr++)
			if (u == *itr)
			{
				base.second.mineralWorkers.erase(itr);
				totalMinWorkers--;
				return;
			}
		for (auto itr = base.second.gasWorkers.begin(); itr != base.second.gasWorkers.end(); itr++)
			if (u == *itr)
			{
				base.second.gasWorkers.erase(itr);
				return;
			}
	}
}

void Boris::removePossibleEnemyLocation(BWAPI::Position pos)
{
	if (pos == BWAPI::Position(Broodwar->self()->getStartLocation()))
		return;
	else
	{
		auto b = findAssignedBase(Broodwar->getClosestUnit(pos, BWAPI::Filter::IsMineralField));
		if (b && b->possible)
		{
			//Broodwar << pos << ", " << b->loc << ", " << b->possible << std::endl;
			b->possible = false;
			possible--;
			if (possible == 1 && !found)
				mainEnemyBase = getFinalPosition();
			if (b->scout)
			{
				auto t = b->scout->getClosestUnit(BWAPI::Filter::IsBuilding && BWAPI::Filter::IsOwned);
				if (t)
					b->scout->move(t->getPosition());
				else b->scout->move(BWAPI::Position(Broodwar->self()->getStartLocation()));
				availScouts.push_back(b->scout);
				b->scout = nullptr;
			}
		}
	}
}

void Boris::removeFromAvailable(BWAPI::Unit s)
{
	auto itr = availScouts.begin();
	while (itr != availScouts.end())
	{
		if (*itr == s)
			itr = availScouts.erase(itr);
		else itr++;
	}
}

BWAPI::Position Boris::getFinalPosition()
{
	for (auto& b : baseList)
	{
		if (b.second.possible)
		{
			found = true;
			return b.second.loc;
		}
	}
	return BWAPI::Positions::None;
	
}

bool Boris::checkBaseForEnemy(BWAPI::Unit u)
{
	auto success = false;
	auto pos = u->getTargetPosition();
	if (u->getPosition().getDistance(u->getTargetPosition()) <= supply.sightRange())
	{
		auto possible = u->getClosestUnit(BWAPI::Filter::IsEnemy && BWAPI::Filter::IsBuilding, supply.sightRange());
		if (!possible)
		{
			removePossibleEnemyLocation(u->getTargetPosition());
			success = true;
		}
	}
	return success;
}

void Boris::processAttack(BWAPI::Unit u)
{
	if (u->getHitPoints() < 12)
	{
		auto t = u->getClosestUnit(BWAPI::Filter::IsOwned && BWAPI::Filter::IsBuilding);
		if (t)
			u->move(t->getPosition());
		else u->move(BWAPI::Position(Broodwar->self()->getStartLocation()));
	}
	if (u->isMoving() && !u->getTarget())
	{
		auto t = u->getClosestUnit(BWAPI::Filter::IsEnemy);
		if (t)
			u->attack(t->getPosition());
	}
	if (u->getTarget() && u->getTarget()->getType().isBuilding())
	{
		auto t = u->getClosestUnit(BWAPI::Filter::IsEnemy && BWAPI::Filter::IsBuilding);
		if (t)
		{
			if (!t->getType().isWorker())
			{
				u->attack(t->getPosition());
				return;
			}
			else
			{
				u->attack(t->getPosition());
				return;
			}
		}
	}
	if (u->isIdle())
	{
		auto t = u->getClosestUnit(BWAPI::Filter::IsEnemy);
		if (t && t->getDistance(u) >= 64)
		{
			if (t->getType().isBuilding())
				u->attack(t->getPosition());
		}
		else
		{
			for (auto& b : baseList)
				if (b.second.possible)
					u->attack(b.second.loc);
		}
	}
}