
#include "GoBattleSim_extern.h"

#include <fstream>
#include <iostream>

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

    ifs.seekg(0, std::ios::end);
    auto length = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    auto buf = new char[length];
    ifs.read(buf, length);

    GBS_prepare(buf);
    
    GBS_run();

    int output_len;
    GBS_collect(nullptr, &output_len);
    if (output_len > length)
    {
        delete buf;
        buf = new char[output_len];
    }
    GBS_collect(buf, &output_len);

    std::cout << buf << std::endl;

    delete buf;

    return 0;
}
