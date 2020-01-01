
cmake_minimum_required (VERSION 3.1)
project (GoBattleSim)

SET(CMAKE_CXX_STANDARD 11)
SET(COMPILE_FLAGS "-Wall")

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/thirdparty
)

add_library(GoBattleSim SHARED
    ${PROJECT_SOURCE_DIR}/src/Application.cpp
    ${PROJECT_SOURCE_DIR}/src/Battle.cpp
    ${PROJECT_SOURCE_DIR}/src/BattleMatrix.cpp
    ${PROJECT_SOURCE_DIR}/src/GameMaster.cpp
    ${PROJECT_SOURCE_DIR}/src/GoBattleSim_extern.cpp
    ${PROJECT_SOURCE_DIR}/src/Move.cpp
    ${PROJECT_SOURCE_DIR}/src/Party.cpp
    ${PROJECT_SOURCE_DIR}/src/Player.cpp
    ${PROJECT_SOURCE_DIR}/src/Pokemon.cpp
    ${PROJECT_SOURCE_DIR}/src/PvPPokemon.cpp
    ${PROJECT_SOURCE_DIR}/src/PvPStrategy.cpp
    ${PROJECT_SOURCE_DIR}/src/SimplePvPBattle.cpp
    ${PROJECT_SOURCE_DIR}/src/Strategy.cpp
)

add_executable(gbs
    ${PROJECT_SOURCE_DIR}/src/main.cpp
)

target_link_libraries(gbs 
    GoBattleSim
)


enable_testing()

add_executable(test_Raid_Battle
    ${PROJECT_SOURCE_DIR}/test/unit_test/test_Raid_Battle.cpp
)
target_link_libraries(test_Raid_Battle
    GoBattleSim
)
add_test(Raid_Battle test_Raid_Battle)

add_executable(test_PvP_Battle
    ${PROJECT_SOURCE_DIR}/test/unit_test/test_PvP_Battle.cpp
)
target_link_libraries(test_PvP_Battle
    GoBattleSim
)
add_test(PvP_Battle test_PvP_Battle)

add_executable(test_json_converter
    ${PROJECT_SOURCE_DIR}/test/unit_test/test_json_converter.cpp
)
target_link_libraries(test_json_converter
    GoBattleSim
)
add_test(json_converter test_json_converter)
