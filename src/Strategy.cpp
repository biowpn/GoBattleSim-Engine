
#include "Strategy.h"

#include "GameMaster.h"

namespace GoBattleSim
{

// Helper functions
inline int get_projected_energy(const StrategyInput &si)
{
	int projected_energy = si.subject_state->energy;
	if (si.subject_action.type == ActionType::Fast)
	{
		projected_energy += si.subject->fmove.energy;
	}
	else if (si.subject_action.type == ActionType::Charged)
	{
		projected_energy += si.subject->cmove->energy;
	}
	return projected_energy;
}

inline int calc_damage_weather(const Pokemon *attacker,
							   const Move *move,
							   const Pokemon *defender,
							   int weather)
{
	const auto &gm = GameMaster::get();
	double multiplier = gm.boosted_weather(move->poketype) == weather ? gm.wab_multiplier : 1.0;
	return calc_damage(attacker, move, defender, multiplier);
}

void defender_on_clear(const StrategyInput &si, Action *r_action)
{
	r_action->type = ActionType::Fast;
	r_action->value = 0;
	auto projected_energy = get_projected_energy(si);
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
	if (si.subject_state->energy + si.subject->cmove->energy >= 0)
	{
		r_action->type = ActionType::Charged;
	}
	else
	{
		r_action->type = ActionType::Fast;
	}
}

void attacker_fast_only_no_dodge_on_free(const StrategyInput &si, Action *r_action)
{
	r_action->type = ActionType::Fast;
}

void attacker_burst_no_dodge_on_free(const StrategyInput &si, Action *r_action)
{
	if (si.subject_state->energy >= static_cast<int>(GameMaster::get().max_energy))
	{
		r_action->type = ActionType::Charged;
	}
	else if (si.subject_action.type == ActionType::Charged)
	{
		attacker_no_dodge_on_free(si, r_action);
	}
	else
	{
		r_action->type = ActionType::Fast;
	}
}

void attacker_dodge_charged_on_free(const StrategyInput &si, Action *r_action)
{
	bool predicted_attack = false;
	auto time_of_damage = 0u, time_of_enemy_cooldown = si.enemy_action.time;
	if (si.enemy_action.type == ActionType::Fast)
	{
		time_of_enemy_cooldown += si.enemy->fmove.duration + 1500;
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
	else if (time_till_damage > si.subject->fmove.duration) // Can squeeze in one fast
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
	unsigned time_of_damage;
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
	else if (time_till_damage > si.subject->fmove.duration) // Can squeeze in one fast
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
	auto time_of_damage = 0u, time_of_enemy_cooldown = si.enemy_action.time;
	if (si.enemy_action.type == ActionType::Fast)
	{
		time_of_damage = si.enemy_action.time + si.enemy->fmove.dws;
		time_of_enemy_cooldown += si.enemy->fmove.duration + 1500;
	}
	else if (si.enemy_action.type == ActionType::Charged)
	{
		time_of_damage = si.enemy_action.time + si.enemy->cmove->dws;
		time_of_enemy_cooldown += si.enemy->cmove->duration + 1500;
	}
	else // enemy just enters battle
	{
		time_of_damage = si.enemy_action.time + 1500 + si.enemy->fmove.dws;
		time_of_enemy_cooldown += 1500;
	}
	if (time_of_damage < si.time_free || time_of_damage < si.subject_state->damage_reduction_expiry)
	{
		// Predict next time of damage
		time_of_damage = time_of_enemy_cooldown + si.enemy->fmove.dws;
		predicted_attack = true;
	}
	int time_till_damage = time_of_damage - si.time_free;
	if (time_till_damage > si.subject->cmove->duration && si.subject_state->energy + si.subject->cmove->energy >= 0) // Can squeeze in one charge
	{
		r_action->type = ActionType::Charged;
		r_action->value = si.subject->cmove - si.subject->cmoves;
	}
	else if (time_till_damage > si.subject->fmove.duration) // Can squeeze in one fast
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
	unsigned time_of_damage;
	if (si.enemy_action.type == ActionType::Fast)
	{
		time_of_damage = si.enemy_action.time + si.enemy->fmove.dws;
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
	else if (time_till_damage > si.subject->fmove.duration) // Can squeeze in one fast
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

inline double calc_cycle_dps(const Pokemon *subj, const Move *fmove, const Move *cmove, const Pokemon *enemy, int weather)
{
	auto fdmg = calc_damage_weather(subj, fmove, enemy, weather);
	auto cdmg = calc_damage_weather(subj, cmove, enemy, weather);
	double fdps = static_cast<double>(fdmg) / fmove->duration;
	double cdps = static_cast<double>(cdmg) / cmove->duration;
	double feps = static_cast<double>(fmove->energy) / fmove->duration;
	double ceps = static_cast<double>(-cmove->energy) / cmove->duration;
	return (fdps * ceps + cdps * feps) / (feps + ceps);
}

void attacker_combo_no_dodge_on_free(const StrategyInput &si, Action *action)
{
	// find out better move and cheaper move
	double better_dps;
	const Move *better = nullptr;
	int cheaper_energy;
	const Move *cheaper = nullptr;
	for (auto cmove = si.subject->cmoves; cmove < si.subject->cmoves + si.subject->cmoves_count; ++cmove)
	{
		double cur_dps = calc_cycle_dps(si.subject, &si.subject->fmove, cmove, si.enemy, si.weather);
		if (cur_dps > better_dps || better == nullptr)
		{
			better = cmove;
			better_dps = cur_dps;
		}
		if (-cmove->energy < cheaper_energy || cheaper == nullptr)
		{
			cheaper = cmove;
			cheaper_energy = -cmove->energy;
		}
	}

	if (better == nullptr)
	{
		action->type = ActionType::Fast;
		return;
	}

	if (better == cheaper)
	{
		if (si.subject_state->energy + better->energy >= 0)
		{
			action->type = ActionType::Charged;
			action->value = better - si.subject->cmoves;
		}
		else
		{
			action->type = ActionType::Fast;
		}
		return;
	}

	if (si.subject_state->energy + better->energy >= 0)
	{
		action->type = ActionType::Charged;
		action->value = better - si.subject->cmoves;
		return;
	}

	auto enemy_fdmg = calc_damage_weather(si.enemy, &si.enemy->fmove, si.subject, si.weather);
	auto enemy_cdmg = calc_damage_weather(si.enemy, si.enemy->cmove, si.subject, si.weather);

	if (si.subject_state->hp <= 2 * enemy_fdmg ||
		(si.subject_state->hp <= enemy_cdmg && si.enemy_state->energy + si.enemy->cmove->energy >= 0))
	{
		if (si.subject_state->energy + cheaper->energy >= 0)
		{
			action->type = ActionType::Charged;
			action->value = cheaper - si.subject->cmoves;
		}
		return;
	}

	action->type = ActionType::Fast;
}

} // namespace GoBattleSim
