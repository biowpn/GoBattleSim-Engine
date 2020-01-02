
#include "GoBattleSim_extern.h"
#include "GoBattleSim.h"
#include "json_converter.hpp"

#include <stdlib.h>

using namespace GoBattleSim;

void GBS_prepare(const char *input_j)
{
	SimInput input = nlohmann::json::parse(input_j);
	// for now, hardcode to no aggregation
	input.aggreation = AggregationMode::None;
	GoBattleSimApp::get().prepare(input);
}

void GBS_run()
{
	GoBattleSimApp::get().run();
}

void GBS_collect(char *output_j, int *output_j_len)
{
	// for now, hardcode to get last output
	SimOutput output;
	GoBattleSimApp::get().collect(output);

	auto j_str = nlohmann::json(output).dump(4);
	if (output_j_len != nullptr)
	{
		*output_j_len = j_str.size();
	}
	if (output_j != nullptr)
	{
		strcpy(output_j, j_str.c_str());
	}
}

void GBS_GameMaster_set(const char *gm_j)
{
	GameMaster::get() = nlohmann::json::parse(gm_j);
}

void GBS_GameMaster_get(char *gm_j, int *gm_j_len)
{
	auto j_str = nlohmann::json(GameMaster::get()).dump(4);
	if (gm_j_len != nullptr)
	{
		*gm_j_len = j_str.size();
	}
	if (gm_j != nullptr)
	{
		strcpy(gm_j, j_str.c_str());
	}
}
