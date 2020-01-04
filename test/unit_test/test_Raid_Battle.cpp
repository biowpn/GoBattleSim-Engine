
#include <iostream>
#include <chrono>
#include <assert.h>

#include "GameMaster.h"
#include "Battle.h"

using namespace GoBattleSim;

int main()
{
	std::cout << "\nRaid Battle Test:" << std::endl;

	// In this demo, 0 = Psychic, 1 = Fighting, 2 = Steel, 3 = Flying
	GameMaster::get().num_types(4);
	GameMaster::get().effectiveness(0, 1, 1.6);
	GameMaster::get().effectiveness(0, 2, 1 / 1.6);
	GameMaster::get().effectiveness(1, 0, 1 / 1.6);
	GameMaster::get().effectiveness(1, 2, 1.6);
	GameMaster::get().effectiveness(1, 3, 1 / 1.6);
	GameMaster::get().effectiveness(3, 1, 1.6);
	GameMaster::get().effectiveness(3, 2, 1 / 1.6);
	GameMaster::get().boosted_weather(0, 1);
	GameMaster::get().boosted_weather(3, 1);

	// Set up moves
	// (poketype, power, energy, duration, damage_window_start)
	Move move_confusion = Move{0, 20, 15, 1600, 600};
	Move move_zen_headbutt = Move{0, 12, 10, 1100, 850};
	Move move_extrasensory = Move{0, 12, 12, 1100, 850};
	Move move_counter = Move{1, 12, 8, 900, 700};
	// Move move_bullet_punch = Move{2, 9, 10, 900, 300};
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

	srand(1000);

	// 1 x Mewtwo VS T3 Machamp
	{
		Party attacker_party;
		attacker_party.add(&pokemon_mewtwo);

		Party raid_boss_party;
		raid_boss_party.add(&pokemon_machamp);

		Player attacker;
		attacker.team = 1;
		attacker.add(&attacker_party);
		attacker.set_strategy(STRATEGY_ATTACKER_DODGE_ALL);

		Player raid_boss;
		raid_boss.team = 0;
		raid_boss.add(&raid_boss_party);
		raid_boss.set_strategy(STRATEGY_DEFENDER);

		Battle battle;
		battle.add(&raid_boss);
		battle.add(&attacker);
		battle.set_time_limit(180000);
		battle.set_weather(1);

		battle.init();
		battle.start();
		PvEBattleOutcome battle_outcome = battle.get_outcome(1);

		std::cout << "test#1 Duration: " << battle_outcome.duration << std::endl;
		std::cout << "test#1 TDO%: " << battle_outcome.tdo_percent << std::endl;

		assert(!battle_outcome.win);
		assert(battle_outcome.tdo_percent > 0.8);
		assert(battle_outcome.tdo_percent < 1);
		assert(battle_outcome.duration > 120000);
		assert(battle_outcome.duration < 160000);
		assert(battle_outcome.num_deaths == 1);
	}

	// 6 x Latios VS T3 Machamp, no rejoin
	{
		Party attacker_party = Party();
		for (int i = 0; i < 6; ++i)
		{
			attacker_party.add(&pokemon_latios);
		}

		Party raid_boss_party = Party();
		raid_boss_party.add(&pokemon_machamp);

		Player attacker;
		attacker.team = 1;
		attacker.add(&attacker_party);
		attacker.set_strategy(STRATEGY_ATTACKER_NO_DODGE);

		Player raid_boss;
		raid_boss.team = 0;
		raid_boss.add(&raid_boss_party);
		raid_boss.set_strategy(STRATEGY_DEFENDER);

		Battle battle;
		battle.add(&raid_boss);
		battle.add(&attacker);
		battle.set_time_limit(180000);
		battle.set_weather(1);

		battle.init();
		battle.start();
		PvEBattleOutcome battle_outcome = battle.get_outcome(1);

		std::cout << "test#2 Duration: " << battle_outcome.duration << std::endl;
		std::cout << "test#2 TDO%: " << battle_outcome.tdo_percent << std::endl;
		std::cout << "test#2 numDeaths: " << battle_outcome.num_deaths << std::endl;

		assert(battle_outcome.win);
		assert(battle_outcome.tdo_percent >= 1);
		assert(battle_outcome.duration > 100000);
		assert(battle_outcome.duration < 140000);
		assert(battle_outcome.num_deaths >= 2);
		assert(battle_outcome.num_deaths < 6);
	}

	// 2 x Latios VS T3 Machamp, rejoin
	{
		Party attacker_party;
		attacker_party.add(&pokemon_latios);
		attacker_party.add(&pokemon_latios);
		attacker_party.revive_policy(true);

		Party raid_boss_party;
		raid_boss_party.add(&pokemon_machamp);

		Player attacker;
		attacker.team = 1;
		attacker.add(&attacker_party);
		attacker.set_strategy(STRATEGY_ATTACKER_NO_DODGE);

		Player raid_boss;
		raid_boss.team = 0;
		raid_boss.add(&raid_boss_party);
		raid_boss.set_strategy(STRATEGY_DEFENDER);

		Battle battle;
		battle.add(&raid_boss);
		battle.add(&attacker);
		battle.set_time_limit(180000);
		battle.set_weather(1);

		battle.init();
		battle.start();
		auto battle_outcome = battle.get_outcome(1);

		std::cout << "test#3 Duration: " << battle_outcome.duration << std::endl;
		std::cout << "test#3 TDO%: " << battle_outcome.tdo_percent << std::endl;
		std::cout << "test#3 numDeaths: " << battle_outcome.num_deaths << std::endl;

		assert(battle_outcome.win);
		assert(battle_outcome.tdo_percent >= 1);
		assert(battle_outcome.duration > 100000);
		assert(battle_outcome.duration < 140000);
		assert(battle_outcome.num_deaths >= 2);
		assert(battle_outcome.num_deaths < 6);
	}

	// 1 x lugia + 1 x Mewtwo vs T3 Machamp, rejoin
	{
		Party attacker_party_1, attacker_party_2;
		attacker_party_1.add(&pokemon_lugia);
		attacker_party_1.revive_policy(true);
		attacker_party_2.add(&pokemon_mewtwo);
		attacker_party_2.revive_policy(true);

		Party raid_boss_party;
		raid_boss_party.add(&pokemon_machamp);

		Player attacker_1, attacker_2;
		attacker_1.team = 1;
		attacker_1.add(&attacker_party_1);
		attacker_1.set_strategy(STRATEGY_ATTACKER_NO_DODGE);
		attacker_2.team = 1;
		attacker_2.add(&attacker_party_2);
		attacker_2.set_strategy(STRATEGY_ATTACKER_NO_DODGE);

		Player raid_boss;
		raid_boss.team = 0;
		raid_boss.add(&raid_boss_party);
		raid_boss.set_strategy(STRATEGY_DEFENDER);

		Battle battle;
		battle.add(&raid_boss);
		battle.add(&attacker_1);
		battle.add(&attacker_2);
		battle.set_time_limit(180000);
		battle.set_weather(1);

		battle.init();
		battle.start();
		PvEBattleOutcome battle_outcome = battle.get_outcome(1);

		std::cout << "test#4 Duration: " << battle_outcome.duration << std::endl;
		std::cout << "test#4 TDO%: " << battle_outcome.tdo_percent << std::endl;
		std::cout << "test#4 numDeaths: " << battle_outcome.num_deaths << std::endl;

		assert(battle_outcome.win);
		assert(battle_outcome.tdo_percent >= 1);
		assert(battle_outcome.duration > 50000);
		assert(battle_outcome.duration < 70000);
		assert(battle_outcome.num_deaths > 0);
		assert(battle_outcome.num_deaths < 4);
	}

	std::cout << "Raid Battle Test passed" << std::endl;

	return 0;
}
