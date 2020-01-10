
# GoBattleSim-Engine

Yet another Pokemon Go battle Simulator. 

*For now, GoBattleSim Engine is independent from [Web GoBattleSim](https://github.com/biowpn/GoBattleSim), but it is planned to replace the latter's simulation engine part. Engine GBS is expected to be much more high-performancing than Web GBS, especially in generating battle matrix. Most of the Web GBS's features are implemented.*

## Build & Testing

System requirements: 

- a compiler than supports C++ 11. I have tested it on GCC 8.1.0 (mingw).

GoBattleSim Engine uses [CMake](https://cmake.org/) as build/testing system. The primary targets are:

- GoBattleSim (SHARED_LIBRARY)
- gbs (EXECUTABLE)

Other targets are for testing.

## Usage

For the executable:

```
gbs {path/to/input.json} [, path/to/game_master.json]
```

Refer to [examples](./examples/) for example simulation input.

To generate `GBS_GAME_MASTER.json` from official game master (such as [this one](https://github.com/pokemongo-dev-contrib/pokemongo-game-master)), refer to [GoBattleSim-Python](https://github.com/biowpn/GoBattleSim-Python):

```
python -m gobattlesim.GameMaster path/to/GAME_MASTER.json -o GBS_GAME_MASTER.json
```

Other options are also available. Run `gbs --help` to see the list of them.

## C API

Refer to [GoBattleSim_extern.h](./include/GoBattleSim_extern.h). The three core functions are

```
// pass JSON input to simulator
void GBS_prepare(const char *input_j);

// run the simulation
void GBS_run();

// get the simulation output in JSON
const char * GBS_collect();
```

## License

GNU GENERAL PUBLIC LICENSE Version 3. For more details refer to [LICENSE](./LICENSE).
