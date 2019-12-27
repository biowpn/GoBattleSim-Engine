
#include "Strategy.h"

Action::Action(ActionType t_type, int t_delay, int t_value)
{
	type = t_type;
	delay = t_delay;
	value = t_value;

	time = 0;
}

std::ostream &operator<<(std::ostream &os, const Action &t_action)
{
	os << t_action.type << ' ' << t_action.delay << ' ' << t_action.value << ' ' << t_action.time;
	return os;
}

Strategy::Strategy(EventResponder t_on_free, EventResponder t_on_clear, EventResponder t_on_attack)
	: on_free(t_on_free ? t_on_free : attacker_no_dodge_on_free), on_clear(t_on_clear), on_attack(t_on_attack)
{
}

// Helper function
int get_projected_energy(const StrategyInput &si)
{
	int projected_energy = si.subject->energy;
	if (si.subject_action.type == atype_Fast)
	{
		projected_energy += si.subject->fmove->energy;
	}
	else if (si.subject_action.type == atype_Charged)
	{
		projected_energy += si.subject->cmove->energy;
	}
	return projected_energy;
}

void defender_on_clear(const StrategyInput &si, Action *r_action)
{
	r_action->type = atype_Fast;
	r_action->value = 0;
	int projected_energy = get_projected_energy(si);
	for (int i = 0; i < si.subject->cmoves_count; ++i)
	{
		Move *cmove = si.subject->cmoves[i];
		if (projected_energy + cmove->energy >= 0 && ((si.random_number >> i) & 1))
		{
			r_action->type = atype_Charged;
			r_action->value = i;
			break;
		}
	}
}

void attacker_no_dodge_on_free(const StrategyInput &si, Action *r_action)
{
	if (si.subject->energy + si.subject->cmove->energy >= 0)
	{
		r_action->type = atype_Charged;
	}
	else
	{
		r_action->type = atype_Fast;
	}
}

void attacker_dodge_charged_on_free(const StrategyInput &si, Action *r_action)
{
	bool predicted_attack = false;
	int time_of_damage = -1, time_of_enemy_cooldown = -1;
	if (si.enemy_action.type == atype_Fast)
	{
		time_of_enemy_cooldown = si.enemy_action.time + si.enemy->fmove->duration;
	}
	else if (si.enemy_action.type == atype_Charged)
	{
		time_of_damage = si.enemy_action.time + si.enemy->cmove->dws;
		time_of_enemy_cooldown = si.enemy_action.time + si.enemy->cmove->duration;
	}
	if (time_of_damage < si.time || time_of_damage < si.subject->damage_reduction_expired_time)
	{
		// Predict next time of damage
		time_of_damage = time_of_enemy_cooldown + 1500 + si.enemy->cmove->dws;
		predicted_attack = true;
	}
	int time_till_damage = time_of_damage - si.time;
	if (time_till_damage > si.subject->cmove->duration && si.subject->energy + si.subject->cmove->energy >= 0) // Can squeeze in one charge
	{
		r_action->type = atype_Charged;
		r_action->value = -1;
	}
	else if (time_till_damage > si.subject->fmove->duration) // Can squeeze in one fast
	{
		r_action->type = atype_Fast;
	}
	else
	{
		if (predicted_attack)
		{
			r_action->type = atype_Wait;
		}
		else
		{
			int delay = time_till_damage - GameMaster::dodge_window;
			r_action->type = atype_Dodge;
			r_action->delay = delay > 0 ? delay : 0;
		}
	}
}

void attacker_dodge_charged_on_attack(const StrategyInput &si, Action *r_action)
{
	int time_of_damage;
	if (si.enemy_action.type == atype_Fast)
	{
		// Ignore the coming enemy fast attack
		attacker_no_dodge_on_free(si, r_action);
		return;
	}
	else
	{
		time_of_damage = si.enemy_action.time + si.enemy->cmove->dws;
	}
	int time_till_damage = time_of_damage - si.time;
	if (time_till_damage > si.subject->cmove->duration && si.subject->energy + si.subject->cmove->energy >= 0) // Can squeeze in one charge
	{
		r_action->type = atype_Charged;
		r_action->value = -1;
	}
	else if (time_till_damage > si.subject->fmove->duration) // Can squeeze in one fast
	{
		r_action->type = atype_Fast;
	}
	else // Just dodge
	{
		int delay = time_till_damage - GameMaster::dodge_window;
		r_action->type = atype_Dodge;
		r_action->delay = delay > 0 ? delay : 0;
	}
}

void attacker_dodge_all_on_free(const StrategyInput &si, Action *r_action)
{
	bool predicted_attack = false;
	int time_of_damage = -1, time_of_enemy_cooldown = -1;
	if (si.enemy_action.type == atype_Fast)
	{
		time_of_damage = si.enemy_action.time + si.enemy->fmove->dws;
		time_of_enemy_cooldown = si.enemy_action.time + si.enemy->fmove->duration;
	}
	else if (si.enemy_action.type == atype_Charged)
	{
		time_of_damage = si.enemy_action.time + si.enemy->cmove->dws;
		time_of_enemy_cooldown = si.enemy_action.time + si.enemy->cmove->duration;
	}
	if (time_of_damage < si.time || time_of_damage < si.subject->damage_reduction_expired_time)
	{
		// Predict next time of damage
		// int min_dws = (si.enemy->fmove->dws < si.enemy->cmove->dws ? si.enemy->fmove->dws : si.enemy->cmove->dws);
		time_of_damage = time_of_enemy_cooldown + 1500 + si.enemy->fmove->dws;
		predicted_attack = true;
	}
	int time_till_damage = time_of_damage - si.time;
	if (time_till_damage > si.subject->cmove->duration && si.subject->energy + si.subject->cmove->energy >= 0) // Can squeeze in one charge
	{
		r_action->type = atype_Charged;
		r_action->value = -1;
	}
	else if (time_till_damage > si.subject->fmove->duration) // Can squeeze in one fast
	{
		r_action->type = atype_Fast;
	}
	else
	{
		if (predicted_attack)
		{
			r_action->type = atype_Wait;
		}
		else
		{
			int delay = time_till_damage - GameMaster::dodge_window;
			r_action->type = atype_Dodge;
			r_action->delay = delay > 0 ? delay : 0;
		}
	}
}

void attacker_dodge_all_on_attack(const StrategyInput &si, Action *r_action)
{
	int time_of_damage;
	if (si.enemy_action.type == atype_Fast)
	{
		time_of_damage = si.enemy_action.time + si.enemy->fmove->dws;
	}
	else
	{
		time_of_damage = si.enemy_action.time + si.enemy->cmove->dws;
	}
	int time_till_damage = time_of_damage - si.time;
	if (time_till_damage > si.subject->cmove->duration && si.subject->energy + si.subject->cmove->energy >= 0) // Can squeeze in one charge
	{
		r_action->type = atype_Charged;
		r_action->value = -1;
	}
	else if (time_till_damage > si.subject->fmove->duration) // Can squeeze in one fast
	{
		r_action->type = atype_Fast;
	}
	else // Just dodge
	{
		int delay = time_till_damage - GameMaster::dodge_window;
		r_action->type = atype_Dodge;
		r_action->delay = delay > 0 ? delay : 0;
	}
}

// int damage_if_dodged = (1 - GameMaster::dodge_damage_reduction_percent) * calc_damage(si.battle_p, si.enemy, enemy_move, subject);
