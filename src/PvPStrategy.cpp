
#include "PvPStrategy.h"

namespace GoBattleSim
{

PvPStrategy::PvPStrategy(PvPEventResponder t_on_free, PvPEventResponder t_on_attack, PvPEventResponder t_on_switch)
	: on_free(t_on_free), on_attack(t_on_attack), on_switch(t_on_switch)
{
}

/*
	PvP Basic Strategy
	Use default cmove as soon as possible
*/
void pvp_basic_on_free(const PvPStrategyInput &si, Action *r_action)
{
	if (si.subject_energy + si.subject->cmove->energy >= 0)
	{
		r_action->type = atype_Charged;
	}
	else
	{
		r_action->type = atype_Fast;
	}
}

/*
	PvP Advance Strategy
	See docs
*/
void pvp_advance_on_free(const PvPStrategyInput &si, Action *r_action)
{
	int my_fmove_damage = calc_damage_pvp_cmove(si.subject, si.subject->get_fmove(0), si.enemy);
	if (my_fmove_damage >= si.enemy_hp)
	{
		r_action->type = atype_Fast;
		return;
	}

	// Find the better move and the cheaper move.
	int better_move_idx = 0, cheaper_move_idx = 0;
	double higher_dpe = 0;
	int lower_energy_cost = -1;
	bool cheaper_move_ko = false;
	for (int i = 0; i < si.subject->cmoves_count; ++i)
	{
		auto move = si.subject->get_cmove(i);
		int damage = calc_damage_pvp_cmove(si.subject, move, si.enemy);
		double dpe = (double)damage / (-move->energy);
		if (dpe > higher_dpe)
		{
			higher_dpe = dpe;
			better_move_idx = i;
		}
		if (-move->energy < lower_energy_cost || (-move->energy == lower_energy_cost && move->effect.activation_chance > 0) || lower_energy_cost < 0)
		{
			lower_energy_cost = -move->energy;
			cheaper_move_idx = i;
			cheaper_move_ko = damage >= si.enemy_hp;
		}
	}

	if (si.subject_energy + si.subject->get_cmove(better_move_idx)->energy >= 0)
	{
		if (si.enemy_shields > 0)
		{
			r_action->type = atype_Charged;
			r_action->value = cheaper_move_idx;
		}
		else
		{
			r_action->type = atype_Charged;
			r_action->value = better_move_idx;
		}
	}
	else if (si.subject_energy + si.subject->get_cmove(cheaper_move_idx)->energy >= 0)
	{
		if (cheaper_move_ko || calc_damage_pvp_fmove(si.enemy, si.enemy->get_fmove(0), si.subject) >= si.subject_hp)
		{
			r_action->type = atype_Charged;
			r_action->value = cheaper_move_idx;
		}
		else
		{
			r_action->type = atype_Fast;
		}
	}
	else
	{
		r_action->type = atype_Fast;
	}
}

} // namespace GoBattleSim
