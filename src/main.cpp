
#include "GoBattleSim_extern.h"

#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>

#include "argparse.h"

std::string get_file_contents(std::ifstream &ifs)
{
    return std::string((std::istreambuf_iterator<char>(ifs)),
                       std::istreambuf_iterator<char>());
}

void print_usage()
{
    std::cout << "Usage: gbs {path/to/input.json} [, path/to/game_master.json]" << std::endl;
    std::cout << "Usage: gbs --config" << std::endl;
    std::cout << "Usage: gbs --version" << std::endl;
}

void print_version()
{
    std::cout << "GoBattleSim-Engine " << GBS_version() << std::endl;
}

int main(int argc, const char **argv)
{
    ArgumentParser parser("GoBattleSim");
    parser.add_argument("c", "config", "print game master and exit", false);
    parser.add_argument("v", "version", "print version info and exit", false);
    parser.add_argument("o", "out", "save simulation output to file", false);
    parser.parse(argc, argv);

    if (parser.exists("version") || parser.exists("v"))
    {
        print_version();
        return 0;
    }

    if (parser.exists("config") || parser.exists("c"))
    {
        auto cfg_fpath = parser.get<std::string>("config");
        if (cfg_fpath.size() > 0)
        {
            std::ifstream ifs(cfg_fpath);
            if (!ifs.good())
            {
                std::cerr << "bad file: " << cfg_fpath << std::endl;
                return -5;
            }
            auto content = get_file_contents(ifs);
            GBS_config(content.c_str());
        }
        std::cout << GBS_config(NULL) << std::endl;
        return 0;
    }

    auto positional_args = parser.getv<std::string>("");

    if (positional_args.size() == 0 || positional_args.size() >= 3)
    {
        if (positional_args.size() == 0)
        {
            print_version();
        }
        print_usage();
        return -1;
    }

    std::ifstream ifs(positional_args[0]);
    if (!ifs.good())
    {
        std::cerr << "bad file: " << positional_args[0] << std::endl;
        return -2;
    }

    if (positional_args.size() == 2)
    {
        std::ifstream gm_ifs(positional_args[1]);
        if (!gm_ifs.good())
        {
            std::cerr << "bad file: " << positional_args[1] << std::endl;
            return -3;
        }
        GBS_config(get_file_contents(gm_ifs).c_str());
    }

    GBS_prepare(get_file_contents(ifs).c_str());

    GBS_run();

    auto output = GBS_collect();

    if (parser.exists("out") || parser.exists("o"))
    {
        auto out_fpath = parser.get<std::string>("out");
        std::ofstream ofs(out_fpath);
        if (!ofs.good())
        {
            std::cerr << "bad file: " << out_fpath << std::endl;
            return -4;
        }
        ofs << output << std::endl;
    }
    else
    {
        std::cout << output << std::endl;
    }

    return 0;
}
