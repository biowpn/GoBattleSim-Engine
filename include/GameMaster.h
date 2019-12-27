
#ifndef _GAME_MASTER_H_
#define _GAME_MASTER_H_

#include <string.h>

class GameMaster
{
public:
	~GameMaster();

	// Interface functions
	static void set_num_types(int);
	static void set_effectiveness(int, int, double);
	static void set_type_boosted_weather(int, int);
	static void set_stage_bounds(int, int);
	static void set_stage_multiplier(int, double);
	static void set_parameter(char *, double);

	// Array-like parameter accessors
	static double get_effectiveness(int, int);
	static int get_type_boosted_weather(int);
	static double get_stage_multiplier(int);

	// Simple battle parameters are public and available for direct access
	static int max_energy;
	static int min_stage;
	static int max_stage;
	static int dodge_duration;
	static int dodge_window;
	static int swap_duration;
	static int switching_cooldown;
	static int rejoin_duration;
	static int item_menu_animation_time;
	static int max_revive_time_per_pokemon;
	static double same_type_attack_bonus_multiplier;
	static double weather_attack_bonus_multiplier;
	static double pvp_fast_attack_bonus_multiplier;
	static double pvp_charged_attack_bonus_multiplier;
	static double dodge_damage_reduction_percent;
	static double energy_delta_per_health_lost;

private:
	static int num_types;

	static double **type_effectiveness;
	static int *type_boosted_weathers;

	static double *stage_multipliers;
};

#endif