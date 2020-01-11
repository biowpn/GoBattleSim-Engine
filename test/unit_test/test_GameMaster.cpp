
#include <iostream>
#include <assert.h>

#include "GameMaster.h"
#include "Pokemon.h"

using namespace GoBattleSim;

int main()
{
    constexpr unsigned num_types = 18;
    auto &gm = GameMaster::get();

    gm.num_types(num_types);

    std::cout << "testing effectiveneses initial values ... ";
    for (unsigned i = 0; i < num_types; ++i)
    {
        for (unsigned j = 0; j < num_types; ++j)
        {
            assert(gm.effectiveness(i, j) == 1.0);
        }
    }
    std::cout << "success" << std::endl;

    std::cout << "testing type boosted weather initial values ... ";
    for (unsigned i = 0; i < num_types; ++i)
    {
        assert(gm.boosted_weather(i) < 0);
    }
    std::cout << "success" << std::endl;

    std::cout << "testing calc_damage ... ";

    Pokemon attacker, defender;
    attacker.poketype1 = 0;
    attacker.poketype2 = -1;
    attacker.attack = 200;

    defender.poketype1 = 1;
    defender.poketype2 = 2;
    defender.defense = 100;

    attacker.fmove.poketype = 2;
    attacker.fmove.power = 24;

    int dmg1 = calc_damage(&attacker, &attacker.fmove, &defender, 0);

    // assert(dmg1 == 24); // not sure if it's (floor(x) + 1) or ceil(x). to be researched

    std::cout << "success" << std::endl;

    std::cout << "testing STAB ... ";
    attacker.fmove.poketype = attacker.poketype1;
    int dmg2 = calc_damage(&attacker, &attacker.fmove, &defender, 1.0);
    assert(dmg2 > dmg1);
    std::cout << "success" << std::endl;

    std::cout << "testing 1x super effectiveness ... ";
    gm.effectiveness(attacker.fmove.poketype, defender.poketype1, 1.6);
    int dmg3 = calc_damage(&attacker, &attacker.fmove, &defender, 1.0);
    assert(dmg3 > dmg2);
    std::cout << "success" << std::endl;

    std::cout << "testing 2x super effectiveness ... ";
    gm.effectiveness(attacker.fmove.poketype, defender.poketype2, 1.6);
    int dmg4 = calc_damage(&attacker, &attacker.fmove, &defender, 1.0);
    assert(dmg4 > dmg3);
    std::cout << "success" << std::endl;

    std::cout << "testing multiplier ... ";
    gm.boosted_weather(attacker.fmove.poketype, 7);
    int dmg5 = calc_damage(&attacker, &attacker.fmove, &defender, 1.3);
    assert(dmg5 > dmg4);
    std::cout << "success" << std::endl;

    return 0;
}