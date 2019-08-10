
#ifndef _BATTLE_H_
#define _BATTLE_H_

#include <stdlib.h>
#include <math.h>

#include <iostream>

#include "Player.h"
#include "Timeline.h"


struct TimelineEventNode {
	TimelineEventNode* next;
	TimelineEvent item;
};


typedef struct {
	int duration;
	bool win;
	int tdo;
	double tdo_percent;
	int num_deaths;
} BattleOutcome;



typedef struct {
	Player* player;
	int head_index;
	int time_free;
	Action current_action;
	Action buffer_action;
} PlayerState;


class Battle
{
public:
	Battle();
	~Battle();
	
	// Interface functions
	Player* get_player(int);
	void add(Player*);
	void update(Player*);
	void update(Pokemon*);
	void erase_players();
	void set_time_limit(int);
	void set_weather(int);
	bool has_attr(const char*);
	void set_attr(const char*, int);
	int get_attr(const char*);
	
	void init();
	void start();
	BattleOutcome get_outcome(int);
	TimelineEventNode* get_log();
	// End of Interface functions
	
	
	
	// for debug
	// void debug();
	

protected:
	void fetch_pokemon();
	void erase_pokemon();

	int search(const Pokemon*);
	int search(const Player*);
	int search_rival(int);
	
	void go();
	
	void handle_fainted_pokemon(int, Pokemon*);
	bool is_defeated(int);
	bool is_end();

	void register_action(int, const Action&);
	
	void register_action_fast(int, const Action&);
	void register_action_charged(int, const Action&);
	void register_action_dodge(int, const Action&);
	void register_action_switch(int, const Action&);
	void register_action_wait(int, const Action&);
	
	inline StrategyInput generate_strat_input(int);
	
	void next(const TimelineEvent&);
	
	void handle_event_free(const TimelineEvent&);
	void handle_event_announce(const TimelineEvent&);
	void handle_event_fast(const TimelineEvent&);
	void handle_event_charged(const TimelineEvent&);
	void handle_event_dodge(const TimelineEvent&);
	void handle_event_enter(const TimelineEvent&);
	
	
	inline void append_log(const TimelineEvent&);
	void erase_log();


	
	Timeline m_timeline;
	
	int m_players_count;
	int m_players_count_max;
	PlayerState* m_player_states;
	
	int m_pokemon_count;
	Pokemon** m_pokemon;

	int m_has_log;
	int m_time_limit;
	int m_time;
	int m_weather;
	int m_defeated_team;
	
	TimelineEventNode* m_tenode_first;
	TimelineEventNode* m_tenode_last;
	
	
private:
	int* search_int_member(const char*);
	
	
};






#endif