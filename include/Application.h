
#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "GoBattleSim.h"

#include <vector>

namespace GoBattleSim
{

enum class BattleMode
{
    PvE,
    PvP
};

struct SimInput
{
    BattleMode mode;
    int time_limit;
    int weather;
    int num_sims;
    bool enable_log;
    std::vector<Player> players;
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
    SimInput m_sim_input;
    Battle m_pve_battle;
    SimplePvPBattle m_pvp_battle;
    std::vector<SimOutput> m_sim_output;
};
} // namespace GoBattleSim

#endif
