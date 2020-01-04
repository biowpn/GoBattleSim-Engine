
#pragma once

#include <string>
#include <unordered_map>

namespace GoBattleSim
{

/**
 * base class for helper classes to convert name from/to index
 */
class NameMapping
{
public:
    /**
     * reset all mapping
     */
    void reset()
    {
        m_idx_to_name.clear();
        m_name_to_idx.clear();
    }

    /**
     * map type index @param idx to type name @param name
     */
    void map(int idx, const std::string &name)
    {
        m_idx_to_name[idx] = name;
        m_name_to_idx[name] = idx;
    }

    /**
     * get type name from index @param idx
     */
    std::string to_name(int idx) const
    {
        auto it = m_idx_to_name.find(idx);
        if (it != m_idx_to_name.end())
        {
            return it->second;
        }
        else
        {
            return "none";
        }
    }

    /**
     * get type index from name @param name
     */
    int to_idx(const std::string &name) const
    {
        auto it = m_name_to_idx.find(name);
        if (it != m_name_to_idx.end())
        {
            return it->second;
        }
        else
        {
            return -1;
        }
    }

private:
    std::unordered_map<int, std::string> m_idx_to_name;
    std::unordered_map<std::string, int> m_name_to_idx;
};

class PokeTypeMapping : public NameMapping
{
public:
    static PokeTypeMapping &get()
    {
        return instance;
    }

private:
    PokeTypeMapping() = default;
    static PokeTypeMapping instance;
};

PokeTypeMapping PokeTypeMapping::instance;

class WeatherMapping : public NameMapping
{
public:
    static WeatherMapping &get()
    {
        return instance;
    }

private:
    WeatherMapping() = default;
    static WeatherMapping instance;
};

WeatherMapping WeatherMapping::instance;

} // namespace GoBattleSim
