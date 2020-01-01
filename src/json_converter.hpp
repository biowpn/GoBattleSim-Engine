
/**
 * Convert GoBattleSim structs to/from json. 
 */

#include "GoBattleSim.h"
#include "Application.h"

#include "json.hpp"

#include <string>
#include <vector>

namespace GoBattleSim
{
using nlohmann::json;

template <class T>
T try_get(const json &j, const std::string &key, const T &t_default)
{
    if (j.find(key) != j.end())
    {
        return j.at(key).get<T>();
    }
    else
    {
        return t_default;
    }
}

template <class T>
bool try_get_to(const json &j, const std::string &key, const T &t_default, T &dst)
{
    if (j.find(key) != j.end())
    {
        j.at(key).get_to(dst);
        return true;
    }
    else
    {
        dst = t_default;
        return false;
    }
}

void to_json(json &j, const MoveEffect &effect)
{
    j["activation_chance"] = effect.activation_chance;
    j["self_attack_stage_delta"] = effect.self_attack_stage_delta;
    j["self_defense_stage_delta"] = effect.self_defense_stage_delta;
    j["target_attack_stage_delta"] = effect.target_attack_stage_delta;
    j["target_defense_stage_delta"] = effect.target_defense_stage_delta;
}

void from_json(const json &j, MoveEffect &effect)
{
    effect.activation_chance = j["activation_chance"];
    effect.self_attack_stage_delta = j["self_attack_stage_delta"];
    effect.self_defense_stage_delta = j["self_defense_stage_delta"];
    effect.target_attack_stage_delta = j["target_attack_stage_delta"];
    effect.target_defense_stage_delta = j["target_defense_stage_delta"];
}

void to_json(json &j, const Move &move)
{
    j["poketype"] = move.poketype;
    j["power"] = move.power;
    j["energy"] = move.energy;
    j["duration"] = move.duration;
    j["dws"] = move.dws;
    j["effect"] = move.effect;
}

void from_json(const json &j, Move &move)
{
    j.at("poketype").get_to(move.poketype);
    j.at("power").get_to(move.power);
    j.at("energy").get_to(move.energy);
    try_get_to(j, "duration", move.duration, move.duration);
    try_get_to(j, "dws", move.dws, move.dws);
    try_get_to(j, "effect", move.effect, move.effect);
}

void to_json(json &j, const Pokemon &pkm)
{
    j["id"] = pkm.id;
    j["poketype1"] = pkm.poketype1;
    j["poketype2"] = pkm.poketype2;
    j["attack"] = pkm.attack;
    j["defense"] = pkm.defense;
    j["max_hp"] = pkm.max_hp;
    j["immortal"] = pkm.immortal;
    j["fmove"] = pkm.fmove;
    j["cmoves"] = std::vector<Move>(pkm.cmoves, pkm.cmoves + pkm.cmoves_count);
    j["attack_multiplier"] = pkm.attack_multiplier;
    j["clone_multiplier"] = pkm.clone_multiplier;
}

void from_json(const json &j, Pokemon &pkm)
{
    j.at("poketype1").get_to(pkm.poketype1);
    j.at("poketype2").get_to(pkm.poketype2);
    j.at("attack").get_to(pkm.attack);
    j.at("defense").get_to(pkm.defense);
    j.at("max_hp").get_to(pkm.max_hp);

    j.at("fmove").get_to(pkm.fmove);
    for (const auto &cmove_j : j.at("cmoves"))
    {
        auto cmove = cmove_j.get<Move>();
        pkm.add_cmove(&cmove);
    }

    try_get_to(j, "id", pkm.id, pkm.id);
    try_get_to(j, "immortal", pkm.immortal, pkm.immortal);
    try_get_to(j, "attack_multiplier", pkm.attack_multiplier, pkm.attack_multiplier);
    try_get_to(j, "clone_multiplier", pkm.clone_multiplier, pkm.clone_multiplier);
}

void to_json(json &j, const PvPPokemon &pkm)
{
    to_json(j, static_cast<const Pokemon &>(pkm));
}

void from_json(const json &j, PvPPokemon &pkm)
{
    from_json(j, static_cast<Pokemon &>(pkm));
}

void to_json(json &j, const Party &party)
{
    std::vector<Pokemon> pkm;
    for (int i = 0; i < party.get_pokemon_count(); ++i)
    {
        pkm.push_back(*party.get_pokemon(i));
    }
    j["pokemon"] = pkm;
    j["revive_policy"] = party.get_revive_policy();
}

void from_json(const json &j, Party &party)
{
    party.erase_pokemon();
    for (const auto &pkm_j : j.at("pokemon"))
    {
        auto pkm = pkm_j.get<Pokemon>();
        party.add(&pkm);
    }
    party.set_revive_policy(try_get(j, "revive_policy", 0));
}

void to_json(json &j, const Player &player)
{
    std::vector<Party> parties;
    for (int i = 0; i < player.get_parties_count(); ++i)
    {
        parties.push_back(*player.get_party(i));
    }
    j["parties"] = parties;
    j["team"] = player.team;
    j["id"] = player.id;
    j["strategy"] = player.strategy.name;
    j["attack_multiplier"] = player.get_attack_multiplier();
    j["clone_multiplier"] = player.get_clone_multiplier();
}

void from_json(const json &j, Player &player)
{
    player.erase_parties();
    for (const auto &party_j : j["parties"])
    {
        auto party = party_j.get<Party>();
        player.add(&party);
    }
    player.team = j.at("team");
    try_get_to(j, "id", player.id, player.id);
    auto strategy_name = j["strategy"].get<std::string>();
    for (int i = 0; i < NUM_STRATEGIES; ++i)
    {
        if (strcmp(BUILT_IN_STRATEGIES[i].name, strategy_name.c_str()) == 0)
        {
            player.strategy = BUILT_IN_STRATEGIES[i];
        }
    }

    if (j.find("attack_multiplier") != j.end())
    {
        player.set_attack_multiplier(j.at("attack_multiplier"));
    }
    if (j.find("clone_multiplier") != j.end())
    {
        player.set_clone_multiplier(j.at("clone_multiplier"));
    }
}

void to_json(json &j, const GameMaster &gm)
{
    j["max_energy"] = gm.max_energy;
    j["min_stage"] = gm.min_stage;
    j["max_stage"] = gm.max_stage;
    j["dodge_duration"] = gm.dodge_duration;
    j["dodge_window"] = gm.dodge_window;
    j["swap_duration"] = gm.swap_duration;
    j["switching_cooldown"] = gm.switching_cooldown;
    j["rejoin_duration"] = gm.rejoin_duration;
    j["item_menu_animation_time"] = gm.item_menu_animation_time;
    j["max_revive_time_per_pokemon"] = gm.max_revive_time_per_pokemon;
    j["same_type_attack_bonus_multiplier"] = gm.same_type_attack_bonus_multiplier;
    j["weather_attack_bonus_multiplier"] = gm.weather_attack_bonus_multiplier;
    j["pvp_fast_attack_bonus_multiplier"] = gm.pvp_fast_attack_bonus_multiplier;
    j["pvp_charged_attack_bonus_multiplier"] = gm.pvp_charged_attack_bonus_multiplier;
    j["dodge_damage_reduction_percent"] = gm.dodge_damage_reduction_percent;
    j["energy_delta_per_health_lost"] = gm.energy_delta_per_health_lost;

    std::vector<std::vector<double>> matrix;
    auto num_types = gm.num_types();
    for (int i = 0; i < num_types; ++i)
    {
        std::vector<double> row;
        for (int j = 0; j < num_types; ++j)
        {
            row.push_back(gm.effectiveness(i, j));
        }
        matrix.push_back(row);
    }
    j["type_effectiveness"] = matrix;

    std::vector<double> stage_multipliers;
    for (auto s = gm.min_stage; s <= gm.max_stage; ++s)
    {
        stage_multipliers.push_back(gm.stage_multiplier(s));
    }
    j["stage_multipliers"] = stage_multipliers;

    std::vector<int> type_boosted_weather;
    for (int i = 0; i < num_types; ++i)
    {
        type_boosted_weather.push_back(gm.boosted_weather(i));
    }
    j["type_boosted_weather"] = type_boosted_weather;
}

void from_json(const json &j, GameMaster &gm)
{
    gm.max_energy = j["max_energy"];
    gm.min_stage = j["min_stage"];
    gm.max_stage = j["max_stage"];
    gm.dodge_duration = j["dodge_duration"];
    gm.dodge_window = j["dodge_window"];
    gm.swap_duration = j["swap_duration"];
    gm.switching_cooldown = j["switching_cooldown"];
    gm.rejoin_duration = j["rejoin_duration"];
    gm.item_menu_animation_time = j["item_menu_animation_time"];
    gm.max_revive_time_per_pokemon = j["max_revive_time_per_pokemon"];
    gm.same_type_attack_bonus_multiplier = j["same_type_attack_bonus_multiplier"];
    gm.weather_attack_bonus_multiplier = j["weather_attack_bonus_multiplier"];
    gm.pvp_fast_attack_bonus_multiplier = j["pvp_fast_attack_bonus_multiplier"];
    gm.pvp_charged_attack_bonus_multiplier = j["pvp_charged_attack_bonus_multiplier"];
    gm.dodge_damage_reduction_percent = j["dodge_damage_reduction_percent"];
    gm.energy_delta_per_health_lost = j["energy_delta_per_health_lost"];

    auto matrix = j["type_effectiveness"].get<std::vector<std::vector<double>>>();
    auto num_types = matrix.size();
    gm.num_types(num_types);
    for (int i = 0; i < num_types; ++i)
    {
        for (int j = 0; j < num_types; ++j)
        {
            gm.effectiveness(i, j, matrix.at(i).at(j));
        }
    }

    auto stage_multipliers = j["stage_multipliers"].get<std::vector<double>>();
    gm.set_stage_bounds(gm.min_stage, gm.max_stage);
    for (auto s = gm.min_stage; s <= gm.max_stage; ++s)
    {
        gm.stage_multiplier(s, stage_multipliers.at(s - gm.min_stage));
    }

    auto type_boosted_weather = j["type_boosted_weather"].get<std::vector<int>>();
    for (int i = 0; i < num_types; ++i)
    {
        gm.boosted_weather(i, type_boosted_weather.at(i));
    }
}

void to_json(json &j, const ActionType &action_type)
{
    switch (action_type)
    {
    case ActionType::None:
        j = "None";
        break;
    case ActionType::Wait:
        j = "Wait";
        break;
    case ActionType::Fast:
        j = "Fast";
        break;
    case ActionType::Charged:
        j = "Charged";
        break;
    case ActionType::Dodge:
        j = "Dodge";
        break;
    case ActionType::Switch:
        j = "Switch";
        break;
    default:
        j = "Unknown";
        break;
    }
}

void to_json(json &j, const Action &action)
{
    j["time"] = action.time;
    j["type"] = action.type;
    j["delay"] = action.delay;
    j["value"] = action.value;
}

void to_json(json &j, const EventType &event_type)
{
    switch (event_type)
    {
    case EventType::None:
        j = "None";
        break;
    case EventType::Announce:
        j = "Announce";
        break;
    case EventType::Free:
        j = "Free";
        break;
    case EventType::Fast:
        j = "Fast";
        break;
    case EventType::Charged:
        j = "Charged";
        break;
    case EventType::Dodge:
        j = "Dodge";
        break;
    case EventType::Enter:
        j = "Enter";
        break;
    default:
        j = "Unknown";
        break;
    }
}

void to_json(json &j, const TimelineEvent &event)
{
    j["time"] = event.time;
    j["type"] = event.type;
    j["player"] = event.player;
    j["value"] = event.value;
}

void to_json(json &j, const PokemonState &pkm_st)
{
    j["active"] = pkm_st.active;
    j["immortal"] = pkm_st.immortal;
    j["max_hp"] = pkm_st.max_hp;
    j["hp"] = pkm_st.hp;
    j["energy"] = pkm_st.energy;
    j["damage_reduction_expiry"] = pkm_st.damage_reduction_expiry;
    j["tdo"] = pkm_st.tdo;
    j["tdo_fast"] = pkm_st.tdo_fast;
    j["duration"] = pkm_st.duration;
    j["num_deaths"] = pkm_st.num_deaths;
    j["num_fmoves_used"] = pkm_st.num_fmoves_used;
    j["num_cmoves_used"] = pkm_st.num_cmoves_used;
}

void to_json(json &j, const BattleOutcome &outcome)
{
    j["duration"] = outcome.duration;
    j["win"] = outcome.win;
    j["tdo"] = outcome.tdo;
    j["tdo_percent"] = outcome.tdo_percent;
    j["num_deaths"] = outcome.num_deaths;
}

void from_json(const json &j, BattleMode &mode)
{
    auto mode_str = j.get<std::string>();
    if (mode_str == "raid" || mode_str == "gym")
    {
        mode = BattleMode::PvE;
    }
    else if (mode_str == "pvp")
    {
        mode = BattleMode::PvP;
    }
    else
    {
        sprintf(err_msg, "unknown battle mode '%s'", mode_str.c_str());
        throw std::runtime_error(err_msg);
    }
}

void from_json(const json &j, SimInput &input)
{
    j.at("mode").get_to(input.mode);
    j.at("time_limit").get_to(input.time_limit);
    j.at("players").get_to(input.players);

    try_get_to(j, "pokemon", input.pvp_pokemon, input.pvp_pokemon);
    try_get_to(j, "num_shields", input.pvp_num_shields, input.pvp_num_shields);
    try_get_to(j, "weather", input.weather, input.weather);
    try_get_to(j, "num_sims", 1, input.num_sims);
    try_get_to(j, "enable_log", false, input.enable_log);

    input.aggreation = AggregationMode::None;
    if (j.find("aggreation") != j.end())
    {
        auto agg_str = j.at("aggreation").get<std::string>();
        if (agg_str == "avrg")
        {
            input.aggreation = AggregationMode::Average;
        }
        else if (agg_str == "none")
        {
            input.aggreation = AggregationMode::None;
        }
        else
        {
            sprintf(err_msg, "unknown aggreation ('%s')", agg_str.c_str());
        }
    }
}

void to_json(json &j, const SimOutput &output)
{
    j["statistics"] = output.statistics;
    j["battle_log"] = output.battle_log;
}

void to_json(json &j, const AverageBattleOutcome &outcome)
{
    j["duration"] = outcome.duration;
    j["win"] = outcome.win;
    j["tdo"] = outcome.tdo;
    j["tdo_percent"] = outcome.tdo_percent;
    j["num_deaths"] = outcome.num_deaths;
}

void to_json(json &j, const AverageSimOutput &output)
{
    j["statistics"] = output.statistics;
}

}; // namespace GoBattleSim
