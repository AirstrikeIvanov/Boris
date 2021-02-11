#pragma once
#include <BWAPI.h>
#include <vector>

using namespace BWAPI;

namespace Boris
{
	class UnitInfo
	{
	public:
		bool isDepot() { return type.isResourceDepot(); }
		bool isWorker() { return type.isWorker(); }
		bool isBuilding() { return type.isBuilding(); }
		bool isSupply() { return type == UnitTypes::Zerg_Overlord || type == UnitTypes::Terran_Supply_Depot || type == UnitTypes::Protoss_Pylon; }
		bool isLightAir() { return type == UnitTypes::Terran_Wraith || type == UnitTypes::Protoss_Scout || type == UnitTypes::Zerg_Mutalisk; }
		bool isCapital() { return type == UnitTypes::Zerg_Guardian || type == UnitTypes::Protoss_Carrier || type == UnitTypes::Terran_Battlecruiser; }
		bool isTransport() { return type == UnitTypes::Zerg_Overlord || type == UnitTypes::Terran_Dropship || type == UnitTypes::Protoss_Shuttle; }
		bool isAirSuperiority() { return type == UnitTypes::Terran_Valkyrie || type == UnitTypes::Protoss_Corsair || type == UnitTypes::Zerg_Devourer; }
		bool isCaster() { return type.isSpellcaster(); }

		UnitInfo(Unit u);
		UnitInfo(const UnitInfo&) = delete;
		UnitInfo& operator= (const UnitInfo&) = delete;

		void drawBars(bool unit = true);
		void update();
		BWAPI::Position text(int x = 0, int y = 0);
		BWAPI::Position text(BWAPI::UnitType t, int y = 0);
		BWAPI::Position box();
		bool canSee(BWAPI::Position t);
		bool isAssigned = false;
		Unit unit = nullptr;
		UnitType type = UnitTypes::Unknown;
		int id = -1, health = 0, shields = 0, energy = 0, resources = 0;
		Position position = BWAPI::Positions::None, target = BWAPI::Positions::None;
		Player owner = nullptr;

	private:

	};
}