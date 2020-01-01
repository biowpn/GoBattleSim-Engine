
#include "GoBattleSim_extern.h"

#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: input [game_master]" << std::endl;
        return -1;
    }

    std::ifstream ifs(argv[1]);
    if (!ifs.good())
    {
        std::cerr << "bad file: " << argv[1] << std::endl;
        return -1;
    }

    std::string str((std::istreambuf_iterator<char>(ifs)),
                    std::istreambuf_iterator<char>());

    GBS_prepare(str.c_str());

    GBS_run();

    int output_len;
    GBS_collect(nullptr, &output_len);
    auto buf = new char[output_len];
    GBS_collect(buf, &output_len);

    std::cout << buf << std::endl;

    delete buf;

    return 0;
}
