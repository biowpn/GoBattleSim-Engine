
#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "GoBattleSim.h"

#include <vector>

namespace GoBattleSim
{

enum class BattleMode : char
{
    PvE = 'E',
    PvP = 'P',
};

enum class AggregationMode
{
    None,
    Average,
};

struct SimInput
{
    BattleMode mode;

    std::vector<Player> players;
    int weather;
    int time_limit;

    PvPPokemon pvp_pokemon[2];
    int pvp_num_shields[2];
    PvPStrategy pvp_strateies[2];
    
    int num_sims;
    AggregationMode aggreation;
    bool enable_log;
};

struct SimOutput
{
    BattleOutcome statistics;
    // TODO: by-player stats
    // TODO: battle log
};

struct AverageBattleOutcome
{
    double duration{0};
    double win{0};
    double tdo{0};
    double tdo_percent{0};
    double num_deaths{0};
};

struct AverageSimOutput
{
    AverageBattleOutcome statistics;
    // TODO: by-player stats
};

class GoBattleSimApp
{
public:
    static GoBattleSimApp &get();

    /**
     * initialize new simulation. This will clear all output.
     */
    void prepare(const SimInput &);

    /**
     * run the new simulation.
     */
    void run();

    /**
     * collect the latest output.
     */
    void collect(SimOutput &);

    /**
     * collect all output.
     */
    void collect(std::vector<SimOutput> &);

    /**
     * collect the average of all output.
     */
    void collect(AverageSimOutput &);

private:
    static GoBattleSimApp instance;

    SimInput m_sim_input;
    Battle m_pve_battle;
    SimplePvPBattle m_pvp_battle;
    std::vector<SimOutput> m_sim_output;
};
} // namespace GoBattleSim

#endif
