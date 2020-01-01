
#ifndef _GAME_MASTER_H_
#define _GAME_MASTER_H_

namespace GoBattleSim
{
extern char err_msg[256];

constexpr unsigned MAX_NUM_TYPES = 20;
constexpr unsigned MAX_NUM_STAGES = 16;

class GameMaster
{
public:
	static GameMaster &get();

	GameMaster();

	// Some battle parameters are accessed via setters/getters (for bound checks)
	// setters
	unsigned num_types(unsigned);
	double effectiveness(unsigned, unsigned, double);
	int type_boosted_weather(unsigned, int);
	double stage_multiplier(int, double);

	// special setters who don't have their getter counterpart
	void set_stage_bounds(int, int);
	void set_parameter(char *, double);

	// getters
	unsigned num_types() const;
	double effectiveness(int, int) const;
	int boosted_weather(int) const;
	double stage_multiplier(int) const;

	// Other simple battle parameters are just public members for direct access
	int max_energy{100};
	int min_stage{-4};
	int max_stage{4};
	int dodge_duration{500};
	int dodge_window{700};
	int swap_duration{100};
	int switching_cooldown{60000};
	int rejoin_duration{10000};
	int item_menu_animation_time{2000};
	int max_revive_time_per_pokemon{1000};
	double same_type_attack_bonus_multiplier{1.2};
	double weather_attack_bonus_multiplier{1.2};
	double pvp_fast_attack_bonus_multiplier{1.3};
	double pvp_charged_attack_bonus_multiplier{1.3};
	double dodge_damage_reduction_percent{0.75};
	double energy_delta_per_health_lost{0.5};

private:
	static GameMaster instance;

	int m_num_types{0};
	double m_type_effectiveness[MAX_NUM_TYPES][MAX_NUM_TYPES];
	int m_type_boosted_weathers[MAX_NUM_TYPES];

	double m_stage_multipliers[MAX_NUM_STAGES];
};

} // namespace GoBattleSim

#endif