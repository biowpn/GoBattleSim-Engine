
#include "GoBattleSim_Extern.h"
#include "GoBattleSim.h"

#include <stdlib.h>

using namespace GoBattleSim;

void Global_set_random_seed(int t_seed)
{
	srand(t_seed);
}

int Global_calc_damage(void *t_attacker, void *t_move, void *t_defender, int t_weather)
{
	return calc_damage(static_cast<Pokemon *>(t_attacker), static_cast<Move *>(t_move), static_cast<Pokemon *>(t_defender), t_weather);
}

int Global_calc_damage_pvp_fmove(void *t_attacker, void *t_move, void *t_defender)
{
	return calc_damage_pvp_fmove(static_cast<Pokemon *>(t_attacker), static_cast<Move *>(t_move), static_cast<Pokemon *>(t_defender));
}

int Global_calc_damage_pvp_cmove(void *t_attacker, void *t_move, void *t_defender)
{
	return calc_damage_pvp_cmove(static_cast<Pokemon *>(t_attacker), static_cast<Move *>(t_move), static_cast<Pokemon *>(t_defender));
}

void GameMaster_set_num_types(int t_num_types)
{
	GameMaster::set_num_types(t_num_types);
}

void GameMaster_set_effectiveness(int t_type1, int t_type2, double t_effe)
{
	GameMaster::set_effectiveness(t_type1, t_type2, t_effe);
}

void GameMaster_set_type_boosted_weather(int t_type, int t_weather)
{
	GameMaster::set_type_boosted_weather(t_type, t_weather);
}

void GameMaster_set_stage_bounds(int t_min_stage, int t_max_stage)
{
	GameMaster::set_stage_bounds(t_min_stage, t_max_stage);
}

void GameMaster_set_stage_multiplier(int t_stage, double t_multiplier)
{
	GameMaster::set_stage_multiplier(t_stage, t_multiplier);
}

void GameMaster_set_parameter(char *t_name, double t_value)
{
	GameMaster::set_parameter(t_name, t_value);
}

void *Move_new(int t_poketype, int t_power, int t_energy, int t_duration, int t_dws)
{
	return new Move{t_poketype, t_power, t_energy, t_duration, t_dws};
}

void Move_delete(void *__this__)
{
	delete static_cast<Move *>(__this__);
}

void Move_get_effect(void *__this__, void *t_effect)
{
	*static_cast<MoveEffect *>(t_effect) = static_cast<Move *>(__this__)->effect;
}

void Move_set_effect(void *__this__, void *t_effect)
{
	static_cast<Move *>(__this__)->effect = *static_cast<MoveEffect *>(t_effect);
}

bool Move_has_attr(void *__this__, char *t_name)
{
	return static_cast<Move *>(__this__)->has_attr(t_name);
}

int Move_get_attr(void *__this__, char *t_name)
{
	return static_cast<Move *>(__this__)->get_attr(t_name);
}

void Move_set_attr(void *__this__, char *t_name, int t_value)
{
	static_cast<Move *>(__this__)->set_attr(t_name, t_value);
}

void *Pokemon_new(int t_poketype1, int t_poketype2, double t_attack, double t_defense, int t_max_hp)
{
	return new Pokemon(t_poketype1, t_poketype2, t_attack, t_defense, t_max_hp);
}

void Pokemon_delete(void *__this__)
{
	delete static_cast<Pokemon *>(__this__);
}

void *Pokemon_get_fmove(void *__this__, int t_index)
{
	return const_cast<Move * >(static_cast<Pokemon *>(__this__)->get_fmove(t_index));
}

void Pokemon_add_fmove(void *__this__, void *t_move)
{
	return static_cast<Pokemon *>(__this__)->add_fmove(static_cast<Move *>(t_move));
}

void *Pokemon_get_cmove(void *__this__, int t_index)
{
	return const_cast<Move * >(static_cast<Pokemon *>(__this__)->get_cmove(t_index));
}

void Pokemon_add_cmove(void *__this__, void *t_move)
{
	return static_cast<Pokemon *>(__this__)->add_cmove(static_cast<Move *>(t_move));
}

void Pokemon_erase_cmoves(void *__this__)
{
	return static_cast<Pokemon *>(__this__)->erase_cmoves();
}

bool Pokemon_has_attr(void *__this__, char *t_name)
{
	return static_cast<Pokemon *>(__this__)->has_attr(t_name);
}

double Pokemon_get_attr(void *__this__, char *t_name)
{
	return static_cast<Pokemon *>(__this__)->get_attr(t_name);
}

