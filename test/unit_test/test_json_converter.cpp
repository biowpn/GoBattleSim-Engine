
#include "json_converter.hpp"

#include <assert.h>
#include <iostream>
#include <memory.h>

using namespace GoBattleSim;

template <class T>
void test_two_way_conversion(T &&obj1)
{
    T obj2(obj1);

    std::cout << "testing to_json" << std::endl;
    json j1 = obj1;

    std::cout << "testing from_json" << std::endl;
    obj2 = j1;

    json j2 = obj2;

    std::cout << "testing two-way equality" << std::endl;
    assert(j1.dump() == j2.dump());
}

int main()
{
    std::cout << "testing MoveEffect" << std::endl;
    test_two_way_conversion(MoveEffect());

    std::cout << "testing Move" << std::endl;
    test_two_way_conversion(Move());

    std::cout << "testing Pokemon" << std::endl;
    test_two_way_conversion(Pokemon());

    std::cout << "testing PvPPokemon" << std::endl;
    test_two_way_conversion(PvPPokemon());

    std::cout << "testing Party" << std::endl;
    test_two_way_conversion(Party());

    std::cout << "testing Player" << std::endl;
    test_two_way_conversion(Player());

    std::cout << "testing GameMaster" << std::endl;
    test_two_way_conversion(GameMaster::get());

    return 0;
}
