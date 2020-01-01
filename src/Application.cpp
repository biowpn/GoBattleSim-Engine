
#include "Application.h"

#include <stdexcept>

namespace GoBattleSim
{

GoBattleSimApp GoBattleSimApp::instance;

GoBattleSimApp &GoBattleSimApp::get()
{
    return instance;
}

void GoBattleSimApp::prepare(const SimInput &input)
{
    m_sim_output.clear();
    m_sim_input = input;

    if (m_sim_input.time_limit <= 0)
    {
        sprintf(err_msg, "timelimit must be positive (got %d)", m_sim_input.time_limit);
        throw std::runtime_error(err_msg);
    }
    m_pve_battle.set_time_limit(m_sim_input.time_limit);

    if (m_sim_input.mode == BattleMode::PvE)
    {
        m_pve_battle.set_weather(m_sim_input.weather);
        m_pve_battle.set_enable_log(m_sim_input.enable_log);
        m_pve_battle.erase_players();
        for (const auto &player : m_sim_input.players)
        {
            m_pve_battle.add(&player);
        }
    }
    else if (m_sim_input.mode == BattleMode::PvP)
    {
        throw std::runtime_error("PvP not supported yet");
    }
    else
    {
        sprintf(err_msg, "unknown battle mode (%d)", m_sim_input.mode);
        throw std::runtime_error(err_msg);
    }
}

void GoBattleSimApp::run()
{
    if (m_sim_input.mode == BattleMode::PvE)
    {
        for (int i = 0; i < m_sim_input.num_sims; ++i)
        {
            m_pve_battle.init();
            m_pve_battle.start();
            SimOutput output;
            output.statistics = m_pve_battle.get_outcome(1);
            // TODO: get log
            m_sim_output.push_back(output);
        }
    }
}

void GoBattleSimApp::collect(SimOutput &output)
{
    output = m_sim_output.back();
}

void GoBattleSimApp::collect(std::vector<SimOutput> &outputs)
{
    outputs.insert(outputs.end(), m_sim_output.begin(), m_sim_output.end());
}

void GoBattleSimApp::collect(AverageSimOutput &output)
{
    auto count = m_sim_output.size();
    output.statistics.duration = 0;
    output.statistics.win = 0;
    output.statistics.tdo = 0;
    output.statistics.tdo_percent = 0;
    output.statistics.num_deaths = 0;
    for (const auto &sim_output : m_sim_output)
    {
        output.statistics.duration += sim_output.statistics.duration;
        output.statistics.win += sim_output.statistics.win ? 1 : 0;
        output.statistics.tdo += sim_output.statistics.tdo;
        output.statistics.tdo_percent += sim_output.statistics.tdo_percent;
        output.statistics.num_deaths += sim_output.statistics.num_deaths;
    }
    output.statistics.duration /= count;
    output.statistics.win /= count;
    output.statistics.tdo /= count;
    output.statistics.tdo_percent /= count;
    output.statistics.num_deaths /= count;
}

} // namespace GoBattleSim
