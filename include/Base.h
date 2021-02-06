#pragma once
#include <BWAPI.h>
#include <vector>

namespace Boris
{
	struct Base
	{
	public:
		Base();
		std::vector<BWAPI::Unit> mineralWorkers;
		std::vector<BWAPI::Unit> gasWorkers;
		std::vector<BWAPI::Unit> minerals;
		std::vector<BWAPI::Unit> geysers;
		std::vector<BWAPI::Unit> buildings;
		bool spawn;
		bool owned;
		bool constructing;
		bool possible;
		BWAPI::Unit* mainDepot;
		BWAPI::Position loc;
		BWAPI::Unit scout;

	private:

	};
}