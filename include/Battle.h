
#ifndef _BATTLE_H_
#define _BATTLE_H_

#include "Player.h"

#include <vector>

namespace GoBattleSim
{

constexpr unsigned MAX_NUM_PLAYERS = 32;

enum class EventType
{
	None,
	Announce,
	Free,
	Fast,
	Charged,
	Dodge,
	Enter
};

struct TimelineEvent
{
	EventType type;
	int time;
	int player;
	int value;
};

bool operator<(const TimelineEvent &, const TimelineEvent &);

struct BattleOutcome
{
	int duration;
	bool win;
	int tdo;
	double tdo_percent;
	int num_deaths;
};

class Battle
{
public:
	Battle();
	~Battle();

	// Interface functions
	Player *get_player(int);
	void add(const Player *);
	void update(const Player *);
	void update(const Pokemon *);
	void erase_players();
	void set_time_limit(int);
	void set_weather(int);
	void set_enable_log(bool);
	bool has_attr(const char *);
	void set_attr(const char *, int);
	int get_attr(const char *);

	void init();
	void start();
	BattleOutcome get_outcome(int);
	const std::vector<TimelineEvent> &get_log();
	// End of Interface functions

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
	int search_rival(int);

	void enqueue(TimelineEvent &&);
	TimelineEvent dequeue();

	void go();

	void handle_fainted_pokemon(int, int);

	// three possible actions when a Pokemon faints
	bool select_next_pokemon(PlayerState &);
	bool revive_current_party(PlayerState &);
	bool select_next_party(PlayerState &);

	bool is_defeated(int);
	bool is_end();

	void register_action(int, const Action &);

	void register_action_fast(int, const Action &);
	void register_action_charged(int, const Action &);
	void register_action_dodge(int, const Action &);
	void register_action_switch(int, const Action &);
	void register_action_wait(int, const Action &);

	inline StrategyInput generate_strat_input(int);

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
	int m_players_count;

	Pokemon *m_pokemon[MAX_NUM_PLAYERS * MAX_NUM_PARTIES * MAX_NUM_POKEMON];
	PokemonState m_pokemon_states[MAX_NUM_PLAYERS * MAX_NUM_PARTIES * MAX_NUM_POKEMON];
	int m_pokemon_count;

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