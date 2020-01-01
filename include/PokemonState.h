
#ifndef _POKEMON_STATE_H_
#define _POKEMON_STATE_H_

struct PokemonState
{
	bool active{false};
	bool immortal{false};
	int max_hp{0};
	int hp{0};
	int energy{0};
	int damage_reduction_expiry{0};
	int tdo;
	int tdo_fast;
	int duration;
	int num_deaths;
	int num_fmoves_used;
	int num_cmoves_used;

	void init();
	void heal();
	bool is_alive() const;
	void charge(int);
	void hurt(int);
	void attribute_damage(int, bool);
};

#endif
