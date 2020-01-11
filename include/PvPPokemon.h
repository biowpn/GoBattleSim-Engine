
#ifndef _PVP_POKEMON_H_
#define _PVP_POKEMON_H_

#include "Pokemon.h"

namespace GoBattleSim
{

class PvPPokemon : public Pokemon
{
public:
	PvPPokemon() = default;
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
};

} // namespace GoBattleSim

#endif