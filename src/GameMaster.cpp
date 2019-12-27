
#include "GameMaster.h"

int GameMaster::num_types = 0;
double **GameMaster::type_effectiveness = nullptr;
int *GameMaster::type_boosted_weathers = nullptr;
int GameMaster::min_stage = -4;
int GameMaster::max_stage = 4;
double *GameMaster::stage_multipliers = nullptr;

int GameMaster::max_energy = 100;
int GameMaster::dodge_duration = 500;
int GameMaster::dodge_window = 700;
int GameMaster::swap_duration = 1000;
int GameMaster::switching_cooldown = 60000;
int GameMaster::rejoin_duration = 10000;
int GameMaster::item_menu_animation_time = 2000;
int GameMaster::max_revive_time_per_pokemon = 1000;
double GameMaster::same_type_attack_bonus_multiplier = 1.2;
double GameMaster::weather_attack_bonus_multiplier = 1.2;
double GameMaster::pvp_fast_attack_bonus_multiplier = 1.3;
double GameMaster::pvp_charged_attack_bonus_multiplier = 1.3;
double GameMaster::dodge_damage_reduction_percent = 0.75;
double GameMaster::energy_delta_per_health_lost = 0.5;

// A dummay game master instance. When the program exits, its destructor is called,
// so we can delete dynamic arrays static members
// const GameMaster _game_master_dummy = GameMaster();

// But why bother? Program will free all its memory when exit anyway

GameMaster::~GameMaster()
{
	if (type_effectiveness)
	{
		for (int i = 0; i < num_types; ++i)
		{
			delete[] type_effectiveness[i];
		}
		delete[] type_effectiveness;
		type_effectiveness = nullptr;
	}
	if (type_boosted_weathers)
	{
		delete[] type_boosted_weathers;
		type_boosted_weathers = nullptr;
	}
	if (stage_multipliers)
	{
		delete[] stage_multipliers;
		stage_multipliers = nullptr;
	}
}

void GameMaster::set_num_types(int t_num_types)
{
	if (type_effectiveness)
	{
		for (int i = 0; i < num_types; ++i)
		{
			delete[] type_effectiveness[i];
		}
		delete[] type_effectiveness;
		type_effectiveness = nullptr;
		num_types = 0;
	}
	num_types = t_num_types;
	type_effectiveness = new double *[num_types];
	for (int i = 0; i < num_types; ++i)
	{
		type_effectiveness[i] = new double[num_types];
		for (int j = 0; j < num_types; ++j)
		{
			type_effectiveness[i][j] = 1;
		}
	}
	if (type_boosted_weathers)
	{
		delete[] type_boosted_weathers;
		type_boosted_weathers = nullptr;
	}
	type_boosted_weathers = new int[num_types];
	for (int i = 0; i < num_types; ++i)
	{
		type_boosted_weathers[i] = 0;
	}
}

void GameMaster::set_effectiveness(int t_type_i, int t_type_j, double t_multiplier)
{
	if (0 <= t_type_i && t_type_i < num_types && 0 <= t_type_j && t_type_j < num_types)
		type_effectiveness[t_type_i][t_type_j] = t_multiplier;
	else
		throw 1;
}

double GameMaster::get_effectiveness(int t_type_i, int t_type_j)
{
	if (t_type_i < 0 || t_type_j < 0 || t_type_i > num_types || t_type_j > num_types)
	{
		return 1;
	}
	else
	{
		return type_effectiveness[t_type_i][t_type_j];
	}
}

void GameMaster::set_type_boosted_weather(int t_type, int type_boosted_weather)
{
	if (0 <= t_type && t_type < num_types)
		type_boosted_weathers[t_type] = type_boosted_weather;
	else
		throw 1;
}

int GameMaster::get_type_boosted_weather(int t_type)
{
	if (0 <= t_type && t_type < num_types)
		return type_boosted_weathers[t_type];
	else
		throw 1;
}

void GameMaster::set_stage_bounds(int t_min_stage, int t_max_stage)
{
	if (t_min_stage > t_max_stage)
	{
		throw 1;
	}
	min_stage = t_min_stage;
	max_stage = t_max_stage;
	if (stage_multipliers)
	{
		delete[] stage_multipliers;
	}
	stage_multipliers = new double[max_stage - min_stage + 1];
}

void GameMaster::set_stage_multiplier(int t_stage, double t_multiplier)
{
	if (min_stage <= t_stage && t_stage <= max_stage)
	{
		stage_multipliers[t_stage - min_stage] = t_multiplier;
	}
	else
	{
		throw 1;
	}
}

double GameMaster::get_stage_multiplier(int t_stage)
{
	return stage_multipliers ? stage_multipliers[t_stage - min_stage] : 1;
}

void GameMaster::set_parameter(char *t_name, double t_value)
{
	if (strcmp(t_name, "max_energy") == 0)
		max_energy = t_value;
	else if (strcmp(t_name, "min_stage") == 0)
		min_stage = t_value;
	else if (strcmp(t_name, "max_stage") == 0)
		max_stage = t_value;
	else if (strcmp(t_name, "dodge_duration") == 0)
		dodge_duration = t_value;
	else if (strcmp(t_name, "dodge_window") == 0)
		dodge_window = t_value;
	else if (strcmp(t_name, "swap_duration") == 0)
		swap_duration = t_value;
	else if (strcmp(t_name, "switching_cooldown") == 0)
		switching_cooldown = t_value;
	else if (strcmp(t_name, "rejoin_duration") == 0)
		rejoin_duration = t_value;
	else if (strcmp(t_name, "item_menu_animation_time") == 0)
		item_menu_animation_time = t_value;
	else if (strcmp(t_name, "max_revive_time_per_pokemon") == 0)
		max_revive_time_per_pokemon = t_value;
	else if (strcmp(t_name, "same_type_attack_bonus_multiplier") == 0)
		same_type_attack_bonus_multiplier = t_value;
	else if (strcmp(t_name, "weather_attack_bonus_multiplier") == 0)
		weather_attack_bonus_multiplier = t_value;
	else if (strcmp(t_name, "pvp_fast_attack_bonus_multiplier") == 0)
		pvp_fast_attack_bonus_multiplier = t_value;
	else if (strcmp(t_name, "pvp_charged_attack_bonus_multiplier") == 0)
		pvp_charged_attack_bonus_multiplier = t_value;
	else if (strcmp(t_name, "dodge_damage_reduction_percent") == 0)
		dodge_damage_reduction_percent = t_value;
	else if (strcmp(t_name, "energy_delta_per_health_lost") == 0)
		energy_delta_per_health_lost = t_value;
}
