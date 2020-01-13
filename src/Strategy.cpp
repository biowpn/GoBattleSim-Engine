
#include "Strategy.h"

#include "GameMaster.h"

namespace GoBattleSim
{

// Helper function
int get_projected_energy(const StrategyInput &si)
{
	int projected_energy = si.subject_state->energy;
	if (si.subject_action.type == ActionType::Fast)
	{
		projected_energy += si.subject->get_fmove(0)->energy;
	}
	else if (si.subject_action.type == ActionType::Charged)
	{
		projected_energy += si.subject->get_cmove(-1)->energy;
	}
	return projected_energy;
}

void defender_on_clear(const StrategyInput &si, Action *r_action)
{
	r_action->type = ActionType::Fast;
	r_action->value = 0;
	int projected_energy = get_projected_energy(si);
	for (unsigned char i = 0; i < si.subject->cmoves_count; ++i)
	{
		auto cmove = si.subject->get_cmove(i);
		if (projected_energy + cmove->energy >= 0 && ((si.random_number >> i) & 1))
		{
			r_action->type = ActionType::Charged;
			r_action->value = i;
			break;
		}
	}
}

void attacker_no_dodge_on_free(const StrategyInput &si, Action *r_action)
{
	if (si.subject_state->energy + si.subject->get_cmove(-1)->energy >= 0)
	{
		r_action->type = ActionType::Charged;
	}
	else
	{
		r_action->type = ActionType::Fast;
	}
}

void attacker_dodge_charged_on_free(const StrategyInput &si, Action *r_action)
{
	bool predicted_attack = false;
	int time_of_damage = -1, time_of_enemy_cooldown = si.enemy_action.time;
	if (si.enemy_action.type == ActionType::Fast)
	{
		time_of_enemy_cooldown += si.enemy->get_fmove(0)->duration + 1500;
	}
	else if (si.enemy_action.type == ActionType::Charged)
	{
		time_of_damage = si.enemy_action.time + si.enemy->cmove->dws;
		time_of_enemy_cooldown += si.enemy->cmove->duration + 1500;
	}
	else // enemy just enters battle
	{
		time_of_damage = si.enemy_action.time + 1500 + si.enemy->cmove->dws;
		time_of_enemy_cooldown += 1500;
	}
	if (time_of_damage < si.time_free || time_of_damage < si.subject_state->damage_reduction_expiry)
	{
		// Predict next time of damage
		time_of_damage = time_of_enemy_cooldown + si.enemy->cmove->dws;
		predicted_attack = true;
	}
	int time_till_damage = time_of_damage - si.time_free;
	if (time_till_damage > si.subject->cmove->duration && si.subject_state->energy + si.subject->cmove->energy >= 0) // Can squeeze in one charge
	{
		r_action->type = ActionType::Charged;
		r_action->value = si.subject->cmove - si.subject->cmoves;
	}
	else if (time_till_damage > si.subject->get_fmove(0)->duration) // Can squeeze in one fast
	{
		r_action->type = ActionType::Fast;
	}
	else
	{
		if (predicted_attack)
		{
			r_action->type = ActionType::Wait;
		}
		else
		{
			int delay = time_till_damage - GameMaster::get().dodge_window;
			r_action->type = ActionType::Dodge;
			r_action->delay = delay > 0 ? delay : 0;
		}
	}
}

void attacker_dodge_charged_on_attack(const StrategyInput &si, Action *r_action)
{
	int time_of_damage;
	if (si.enemy_action.type == ActionType::Fast)
	{
		// Ignore the coming enemy fast attack
		attacker_no_dodge_on_free(si, r_action);
		return;
	}
	else
	{
		time_of_damage = si.enemy_action.time + si.enemy->cmove->dws;
	}
	if (time_of_damage < si.subject_state->damage_reduction_expiry)
	{
		// previous dodge still in effect
		attacker_no_dodge_on_free(si, r_action);
		return;
	}
	int time_till_damage = time_of_damage - si.time_free;
	if (time_till_damage > si.subject->cmove->duration && si.subject_state->energy + si.subject->cmove->energy >= 0) // Can squeeze in one charge
	{
		r_action->type = ActionType::Charged;
		r_action->value = si.subject->cmove - si.subject->cmoves;
	}
	else if (time_till_damage > si.subject->get_fmove(0)->duration) // Can squeeze in one fast
	{
		r_action->type = ActionType::Fast;
	}
	else // Just dodge
	{
		int delay = time_till_damage - GameMaster::get().dodge_window;
		r_action->type = ActionType::Dodge;
		r_action->delay = delay > 0 ? delay : 0;
	}
}

void attacker_dodge_all_on_free(const StrategyInput &si, Action *r_action)
{
	bool predicted_attack = false;
	int time_of_damage = -1, time_of_enemy_cooldown = si.enemy_action.time;
	if (si.enemy_action.type == ActionType::Fast)
	{
		time_of_damage = si.enemy_action.time + si.enemy->get_fmove(0)->dws;
		time_of_enemy_cooldown += si.enemy->get_fmove(0)->duration + 1500;
	}
	else if (si.enemy_action.type == ActionType::Charged)
	{
		time_of_damage = si.enemy_action.time + si.enemy->cmove->dws;
		time_of_enemy_cooldown += si.enemy->cmove->duration + 1500;
	}
	else // enemy just enters battle
	{
		time_of_damage = si.enemy_action.time + 1500 + si.enemy->get_fmove(0)->dws;
		time_of_enemy_cooldown += 1500;
	}
	if (time_of_damage < si.time_free || time_of_damage < si.subject_state->damage_reduction_expiry)
	{
		// Predict next time of damage
		time_of_damage = time_of_enemy_cooldown + si.enemy->get_fmove(0)->dws;
		predicted_attack = true;
	}
	int time_till_damage = time_of_damage - si.time_free;
	if (time_till_damage > si.subject->cmove->duration && si.subject_state->energy + si.subject->cmove->energy >= 0) // Can squeeze in one charge
	{
		r_action->type = ActionType::Charged;
		r_action->value = si.subject->cmove - si.subject->cmoves;
	}
	else if (time_till_damage > si.subject->get_fmove(0)->duration) // Can squeeze in one fast
	{
		r_action->type = ActionType::Fast;
	}
	else
	{
		if (predicted_attack)
		{
			r_action->type = ActionType::Wait;
		}
		else
		{
			int delay = time_till_damage - GameMaster::get().dodge_window;
			r_action->type = ActionType::Dodge;
			r_action->delay = delay > 0 ? delay : 0;
		}
	}
}

void attacker_dodge_all_on_attack(const StrategyInput &si, Action *r_action)
{
	int time_of_damage;
	if (si.enemy_action.type == ActionType::Fast)
	{
		time_of_damage = si.enemy_action.time + si.enemy->get_fmove(0)->dws;
	}
	else
	{
		time_of_damage = si.enemy_action.time + si.enemy->cmove->dws;
	}
	if (time_of_damage < si.subject_state->damage_reduction_expiry)
	{
		// previous dodge still in effect
		// to play if safe, use fast move
		r_action->type = ActionType::Fast;
		return;
	}
	int time_till_damage = time_of_damage - si.time_free;
	if (time_till_damage > si.subject->cmove->duration && si.subject_state->energy + si.subject->cmove->energy >= 0) // Can squeeze in one charge
	{
		r_action->type = ActionType::Charged;
		r_action->value = si.subject->cmove - si.subject->cmoves;
	}
	else if (time_till_damage > si.subject->get_fmove(0)->duration) // Can squeeze in one fast
	{
		r_action->type = ActionType::Fast;
	}
	else // Just dodge
	{
		int delay = time_till_damage - GameMaster::get().dodge_window;
		r_action->type = ActionType::Dodge;
		r_action->delay = delay > 0 ? delay : 0;
	}
}

} // namespace GoBattleSim
