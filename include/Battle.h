
#ifndef _BATTLE_H_
#define _BATTLE_H_

#include "Player.h"
#include "TimelineEvent.h"

#include <vector>

namespace GoBattleSim
{

constexpr unsigned MAX_NUM_PLAYERS = 32;

struct PvEBattleOutcome
{
	int duration;
	bool win;
	int tdo;
	double tdo_percent;
	int num_deaths;

	// TODO: by-player stats

	std::vector<TimelineEvent> battle_log;
};

class Battle
{
public:
	Battle();
	~Battle();

	Player *get_player(int);
	void add(const Player *);
	void update(const Player *);
	void update(const Pokemon *);
	void erase_players();
	void set_time_limit(int);
	void set_weather(int);
	void set_enable_log(bool);
	void init();
	void start();
	PvEBattleOutcome get_outcome(int);
	const std::vector<TimelineEvent> &get_log();

protected:
	struct PlayerState
	{
		Player player;
		int head_index;
		int time_free;
		Action current_action;
		Action buffer_action;
	};

	void fetch_pokemon();
	void erase_pokemon();

	int search(const Pokemon *);
	int search_rival(unsigned);

	void enqueue(TimelineEvent &&);
	TimelineEvent dequeue();

	void go();

	void handle_fainted_pokemon(unsigned, unsigned);

	// three possible actions when a Pokemon faints
	bool select_next_pokemon(PlayerState &);
	bool revive_current_party(PlayerState &);
	bool select_next_party(PlayerState &);

	bool is_defeated(int);
	bool is_end();

	void register_action(unsigned, const Action &);

	void register_action_fast(unsigned, const Action &);
	void register_action_charged(unsigned, const Action &);
	void register_action_dodge(unsigned, const Action &);
	void register_action_switch(unsigned, const Action &);
	void register_action_wait(unsigned, const Action &);

	inline StrategyInput generate_strat_input(unsigned);

	void next(const TimelineEvent &);

	void handle_event_free(const TimelineEvent &);
	void handle_event_announce(const TimelineEvent &);
	void handle_event_fast(const TimelineEvent &);
	void handle_event_charged(const TimelineEvent &);
	void handle_event_dodge(const TimelineEvent &);
	void handle_event_enter(const TimelineEvent &);

	inline void append_log(const TimelineEvent &);
	void erase_log();

	std::vector<TimelineEvent> m_event_queue;
	std::vector<TimelineEvent> m_event_history;

	PlayerState m_player_states[MAX_NUM_PLAYERS];
	unsigned m_players_count;

	Pokemon *m_pokemon[MAX_NUM_PLAYERS * MAX_NUM_PARTIES * MAX_NUM_POKEMON];
	PokemonState m_pokemon_states[MAX_NUM_PLAYERS * MAX_NUM_PARTIES * MAX_NUM_POKEMON];
	unsigned m_pokemon_count;

	int m_has_log;
	int m_time_limit;
	int m_time;
	int m_weather;
	int m_defeated_team;

private:
	int *search_int_member(const char *);
};

} // namespace GoBattleSim

#endif