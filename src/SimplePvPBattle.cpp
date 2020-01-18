
#include "SimplePvPBattle.h"
#include "GameMaster.h"

#include <cstdlib>
#include <stdio.h>
#include <stdexcept>

namespace GoBattleSim
{

SimplePvPBattle::SimplePvPBattle(const SimplePvPBattle &other)
{
	m_pkm[0] = other.m_pkm[0];
	m_pkm[1] = other.m_pkm[1];
	m_pkm_states[0] = other.m_pkm_states[0];
	m_pkm_states[1] = other.m_pkm_states[1];
	m_ended = other.m_ended;
	m_turn = other.m_turn;
	m_enable_log = false;
	m_enable_branching = other.m_enable_branching;

	m_num_shields_max[0] = other.m_num_shields_max[0];
	m_num_shields_max[1] = other.m_num_shields_max[1];
	m_strategies[0] = other.m_strategies[0];
	m_strategies[1] = other.m_strategies[1];
	m_branch[0] = nullptr;
	m_branch[1] = nullptr;
}

SimplePvPBattle::~SimplePvPBattle()
{
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

void SimplePvPBattle::set_pokemon(const PvPPokemon &pkm1, const PvPPokemon &pkm2)
{
	m_pkm[0] = pkm1;
	m_pkm[1] = pkm2;

	m_num_shields_max[0] = m_pkm[0].num_shields_max;
	m_num_shields_max[1] = m_pkm[1].num_shields_max;
	m_strategies[0] = m_pkm[0].cmoves_count > 1 ? STRATEGY_PVP_ADVANCE : STRATEGY_PVP_BASIC;
	m_strategies[1] = m_pkm[1].cmoves_count > 1 ? STRATEGY_PVP_ADVANCE : STRATEGY_PVP_BASIC;
}

void SimplePvPBattle::set_num_shields_max(int shields_1, int shields_2)
{
	m_num_shields_max[0] = shields_1;
	m_num_shields_max[1] = shields_2;
}

void SimplePvPBattle::set_strategy(const PvPStrategy &strategy1, const PvPStrategy &strategy2)
{
	m_strategies[0] = strategy1;
	m_strategies[1] = strategy2;
}

void SimplePvPBattle::set_enable_log(bool enable)
{
	if (enable && m_enable_branching)
	{
		sprintf(err_msg, "cannot enable logging when branching is enabled");
		throw std::runtime_error(err_msg);
	}
	m_enable_log = enable;
}

void SimplePvPBattle::set_enable_branching(bool enable)
{
	m_enable_branching = enable;
	m_enable_log = false;
}

void SimplePvPBattle::init()
{
	for (int i = 0; i < 2; ++i)
	{
		m_pkm[i].init();
		m_pkm_states[i].hp = m_pkm[i].max_hp;
		m_pkm_states[i].energy = 0;
		m_pkm_states[i].cooldown = 0;
		m_pkm_states[i].shields = m_num_shields_max[i];
		m_pkm_states[i].decision.type = ActionType::None;
	}
	m_turn = 0;
	m_ended = false;
	erase_log();
}

void SimplePvPBattle::start()
{
	if (m_enable_log)
	{
		append_log({0, EventType::Enter, 0, 0});
		append_log({0, EventType::Enter, 1, 1});
	}

	go();

	if (m_enable_log)
	{
		for (Player_Index_t i = 0; i < 2; ++i)
		{
			if (m_pkm_states[i].hp <= 0)
			{
				append_log({m_turn, EventType::Exit, i, i});
			}
		}
	}
}

void SimplePvPBattle::go()
{
	while (!m_ended)
	{
		for (Player_Index_t i = 0; i < 2; ++i)
		{
			if (m_pkm_states[i].decision.type == ActionType::None && m_pkm_states[i].cooldown <= 0)
			{
				m_strategies[i].on_free(generate_strat_input(i), &(m_pkm_states[i].decision));
			}
		}

		switch (m_pkm_states[0].decision.type)
		{
		case ActionType::None:
			switch (m_pkm_states[1].decision.type)
			{
			case ActionType::Fast:
				register_action_fast(1, m_pkm_states[1].decision);
				break;
			case ActionType::Charged:
				register_action_charged(1, m_pkm_states[1].decision);
				break;
			default:
				break;
			}
			break;
		case ActionType::Fast:
			register_action_fast(0, m_pkm_states[0].decision);
			// do not add if(m_ended){...} here
			// want to support simultaneous knock out
			switch (m_pkm_states[1].decision.type)
			{
			case ActionType::Fast:
				// want to support simultaneous knock out
				register_action_fast(1, m_pkm_states[1].decision);
				break;
			case ActionType::Charged:
				if (!m_ended)
				{
					register_action_charged(1, m_pkm_states[1].decision);
				}
				break;
			default:
				break;
			}
			break;
		case ActionType::Charged:
			switch (m_pkm_states[1].decision.type)
			{
			case ActionType::Fast:
				register_action_fast(1, m_pkm_states[1].decision);
				if (!m_ended)
				{
					register_action_charged(0, m_pkm_states[0].decision);
				}
				break;
			case ActionType::Charged:
				decide_branch_charged_move_priority();
				break;
			default:
				register_action_charged(0, m_pkm_states[0].decision);
				break;
			}
			break;
		default:
			break;
		}

		for (Player_Index_t i = 0; i < 2; ++i)
		{
			--m_pkm_states[i].cooldown;
		}

		++m_turn;
	}
}

void SimplePvPBattle::register_action_fast(Player_Index_t i, const Action &t_action)
{
	auto move = m_pkm[i].get_fmove(0);
	m_pkm_states[i].energy += move->energy;
	if (m_pkm_states[i].energy > static_cast<int>(GameMaster::get().max_energy))
	{
		m_pkm_states[i].energy = GameMaster::get().max_energy;
	}
	auto damage = calc_damage(&m_pkm[i], move, &m_pkm[1 - i], GameMaster::get().fast_attack_bonus_multiplier);
	m_pkm_states[1 - i].hp -= damage;

	m_pkm_states[i].cooldown = move->duration;
	m_pkm_states[i].decision.type = ActionType::None;

	if (m_enable_log)
	{
		append_log({m_turn, EventType::Fast, i, 0});
		append_log({m_turn, EventType::Damage, static_cast<Player_Index_t>(1 - i), static_cast<short>(damage)});
	}

	if (m_pkm_states[1 - i].hp <= 0)
	{
		handle_fainted_pokemon(1 - i);
	}
}

void SimplePvPBattle::register_action_charged(Player_Index_t i, const Action &t_action)
{
	int damage = 0;
	auto move = m_pkm[i].get_cmove(t_action.value);
	if (m_pkm_states[i].energy + move->energy < 0)
	{
		return;
	}
	m_pkm_states[i].energy += move->energy;
	if (m_pkm_states[1 - i].shields > 0)
	{
		Action action{m_turn, ActionType::Dodge}; // Default use shield, unless strategy decides otherwise
		if (m_strategies[1 - i].on_attack)
		{
			m_strategies[1 - i].on_attack(generate_strat_input(1 - i), &action);
		}
		if (action.type == ActionType::Dodge)
		{
			damage = 1;
			--m_pkm_states[1 - i].shields;
			if (m_enable_log)
			{
				append_log({m_turn,
							EventType::Dodge,
							static_cast<Player_Index_t>(1 - i),
							static_cast<short>(1 - i)});
			}
		}
	}
	if (damage == 0)
	{
		damage = calc_damage(&m_pkm[i], move, &m_pkm[1 - i], GameMaster::get().charged_attack_bonus_multiplier);
	}
	m_pkm_states[1 - i].hp -= damage;
	m_pkm_states[i].cooldown = 0;
	m_pkm_states[1 - i].cooldown = 0;
	m_pkm_states[i].decision.type = ActionType::None;

	if (m_enable_log)
	{
		append_log({m_turn, EventType::Charged, i, t_action.value});
		append_log({m_turn, EventType::Damage, static_cast<Player_Index_t>(1 - i), static_cast<short>(damage)});
	}

	if (m_pkm_states[1 - i].hp <= 0)
	{
		handle_fainted_pokemon(1 - i);
	}
	else if (move->effect.activation_chance > 0)
	{
		decide_branch_move_effect(i, move->effect);
	}
}

void SimplePvPBattle::decide_branch_charged_move_priority()
{
	if (m_pkm[0].attack != m_pkm[1].attack)
	{
		Player_Index_t first = m_pkm[0].attack > m_pkm[1].attack ? 0 : 1;
		handle_simultaneous_charged_attacks(first);
		return;
	}

	if (m_enable_branching)
	{
		m_branch[0] = new SimplePvPBattle(*this);
		m_branch_weight[0] = 0.5;
		m_branch[0]->handle_simultaneous_charged_attacks(0);

		m_branch[1] = new SimplePvPBattle(*this);
		m_branch_weight[1] = 0.5;
		m_branch[0]->handle_simultaneous_charged_attacks(1);

		m_branch[0]->start();
		m_branch[1]->start();
		m_ended = true;
	}
	else
	{
		Player_Index_t first = rand() % 2u;
		handle_simultaneous_charged_attacks(first);
	}
}

void SimplePvPBattle::handle_simultaneous_charged_attacks(Player_Index_t first)
{
	Player_Index_t second = (Player_Index_t)1 - first;
	register_action_charged(first, m_pkm_states[first].decision);
	if (!m_ended)
	{
		register_action_charged(second, m_pkm_states[second].decision);
	}
}

void SimplePvPBattle::decide_branch_move_effect(Player_Index_t i, const MoveEffect &t_effect)
{
	if (t_effect.activation_chance >= 1)
	{
		handle_move_effect(i, t_effect);
		return;
	}

	if (m_enable_branching)
	{
		m_branch[0] = new SimplePvPBattle(*this);
		m_branch_weight[0] = 1 - t_effect.activation_chance;

		m_branch[1] = new SimplePvPBattle(*this);
		m_branch_weight[1] = t_effect.activation_chance;
		m_branch[1]->handle_move_effect(i, t_effect);

		m_branch[0]->start();
		m_branch[1]->start();
		m_ended = true;
	}
	else
	{
		if ((double)(rand() % 10000) / 10000 < t_effect.activation_chance)
		{
			handle_move_effect(i, t_effect);
		}
	}
}

void SimplePvPBattle::handle_move_effect(Player_Index_t i, const MoveEffect &t_effect)
{
	m_pkm[i].buff(t_effect.self_atk_delta, t_effect.self_def_delta);
	m_pkm[1 - i].buff(t_effect.target_atk_delta, t_effect.target_def_delta);

	if (m_enable_log)
	{
		append_log({m_turn, EventType::Effect, i, i});
	}
}

void SimplePvPBattle::handle_fainted_pokemon(Player_Index_t i)
{
	m_ended = true;
}

void SimplePvPBattle::append_log(TimelineEvent &&event)
{
	m_battle_log.emplace_back(std::move(event));
}

void SimplePvPBattle::erase_log()
{
	m_battle_log.clear();
}

PvPStrategyInput SimplePvPBattle::generate_strat_input(Player_Index_t i)
{
	return {
		&m_pkm[i],
		&m_pkm[1 - i],
		m_pkm_states[i].hp,
		m_pkm_states[1 - i].hp,
		m_pkm_states[i].energy,
		m_pkm_states[1 - i].energy,
		m_pkm_states[i].shields,
		m_pkm_states[1 - i].shields};
}

SimplePvPBattleOutcome SimplePvPBattle::get_outcome()
{
	if (m_enable_branching && m_branch[0] && m_branch[1])
	{
		SimplePvPBattleOutcome branch[2] = {
			m_branch[0]->get_outcome(),
			m_branch[1]->get_outcome()};
		return {
			{m_branch_weight[0] * branch[0].tdo_percent[0] + m_branch_weight[1] * branch[1].tdo_percent[0],
			 m_branch_weight[0] * branch[0].tdo_percent[1] + m_branch_weight[1] * branch[1].tdo_percent[1]}};
	}
	else
	{
		int tdo[2] = {m_pkm[1].max_hp - m_pkm_states[1].hp, m_pkm[0].max_hp - m_pkm_states[0].hp};
		double percents[2] = {
			(double)tdo[0] / m_pkm[1].max_hp,
			(double)tdo[1] / m_pkm[0].max_hp};
		return {
			{std::min(percents[0], 1.0), std::min(percents[1], 1.0)},
			{tdo[0], tdo[1]},
			m_turn,
			{m_pkm_states[0], m_pkm_states[1]},
			m_battle_log};
	}
}

} // namespace GoBattleSim
