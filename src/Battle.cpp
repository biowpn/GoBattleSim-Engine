
#include "Battle.h"

#include <math.h>
#include <string.h>

namespace GoBattleSim
{

Battle::Battle()
{
	m_has_log = 0;
	m_time_limit = 0;
	m_time = 0;
	m_weather = -1;
	m_defeated_team = -1;

	m_player_states = nullptr;
	m_players_count = 0;
	m_players_count_max = 0;

	m_pokemon = nullptr;
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
	return m_player_states[t_index].player;
}

void Battle::add(Player *t_player)
{
	if (m_players_count >= m_players_count_max)
	{
		m_players_count_max += 7;
		PlayerState *temp_player_states = new PlayerState[m_players_count_max];
		for (int i = 0; i < m_players_count; ++i)
		{
			temp_player_states[i] = m_player_states[i];
		}
		if (m_player_states)
		{
			delete[] m_player_states;
		}
		m_player_states = temp_player_states;
	}
	t_player = new Player(*t_player);
	m_player_states[m_players_count].player = t_player;
	++m_players_count;

	fetch_pokemon();

	m_player_states[m_players_count].head_index = search(t_player->get_head());
}

void Battle::update(Player *t_player)
{
	for (int i = 0; i < m_players_count; ++i)
	{
		if (m_player_states[i].player->id == t_player->id)
		{
			delete m_player_states[i].player;
			m_player_states[i].player = new Player(*t_player);
		}
	}
	fetch_pokemon();
}

void Battle::update(Pokemon *t_pokemon)
{
	for (int i = 0; i < m_players_count; ++i)
	{
		m_player_states[i].player->update(t_pokemon);
	}
	for (int i = 0; i < m_pokemon_count; ++i)
	{
		if (m_pokemon[i]->id == t_pokemon->id)
		{
			delete m_pokemon[i];
			m_pokemon[i] = new Pokemon(*t_pokemon);
		}
	}
}

void Battle::erase_players()
{
	if (m_player_states && m_players_count > 0)
	{
		for (int i = 0; i < m_players_count; ++i)
		{
			delete m_player_states[i].player;
		}
		delete[] m_player_states;
	}
	erase_pokemon();
	m_player_states = nullptr;
	m_players_count = 0;
	m_players_count_max = 0;
}

void Battle::erase_pokemon()
{
	if (m_pokemon && m_pokemon_count > 0)
	{
		delete[] m_pokemon;
	}
	m_pokemon = nullptr;
	m_pokemon_count = 0;
}

void Battle::fetch_pokemon()
{
	erase_pokemon();
	for (int i = 0; i < m_players_count; ++i)
	{
		m_pokemon_count += m_player_states[i].player->get_pokemon_count();
	}
	m_pokemon = new Pokemon *[m_pokemon_count];
	Pokemon **temp = m_pokemon;
	for (int i = 0; i < m_players_count; ++i)
	{
		m_player_states[i].player->get_all_pokemon(temp);
		temp += m_player_states[i].player->get_pokemon_count();
	}
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

int Battle::search(const Player *t_player)
{
	for (int i = 0; i < m_players_count; ++i)
	{
		if (m_player_states[i].player == t_player)
		{
			return i;
		}
	}
	return -1;
}

int Battle::search_rival(int t_player_index)
{
	int team = m_player_states[t_player_index].player->team;
	for (int i = 0; i < m_players_count; ++i)
	{
		if (m_player_states[i].player->team != team)
		{
			return i;
		}
	}
	return -1;
}

void Battle::init()
{
	m_time = 0;
	m_timeline.erase();
	m_defeated_team = -1;

	if (m_has_log)
	{
		erase_log();
		m_tenode_last = m_tenode_first = new TimelineEventNode{nullptr};
	}

	for (int i = 0; i < m_players_count; ++i)
	{
		m_player_states[i].player->init();
		m_player_states[i].head_index = search(m_player_states[i].player->get_head());
		m_player_states[i].time_free = 0;
		m_player_states[i].current_action = Action();
		m_player_states[i].buffer_action = Action();
	}
}

void Battle::start()
{
	for (int i = 0; i < m_players_count; ++i)
	{
		m_timeline.put(TimelineEvent(
			etype_Enter,
			m_time,
			i,
			m_player_states[i].head_index));
	}
	go();
	for (int i = 0; i < m_players_count; ++i)
	{
		Pokemon *pokemon = m_pokemon[m_player_states[i].head_index];
		if (pokemon->active)
		{
			pokemon->duration = m_time - pokemon->duration;
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
		next(m_timeline.get());
	}
}

void Battle::next(const TimelineEvent &t_event)
{
	m_time = t_event.time;
	switch (t_event.type)
	{
	case etype_Free:
		handle_event_free(t_event);
		break;
	case etype_Announce:
		handle_event_announce(t_event);
		break;
	case etype_Fast:
		handle_event_fast(t_event);
		break;
	case etype_Charged:
		handle_event_charged(t_event);
		break;
	case etype_Dodge:
		handle_event_dodge(t_event);
		break;
	case etype_Enter:
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
		if (m_player_states[i].player->team == t_team)
		{
			sum_tdo += m_player_states[i].player->get_tdo();
			sum_deaths += m_player_states[i].player->get_num_deaths();
		}
		else
		{
			sum_rival_max_hp += m_player_states[i].player->get_max_hp();
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

void Battle::handle_fainted_pokemon(int t_player_index, Pokemon *t_pokemon)
{
	Player *player = m_player_states[t_player_index].player;
	Party *party = player->get_head_party();
	++t_pokemon->num_deaths;
	t_pokemon->duration = m_time - t_pokemon->duration;
	t_pokemon->active = false;

	int time_new_enter = -1;
	if (player->choose_next_pokemon()) // Select next Pokemon from current party
	{
		time_new_enter = m_time + GameMaster::swap_duration;
	}
	else if (player->revive_current_party()) // Max revive current party and re-lobby
	{
		time_new_enter = m_time + GameMaster::rejoin_duration + GameMaster::item_menu_animation_time + party->get_pokemon_count() * GameMaster::max_revive_time_per_pokemon;
	}
	else if (player->choose_next_party()) // Select next Party and re-lobby
	{
		time_new_enter = m_time + GameMaster::rejoin_duration;
	}

	if (time_new_enter > 0) // Player chose a new head Pokemon
	{
		m_timeline.put(TimelineEvent(
			etype_Enter,
			time_new_enter,
			t_player_index,
			search(player->get_head())));
	}
	else if (is_defeated(player->team)) // Player is out of play. Check if his team is defeated
	{
		m_defeated_team = player->team;
	}
}

bool Battle::is_defeated(int t_team)
{
	for (int i = 0; i < m_players_count; ++i)
	{
		if (m_player_states[i].player->team == t_team && m_pokemon[m_player_states[i].head_index]->is_alive())
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
	PlayerState &ps = m_player_states[t_player_index];
	ps.current_action = t_action;
	ps.current_action.time = m_time + t_action.delay;
	switch (t_action.type)
	{
	case atype_Fast:
		register_action_fast(t_player_index, t_action);
		break;
	case atype_Charged:
		register_action_charged(t_player_index, t_action);
		break;
	case atype_Dodge:
		register_action_dodge(t_player_index, t_action);
		break;
	case atype_Switch:
		register_action_switch(t_player_index, t_action);
		break;
	case atype_Wait:
		register_action_wait(t_player_index, t_action);
		break;
	default:
		break;
	}
}

void Battle::register_action_fast(int t_player_index, const Action &t_action)
{
	PlayerState &ps = m_player_states[t_player_index];
	int time_action_start = m_time + t_action.delay;
	int t_pokemon_index = ps.head_index;
	Move *move = m_pokemon[t_pokemon_index]->get_fmove(t_action.value);
	m_timeline.put(TimelineEvent(
		etype_Fast,
		time_action_start + move->dws,
		t_player_index,
		t_action.value));
	ps.time_free = time_action_start + move->duration;
	if (ps.player->team == 0)
	{
		ps.time_free += (rand() % 1000 + 1500);
		m_timeline.put(TimelineEvent(
			etype_Announce,
			time_action_start,
			t_player_index,
			t_action.value));
	}
	m_timeline.put(TimelineEvent(
		etype_Free,
		ps.time_free,
		t_player_index));
}

void Battle::register_action_charged(int t_player_index, const Action &t_action)
{
	PlayerState &ps = m_player_states[t_player_index];
	int time_action_start = m_time + t_action.delay;
	int t_pokemon_index = ps.head_index;
	Move *move = m_pokemon[t_pokemon_index]->get_cmove(t_action.value);
	if (m_pokemon[t_pokemon_index]->energy + move->energy < 0)
	{
		ps.time_free = time_action_start + 500;
		return;
	}
	m_timeline.put(TimelineEvent(
		etype_Charged,
		time_action_start + move->dws,
		t_player_index,
		t_action.value));
	ps.time_free = time_action_start + move->duration;
	if (ps.player->team == 0)
	{
		ps.time_free += (rand() % 1000 + 1500);
		m_timeline.put(TimelineEvent(
			etype_Announce,
			time_action_start,
			t_player_index,
			t_action.value));
	}
	m_timeline.put(TimelineEvent(
		etype_Free,
		ps.time_free,
		t_player_index));
}

void Battle::register_action_dodge(int t_player_index, const Action &t_action)
{
	PlayerState &ps = m_player_states[t_player_index];
	int time_action_start = m_time + t_action.delay;
	m_timeline.put(TimelineEvent(
		etype_Dodge,
		time_action_start,
		t_player_index));
	ps.time_free = time_action_start + GameMaster::dodge_duration;
	m_timeline.put(TimelineEvent(
		etype_Free,
		ps.time_free,
		t_player_index));
}

void Battle::register_action_switch(int t_player_index, const Action &t_action)
{
	PlayerState &ps = m_player_states[t_player_index];
	int time_action_start = m_time + t_action.delay;
	m_timeline.put(TimelineEvent(
		etype_Enter,
		time_action_start,
		t_player_index,
		search(ps.player->get_head_party()->get_pokemon(t_action.value))));
	ps.time_free = time_action_start + GameMaster::swap_duration;
	m_timeline.put(TimelineEvent(
		etype_Free,
		ps.time_free,
		t_player_index));
}

void Battle::register_action_wait(int t_player_index, const Action &t_action)
{
	PlayerState &ps = m_player_states[t_player_index];
	ps.time_free = m_time;
	if (t_action.value > 0) // Finite waiting
	{
		m_timeline.put(TimelineEvent(
			etype_Free,
			ps.time_free + t_action.value,
			t_player_index));
	}
}

StrategyInput Battle::generate_strat_input(int t_player_index)
{
	PlayerState &ps = m_player_states[t_player_index];
	PlayerState &enemy_ps = m_player_states[search_rival(t_player_index)];
	if (ps.time_free < m_time)
	{
		ps.time_free = m_time;
	}
	StrategyInput strat_input = {
		ps.time_free,
		m_pokemon[ps.head_index],
		m_pokemon[enemy_ps.head_index],
		ps.current_action,
		enemy_ps.current_action,
		rand(),
		m_weather};
	return strat_input;
}

void Battle::handle_event_free(const TimelineEvent &t_event)
{
	int player_index = t_event.player;
	PlayerState &ps = m_player_states[player_index];
	Pokemon *subject = m_pokemon[ps.head_index];
	if (!subject->active || ps.time_free > m_time)
	{
		return;
	}
	if (ps.buffer_action.type == atype_None) // No buffer action, call on_free
	{
		Action action;
		ps.player->strategy.on_free(generate_strat_input(player_index), &action);
		register_action(player_index, action);
	}
	else // Clear and execute buffer action
	{
		register_action(player_index, ps.buffer_action);
		ps.buffer_action.type = atype_None;
	}
	if (ps.player->strategy.on_clear) // Ask for buffer action is on_clear is not NULL
	{
		ps.player->strategy.on_clear(generate_strat_input(player_index), &(ps.buffer_action));
	}
}

void Battle::handle_event_announce(const TimelineEvent &t_event)
{
	int team = m_player_states[t_event.player].player->team;
	for (int i = 0; i < m_players_count; ++i)
	{
		PlayerState &ps = m_player_states[i];
		if (ps.player->team == team)
			continue;
		if (ps.player->strategy.on_attack)
		{
			if (ps.current_action.type == atype_None || ps.current_action.type == atype_Wait)
			{
				Action action;
				ps.player->strategy.on_attack(generate_strat_input(i), &action);
				register_action(i, action);
			}
			else
			{
				ps.player->strategy.on_attack(generate_strat_input(i), &(ps.buffer_action));
			}
		}
	}
}

void Battle::handle_event_fast(const TimelineEvent &t_event)
{
	int player_index = t_event.player;
	PlayerState &ps = m_player_states[player_index];
	Pokemon *subject = m_pokemon[ps.head_index];
	if (!subject->active)
	{
		return;
	}
	Move *move = subject->get_fmove(t_event.value);
	++subject->num_fmoves_used;
	subject->charge(move->energy);
	for (int i = 0; i < m_players_count; ++i)
	{
		if (m_player_states[i].player->team == ps.player->team)
			continue;
		Pokemon *opponent = m_pokemon[m_player_states[i].head_index];
		if (!opponent->active)
			continue;
		int damage = calc_damage(subject, move, opponent, m_weather);
		if (m_time < opponent->damage_reduction_expired_time)
		{
			damage = (1 - GameMaster::dodge_damage_reduction_percent) * damage;
			damage = damage > 0 ? damage : 1;
		}
		subject->attribute_damage(damage, true);
		opponent->hurt(damage);
		opponent->charge(ceil(GameMaster::energy_delta_per_health_lost * damage));
		if (!opponent->is_alive())
		{
			handle_fainted_pokemon(i, opponent);
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
	PlayerState &ps = m_player_states[player_index];
	Pokemon *subject = m_pokemon[ps.head_index];
	if (!subject->active)
	{
		return;
	}
	Move *move = subject->get_cmove(t_event.value);
	++subject->num_cmoves_used;
	subject->charge(move->energy);
	for (int i = 0; i < m_players_count; ++i)
	{
		if (m_player_states[i].player->team == ps.player->team)
			continue;
		Pokemon *opponent = m_pokemon[m_player_states[i].head_index];
		if (!opponent->active)
			continue;
		int damage = calc_damage(subject, move, opponent, m_weather);
		if (m_time < opponent->damage_reduction_expired_time)
		{
			damage = (1 - GameMaster::dodge_damage_reduction_percent) * damage;
			damage = damage > 0 ? damage : 1;
		}
		subject->attribute_damage(damage, false);
		opponent->hurt(damage);
		opponent->charge(ceil(GameMaster::energy_delta_per_health_lost * damage));
		if (!opponent->is_alive())
		{
			handle_fainted_pokemon(i, opponent);
		}
	}
	if (m_has_log)
	{
		append_log(t_event);
	}
}

void Battle::handle_event_dodge(const TimelineEvent &t_event)
{
	int player_index = t_event.player;
	PlayerState &ps = m_player_states[player_index];
	Pokemon *subject = m_pokemon[ps.head_index];
	subject->damage_reduction_expired_time = m_time + GameMaster::dodge_window + 1;
	if (m_has_log)
	{
		append_log(t_event);
	}
}

void Battle::handle_event_enter(const TimelineEvent &t_event)
{
	int player_index = t_event.player;
	PlayerState &ps = m_player_states[player_index];
	Pokemon *cur_head_pokemon = m_pokemon[ps.head_index];
	Pokemon *new_head_pokemon = m_pokemon[t_event.value];
	cur_head_pokemon->active = false;
	ps.player->set_head(new_head_pokemon);
	ps.head_index = t_event.value;
	ps.current_action = Action(atype_Switch);
	new_head_pokemon->active = true;
	new_head_pokemon->duration = m_time;
	m_timeline.put(TimelineEvent(
		etype_Free,
		m_time + 500,
		player_index));
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