void Pokemon_set_attr(void *__this__, char *t_name, double t_value)
{
	static_cast<Pokemon *>(__this__)->set_attr(t_name, t_value);
}

void *Party_new()
{
	return new Party();
}

void Party_delete(void *__this__)
{
	delete static_cast<Party *>(__this__);
}

void *Party_get_pokemon(void *__this__, int t_index)
{
	return static_cast<Party *>(__this__)->get_pokemon(t_index);
}

void Party_add_pokemon(void *__this__, void *t_pokemon)
{
	return static_cast<Party *>(__this__)->add(static_cast<Pokemon *>(t_pokemon));
}

void Party_erase_pokemon(void *__this__)
{
	static_cast<Party *>(__this__)->erase_pokemon();
}

bool Party_has_attr(void *__this__, char *t_name)
{
	return static_cast<Party *>(__this__)->has_attr(t_name);
}

int Party_get_attr(void *__this__, char *t_name)
{
	return static_cast<Party *>(__this__)->get_attr(t_name);
}

void Party_set_attr(void *__this__, char *t_name, int t_value)
{
	static_cast<Party *>(__this__)->set_attr(t_name, t_value);
}

void *Player_new()
{
	return new Player();
}

void Player_delete(void *__this__)
{
	delete static_cast<Player *>(__this__);
}

void *Player_get_party(void *__this__, int t_index)
{
	return static_cast<Player *>(__this__)->get_party(t_index);
}

void Player_add_party(void *__this__, void *t_party)
{
	return static_cast<Player *>(__this__)->add(static_cast<Party *>(t_party));
}

void Player_erase_parties(void *__this__)
{
	static_cast<Player *>(__this__)->erase_parties();
}

void Player_set_attack_multiplier(void *__this__, double t_multiplier)
{
	static_cast<Player *>(__this__)->set_attack_multiplier(t_multiplier);
}

void Player_set_clone_multiplier(void *__this__, int t_multiplier)
{
	static_cast<Player *>(__this__)->set_clone_multiplier(t_multiplier);
}

void *Player_get_strategy(void *__this__)
{
	return &(static_cast<Player *>(__this__)->strategy);
}

void Player_set_strategy(void *__this__, int t_strategy_index)
{
	static_cast<Player *>(__this__)->set_strategy(t_strategy_index);
}

void Player_set_custom_strategy(void *__this__, void *t_strategy)
{
	return static_cast<Player *>(__this__)->set_strategy(*static_cast<Strategy *>(t_strategy));
}

bool Player_has_attr(void *__this__, char *t_name)
{
	return static_cast<Player *>(__this__)->has_attr(t_name);
}

int Player_get_attr(void *__this__, char *t_name)
{
	return static_cast<Player *>(__this__)->get_attr(t_name);
}

void Player_set_attr(void *__this__, char *t_name, int t_value)
{
	static_cast<Player *>(__this__)->set_attr(t_name, t_value);
}

void *Strategy_new()
{
	return new Strategy();
}

void Strategy_delete(void *__this__)
{
	delete static_cast<Strategy *>(__this__);
}

void Strategy_set_on_free(void *__this__, void *t_event_responder)
{
	static_cast<Strategy *>(__this__)->on_free = reinterpret_cast<EventResponder>(t_event_responder);
}

void Strategy_set_on_clear(void *__this__, void *t_event_responder)
{
	static_cast<Strategy *>(__this__)->on_clear = reinterpret_cast<EventResponder>(t_event_responder);
}

void Strategy_set_on_attack(void *__this__, void *t_event_responder)
{
	static_cast<Strategy *>(__this__)->on_attack = reinterpret_cast<EventResponder>(t_event_responder);
}

void *Battle_new()
{
	return new Battle();
}

void Battle_delete(void *__this__)
{
	delete static_cast<Battle *>(__this__);
}

void *Battle_get_player(void *__this__, int t_index)
{
	return static_cast<Battle *>(__this__)->get_player(t_index);
}

void Battle_add_player(void *__this__, void *t_player)
{
	return static_cast<Battle *>(__this__)->add(static_cast<Player *>(t_player));
}

void Battle_update_player(void *__this__, void *t_player)
{
	static_cast<Battle *>(__this__)->update(static_cast<Player *>(t_player));
}

void Battle_update_pokemon(void *__this__, void *t_pokemon)
{
	static_cast<Battle *>(__this__)->update(static_cast<Pokemon *>(t_pokemon));
}

void Battle_erase_players(void *__this__)
{
	static_cast<Battle *>(__this__)->erase_players();
}

