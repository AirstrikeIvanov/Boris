#pragma once
#include <BWAPI.h>
#include <string>
#include <map>

#include "Base.h"

struct Boris
{
public:
	void onFrame();
	void onStart();
	void onUnitComplete(BWAPI::Unit u);
	void onUnitDestroy(BWAPI::Unit u);
	void onEnd(bool won);
	void onMorph(BWAPI::Unit u);
	void drawInfo();
	void removeWorker(BWAPI::Unit u);
	const BWAPI::UnitType drone = BWAPI::UnitTypes::Zerg_Drone;
	const BWAPI::UnitType hatch = BWAPI::UnitTypes::Zerg_Hatchery;
	const BWAPI::UnitType supply = BWAPI::UnitTypes::Zerg_Overlord;
	const BWAPI::UnitType soldier = BWAPI::UnitTypes::Zerg_Zergling;
	const BWAPI::UnitType producer = BWAPI::UnitTypes::Zerg_Spawning_Pool;

private:
	std::map<int, Base> baseList;
	Base* spawnBase;
	int minWorkersDesired;
	int totalMinWorkers;
	int gasWorkersDesired;
	int totalGasWorkers;
	int desHatch;
	int curHatch;
	bool hasPool;
	bool startPool, startHatch;
	bool addingProduction;
	int prodDrones;
	int prodLings;
	int allLings;
	int prodSupply;
	std::vector<BWAPI::Unit> zerglings;
	//std::vector<BWAPI::Unit> hatcheries;
	BWAPI::TilePosition buildPool, buildHatch;
	Base* findAssignedBase(BWAPI::Unit u, bool ownedOnly);
	std::map<BWAPI::UnitType, std::vector<int>> buildOrder;
	std::map<BWAPI::UnitType, bool> unitTypes;
	std::vector<BWAPI::Unit> scouts;
	std::vector<BWAPI::TilePosition> spawns;
	//std::vector<BWAPI::Unit> mineralWorkers;
	//int getRefineries(Base* b);
	int getNumOfUnitType(BWAPI::UnitType type);
	int getNumOfOwnedBases();
};