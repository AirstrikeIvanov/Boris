#pragma once
#include <BWAPI.h>
#include <vector>

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
	BWAPI::Unit* mainDepot;

private:

};