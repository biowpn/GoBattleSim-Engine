
#include <iostream>
#include <chrono>
#include <assert.h>

#include "Battle.h"

using namespace GoBattleSim;

int main()
{
	std::cout << "\nRaid Battle Test:" << std::endl;

	// T3 Machamp 1v1 Solo

	// In this demo, 0 = Psychic, 1 = Fighting, 2 = Steel, 3 = Flying
	GameMaster::set_num_types(4);
	GameMaster::set_effectiveness(0, 1, 1.6);
	GameMaster::set_effectiveness(0, 2, 1 / 1.6);
	GameMaster::set_effectiveness(1, 0, 1 / 1.6);
	GameMaster::set_effectiveness(1, 2, 1.6);
	GameMaster::set_effectiveness(1, 3, 1 / 1.6);
	GameMaster::set_effectiveness(3, 1, 1.6);
	GameMaster::set_effectiveness(3, 2, 1 / 1.6);
	GameMaster::set_type_boosted_weather(0, 1);
	GameMaster::set_type_boosted_weather(3, 1);

	// Set up moves
	// (poketype, power, energy, duration, damage_window_start)
	Move move_confusion = Move{0, 20, 15, 1600, 600};
	Move move_zen_headbutt = Move{0, 12, 10, 1100, 850};
	Move move_extrasensory = Move{0, 12, 12, 1100, 850};
	Move move_counter = Move{1, 12, 8, 900, 700};
	Move move_bullet_punch = Move{2, 9, 10, 900, 300};
	Move move_dynamic_punch = Move{1, 90, -50, 2700, 1200};
	Move move_psychic = Move{0, 100, -100, 2800, 1300};
	Move move_sky_attack = Move{3, 80, -50, 2000, 1500};

	// Set up Pokemon
	// (poketyp1, poketyp2, attack, defense, max_hp)
	Pokemon pokemon_mewtwo = Pokemon(0, -1, 248.94450315, 155.68910197000002, 180);
	pokemon_mewtwo.add_fmove(&move_confusion);
	pokemon_mewtwo.add_cmove(&move_psychic);

	Pokemon pokemon_latios = Pokemon(0, -1, 223.65490283000003, 179.39810227, 162);
	pokemon_latios.add_fmove(&move_zen_headbutt);
	pokemon_latios.add_cmove(&move_psychic);

	Pokemon pokemon_lugia = Pokemon(0, 3, 164.38240208000002, 256.84750325000005, 197);
	pokemon_lugia.add_fmove(&move_extrasensory);
	pokemon_lugia.add_cmove(&move_sky_attack);

	Pokemon pokemon_machamp = Pokemon(1, -1, 181.7700047492981, 127.02000331878662, 3600);
	pokemon_machamp.add_fmove(&move_counter);
	pokemon_machamp.add_cmove(&move_dynamic_punch);

	// Set up parties
	Party attacker_party = Party();
	attacker_party.add(&pokemon_mewtwo);

	Party raid_boss_party = Party();
	raid_boss_party.add(&pokemon_machamp);

	// Set up players
	Player attacker = Player();
	attacker.team = 1;
	attacker.add(&attacker_party);
	attacker.set_strategy(STRATEGY_ATTACKER_DODGE_ALL);

	Player raid_boss = Player();
	raid_boss.team = 0;
	raid_boss.add(&raid_boss_party);
	raid_boss.set_strategy(STRATEGY_DEFENDER);

	// Construct battle
	Battle battle = Battle();
	battle.add(&raid_boss);
	battle.add(&attacker);
	battle.set_time_limit(180000);
	battle.set_weather(1);

	srand(1000);

	// Run one battle
	battle.init();
	battle.start();
	BattleOutcome battle_outcome = battle.get_outcome(1);
	std::cout << "Duration: " << battle_outcome.duration << std::endl;
	std::cout << "TDO%: " << battle_outcome.tdo_percent << std::endl;

	const int num_sims = 10000;
	std::cout << "Simulating " << num_sims << " battles..." << std::endl;

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	for (int i = 0; i < num_sims; ++i)
	{
		battle.init();
		battle.start();
	}
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Time elapsed (s) = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0 << std::endl;

	std::cout << "Raid Battle Test passed" << std::endl;

	return 0;
}
