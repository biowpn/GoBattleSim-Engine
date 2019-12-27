
#include <iostream>
#include <chrono>
#include <assert.h>

#include "SimplePvPBattle.h"
#include "BattleMatrix.h"

int main()
{
	std::cout << "\nSimple PvP Battle Test:" << std::endl;

	double stage_multipliers[9] = {0.5, 0.5714286, 0.66666669, 0.8, 1, 1.25, 1.5, 1.75, 2};
	GameMaster::set_stage_bounds(-4, 4);
	for (int i = -4; i <= 4; ++i)
	{
		GameMaster::set_stage_multiplier(i, stage_multipliers[i + 4]);
	}

	// In this demo, 0 = Dragon, 1 = Fighting, 2 = Steel, 3 = Ghost, 4 = Rock, 5 = Ground, 6 = Water
	GameMaster::set_num_types(7);
	GameMaster::set_effectiveness(0, 0, 1.6);
	GameMaster::set_effectiveness(0, 2, 1 / 1.6);
	GameMaster::set_effectiveness(1, 2, 1.6);
	GameMaster::set_effectiveness(1, 3, 1 / 1.6 / 1.6);
	GameMaster::set_effectiveness(2, 6, 1 / 1.6);
	GameMaster::set_effectiveness(3, 3, 1.6);
	GameMaster::set_effectiveness(4, 1, 1 / 1.6);
	GameMaster::set_effectiveness(4, 2, 1 / 1.6);
	GameMaster::set_effectiveness(4, 5, 1 / 1.6);
	GameMaster::set_effectiveness(5, 2, 1.6);
	GameMaster::set_effectiveness(5, 4, 1.6);
	GameMaster::set_effectiveness(6, 0, 1 / 1.6);
	GameMaster::set_effectiveness(6, 4, 1.6);
	GameMaster::set_effectiveness(6, 5, 1.6);
	GameMaster::set_effectiveness(6, 6, 1 / 1.6);

	// Set up PvP Moves
	// (poketype, power, energy, duration (in turns), dws, move effect)
	Move move_counter = Move(1, 8, 7, 2);
	Move move_shadow_claw = Move(3, 6, 8, 2);
	Move move_mud_shot = Move(5, 3, 9, 2);
	Move move_dragon_breath = Move(0, 4, 3, 1);
	Move move_bubble = Move(6, 8, 11, 3);

	Move move_power_up_punch = Move(1, 40, -35, 0, 0, {1, 1});
	Move move_shadow_ball = Move(3, 100, -55);
	Move move_dragon_claw = Move(0, 50, -35);
	Move move_ancient_power = Move(4, 70, -45, 0, 0, {0.1, 2, 2});
	Move move_iron_head = Move(2, 70, -50);
	Move move_earthquake = Move(5, 120, -65);

	// Set up Pokemon
	// (poketyp1, poketyp2, attack, defense, max_hp)
	PvPPokemon pokemon_lucario = PvPPokemon(1, 2, 190.39096, 121.08865056, 142);
	pokemon_lucario.add_fmove(&move_counter);
	pokemon_lucario.add_cmove(&move_power_up_punch);
	//pokemon_lucario.add_cmove(&move_shadow_ball);

	PvPPokemon pokemon_groudon = PvPPokemon(5, -1, 225.23550285000002, 192.04290243000003, 173);
	pokemon_groudon.add_fmove(&move_mud_shot);
	pokemon_groudon.add_cmove(&move_earthquake);

	PvPPokemon pokemon_dialga = PvPPokemon(0, 2, 229.1870029, 178.60780226, 173);
	pokemon_dialga.add_fmove(&move_dragon_breath);
	pokemon_dialga.add_cmove(&move_iron_head);

	PvPPokemon pokemon_giratina_altered = PvPPokemon(0, 3, 137.59531384, 162.11725095999998, 203);
	pokemon_giratina_altered.add_fmove(&move_shadow_claw);
	pokemon_giratina_altered.add_cmove(&move_ancient_power);
	//pokemon_giratina_altered.add_cmove(&move_dragon_claw);

	PvPPokemon pokemon_poliwrath = PvPPokemon(1, 6, 119.149306358, 120.35894398600001, 131);
	pokemon_poliwrath.add_fmove(&move_bubble);
	pokemon_poliwrath.add_cmove(&move_power_up_punch);

	SimplePvPBattle battle = SimplePvPBattle(&pokemon_lucario, &pokemon_giratina_altered);
	battle.set_num_shields_max(0, 0);
	battle.init();
	battle.start();
	SimplePvPBattleOutcome outcome = battle.get_outcome();
	std::cout << outcome.tdo_percent[0] << " ; " << outcome.tdo_percent[1] << std::endl;

	std::cout << "\nTesting battle score..." << std::endl;

	double score = get_battle_score(&pokemon_lucario, &pokemon_giratina_altered, 0, 0);
	std::cout << score << std::endl;

	std::cout << "Battle score completed\n"
			  << std::endl;

	std::cout << "\nTesting battle matrix..." << std::endl;

	const PvPPokemon **pkm_list = new const PvPPokemon *[5];
	pkm_list[0] = &pokemon_groudon;
	pkm_list[1] = &pokemon_dialga;
	pkm_list[2] = &pokemon_lucario;
	pkm_list[3] = &pokemon_giratina_altered;
	pkm_list[4] = &pokemon_poliwrath;

	BattleMatrix bm(pkm_list, 5, pkm_list, 5, false);
	bm.run();

	double **matrix = new double *[5];
	for (int i = 0; i < 5; ++i)
		matrix[i] = new double[5];
	bm.get(matrix);

	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 5; ++j)
		{
			std::cout << matrix[i][j] << " , ";
		}
		std::cout << std::endl;
	}

	std::cout << "Battle matrix completed\n"
			  << std::endl;

	// Construct and run battle
	const int num_sims = 10000;
	std::cout << "Simulating " << num_sims << " PvP battles..." << std::endl;
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	for (int i = 0; i < num_sims; ++i)
	{
		SimplePvPBattle battle = SimplePvPBattle(&pokemon_lucario, &pokemon_giratina_altered);
		battle.init();
		battle.start();
		SimplePvPBattleOutcome outcome = battle.get_outcome();
	}
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Time elapsed (s) = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0 << std::endl;

	std::cout << "Simple PvP Battle Test passed" << std::endl;

	return 0;
}