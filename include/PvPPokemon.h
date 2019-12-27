
#ifndef _PVP_POKEMON_H_
#define _PVP_POKEMON_H_

#include "Pokemon.h"

// Damage calculation for pvp as public functions
int calc_damage_pvp_fmove(const Pokemon *, const Move *, const Pokemon *);

int calc_damage_pvp_cmove(const Pokemon *, const Move *, const Pokemon *);

class PvPPokemon : public Pokemon
{
public:
	PvPPokemon(int, int, double, double, int);
	PvPPokemon(const PvPPokemon &);
	~PvPPokemon();

	void init();
	void buff(int, int);

	double attack_init;
	double defense_init;
	int attack_stage;
	int defense_stage;

	int num_shields_max;

protected:
	// Don't use virtual functions to slow things down
	// int* search_int_member(const char*);
};

#endif