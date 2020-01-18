
#include <iostream>
#include <chrono>
#include <assert.h>

#include "GameMaster.h"
#include "SimplePvPBattle.h"

using namespace GoBattleSim;

int main()
{
	std::cout << "setting gamemaster ... ";

	// In this demo, 0 = Grass, 1 = Ground
	GameMaster::get().num_types(2);
	GameMaster::get().effectiveness(0, 1, 1.6);

	std::cout << "done" << std::endl;

	std::cout << "defining moves ... ";

	// (poketype, power, energy, duration (in turns), dws, move effect)
	Move move_mud_shot{1, 3, 9, 2};

	Move move_solar_beam{0, 150, -80};

	std::cout << "done" << std::endl;

	std::cout << "defining pokemon ... ";

	// (poketyp1, poketyp2, attack, defense, max_hp)
	PvPPokemon pokemon_groudon(1, -1, 225.2355, 192.0429, 173);
	pokemon_groudon.add_fmove(&move_mud_shot);
	pokemon_groudon.add_cmove(&move_solar_beam);

	PvPPokemon pokemon_groudon2(1, -1, 226.0, 192.0429, 173); // higher attack
	pokemon_groudon2.add_fmove(&move_mud_shot);
	pokemon_groudon2.add_cmove(&move_solar_beam);

	std::cout << "done" << std::endl;

	std::cout << "testing higher attack having priority ... " << std::endl;

	SimplePvPBattle battle;
	battle.set_pokemon(pokemon_groudon, pokemon_groudon2);
	battle.set_num_shields_max(0, 0);
	battle.set_enable_branching(false);
	battle.init();
	battle.start();
	auto outcome = battle.get_outcome();

	std::cout << outcome.tdo_percent[0] << ", " << outcome.tdo_percent[1] << std::endl;
	assert(outcome.tdo_percent[0] < 1);
	assert(outcome.tdo_percent[1] >= 1);

	battle.set_pokemon(pokemon_groudon2, pokemon_groudon);
	battle.init();
	battle.start();
	outcome = battle.get_outcome();

	std::cout << outcome.tdo_percent[0] << ", " << outcome.tdo_percent[1] << std::endl;
	assert(outcome.tdo_percent[0] >= 1);
	assert(outcome.tdo_percent[1] < 1);

	std::cout << "success" << std::endl;

	std::cout << "testing equal attack & non-branching ... " << std::endl;
	battle.set_pokemon(pokemon_groudon, pokemon_groudon);
	battle.set_enable_branching(false);
	double tdo_s0 = 0, tdo_s1 = 0;
	constexpr unsigned num_sims = 10000;
	for (unsigned i = 0; i < num_sims; ++i)
	{
		battle.set_pokemon(pokemon_groudon, pokemon_groudon);
		battle.init();
		battle.start();
		auto outcome = battle.get_outcome();
		tdo_s0 += outcome.tdo_percent[0];
		tdo_s1 += outcome.tdo_percent[1];
	}
	tdo_s0 /= num_sims;
	tdo_s1 /= num_sims;

	std::cout << tdo_s0 << "," << tdo_s1 << std::endl;
	assert(std::abs(tdo_s0 - tdo_s1) < 0.02);

	std::cout << "success" << std::endl;

	std::cout << "testing equal attack & branching ... " << std::endl;

	battle.set_enable_branching(true);
	battle.init();
	battle.start();
	outcome = battle.get_outcome();

	std::cout << outcome.tdo_percent[0] << ", " << outcome.tdo_percent[1] << std::endl;
	assert(outcome.tdo_percent[0] == outcome.tdo_percent[1]);

	std::cout << "success" << std::endl;

	return 0;
}
