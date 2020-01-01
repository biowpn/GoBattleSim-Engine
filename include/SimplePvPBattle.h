
#ifndef _SIMPLE_PVP_BATTLE_H_
#define _SIMPLE_PVP_BATTLE_H_

#include "PvPPokemon.h"
#include "PvPStrategy.h"

namespace GoBattleSim
{

struct SimplePvPBattleOutcome
{
	double tdo_percent[2];
};

class SimplePvPBattle
{
public:
	SimplePvPBattle() = default;
	SimplePvPBattle(const PvPPokemon *, const PvPPokemon *);
	SimplePvPBattle(const SimplePvPBattle &other);
	~SimplePvPBattle();

	// Interface functions
	void set_pokemon(const PvPPokemon *, const PvPPokemon *);
	void set_num_shields_max(int, int);
	void set_strategy(int, const PvPStrategy &);

	void init();
	void start();
	SimplePvPBattleOutcome get_outcome();
	// End of Interface functions

protected:
	struct PokemonState
	{
		int hp;
		int energy;
		int cooldown;
		int shields;
		Action decision;
	};

	void register_action_fast(int, const Action &);
	void register_action_charged(int, const Action &);
	void handle_move_effect(int, const MoveEffect &);

	PvPStrategyInput generate_strat_input(int);

	const PvPPokemon *m_pokemon[2];
	bool m_own_pokemon;
	int m_num_shields_max[2];
	PvPStrategy m_strategies[2];
	PokemonState m_pkms[2];
	bool m_ended;

	SimplePvPBattle *m_branch[2];
	double m_branch_weight[2];
};

} // namespace GoBattleSim

#endif