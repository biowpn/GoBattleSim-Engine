
#ifndef _POKEMON_STATE_H_
#define _POKEMON_STATE_H_

namespace GoBattleSim
{

struct PokemonState
{
	bool active{false};
	bool immortal{false};
	int max_hp{0};
	int hp{0};
	int energy{0};
	unsigned damage_reduction_expiry{0};
	unsigned tdo{0};
	unsigned tdo_fast{0};
	unsigned duration{0};
	unsigned num_deaths{0};
	unsigned num_fmoves_used{0};
	unsigned num_cmoves_used{0};

	void init();
	void heal();
	bool is_alive() const;
	void charge(int);
	void hurt(int);
	void attribute_damage(unsigned, bool);
};

} // namespace GoBattleSim

#endif
