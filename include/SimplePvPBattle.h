
#ifndef _SIMPLE_PVP_BATTLE_H_
#define _SIMPLE_PVP_BATTLE_H_

#include "PvPPokemon.h"
#include "PvPStrategy.h"
#include "TimelineEvent.h"

#include <vector>

namespace GoBattleSim
{

struct PvPPokemonState
{
	int hp;
	int energy;
	int cooldown;
	int shields;
	Action decision;
};

struct SimplePvPBattleOutcome
{
	double tdo_percent[2];

	// only available for non-branching mode
	int turns;
	PvPPokemonState pokemon_states[2];
	std::vector<TimelineEvent> battle_log;
};

class SimplePvPBattle
{
public:
	SimplePvPBattle() = default;
	SimplePvPBattle(const SimplePvPBattle &other);
	~SimplePvPBattle();

	void set_pokemon(const PvPPokemon &pkm1, const PvPPokemon &pkm2);
	void set_num_shields_max(int shields1, int shields2);
	void set_strategy(const PvPStrategy &strategy1, const PvPStrategy &strategy2);
	void set_enable_log(bool);
	void set_enable_branching(bool);

	void init();
	void start();
	SimplePvPBattleOutcome get_outcome();

protected:
	void register_action_fast(int, const Action &);
	void register_action_charged(int, const Action &);
	void decide_branch_move_effect(int, const MoveEffect &);
	void handle_move_effect(int, const MoveEffect &);

	void append_log(const TimelineEvent &);
	void erase_log();

	PvPStrategyInput generate_strat_input(int);

private:
	PvPPokemon m_pkm[2];
	int m_num_shields_max[2];
	PvPStrategy m_strategies[2];
	PvPPokemonState m_pkm_states[2];
	bool m_ended{false};
	int m_turn{0};

	bool m_enable_log{false};
	std::vector<TimelineEvent> m_battle_log;

	bool m_enable_branching{false};
	SimplePvPBattle *m_branch[2]{nullptr, nullptr};
	double m_branch_weight[2]{0.0, 0.0};
};

} // namespace GoBattleSim

#endif