#pragma once
#include <BWAPI.h>
#include <string>
#include <map>

#include "BaseInfo.h"
#include "UnitInfo.h"
#include "PlayerInfo.h"
//#include "MapInfo.h"
//#include "Builder.h"
#include "Fighter.h"
//#include "Strategy.h"

namespace Boris
{
	struct BorisClient
	{
	public:
		void onFrame();
		void onStart();
		void onUnitComplete(BWAPI::Unit u);
		void onUnitDestroy(BWAPI::Unit u);
		void onEnd(bool won);
		void onMorph(BWAPI::Unit u);
		void onDiscover(BWAPI::Unit u);
		void drawInfo();
		bool removeWorker(BWAPI::Unit u);
		void removePossibleEnemyLocation(BWAPI::Position pos);
		bool removeFromAvailable(BWAPI::Unit s);
		void processAttack(UnitInfo* u);
		const BWAPI::UnitType drone		= BWAPI::UnitTypes::Zerg_Drone;
		const BWAPI::UnitType hatch		= BWAPI::UnitTypes::Zerg_Hatchery;
		const BWAPI::UnitType supply	= BWAPI::UnitTypes::Zerg_Overlord;
		const BWAPI::UnitType ling		= BWAPI::UnitTypes::Zerg_Zergling;
		const BWAPI::UnitType hydra		= BWAPI::UnitTypes::Zerg_Hydralisk;
		const BWAPI::UnitType flyer		= BWAPI::UnitTypes::Zerg_Mutalisk;
		const BWAPI::UnitType pool		= BWAPI::UnitTypes::Zerg_Spawning_Pool;
		const BWAPI::UnitType spire		= BWAPI::UnitTypes::Zerg_Spire;
		const BWAPI::UnitType den		= BWAPI::UnitTypes::Zerg_Hydralisk_Den;
		std::map<int, BaseInfo> baseList;

	private:
		BaseInfo* spawnBase;
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
		bool found;
		std::vector<UnitInfo*> zerglings, hydralisks, mutalisks, lurkers;
		BWAPI::TilePosition buildPool, buildHatch;
		BaseInfo* findAssignedBase(BWAPI::Unit r, bool ownedOnly);
		std::map<BWAPI::UnitType, std::vector<int>> buildOrder;
		std::map<BWAPI::UnitType, bool> unitTypes;
		std::vector<BWAPI::Unit> availScouts;
		//std::unordered_map<BWAPI::Position, BWAPI::Unit> scouts2;
		int possible;
		BWAPI::Position mainEnemyBase;
		int getNumOfUnitType(BWAPI::UnitType type);
		int getNumOfOwnedBases();
		BWAPI::Position getFinalPosition();
		bool checkBaseForEnemy(BWAPI::Unit u);
		UnitInfo* createUnitInfo(BWAPI::Unit u);
		UnitInfo* getInfo(BWAPI::Unit u);
		PlayerInfo* getInfo(BWAPI::Player p);
		BaseInfo* getInfo(BWAPI::Position p);
		std::vector<UnitInfo*> friendlyUnits, neutralUnits, enemyUnits;
		bool removeFromList(UnitInfo* u);
		std::vector<PlayerInfo> players;
		//Builder builder;
		Fighter fighter;
	};
}