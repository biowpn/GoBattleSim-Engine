
#include "SimplePvPBattle.h"

namespace GoBattleSim
{

SimplePvPBattle::SimplePvPBattle(const PvPPokemon *t_pokemon_0, const PvPPokemon *t_pokemon_1)
{
	m_pokemon[0] = t_pokemon_0;
	m_pokemon[1] = t_pokemon_1;
	m_own_pokemon = false;

	m_num_shields_max[0] = m_pokemon[0]->num_shields_max;
	m_num_shields_max[1] = m_pokemon[1]->num_shields_max;
	m_strategies[0] = m_pokemon[0]->cmoves_count > 1 ? pvp_advance : pvp_basic;
	m_strategies[1] = m_pokemon[1]->cmoves_count > 1 ? pvp_advance : pvp_basic;
	m_branch[0] = nullptr;
	m_branch[1] = nullptr;
}

SimplePvPBattle::SimplePvPBattle(const SimplePvPBattle &other)
{
	m_pokemon[0] = other.m_pokemon[0];
	m_pokemon[1] = other.m_pokemon[1];
	m_own_pokemon = false;
	m_pkms[0] = other.m_pkms[0];
	m_pkms[1] = other.m_pkms[1];
	m_ended = other.m_ended;

	m_num_shields_max[0] = other.m_num_shields_max[0];
	m_num_shields_max[1] = other.m_num_shields_max[1];
	m_strategies[0] = other.m_strategies[0];
	m_strategies[1] = other.m_strategies[1];
	m_branch[0] = nullptr;
	m_branch[1] = nullptr;
}

SimplePvPBattle::~SimplePvPBattle()
{
	if (m_own_pokemon)
	{
		delete m_pokemon[0];
		delete m_pokemon[1];
		m_pokemon[0] = nullptr;
		m_pokemon[1] = nullptr;
	}
	if (m_branch[0])
	{
		delete m_branch[0];
		m_branch[0] = nullptr;
	}
	if (m_branch[1])
	{
		delete m_branch[1];
		m_branch[1] = nullptr;
	}
}

void SimplePvPBattle::set_num_shields_max(int t_num_0, int t_num_1)
{
	m_num_shields_max[0] = t_num_0;
	m_num_shields_max[1] = t_num_1;
}

void SimplePvPBattle::set_strategy(int t_pkm_idx, const PvPStrategy &t_strategy)
{
	m_strategies[t_pkm_idx] = t_strategy;
}

void SimplePvPBattle::init()
{
	for (int i = 0; i < 2; ++i)
	{
		m_pkms[i].hp = m_pokemon[i]->max_hp;
		m_pkms[i].energy = 0;
		m_pkms[i].cooldown = 0;
		m_pkms[i].shields = m_num_shields_max[i];
		m_pkms[i].decision.type = atype_None;
	}
	m_ended = false;
}

void SimplePvPBattle::start()
{
	while (!m_ended)
	{
		for (int i = 0; i < 2; ++i)
		{
			if (m_pkms[i].decision.type == atype_None && m_pkms[i].cooldown <= 0)
			{
				m_strategies[i].on_free(generate_strat_input(i), &(m_pkms[i].decision));
			}
		}

		switch (m_pkms[0].decision.type)
		{
		case atype_None:
			switch (m_pkms[1].decision.type)
			{
			case atype_Fast:
				register_action_fast(1, m_pkms[1].decision);
				break;
			case atype_Charged:
				register_action_charged(1, m_pkms[1].decision);
				break;
			default:
				break;
			}
			break;
		case atype_Fast:
			register_action_fast(0, m_pkms[0].decision);
			switch (m_pkms[1].decision.type)
			{
			case atype_Fast:
				register_action_fast(1, m_pkms[1].decision);
				break;
			case atype_Charged:
				if (!m_ended)
					register_action_charged(1, m_pkms[1].decision);
				break;
			default:
				break;
			}
			break;
		case atype_Charged:
			switch (m_pkms[1].decision.type)
			{
			case atype_Fast:
				register_action_fast(1, m_pkms[1].decision);
				if (!m_ended)
					register_action_charged(0, m_pkms[0].decision);
				break;
			case atype_Charged:
				register_action_charged(0, m_pkms[0].decision);
				if (!m_ended)
					register_action_charged(1, m_pkms[1].decision);
				break;
			default:
				register_action_charged(0, m_pkms[0].decision);
				break;
			}
			break;
		default:
			break;
		}

		for (int i = 0; i < 2; ++i)
		{
			--m_pkms[i].cooldown;
		}
	}
}

void SimplePvPBattle::register_action_fast(int i, const Action &t_action)
{
	const Move *move = m_pokemon[i]->fmove;
	m_pkms[i].energy += move->energy;
	if (m_pkms[i].energy > GameMaster::max_energy)
	{
		m_pkms[i].energy = GameMaster::max_energy;
	}
	int damage = calc_damage_pvp_fmove(m_pokemon[i], move, m_pokemon[1 - i]);
	m_pkms[1 - i].hp -= damage;
	if (m_pkms[1 - i].hp <= 0)
	{
		m_ended = true;
	}
	m_pkms[i].cooldown = move->duration;
	m_pkms[i].decision.type = atype_None;
}

void SimplePvPBattle::register_action_charged(int i, const Action &t_action)
{
	int damage = 0;
	Move *move = m_pokemon[i]->get_cmove(t_action.value);
	if (m_pkms[i].energy + move->energy < 0)
	{
		return;
	}
	m_pkms[i].energy += move->energy;
	if (m_pkms[1 - i].shields > 0)
	{
		Action action(atype_Dodge); // Default use shield, unless strategy decides otherwise
		if (m_strategies[1 - i].on_attack)
		{
			m_strategies[1 - i].on_attack(generate_strat_input(1 - i), &action);
		}
		if (action.type == atype_Dodge)
		{
			damage = 1;
			--m_pkms[1 - i].shields;
		}
	}
	if (damage == 0)
	{
		damage = calc_damage_pvp_cmove(m_pokemon[i], move, m_pokemon[1 - i]);
	}
	m_pkms[1 - i].hp -= damage;
	if (m_pkms[1 - i].hp <= 0)
	{
		m_ended = true;
	}
	m_pkms[i].cooldown = 0;
	m_pkms[1 - i].cooldown = 0;
	m_pkms[i].decision.type = atype_None;

	if (move->effect.activation_chance > 0)
	{
		handle_move_effect(i, move->effect);
	}
}

void SimplePvPBattle::handle_move_effect(int i, const MoveEffect &t_effect)
{
	PvPPokemon *m_pokemon_temp[2] = {
		new PvPPokemon(*m_pokemon[0]),
		new PvPPokemon(*m_pokemon[1])};
	m_pokemon_temp[i]->buff(t_effect.self_attack_stage_delta, t_effect.self_defense_stage_delta);
	m_pokemon_temp[1 - i]->buff(t_effect.target_attack_stage_delta, t_effect.target_defense_stage_delta);

	if (t_effect.activation_chance < 1)
	{
		m_branch[0] = new SimplePvPBattle(*this);
		m_branch_weight[0] = 1 - t_effect.activation_chance;

		m_branch[1] = new SimplePvPBattle(*this);
		m_branch_weight[1] = t_effect.activation_chance;
		m_branch[1]->m_pokemon[0] = m_pokemon_temp[0];
		m_branch[1]->m_pokemon[1] = m_pokemon_temp[1];
		m_branch[1]->m_own_pokemon = true;

		m_branch[0]->start();
		m_branch[1]->start();
		m_ended = true;
	}
	else
	{
		m_pokemon[0] = m_pokemon_temp[0];
		m_pokemon[1] = m_pokemon_temp[1];
		m_own_pokemon = true;
	}
}

PvPStrategyInput SimplePvPBattle::generate_strat_input(int i)
{
	return {
		m_pokemon[i],
		m_pokemon[1 - i],
		m_pkms[i].hp,
		m_pkms[1 - i].hp,
		m_pkms[i].energy,
		m_pkms[1 - i].energy,
		m_pkms[i].shields,
		m_pkms[1 - i].shields};
}

SimplePvPBattleOutcome SimplePvPBattle::get_outcome()
{
	if (m_branch[0] && m_branch[1])
	{
		SimplePvPBattleOutcome branch_outcomes[2] = {
			m_branch[0]->get_outcome(),
			m_branch[1]->get_outcome()};
		return {
			{m_branch_weight[0] * branch_outcomes[0].tdo_percent[0] + m_branch_weight[1] * branch_outcomes[1].tdo_percent[0],
			 m_branch_weight[0] * branch_outcomes[0].tdo_percent[1] + m_branch_weight[1] * branch_outcomes[1].tdo_percent[1]}};
	}
	else
	{
		double raw_tdos[2] = {
			1 - (double)m_pkms[1].hp / m_pokemon[1]->max_hp,
			1 - (double)m_pkms[0].hp / m_pokemon[0]->max_hp};
		return {
			{raw_tdos[0] < 1 ? raw_tdos[0] : 1,
			 raw_tdos[1] < 1 ? raw_tdos[1] : 1}};
	}
}

} // namespace GoBattleSim
