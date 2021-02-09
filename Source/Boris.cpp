#include "Boris.h"

using namespace BWAPI;
using namespace Boris;

namespace Boris
{
	void BorisClient::onFrame()
	{
		drawInfo();
		fighter.drawInfo();
		
		for (auto u : Broodwar->getAllUnits())
		{
			auto i = getInfo(u);
			if (!i)
				i = createUnitInfo(u);
			i->update();
		}

		if (fighter.squads.empty()) goto ignoreFighter;
		//ZLING CODE
		if (fighter.getSquad(ling))
		{
			for (auto& u : fighter.squads[ling]->units)
			{
				//auto pos = u->unit->getTargetPosition();
				if (u->unit->getPosition().getDistance(u->unit->getTargetPosition()) <= supply.sightRange())
				{
					auto possible = u->unit->getClosestUnit(BWAPI::Filter::IsEnemy && BWAPI::Filter::IsBuilding, supply.sightRange());
					if (!possible)
						removePossibleEnemyLocation(u->unit->getTargetPosition());
				}

				processAttack(u);
			}
		}

		ignoreFighter:
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
							buildPool = Broodwar->getBuildLocation(pool, spawnBase->mineralWorkers[0]->getTilePosition(), 64, true);
							startPool = true;
						}
						if (Broodwar->self()->minerals() >= 200)
							spawnBase->mineralWorkers[0]->build(pool, buildPool);
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
						u->train(ling);
				}

				//POOL
				if (findAssignedBase(u, false)->mineralWorkers.size())
				{
					auto worker = findAssignedBase(u, false)->mineralWorkers[0];
					if (Broodwar->self()->supplyTotal() >= 18 && !hasPool && Broodwar->self()->minerals() >= pool.mineralPrice())
					{
						if (!startPool)
						{
							buildPool = Broodwar->getBuildLocation(pool, findAssignedBase(u, false)->mineralWorkers[0]->getTilePosition(), 64, true);
							startPool = true;
						}
						worker->build(pool, buildPool);
					}
				}
			}

			//HATCH
			auto b = findAssignedBase(u, false);
			if (b && b->mineralWorkers.size() && !spawnBase->constructing && desHatch > curHatch && Broodwar->self()->minerals() >= hatch.mineralPrice() && !addingProduction && hasPool)
			{
				if (!startHatch)
				{
					buildHatch = Broodwar->getBuildLocation(hatch, findAssignedBase(u, false)->mineralWorkers[1]->getTilePosition(), 64, true);
					startHatch = true;
				}

				findAssignedBase(u, false)->mineralWorkers[0]->build(hatch, buildHatch);
			}
		}

		return;
	}

	void BorisClient::onEnd(bool won)
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



	void BorisClient::onStart()
	{
		fighter.onStart();
		players.clear();
		for (auto p : Broodwar->enemies())
			if (p != nullptr)
				players.push_back(p);
		found = false;
		possible = 0;
		zerglings.clear();
		hydralisks.clear();
		mutalisks.clear();
		lurkers.clear();
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
		enemyUnits.clear();
		neutralUnits.clear();
		friendlyUnits.clear();
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

	void BorisClient::onMorph(BWAPI::Unit u)
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
		else if (u->getBuildType() == pool)
		{
			hasPool = true;
			startPool = false;
		}
		else if (u->getBuildType() == drone)
			prodDrones++;
		else if (u->getBuildType() == ling)
			prodLings += 2;

		if (u->getBuildType().isBuilding() && u->getBuildType() != hatch)
			removeWorker(u);
	}

	void BorisClient::onUnitComplete(BWAPI::Unit u)
	{
		if (u->getPlayer() != Broodwar->self())
			return;
		auto i = getInfo(u);
		if (!i) i = createUnitInfo(u);
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
		else if (u->getType() == ling)
		{
			zerglings.push_back(i);
			fighter.assignSquad(i);
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
	void BorisClient::onUnitDestroy(BWAPI::Unit u)
	{
		auto i = getInfo(u);
		if (u->getPlayer() == Broodwar->self())
			fighter.onUnitDestroy(i);
		//if (u->getPlayer() != Broodwar->self() || u->getPlayer() != Broodwar->neutral())
			//return;
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
		else if (u->getType() == ling)
		{
			allLings--;
			//fighter.onUnitDestroy(i);
			removeFromList(i);

		}
		else if (u->getType().isWorker())
			removeWorker(u);
		else if (u->getType().isBuilding())
		{
			if (u->getType() == pool)
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
		//if (u->getPlayer() == Broodwar->self())
			//fighter.onUnitDestroy(i);
		removeFromList(i);
	}

	void BorisClient::onDiscover(BWAPI::Unit u)
	{
		auto* i = getInfo(u);
		if (!i)
			createUnitInfo(u);
		else i->update();
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

	/// X coordinate is always 2, indentations are 6 and 10
	/// Y coordinate is always multiple of 10.
	void BorisClient::drawInfo()
	{
		if (players.size() == 1)
			Broodwar->drawTextScreen(2, 0, "%c%s%c vs %c%s (%s)",
				Broodwar->self()->getTextColor(), Broodwar->self()->getName().c_str(), '\001',
				players[0].color, players[0].name.c_str(), players[0].race.c_str());
		else Broodwar->drawTextScreen(2, 0, "%c%s%c vs %d enemies",
			Broodwar->self()->getTextColor(), Broodwar->self()->getName().c_str(), '\001',
			players.size());
		Broodwar->drawTextScreen(2, 10, "Playing on %s", Broodwar->mapName().c_str());
		Broodwar->drawTextScreen(2, 20, "Tracked entities: F: %d, N: %d, E: %d",
			friendlyUnits.size(), neutralUnits.size(), enemyUnits.size());
		if (hasPool)
			Broodwar->drawTextScreen(2, 30, "Producing %d zerglings, %d active", prodLings, allLings);
		else Broodwar->drawTextScreen(2, 30, "No spawning pool");
		Broodwar->drawTextScreen(2, 40, "%d hatcheries, producing %d drones", curHatch, prodDrones);
		Broodwar->drawTextScreen(2, 50, "%d/%d drones on minerals"/*, %d/%d on gas" */, totalMinWorkers, minWorkersDesired);// , totalGasWorkers, gasWorkersDesired);
		if (found)
			Broodwar->drawTextScreen(2, 60, "Enemy main base located");
		else Broodwar->drawTextScreen(2, 60, "Possible enemy base locations: %d", possible);
		if (prodSupply > 0)
			Broodwar->drawTextScreen(2, 70, "Increasing supply");
		else if (addingProduction)
			Broodwar->drawTextScreen(2, 70, "Increasing production");
		else if (addingProduction && prodSupply > 0)
			Broodwar->drawTextScreen(2, 70, "Increasing supply and production");
		else Broodwar->drawTextScreen(2, 70, "Not increasing supply or production");
		if (players.size() > 1)
		{
			int t = 0;
			Broodwar->drawTextScreen(2, 90, "Player list: ");
			for (auto p : players)
			{
				Broodwar->drawTextScreen(6, 100 + t, "%c%s (%s)", p.color, p.name.c_str(), p.race.c_str());
				t += 10;
			}
		}

		//Broodwar->drawTextScreen(2, 70, "%d/%d supply", Broodwar->self()->supplyUsed(), Broodwar->self()->supplyTotal());
	}

	BaseInfo* BorisClient::findAssignedBase(BWAPI::Unit r, bool ownedOnly = false)
	{
		if (!r) goto fail;
		auto u = Broodwar->getClosestUnit(r->getPosition(), BWAPI::Filter::IsMineralField);
		if (!u)
			u = Broodwar->getClosestUnit(r->getPosition(), BWAPI::Filter::IsOwned && BWAPI::Filter::IsBuilding);
		if (!u)
			u = Broodwar->getClosestUnit(r->getPosition());

		for (auto& base : baseList)
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
		fail:
		Broodwar << "NULLBASEREF" << std::endl;
		return nullptr;
	}

	/* int BorisClient::getRefineries(Base* b)
	{
		int r = 0;
		for (auto c : b->buildings)
			if (c->getType().isRefinery())
				r++;
		return r;
	} */

	int BorisClient::getNumOfUnitType(BWAPI::UnitType type)
	{
		int r = 0;
		for (auto u : Broodwar->self()->getUnits())
			if (u->getType() == type)
				r++;
		return r;
	}

	int BorisClient::getNumOfOwnedBases()
	{
		int r = 0;
		for (auto& base : baseList)
			if (base.second.owned)
				r++;
		return r;
	}

	bool BorisClient::removeWorker(BWAPI::Unit u)
	{
		for (auto& base : baseList)
		{
			for (auto itr = base.second.mineralWorkers.begin(); itr != base.second.mineralWorkers.end(); itr++)
				if (u == *itr)
				{
					base.second.mineralWorkers.erase(itr);
					totalMinWorkers--;
					return true;
				}
			for (auto itr = base.second.gasWorkers.begin(); itr != base.second.gasWorkers.end(); itr++)
				if (u == *itr)
				{
					base.second.gasWorkers.erase(itr);
					return true;
				}
		}
		return false;
	}

	void BorisClient::removePossibleEnemyLocation(BWAPI::Position pos)
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

	bool BorisClient::removeFromAvailable(BWAPI::Unit s)
	{
		auto itr = availScouts.begin();
		while (itr != availScouts.end())
		{
			if (*itr == s)
			{
				itr = availScouts.erase(itr);
				return true;
			}
			else itr++;
		}
		return false;
	}

	BWAPI::Position BorisClient::getFinalPosition()
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

	bool BorisClient::checkBaseForEnemy(BWAPI::Unit u)
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

	void BorisClient::processAttack(UnitInfo* u)
	{
		if (u->unit->isAttackFrame() || u->unit->getTarget())
			return;
		if (u->unit->getHitPoints() < 12)
		{
			auto t = u->unit->getClosestUnit(BWAPI::Filter::IsOwned && BWAPI::Filter::IsBuilding);
			if (t)
				u->unit->move(t->getPosition());
			else u->unit->move(BWAPI::Position(Broodwar->self()->getStartLocation()));
		}
		if (u->unit->isMoving() || !u->unit->getTarget())
		{
			auto t = u->unit->getClosestUnit(BWAPI::Filter::IsEnemy);
			if (t && u->unit->canAttackUnit(t) && u->target.getDistance(t->getPosition()) >= 64)
			{
				//Broodwar << Broodwar->getFrameCount() << ": Processing attack for " << u->id << " (" << u->type << ")" << std::endl;
				auto a = fighter.getTarget(u, t->getPosition());
				if (!a) u->unit->attack(t->getPosition());
				if (u->position.getDistance(t->getPosition()) <= 64)
					u->unit->attack(a);
				else u->unit->attack(a->getPosition());
				//if (!a)	u->unit->attack(t->getPosition());
				//else u->unit->attack(a->getPosition());
				//u->unit->attack(t->getPosition());
				return;
			}
		} /*
		if (u->unit->getTarget() && u->unit->getTarget()->getType().isBuilding())
		{
			auto t = u->unit->getClosestUnit(BWAPI::Filter::IsEnemy && BWAPI::Filter::IsBuilding);
			if (t && u->unit->canAttack(t))
			{
				if (!t->getType().isWorker())
				{
					auto a = fighter.getTarget(u, t->getPosition());
					if (!a) u->unit->attack(t->getPosition());
					if (u->position.getDistance(t->getPosition()) <= 64)
						u->unit->attack(a);
					else u->unit->attack(a->getPosition());
					return;
				}
				else
				{
					auto a = fighter.getTarget(u, t->getPosition());
					if (!a) u->unit->attack(t->getPosition());
					if (u->position.getDistance(t->getPosition()) <= 64)
						u->unit->attack(a);
					else u->unit->attack(a->getPosition());
					//u->unit->attack(t->getPosition());
					return;
				}
			}
		} */
		if (u->unit->isIdle())
		{
			//Broodwar << Broodwar->getFrameCount() << ": Processing attack for " << u->id << " (" << u->type << ")" << std::endl;
			auto t = u->unit->getClosestUnit(BWAPI::Filter::IsEnemy);
			if (t && t->getDistance(u->unit) >= 64 && u->unit->canAttack(t))
			{
				if (t->getType().isBuilding())
				{
					auto a = fighter.getTarget(u, t->getPosition());
					if (!a) u->unit->attack(t->getPosition());
					if (u->position.getDistance(t->getPosition()) <= 64)
						u->unit->attack(a);
					else u->unit->attack(a->getPosition());
					//if (a) u->unit->attack(a->getPosition());
					//else u->unit->attack(t->getPosition());
				}
			}
			else
			{
				for (auto& b : baseList)
					if (b.second.possible)
						u->unit->attack(b.second.loc);
			}
		}
	}

	UnitInfo* BorisClient::getInfo(Unit u)
	{
		for (auto& i : friendlyUnits)
			if (i->unit == u)
				return i;
		for (auto& i : neutralUnits)
			if (i->unit == u)
				return i;
		for (auto &i : enemyUnits)
			if (i->unit == u)
				return i;
		return nullptr;
	}

	PlayerInfo* BorisClient::getInfo(Player p)
	{
		for (auto& i : players)
			if (&*i.player == p)
				return &i;
		return nullptr;
	}

	BaseInfo* BorisClient::getInfo(Position p)
	{
		for (auto& i : baseList)
			if (i.second.loc == p)
				return &i.second;
		return nullptr;
	}

	bool BorisClient::removeFromList(UnitInfo* u)
	{
		auto itr = friendlyUnits.begin();
		while (itr != friendlyUnits.end())
		{
			if (u == *itr)
			{
				friendlyUnits.erase(itr);
				delete(u);
				return true;
			}
			else itr++;
		}
		itr = neutralUnits.begin();
		while (itr != neutralUnits.end())
		{
			if (u == *itr)
			{
				neutralUnits.erase(itr);
				return true;
			}
			else itr++;
		}
		itr = enemyUnits.begin();
		while (itr != enemyUnits.end())
		{
			if (u == *itr)
			{
				enemyUnits.erase(itr);
				return true;
			}
			else itr++;
		}
		itr = zerglings.begin();
		while (itr != zerglings.end())
		{
			if (*itr == u)
			{
				itr = zerglings.erase(itr);
				return true;
			}
			else itr++;
		}
		//Broodwar << "Error with remove from list (UnitInfo)" << std::endl;
		return false;
	}

	UnitInfo* BorisClient::createUnitInfo(Unit u)
	{
		auto n = new UnitInfo(u);
		if (u->getPlayer() == Broodwar->self())
			friendlyUnits.emplace_back(n);
		else if (u->getPlayer() == Broodwar->neutral())
			neutralUnits.emplace_back(n);
		else enemyUnits.emplace_back(n);
		return n;
	}

	void BorisClient::onUnitShow(Unit u)
	{
		for (auto s : Broodwar->getUnitsInRadius(u->getPosition(), 99999999, BWAPI::Filter::IsOwned))
		{
			auto i = getInfo(s);
			if (!i) i = createUnitInfo(s);
			if (!i->canSee(u->getPosition()) || !i->type.canAttack() || i->isWorker() ||
				i->unit->isAttackFrame() || i->unit->getTargetPosition())
				return;
			processAttack(i);
			Broodwar << "Unit " << i->id << " (" << i->type.c_str() << ") Re-evaluating targets" << std::endl;
			/*
			auto t = i->unit->getClosestUnit(BWAPI::Filter::IsEnemy);
			if (t && i->unit->canAttackUnit(t))
			{
				auto a = fighter.getTarget(i, t->getPosition());
				if (!a)
				{
					i->unit->attack(t->getPosition());
					return;
				}
				i->unit->attack(a->getPosition());
				//u->unit->attack(t->getPosition());
				return;
			}
			*/
		}
	}
}