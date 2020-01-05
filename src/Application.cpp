
#include "Application.h"

#include <stdexcept>
#include <stdio.h>

namespace GoBattleSim
{

GoBattleSimApp GoBattleSimApp::instance;

GoBattleSimApp &GoBattleSimApp::get()
{
    return instance;
}

void GoBattleSimApp::prepare(const PvESimInput &input)
{
    battle_mode = BattleMode::PvE;
    aggregation_mode = input.aggregation;
    m_num_sims = input.num_sims;

    if (input.time_limit <= 0)
    {
        sprintf(err_msg, "timelimit must be positive (got %d)", input.time_limit);
        throw std::runtime_error(err_msg);
    }
    m_pve_battle.erase_players();
    for (const auto &player : input.players)
    {
        m_pve_battle.add_player(&player);
    }
    m_pve_battle.set_time_limit(input.time_limit);
    m_pve_battle.set_weather(input.weather);
    m_pve_battle.set_enable_log(input.enable_log);

    m_pve_output.clear();
}

void GoBattleSimApp::prepare(const PvPSimInput &input)
{
    battle_mode = BattleMode::PvP;
    aggregation_mode = input.aggregation;
    m_num_sims = input.num_sims;

    m_pvp_battle.set_pokemon(input.pokemon[0], input.pokemon[1]);
    m_pvp_battle.set_strategy(input.strateies[0], input.strateies[1]);
    m_pvp_battle.set_num_shields_max(input.num_shields[0], input.num_shields[1]);
    if (aggregation_mode == AggregationMode::Branching)
    {
        m_pvp_battle.set_enable_branching(true);
    }
    m_pvp_battle.set_enable_log(input.enable_log);

    m_pvp_output.clear();
}

void GoBattleSimApp::prepare(const BattleMatrixSimInput &input)
{
    battle_mode = BattleMode::BattleMatrix;
    m_battle_matrix.set(input.row_pokemon, input.col_pokemon, input.averge_by_shield);
}

void GoBattleSimApp::run()
{
    if (battle_mode == BattleMode::PvE)
    {
        for (int i = 0; i < m_num_sims; ++i)
        {
            m_pve_battle.init();
            m_pve_battle.start();
            auto output = m_pve_battle.get_outcome(1);
            m_pve_output.push_back(output);
        }
    }
    else if (battle_mode == BattleMode::PvP)
    {
        for (int i = 0; i < m_num_sims; ++i)
        {
            m_pvp_battle.init();
            m_pvp_battle.start();
            auto output = m_pvp_battle.get_outcome();
            m_pvp_output.push_back(output);
        }
    }
    else if (battle_mode == BattleMode::BattleMatrix)
    {
        m_battle_matrix.run();
    }
    else
    {
        sprintf(err_msg, "unknown battle mode (%d)", (int)battle_mode);
        throw std::runtime_error(err_msg);
    }
}

void GoBattleSimApp::collect(std::vector<PvEBattleOutcome> &outputs)
{
    outputs.insert(outputs.end(), m_pve_output.begin(), m_pve_output.end());
}

void GoBattleSimApp::collect(PvEAverageBattleOutcome &output)
{
    output.duration = 0;
    output.win = 0;
    output.tdo = 0;
    output.tdo_percent = 0;
    output.num_deaths = 0;
    for (const auto &sim_output : m_pve_output)
    {
        output.duration += sim_output.duration;
        output.win += sim_output.win ? 1 : 0;
        output.tdo += sim_output.tdo;
        output.tdo_percent += sim_output.tdo_percent;
        output.num_deaths += sim_output.num_deaths;
    }
    output.num_sims = m_pve_output.size();
    output.duration /= output.num_sims;
    output.win /= output.num_sims;
    output.tdo /= output.num_sims;
    output.tdo_percent /= output.num_sims;
    output.num_deaths /= output.num_sims;
}

void GoBattleSimApp::collect(std::vector<SimplePvPBattleOutcome> &outputs)
{
    outputs.insert(outputs.end(), m_pvp_output.begin(), m_pvp_output.end());
}

void GoBattleSimApp::collect(SimplePvPBattleOutcome &output)
{
    if (aggregation_mode == AggregationMode::Branching)
    {
        output = m_pvp_output.back();
    }
    else
    {
        output.tdo_percent[0] = 0;
        output.tdo_percent[0] = 1;
        for (const auto &sim_output : m_pvp_output)
        {
            output.tdo_percent[0] += sim_output.tdo_percent[0];
            output.tdo_percent[1] += sim_output.tdo_percent[1];
        }
        auto count = m_pvp_output.size();
        output.tdo_percent[0] /= count;
        output.tdo_percent[1] /= count;
    }
}

void GoBattleSimApp::collect(Matrix_t &output)
{
    output = m_battle_matrix.get();
}

} // namespace GoBattleSim
