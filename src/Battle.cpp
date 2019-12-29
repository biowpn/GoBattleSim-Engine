
#include "Battle.h"

#include <algorithm>
#include <math.h>
#include <string.h>
#include <stdexcept>

namespace GoBattleSim
{

bool operator<(const TimelineEvent &lhs, const TimelineEvent &rhs)
{
	return lhs.time > rhs.time || (lhs.time == rhs.time && lhs.player > rhs.player);
}

Battle::Battle()
{
	m_has_log = 0;
	m_time_limit = 0;
	m_time = 0;
	m_weather = -1;
	m_defeated_team = -1;

	m_players_count = 0;

	m_pokemon_count = 0;

	m_tenode_last = m_tenode_first = new TimelineEventNode{nullptr};
}

Battle::~Battle()
{
	erase_players();
	erase_log();
}

Player *Battle::get_player(int t_index)
{
	return &m_player_states[t_index].player;
}

void Battle::add(const Player *t_player)
{
	if (m_players_count >= MAX_NUM_PLAYERS)
	{
		throw std::runtime_error("too many players");
	}
	m_player_states[m_players_count].player = *t_player;
	++m_players_count;

	fetch_pokemon();

	m_player_states[m_players_count].head_index = search(t_player->get_head());
}

void Battle::update(const Player *t_player)
{
	for (int i = 0; i < m_players_count; ++i)
	{
		if (m_player_states[i].player.id == t_player->id)
		{
			m_player_states[i].player = *t_player;
		}
	}
	fetch_pokemon();
}

void Battle::update(const Pokemon *t_pokemon)
{
	for (int i = 0; i < m_players_count; ++i)
	{
		m_player_states[i].player.update(t_pokemon);
	}
}

void Battle::erase_players()
{
	erase_pokemon();
	m_players_count = 0;
}

void Battle::erase_pokemon()
{
	m_pokemon_count = 0;
}

void Battle::fetch_pokemon()
{
	auto out_first = m_pokemon;
	for (int i = 0; i < m_players_count; ++i)
	{
		out_first = m_player_states[i].player.get_all_pokemon(out_first);
	}
	m_pokemon_count = out_first - m_pokemon;
}

bool Battle::has_attr(const char *t_name)
{
	return search_int_member(t_name);
}

int Battle::get_attr(const char *t_name)
{
	int *int_member_ptr = search_int_member(t_name);
	if (int_member_ptr)
	{
		return *int_member_ptr;
	}
	return 0;
}

void Battle::set_attr(const char *t_name, int t_value)
{
	int *int_member_ptr = search_int_member(t_name);
	if (int_member_ptr)
	{
		*int_member_ptr = t_value;
	}
}

int *Battle::search_int_member(const char *t_name)
{
	if (strcmp(t_name, "players_count") == 0)
		return &m_players_count;
	else if (strcmp(t_name, "pokemon_count") == 0)
		return &m_pokemon_count;
	else if (strcmp(t_name, "has_log") == 0)
		return &m_has_log;
	else if (strcmp(t_name, "time_limit") == 0)
		return &m_time_limit;
	else if (strcmp(t_name, "weather") == 0)
		return &m_weather;
	else if (strcmp(t_name, "time") == 0)
		return &m_time;
	else
		return nullptr;
}

void Battle::set_time_limit(int t_time_limit)
{
	m_time_limit = t_time_limit;
}

void Battle::set_weather(int t_weather)
{
	m_weather = t_weather;
}

void Battle::set_enable_log(bool enable_log)
{
	m_has_log = enable_log;
}

int Battle::search(const Pokemon *t_pokemon)
{
	for (int i = 0; i < m_pokemon_count; ++i)
	{
		if (m_pokemon[i] == t_pokemon)
		{
			return i;
		}
	}
	return -1;
}

int Battle::search_rival(int t_player_index)
{
	int team = m_player_states[t_player_index].player.team;
	for (int i = 0; i < m_players_count; ++i)
	{
		if (m_player_states[i].player.team != team)
		{
			return i;
		}
	}
	return -1;
}

void Battle::enqueue(TimelineEvent &&e)
{
	m_event_queue.emplace_back(std::move(e));
	std::push_heap(m_event_queue.begin(), m_event_queue.end());
}

TimelineEvent Battle::dequeue()
{
	auto e = m_event_queue.front();
	std::pop_heap(m_event_queue.begin(), m_event_queue.end());
	m_event_queue.pop_back();
	return e;
}

void Battle::init()
{
	m_time = 0;
	m_event_queue.clear();
	m_defeated_team = -1;

	if (m_has_log)
	{
		erase_log();
		m_tenode_last = m_tenode_first = new TimelineEventNode{nullptr};
	}

	for (int i = 0; i < m_players_count; ++i)
	{
		m_player_states[i].player.init();
		m_player_states[i].head_index = search(m_player_states[i].player.get_head());
		m_player_states[i].time_free = 0;
		m_player_states[i].current_action = Action();
		m_player_states[i].buffer_action = Action();
	}

	for (int i = 0; i < m_pokemon_count; ++i)
	{
		m_pokemon_states[i].max_hp = m_pokemon[i]->max_hp;
		m_pokemon_states[i].immortal = m_pokemon[i]->immortal;
		m_pokemon_states[i].init();
	}
}

void Battle::start()
{
	for (int i = 0; i < m_players_count; ++i)
	{
		enqueue({EventType::Enter,
				 m_time,
				 i,
				 m_player_states[i].head_index});
	}
	go();
	for (int i = 0; i < m_players_count; ++i)
	{
		auto &pkm_st = m_pokemon_states[m_player_states[i].head_index];
		if (pkm_st.active)
		{
			pkm_st.duration = m_time - pkm_st.duration;
		}
	}
	if (m_has_log)
	{
		m_tenode_last->next = nullptr;
	}
}

void Battle::go()
{
	while (!is_end())
	{
		next(dequeue());
	}
}

void Battle::next(const TimelineEvent &t_event)
{
	m_time = t_event.time;
	switch (t_event.type)
	{
	case EventType::Free:
		handle_event_free(t_event);
		break;
	case EventType::Announce:
		handle_event_announce(t_event);
		break;
	case EventType::Fast:
		handle_event_fast(t_event);
		break;
	case EventType::Charged:
		handle_event_charged(t_event);
		break;
	case EventType::Dodge:
		handle_event_dodge(t_event);
		break;
	case EventType::Enter:
		handle_event_enter(t_event);
		break;
	default:
		break;
	}
}

BattleOutcome Battle::get_outcome(int t_team)
{
	// From team team {t_team}'s perspective
	BattleOutcome outcome = {
		m_time,
		(m_defeated_team != t_team && m_time < m_time_limit)};

	int sum_tdo = 0, sum_rival_max_hp = 0, sum_deaths = 0;
	for (int i = 0; i < m_players_count; ++i)
	{
		const auto &player = m_player_states[i].player;
		auto count = player.get_pokemon_count();
		auto first_idx = search(player.get_party(0)->get_pokemon(0));
		if (m_player_states[i].player.team == t_team)
		{
			for (int i = 0; i < count; ++i)
			{
				sum_tdo += m_pokemon_states[first_idx + i].tdo;
				sum_deaths += m_pokemon_states[first_idx + i].num_deaths;
			}
		}
		else
		{
			for (int i = 0; i < count; ++i)
			{
				sum_rival_max_hp += m_pokemon_states[first_idx + i].max_hp;
			}
		}
	}
	outcome.tdo = sum_tdo;
	outcome.tdo_percent = (double)sum_tdo / sum_rival_max_hp;
	outcome.num_deaths = sum_deaths;
	return outcome;
}

TimelineEventNode *Battle::get_log()
{
	return m_tenode_first;
}

void Battle::handle_fainted_pokemon(int t_player_index, int t_pokemon_index)
{
	auto &ps = m_player_states[t_player_index];
	auto &pkm_st = m_pokemon_states[t_pokemon_index];
	auto party = ps.player.get_head_party();
	++pkm_st.num_deaths;
	pkm_st.duration = m_time - pkm_st.duration;
	pkm_st.active = false;

	int time_new_enter = -1;
	if (select_next_pokemon(ps)) // Select next Pokemon from current party
	{
		time_new_enter = m_time + GameMaster::swap_duration;
	}
	else if (revive_current_party(ps)) // Max revive current party and re-lobby
	{
		time_new_enter = m_time + GameMaster::rejoin_duration + GameMaster::item_menu_animation_time + party->get_pokemon_count() * GameMaster::max_revive_time_per_pokemon;
	}
	else if (select_next_party(ps)) // Select next Party and re-lobby
	{
		time_new_enter = m_time + GameMaster::rejoin_duration;
	}

	if (time_new_enter > 0) // Player chose a new head Pokemon
	{
		enqueue({EventType::Enter,
				 time_new_enter,
				 t_player_index,
				 search(ps.player.get_head())});
	}
	else if (is_defeated(ps.player.team)) // Player is out of play. Check if his team is defeated
	{
		m_defeated_team = ps.player.team;
	}
}

bool Battle::select_next_pokemon(Battle::PlayerState &ps)
{
	auto party = ps.player.get_head_party();
	auto first_index = search(party->get_pokemon(0));
	auto count = ps.player.get_head_party()->get_pokemon_count();
	auto cur_head = ps.head_index;
	do
	{
		if (++ps.head_index >= first_index + count)
		{
			ps.head_index = first_index;
		}
	} while (!m_pokemon_states[ps.head_index].is_alive() && ps.head_index != cur_head);

	if (m_pokemon_states[ps.head_index].is_alive())
	{
		party->set_head(ps.head_index - first_index);
		return true;
	}
	else
	{
		return false;
	}
}

bool Battle::revive_current_party(Battle::PlayerState &ps)
{
	if (ps.player.get_head_party()->revive())
	{
		auto party = ps.player.get_head_party();
		auto first_index = search(party->get_pokemon(0));
		auto count = party->get_pokemon_count();
		for (auto i = 0; i < count; ++i)
		{
			m_pokemon_states[first_index + i].heal();
		}
		party->set_head(0);
		return true;
	}
	else
	{
		return false;
	}
}

bool Battle::select_next_party(Battle::PlayerState &ps)
{
	if (ps.player.set_head_party_to_next())
	{
		ps.head_index = search(ps.player.get_head());
		return true;
	}
	else
	{
		return false;
	}
}

bool Battle::is_defeated(int t_team)
{
	for (int i = 0; i < m_players_count; ++i)
	{
		const auto &ps = m_player_states[i];
		const auto &pkm_st = m_pokemon_states[ps.head_index];
		if (ps.player.team == t_team && pkm_st.is_alive())
		{
			return false;
		}
	}
	return true;
}

bool Battle::is_end()
{
	return m_defeated_team >= 0 || (m_time > m_time_limit && m_time_limit > 0);
}

void Battle::register_action(int t_player_index, const Action &t_action)
{
	auto &ps = m_player_states[t_player_index];
	ps.current_action = t_action;
	ps.current_action.time = m_time + t_action.delay;
	switch (t_action.type)
	{
	case ActionType::Fast:
		register_action_fast(t_player_index, t_action);
		break;
	case ActionType::Charged:
		register_action_charged(t_player_index, t_action);
		break;
	case ActionType::Dodge:
		register_action_dodge(t_player_index, t_action);
		break;
	case ActionType::Switch:
		register_action_switch(t_player_index, t_action);
		break;
	case ActionType::Wait:
		register_action_wait(t_player_index, t_action);
		break;
	default:
		break;
	}
}

void Battle::register_action_fast(int t_player_index, const Action &t_action)
{
	auto &ps = m_player_states[t_player_index];
	int time_action_start = m_time + t_action.delay;
	auto move = m_pokemon[ps.head_index]->get_fmove(t_action.value);
	enqueue({EventType::Fast,
			 time_action_start + move->dws,
			 t_player_index,
			 t_action.value});
	ps.time_free = time_action_start + move->duration;
	if (ps.player.team == 0)
	{
		ps.time_free += (rand() % 1000 + 1500);
		enqueue({EventType::Announce,
				 time_action_start,
				 t_player_index,
				 t_action.value});
	}
	enqueue({EventType::Free,
			 ps.time_free,
			 t_player_index});
}

void Battle::register_action_charged(int t_player_index, const Action &t_action)
{
	auto &ps = m_player_states[t_player_index];
	auto &pkm_st = m_pokemon_states[ps.head_index];
	auto move = m_pokemon[ps.head_index]->get_cmove(t_action.value);
	int time_action_start = m_time + t_action.delay;
	if (pkm_st.energy + move->energy < 0)
	{
		ps.time_free = time_action_start + 500;
		return;
	}
	enqueue({EventType::Charged,
			 time_action_start + move->dws,
			 t_player_index,
			 t_action.value});
	ps.time_free = time_action_start + move->duration;
	if (ps.player.team == 0)
	{
		ps.time_free += (rand() % 1000 + 1500);
		enqueue({EventType::Announce,
				 time_action_start,
				 t_player_index,
				 t_action.value});
	}
	enqueue({EventType::Free,
			 ps.time_free,
			 t_player_index});
}

void Battle::register_action_dodge(int t_player_index, const Action &t_action)
{
	auto &ps = m_player_states[t_player_index];
	int time_action_start = m_time + t_action.delay;
	enqueue({EventType::Dodge,
			 time_action_start,
			 t_player_index});
	ps.time_free = time_action_start + GameMaster::dodge_duration;
	enqueue({EventType::Free,
			 ps.time_free,
			 t_player_index});
}

void Battle::register_action_switch(int t_player_index, const Action &t_action)
{
	auto &ps = m_player_states[t_player_index];
	int time_action_start = m_time + t_action.delay;
	enqueue({EventType::Enter,
			 time_action_start,
			 t_player_index,
			 search(ps.player.get_head_party()->get_pokemon(t_action.value))});
	ps.time_free = time_action_start + GameMaster::swap_duration;
	enqueue({EventType::Free,
			 ps.time_free,
			 t_player_index});
}

void Battle::register_action_wait(int t_player_index, const Action &t_action)
{
	auto &ps = m_player_states[t_player_index];
	ps.time_free = m_time;
	if (t_action.value > 0) // Finite waiting
	{
		enqueue({EventType::Free,
				 ps.time_free + t_action.value,
				 t_player_index});
	}
}

StrategyInput Battle::generate_strat_input(int t_player_index)
{
	auto &ps = m_player_states[t_player_index];
	auto &enemy_ps = m_player_states[search_rival(t_player_index)];
	if (ps.time_free < m_time)
	{
		ps.time_free = m_time;
	}
	StrategyInput strat_input = {
		ps.time_free,
		m_pokemon[ps.head_index],
		m_pokemon[enemy_ps.head_index],
		&m_pokemon_states[ps.head_index],
		&m_pokemon_states[enemy_ps.head_index],
		ps.current_action,
		enemy_ps.current_action,
		rand(),
		m_weather};
	return strat_input;
}

void Battle::handle_event_free(const TimelineEvent &t_event)
{
	int player_index = t_event.player;
	auto &ps = m_player_states[player_index];
	auto &subject_st = m_pokemon_states[ps.head_index];
	if (!subject_st.active || ps.time_free > m_time)
	{
		return;
	}
	if (ps.buffer_action.type == ActionType::None) // No buffer action, call on_free
	{
		Action action;
		ps.player.strategy.on_free(generate_strat_input(player_index), &action);
		register_action(player_index, action);
	}
	else // Clear and execute buffer action
	{
		register_action(player_index, ps.buffer_action);
		ps.buffer_action.type = ActionType::None;
	}
	if (ps.player.strategy.on_clear) // Ask for buffer action is on_clear is not NULL
	{
		ps.player.strategy.on_clear(generate_strat_input(player_index), &(ps.buffer_action));
	}
}

void Battle::handle_event_announce(const TimelineEvent &t_event)
{
	int team = m_player_states[t_event.player].player.team;
	for (int i = 0; i < m_players_count; ++i)
	{
		auto &ps = m_player_states[i];
		if (ps.player.team == team)
			continue;
		if (ps.player.strategy.on_attack)
		{
			if (ps.current_action.type == ActionType::None || ps.current_action.type == ActionType::Wait)
			{
				Action action;
				ps.player.strategy.on_attack(generate_strat_input(i), &action);
				register_action(i, action);
			}
			else
			{
				ps.player.strategy.on_attack(generate_strat_input(i), &(ps.buffer_action));
			}
		}
	}
}

void Battle::handle_event_fast(const TimelineEvent &t_event)
{
	auto &ps = m_player_states[t_event.player];
	auto &subject_st = m_pokemon_states[ps.head_index];
	auto subject = m_pokemon[ps.head_index];
	if (!subject_st.active)
	{
		return;
	}
	auto move = subject->get_fmove(t_event.value);
	++subject_st.num_fmoves_used;
	subject_st.charge(move->energy);

	for (int i = 0; i < m_players_count; ++i)
	{
		if (m_player_states[i].player.team == ps.player.team)
		{
			continue;
		}
		auto opponent_idx = m_player_states[i].head_index;
		auto opponent = m_pokemon[opponent_idx];
		auto &opponent_st = m_pokemon_states[opponent_idx];
		if (!opponent_st.active)
		{
			continue;
		}
		int damage = calc_damage(subject, move, opponent, m_weather);
		if (m_time < opponent_st.damage_reduction_expiry)
		{
			damage = (1 - GameMaster::dodge_damage_reduction_percent) * damage;
			damage = damage > 0 ? damage : 1;
		}
		subject_st.attribute_damage(damage, true);
		opponent_st.hurt(damage);
		opponent_st.charge(ceil(GameMaster::energy_delta_per_health_lost * damage));
		if (!opponent_st.is_alive())
		{
			handle_fainted_pokemon(i, opponent_idx);
		}
	}
	if (m_has_log)
	{
		append_log(t_event);
	}
}

void Battle::handle_event_charged(const TimelineEvent &t_event)
{
	int player_index = t_event.player;
	auto &ps = m_player_states[player_index];
	auto &subject_st = m_pokemon_states[ps.head_index];
	auto subject = m_pokemon[ps.head_index];
	if (!subject_st.active)
	{
		return;
	}
	auto move = subject->get_cmove(t_event.value);
	++subject_st.num_cmoves_used;
	subject_st.charge(move->energy);

	for (int i = 0; i < m_players_count; ++i)
	{
		if (m_player_states[i].player.team == ps.player.team)
		{
			continue;
		}
		auto opponent_idx = m_player_states[i].head_index;
		auto opponent = m_pokemon[opponent_idx];
		auto &opponent_st = m_pokemon_states[opponent_idx];
		if (!opponent_st.active)
		{
			continue;
		}
		int damage = calc_damage(subject, move, opponent, m_weather);
		if (m_time < opponent_st.damage_reduction_expiry)
		{
			damage = (1 - GameMaster::dodge_damage_reduction_percent) * damage;
			damage = damage > 0 ? damage : 1;
		}
		subject_st.attribute_damage(damage, false);
		opponent_st.hurt(damage);
		opponent_st.charge(ceil(GameMaster::energy_delta_per_health_lost * damage));
		if (!opponent_st.is_alive())
		{
			handle_fainted_pokemon(i, opponent_idx);
		}
	}
	if (m_has_log)
	{
		append_log(t_event);
	}
}

void Battle::handle_event_dodge(const TimelineEvent &t_event)
{
	auto &ps = m_player_states[t_event.player];
	auto &pkm_st = m_pokemon_states[ps.head_index];
	pkm_st.damage_reduction_expiry = m_time + GameMaster::dodge_window + 1;
	if (m_has_log)
	{
		append_log(t_event);
	}
}

void Battle::handle_event_enter(const TimelineEvent &t_event)
{
	int player_index = t_event.player;
	auto &ps = m_player_states[player_index];
	auto &cur_head_st = m_pokemon_states[ps.head_index];
	auto &new_head_st = m_pokemon_states[t_event.value];
	cur_head_st.active = false;
	ps.player.set_head(m_pokemon[t_event.value]);
	ps.head_index = t_event.value;
	ps.current_action = {ActionType::Switch};
	new_head_st.active = true;
	new_head_st.duration = m_time;
	enqueue({EventType::Free,
			 m_time + 500,
			 player_index});
	if (m_has_log)
	{
		append_log(t_event);
	}
}

void Battle::append_log(const TimelineEvent &t_event)
{
	m_tenode_last->next = new TimelineEventNode;
	m_tenode_last = m_tenode_last->next;
	m_tenode_last->item = t_event;
	// m_tenode_last->next = nullptr;
}

void Battle::erase_log()
{
	TimelineEventNode *temp;
	while (m_tenode_first)
	{
		temp = m_tenode_first->next;
		delete m_tenode_first;
		m_tenode_first = temp;
	}
	m_tenode_last = nullptr;
}

} // namespace GoBattleSim
