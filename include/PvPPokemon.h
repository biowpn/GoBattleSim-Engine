
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

	double attack_init{0.0};
	double defense_init{0.0};
	int attack_stage{0};
	int defense_stage{0};

	int num_shields_max{0};
};

} // namespace GoBattleSim

#endif