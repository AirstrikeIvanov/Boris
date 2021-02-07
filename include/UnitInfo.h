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
		void update();
		BWAPI::Position text(int x = 0, int y = 0);
		BWAPI::Position text(BWAPI::UnitType t, int y = 0);
		bool canSee(BWAPI::Position t);
		Unit unit = nullptr;
		UnitType type = UnitTypes::Unknown;
		int health = 0, shields = 0, energy = 0, resources = 0;
		Position position = BWAPI::Positions::None, target = BWAPI::Positions::None;
		Player owner = nullptr;

	private:

	};
}