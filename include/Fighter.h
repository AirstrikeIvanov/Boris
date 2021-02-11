#pragma once
#include <BWAPI.h>
#include <vector>
#include <map>

#include "UnitInfo.h"
#include "PlayerInfo.h"
#include "BaseInfo.h"

using namespace BWAPI;

namespace Boris
{
	class UnitPriorities
	{
	public:
		std::unordered_map<int, int> structures;
		void onStart();
	};

	class Squad
	{
	public:
		Squad(UnitInfo* u);
		Squad(const Squad&) = delete;
		Squad& operator= (const Squad&) = delete;
		std::vector<UnitInfo*> units;
		float strength;
		int order; //placeholder, not meant to be an int
		bool containsUnit(UnitInfo* u);
		void addToSquad(UnitInfo* u);
		bool removeFromSquad(UnitInfo* u);
		UnitType type;
	};

	struct Fighter
	{
	public:
		UnitPriorities priorities;
		std::unordered_map<int, Squad*> squads;
		Unit getTarget(UnitInfo* u, std::vector<UnitInfo*> list, Position t, int radius = 64);
		Unit getTarget(Squad* squad, std::vector<UnitInfo*> list, Position t, int radius = 64);
		BWAPI::Position getTargetPos(UnitInfo* u);
		BWAPI::Position getTargetPos(Squad* squad);
		BWAPI::Position getTargetFar(UnitInfo* u);
		BWAPI::Position getTargetFar(Squad* squad);
		Squad* getSquad(UnitInfo* u);
		Squad* getSquad(UnitType t);
		bool assignSquad(UnitInfo* u);
		//bool removeFromList(Squad* s);
		void onStart();
		void onUnitDestroy(UnitInfo* u);
		void drawInfo();
	};
}