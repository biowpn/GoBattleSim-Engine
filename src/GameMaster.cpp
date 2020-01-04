
#include "GameMaster.h"

#include <string.h>
#include <stdexcept>
#include <stdio.h>

namespace GoBattleSim
{
char err_msg[256];

GameMaster GameMaster::instance;

GameMaster &GameMaster::get()
{
	return instance;
}

GameMaster::GameMaster()
{
	for (unsigned i = 0; i < MAX_NUM_STAGES; ++i)
	{
		m_atk_stage_multipliers[i] = 1.0;
		m_def_stage_multipliers[i] = 1.0;
	}
	for (unsigned i = 0; i < MAX_NUM_TYPES; ++i)
	{
		for (unsigned j = 0; j < MAX_NUM_TYPES; ++j)
		{
			m_type_effectiveness[i][j] = 1.0;
		}
		m_type_boosted_weathers[i] = -1;
	}
}

unsigned GameMaster::num_types(unsigned t_num_types)
{
	if (t_num_types >= MAX_NUM_TYPES)
	{
		sprintf(err_msg, "too many types (%d, max %d)", t_num_types, MAX_NUM_TYPES);
		throw std::runtime_error(err_msg);
	}
	return m_num_types = t_num_types;
}

double GameMaster::effectiveness(unsigned t_type_i, unsigned t_type_j, double t_multiplier)
{
	if (t_type_i < m_num_types && t_type_j < m_num_types)
	{
		m_type_effectiveness[t_type_i][t_type_j] = t_multiplier;
	}
	else if (t_type_i >= m_num_types)
	{
		sprintf(err_msg, "invalid first type index (%d)", t_type_i);
		throw std::runtime_error(err_msg);
	}
	else if (t_type_j >= m_num_types)
	{
		sprintf(err_msg, "invalid second type index (%d)", t_type_j);
		throw std::runtime_error(err_msg);
	}
	return t_multiplier;
}

unsigned GameMaster::num_types() const
{
	return m_num_types;
}

double GameMaster::effectiveness(int t_type_i, int t_type_j) const
{
	if (t_type_i < 0 || t_type_j < 0 || t_type_i > m_num_types || t_type_j > m_num_types)
	{
		return 1;
	}
	else
	{
		return m_type_effectiveness[t_type_i][t_type_j];
	}
}

int GameMaster::boosted_weather(unsigned t_type, int t_weather)
{
	if (0 <= t_type && t_type < m_num_types)
	{
		m_type_boosted_weathers[t_type] = t_weather;
	}
	else
	{
		sprintf(err_msg, "invalid type index (%d)", t_type);
		throw std::runtime_error(err_msg);
	}
	return t_weather;
}

int GameMaster::boosted_weather(int t_type) const
{
	if (0 <= t_type && t_type < m_num_types)
	{
		return m_type_boosted_weathers[t_type];
	}
	else
	{
		return 1;
	}
}

void GameMaster::set_stage_bounds(int t_min_stage, int t_max_stage)
{
	if (t_min_stage > t_max_stage)
	{
		sprintf(err_msg, "min_stage (%d) > max_stage (%d)", t_min_stage, t_max_stage);
		throw std::runtime_error(err_msg);
	}
	auto num_stages = t_max_stage - t_min_stage + 1;
	if (m_num_types > MAX_NUM_STAGES)
	{
		sprintf(err_msg, "too many stages (%d, max %d)", num_stages, MAX_NUM_STAGES);
		throw std::runtime_error(err_msg);
	}
	min_stage = t_min_stage;
	max_stage = t_max_stage;
}

double GameMaster::atk_stage_multiplier(int t_stage, double t_multiplier)
{
	if (min_stage <= t_stage && t_stage <= max_stage)
	{
		m_atk_stage_multipliers[t_stage - min_stage] = t_multiplier;
	}
	else
	{
		sprintf(err_msg, "invalid stage (%d, min %d, max %d)", t_stage, min_stage, max_stage);
		throw std::runtime_error(err_msg);
	}
	return t_multiplier;
}

double GameMaster::def_stage_multiplier(int t_stage, double t_multiplier)
{
	if (min_stage <= t_stage && t_stage <= max_stage)
	{
		m_def_stage_multipliers[t_stage - min_stage] = t_multiplier;
	}
	else
	{
		sprintf(err_msg, "invalid stage (%d, min %d, max %d)", t_stage, min_stage, max_stage);
		throw std::runtime_error(err_msg);
	}
	return t_multiplier;
}

double GameMaster::atk_stage_multiplier(int t_stage) const
{
	if (t_stage < min_stage || t_stage > max_stage)
	{
		sprintf(err_msg, "invalid stage (%d, min %d, max %d)", t_stage, min_stage, max_stage);
		throw std::runtime_error(err_msg);
	}
	return m_atk_stage_multipliers[t_stage - min_stage];
}

double GameMaster::def_stage_multiplier(int t_stage) const
{
	if (t_stage < min_stage || t_stage > max_stage)
	{
		sprintf(err_msg, "invalid stage (%d, min %d, max %d)", t_stage, min_stage, max_stage);
		throw std::runtime_error(err_msg);
	}
	return m_def_stage_multipliers[t_stage - min_stage];
}

} // namespace GoBattleSim
