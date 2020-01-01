
#include <iostream>
#include <chrono>
#include <assert.h>

#include "GameMaster.h"
#include "SimplePvPBattle.h"
#include "BattleMatrix.h"

using namespace GoBattleSim;

int main()
{
	std::cout << "setting gamemaster ... ";

	double stage_multipliers[9] = {0.5, 0.5714286, 0.66666669, 0.8, 1, 1.25, 1.5, 1.75, 2};
	GameMaster::get().set_stage_bounds(-4, 4);
	for (int i = -4; i <= 4; ++i)
	{
		GameMaster::get().stage_multiplier(i, stage_multipliers[i + 4]);
	}

	// In this demo, 0 = Dragon, 1 = Fighting, 2 = Steel, 3 = Ghost, 4 = Rock, 5 = Ground, 6 = Water
	GameMaster::get().num_types(7);
	GameMaster::get().effectiveness(0, 0, 1.6);
	GameMaster::get().effectiveness(0, 2, 1 / 1.6);
	GameMaster::get().effectiveness(1, 2, 1.6);
	GameMaster::get().effectiveness(1, 3, 1 / 1.6 / 1.6);
	GameMaster::get().effectiveness(2, 6, 1 / 1.6);
	GameMaster::get().effectiveness(3, 3, 1.6);
	GameMaster::get().effectiveness(4, 1, 1 / 1.6);
	GameMaster::get().effectiveness(4, 2, 1 / 1.6);
	GameMaster::get().effectiveness(4, 5, 1 / 1.6);
	GameMaster::get().effectiveness(5, 2, 1.6);
	GameMaster::get().effectiveness(5, 4, 1.6);
	GameMaster::get().effectiveness(6, 0, 1 / 1.6);
	GameMaster::get().effectiveness(6, 4, 1.6);
	GameMaster::get().effectiveness(6, 5, 1.6);
	GameMaster::get().effectiveness(6, 6, 1 / 1.6);

	std::cout << "done" << std::endl;

	std::cout << "defining moves ... ";

	// (poketype, power, energy, duration (in turns), dws, move effect)
	Move move_counter{1, 8, 7, 2};
	Move move_shadow_claw{3, 6, 8, 2};
	Move move_mud_shot{5, 3, 9, 2};
	Move move_dragon_breath{0, 4, 3, 1};
	Move move_bubble{6, 8, 11, 3};

	Move move_power_up_punch{1, 40, -35, 0, 0, {1, 1}};
	Move move_shadow_ball{3, 100, -55};
	Move move_dragon_claw{0, 50, -35};
	Move move_ancient_power{4, 70, -45, 0, 0, {0.1, 2, 2}};
	Move move_iron_head{2, 70, -50};
	Move move_earthquake{5, 120, -65};

	std::cout << "done" << std::endl;

	std::cout << "defining pokemon ... ";

	// (poketyp1, poketyp2, attack, defense, max_hp)
	PvPPokemon pokemon_lucario(1, 2, 190.39096, 121.08865056, 142);
	pokemon_lucario.add_fmove(&move_counter);
	pokemon_lucario.add_cmove(&move_power_up_punch);
	//pokemon_lucario.add_cmove(&move_shadow_ball);

	PvPPokemon pokemon_groudon(5, -1, 225.23550285000002, 192.04290243000003, 173);
	pokemon_groudon.add_fmove(&move_mud_shot);
	pokemon_groudon.add_cmove(&move_earthquake);

	PvPPokemon pokemon_dialga(0, 2, 229.1870029, 178.60780226, 173);
	pokemon_dialga.add_fmove(&move_dragon_breath);
	pokemon_dialga.add_cmove(&move_iron_head);

	PvPPokemon pokemon_giratina_altered(0, 3, 137.59531384, 162.11725095999998, 203);
	pokemon_giratina_altered.add_fmove(&move_shadow_claw);
	pokemon_giratina_altered.add_cmove(&move_ancient_power);
	//pokemon_giratina_altered.add_cmove(&move_dragon_claw);

	PvPPokemon pokemon_poliwrath(1, 6, 119.149306358, 120.35894398600001, 131);
	pokemon_poliwrath.add_fmove(&move_bubble);
	pokemon_poliwrath.add_cmove(&move_power_up_punch);

	std::cout << "done" << std::endl;

	std::cout << "testing simple battle ... ";

	SimplePvPBattle battle = SimplePvPBattle(&pokemon_lucario, &pokemon_giratina_altered);
	battle.set_num_shields_max(0, 0);
	battle.init();
	battle.start();
	SimplePvPBattleOutcome outcome = battle.get_outcome();

	assert(outcome.tdo_percent[0] > 0.5);
	assert(outcome.tdo_percent[0] < 0.6);
	assert(outcome.tdo_percent[1] >= 1.0);

	std::cout << "success" << std::endl;
	std::cout << outcome.tdo_percent[0] << ", " << outcome.tdo_percent[1] << std::endl;

	std::cout << "testing battle score ... ";

	double score = get_battle_score(&pokemon_lucario, &pokemon_giratina_altered, 0, 0);
	assert(score == outcome.tdo_percent[0] - outcome.tdo_percent[1]);

	std::cout << "success" << std::endl;

	std::cout << "testing battle matrix ... ";

	const PvPPokemon *pkm_list[5] = {
		&pokemon_groudon,
		&pokemon_dialga,
		&pokemon_lucario,
		&pokemon_giratina_altered,
		&pokemon_poliwrath,
	};

	BattleMatrix bm(pkm_list, 5, pkm_list, 5, false);
	bm.run();

	double matrix[25];
	bm.get(matrix);

	std::cout << "success" << std::endl;

	for (unsigned i = 0; i < 5; ++i)
	{
		for (unsigned j = 0; j < 5; ++j)
		{
			printf("%+.04f ", matrix[i * 5 + j]);
		}
		std::cout << std::endl;
	}

	constexpr unsigned num_sims = 10000;
	std::cout << "testing " << num_sims << " battles ... ";

	auto start = std::chrono::high_resolution_clock::now();
	for (unsigned i = 0; i < num_sims; ++i)
	{
		SimplePvPBattle battle(&pokemon_lucario, &pokemon_giratina_altered);
		battle.init();
		battle.start();
		auto outcome = battle.get_outcome();
	}
	auto finish = std::chrono::high_resolution_clock::now();

	std::cout << "success" << std::endl;

	auto duration = finish - start;
	std::cout << "time elapsed (ms) = " << duration.count() / 1000000.0 << std::endl;

	return 0;
}
