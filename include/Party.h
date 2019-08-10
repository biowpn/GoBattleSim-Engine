
#ifndef _PARTY_H_
#define _PARTY_H_

#include <string.h>

#include "Pokemon.h"



class Party
{
public:
	Party();
	Party(const Party&);
	~Party();

	// Interface functions
	Pokemon* get_pokemon(int);
	void add(Pokemon*);
	void erase_pokemon();
	bool has_attr(const char*);
	int get_attr(const char*);
	void set_attr(const char*, int);
	// End of Interface functions	
	
	
	// Battle functions
	void init();
	void heal();
	
	int get_pokemon_count() const;
	void get_all_pokemon(Pokemon**);
	
	void update(Pokemon*);
	
	Pokemon* get_head();
	bool set_head(Pokemon*);
	bool set_head(int);
	bool set_head_to_next();
	bool revive();

	
	// Variables
	int m_revive_policy;
	int m_revive_policy_init;
	Pokemon** m_pokemon;
	int m_pokemon_count;
	int m_pokemon_count_max;
	int m_pokemon_head;
	
private:
	int* search_int_member(const char*);

};

#endif