
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

	// Shared Battle Settings
	unsigned num_types() const;
	unsigned num_types(unsigned);
	double effectiveness(int, int) const;
	double effectiveness(unsigned, unsigned, double);

	unsigned max_energy{100};
	double stab_multiplier{1.2};

	// PvE Settings (all time units are in milliseconds)
	int boosted_weather(unsigned, int);
	int boosted_weather(int) const;

	unsigned dodge_duration{500};
	unsigned dodge_window{700};
	unsigned swap_duration{1000};
	unsigned switching_cooldown{60000};
	unsigned rejoin_duration{10000};
	unsigned item_menu_time{2000};
	unsigned pokemon_revive_time{1000};
	unsigned fast_attack_lag{25};
	unsigned charged_attack_lag{100};
	double wab_multiplier{1.2};
	double dodge_damage_reduction_percent{0.75};
	double energy_delta_per_health_lost{0.5};

	// PvP Settings
	int min_stage{-4};
	int max_stage{4};
	double fast_attack_bonus_multiplier{1.3};
	double charged_attack_bonus_multiplier{1.3};

	void set_stage_bounds(int, int);
	double atk_stage_multiplier(int) const;
	double atk_stage_multiplier(int, double);
	double def_stage_multiplier(int) const;
	double def_stage_multiplier(int, double);

private:
	static GameMaster instance;

	unsigned m_num_types{0};
	double m_type_effectiveness[MAX_NUM_TYPES][MAX_NUM_TYPES];
	int m_type_boosted_weathers[MAX_NUM_TYPES];

	double m_atk_stage_multipliers[MAX_NUM_STAGES];
	double m_def_stage_multipliers[MAX_NUM_STAGES];
};

} // namespace GoBattleSim

#endif