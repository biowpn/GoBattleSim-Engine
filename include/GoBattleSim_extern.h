
/**
 * This file defines the C-interface needed to build DLL.
 */

#ifndef _GOBATTLESIM_EXTERN_H_
#define _GOBATTLESIM_EXTERN_H_

extern "C"
{
	void Global_set_random_seed(int);
	int Global_calc_damage(void *, void *, void *, int);
	int Global_calc_damage_pvp_fmove(void *, void *, void *);
	int Global_calc_damage_pvp_cmove(void *, void *, void *);

	void GameMaster_set_num_types(int);
	void GameMaster_set_effectiveness(int, int, double);
	void GameMaster_set_type_boosted_weather(int, int);
	void GameMaster_set_stage_bounds(int, int);
	void GameMaster_set_stage_multiplier(int, double);
	void GameMaster_set_parameter(char *, double);

	/* PvE */
	void *Move_new(int, int, int, int, int);
	void Move_delete(void *);
	void Move_get_effect(void *, void *);
	void Move_set_effect(void *, void *);
	bool Move_has_attr(void *, char *);
	int Move_get_attr(void *, char *);
	void Move_set_attr(void *, char *, int);

	void *Pokemon_new(int, int, double, double, int);
	void Pokemon_delete(void *);
	void *Pokemon_get_fmove(void *, int);
	void Pokemon_add_fmove(void *, void *);
	void *Pokemon_get_cmove(void *, int);
	void Pokemon_add_cmove(void *, void *);
	void Pokemon_erase_cmoves(void *);
	bool Pokemon_has_attr(void *, char *);
	double Pokemon_get_attr(void *, char *);
	void Pokemon_set_attr(void *, char *, double);

	void *Party_new();
	void Party_delete(void *);
	void *Party_get_pokemon(void *, int);
	void Party_add_pokemon(void *, void *);
	void Party_erase_pokemon(void *);
	bool Party_has_attr(void *, char *);
	int Party_get_attr(void *, char *);
	void Party_set_attr(void *, char *, int);

	void *Player_new();
	void Player_delete(void *);
	void *Player_get_party(void *, int);
	void Player_add_party(void *, void *);
	void Player_erase_parties(void *);
	void Player_set_attack_multiplier(void *, double);
	void Player_set_clone_multiplier(void *, int);
	void *Player_get_strategy(void *);
	void Player_set_strategy(void *, int);
	void Player_set_custom_strategy(void *, void *);
	bool Player_has_attr(void *, char *);
	int Player_get_attr(void *, char *);
	void Player_set_attr(void *, char *, int);

	void *Strategy_new();
	void Strategy_delete(void *);
	void Strategy_set_on_free(void *, void *);
	void Strategy_set_on_clear(void *, void *);
	void Strategy_set_on_attack(void *, void *);

	void *Battle_new();
	void Battle_delete(void *);
	void *Battle_get_player(void *, int);
	void Battle_add_player(void *, void *);
	void Battle_update_player(void *, void *);
	void Battle_update_pokemon(void *, void *);
	void Battle_erase_players(void *);

	bool Battle_has_attr(void *, char *);
	int Battle_get_attr(void *, char *);
	void Battle_set_attr(void *, char *, int);

	void Battle_init(void *);
	void Battle_start(void *);
	void Battle_get_outcome(void *, int, void *);
	void Battle_get_log(void *, void *);

	/* PvP */
	void *PvPPokemon_new(int, int, double, double, int);
	void PvPPokemon_delete(void *);
	void PvPPokemon_set_num_shields(void *, int);

	void *PvPStrategy_new();
	void PvPStrategy_delete(void *);
	void PvPStrategy_set_on_free(void *, void *);
	void PvPStrategy_set_on_attack(void *, void *);
	void PvPStrategy_set_on_switch(void *, void *);

	void *SimplePvPBattle_new(void *, void *);
	void SimplePvPBattle_delete(void *);
	void SimplePvPBattle_set_num_shields_max(void *, int, int);
	void SimplePvPBattle_set_strategy(void *, int, void *);
	void SimplePvPBattle_init(void *);
	void SimplePvPBattle_start(void *);
	void SimplePvPBattle_get_outcome(void *, void *);

	/* PvP Battle Matrix */
	void *BattleMatrix_new(void *, int, void *, int, bool);
	void BattleMatrix_delete(void *);
	void BattleMatrix_run(void *);
	void BattleMatrix_get(void *, double **);
}

#endif
