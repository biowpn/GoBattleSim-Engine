
#include "PokemonState.h"

#include "GameMaster.h"

using namespace GoBattleSim;

void PokemonState::init()
{
    heal();
    active = false;
    hp = max_hp;
    energy = 0;
    damage_reduction_expiry = 0;
    tdo = 0;
    tdo_fast = 0;
    duration = 0;
    num_deaths = 0;
    num_fmoves_used = 0;
    num_cmoves_used = 0;
}

void PokemonState::heal()
{
    hp = max_hp;
    energy = 0;
}

bool PokemonState::is_alive() const
{
    return hp > 0 || immortal;
}

void PokemonState::charge(int t_energy_delta)
{
    energy += t_energy_delta;
    if (energy > static_cast<int>(GameMaster::get().max_energy))
    {
        energy = GameMaster::get().max_energy;
    }
}

void PokemonState::hurt(int t_damage)
{
    hp -= t_damage;
}

void PokemonState::attribute_damage(int t_damage, bool t_is_fmove)
{
    tdo += t_damage;
    if (t_is_fmove)
    {
        tdo_fast += t_damage;
    }
}