bool Battle_has_attr(void *__this__, char *t_name)
{
	return static_cast<Battle *>(__this__)->has_attr(t_name);
}

int Battle_get_attr(void *__this__, char *t_name)
{
	return static_cast<Battle *>(__this__)->get_attr(t_name);
}

void Battle_set_attr(void *__this__, char *t_name, int t_value)
{
	static_cast<Battle *>(__this__)->set_attr(t_name, t_value);
}

void Battle_init(void *__this__)
{
	return static_cast<Battle *>(__this__)->init();
}

void Battle_start(void *__this__)
{
	return static_cast<Battle *>(__this__)->start();
}

void Battle_get_outcome(void *__this__, int t_team, void *r_battle_outcome)
{
	*static_cast<BattleOutcome *>(r_battle_outcome) = static_cast<Battle *>(__this__)->get_outcome(t_team);
}

void Battle_get_log(void *__this__, void *t_node)
{
	*static_cast<TimelineEventNode *>(t_node) = *static_cast<Battle *>(__this__)->get_log();
}

/* PvP */

void *PvPPokemon_new(int t_poketype1, int t_poketype2, double t_attack, double t_defense, int t_max_hp)
{
	return new PvPPokemon(t_poketype1, t_poketype2, t_attack, t_defense, t_max_hp);
}

void PvPPokemon_delete(void *__this__)
{
	delete static_cast<PvPPokemon *>(__this__);
}

void PvPPokemon_set_num_shields(void *__this__, int t_num_shields)
{
	static_cast<PvPPokemon *>(__this__)->num_shields_max = t_num_shields;
}

void *PvPStrategy_new()
{
	return new PvPStrategy();
}

void PvPStrategy_delete(void *__this__)
{
	delete static_cast<PvPStrategy *>(__this__);
}

void PvPStrategy_set_on_free(void *__this__, void *t_event_responder)
{
	static_cast<PvPStrategy *>(__this__)->on_free = reinterpret_cast<PvPEventResponder>(t_event_responder);
}

void PvPStrategy_set_on_attack(void *__this__, void *t_event_responder)
{
	static_cast<PvPStrategy *>(__this__)->on_attack = reinterpret_cast<PvPEventResponder>(t_event_responder);
}

void PvPStrategy_set_on_switch(void *__this__, void *t_event_responder)
{
	static_cast<PvPStrategy *>(__this__)->on_switch = reinterpret_cast<PvPEventResponder>(t_event_responder);
}

void *SimplePvPBattle_new(void *t_pokemon_0, void *t_pokemon_1)
{
	return new SimplePvPBattle(static_cast<PvPPokemon *>(t_pokemon_0), static_cast<PvPPokemon *>(t_pokemon_1));
}

void SimplePvPBattle_delete(void *__this__)
{
	delete static_cast<SimplePvPBattle *>(__this__);
}

void SimplePvPBattle_set_num_shields_max(void *__this__, int t_num_0, int t_num_1)
{
	static_cast<SimplePvPBattle *>(__this__)->set_num_shields_max(t_num_0, t_num_1);
}

void SimplePvPBattle_set_strategy(void *__this__, int t_pkm_idx, void *t_strategy)
{
	static_cast<SimplePvPBattle *>(__this__)->set_strategy(t_pkm_idx, *static_cast<PvPStrategy *>(t_strategy));
}

void SimplePvPBattle_init(void *__this__)
{
	static_cast<SimplePvPBattle *>(__this__)->init();
}

void SimplePvPBattle_start(void *__this__)
{
	static_cast<SimplePvPBattle *>(__this__)->start();
}

void SimplePvPBattle_get_outcome(void *__this__, void *t_outcome)
{
	*static_cast<SimplePvPBattleOutcome *>(t_outcome) = static_cast<SimplePvPBattle *>(__this__)->get_outcome();
}

void *BattleMatrix_new(void *t_row_pkm, int t_row_size, void *t_col_pkm, int t_col_size, bool t_enum_shields)
{
	return new BattleMatrix(static_cast<const PvPPokemon **>(t_row_pkm), t_row_size, static_cast<const PvPPokemon **>(t_col_pkm), t_col_size, t_enum_shields);
}

void BattleMatrix_delete(void *__this__)
{
	delete static_cast<BattleMatrix *>(__this__);
}

void BattleMatrix_run(void *__this__)
{
	static_cast<BattleMatrix *>(__this__)->run();
}

void BattleMatrix_get(void *__this__, double **r_double)
{
	static_cast<BattleMatrix *>(__this__)->get(r_double);
}
