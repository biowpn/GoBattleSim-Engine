
#include "Move.h"


Move::Move(int t_poketype, int t_power, int t_energy, int t_duration, int t_dws, MoveEffect t_effect)
: poketype(t_poketype), power(t_power), energy(t_energy), duration(t_duration), dws(t_dws), effect(t_effect)
{

}


bool Move::has_attr(const char* t_name)
{
	return search_int_member(t_name);
}


int Move::get_attr(const char* t_name)
{
	int* int_member_ptr = search_int_member(t_name);
	if (int_member_ptr)
	{
		return *int_member_ptr;
	}
	return 0;
}


void Move::set_attr(const char* t_name, int t_value)
{
	int* int_member_ptr = search_int_member(t_name);
	if (int_member_ptr)
	{
		*int_member_ptr = t_value;
	}
}


int* Move::search_int_member(const char* t_name)
{
	if (strcmp(t_name, "poketype") == 0)
		return &poketype;
	else if (strcmp(t_name, "power") == 0)
		return &power;
	else if (strcmp(t_name, "energy") == 0)
		return &energy;
	else if (strcmp(t_name, "duration") == 0)
		return &duration;
	else if (strcmp(t_name, "dws") == 0)
		return &dws;
	else
		return nullptr;
}









