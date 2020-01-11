
#include "PvPPokemon.h"

#include "GameMaster.h"

namespace GoBattleSim
{

PvPPokemon::PvPPokemon(int t_poketype1, int t_poketype2, double t_attack, double t_defense, int t_max_hp)
	: Pokemon(t_poketype1, t_poketype2, t_attack, t_defense, t_max_hp)
{
	attack_stage = 0;
	defense_stage = 0;
	attack_init = attack;
	defense_init = defense;
	num_shields_max = 0;
}

PvPPokemon::PvPPokemon(const PvPPokemon &other)
	: Pokemon(other)
{
	attack_stage = other.attack_stage;
	defense_stage = other.defense_stage;
	attack_init = other.attack_init;
	defense_init = other.defense_init;
	num_shields_max = other.num_shields_max;
}

PvPPokemon::~PvPPokemon()
{
}

void PvPPokemon::init()
{
	attack_stage = 0;
	defense_stage = 0;
	attack = attack_init;
	defense = defense_init;
}

void PvPPokemon::buff(int t_attack_stage_delta, int t_defense_stage_delta)
{
	attack_stage += t_attack_stage_delta;
	attack_stage = attack_stage > GameMaster::get().max_stage ? GameMaster::get().max_stage : attack_stage;
	attack_stage = attack_stage < GameMaster::get().min_stage ? GameMaster::get().min_stage : attack_stage;
	attack = attack_init * GameMaster::get().atk_stage_multiplier(attack_stage);

	defense_stage += t_defense_stage_delta;
	defense_stage = defense_stage > GameMaster::get().max_stage ? GameMaster::get().max_stage : defense_stage;
	defense_stage = defense_stage < GameMaster::get().min_stage ? GameMaster::get().min_stage : defense_stage;
	defense = defense_init * GameMaster::get().def_stage_multiplier(defense_stage);
}

} // namespace GoBattleSim
