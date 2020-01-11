
cmake_minimum_required(VERSION 3.14)
project(GoBattleSim)

include(${PROJECT_SOURCE_DIR}/get_version.cmake)
message("gitversion = ${PROJECT_GIT_VERSION}")

configure_file(${PROJECT_SOURCE_DIR}/config/config.h.in ${PROJECT_SOURCE_DIR}/config/config.h)

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "-Wall")

include_directories(
    ${PROJECT_SOURCE_DIR}/config
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
    ${PROJECT_SOURCE_DIR}/src/Party.cpp
    ${PROJECT_SOURCE_DIR}/src/Player.cpp
    ${PROJECT_SOURCE_DIR}/src/Pokemon.cpp
    ${PROJECT_SOURCE_DIR}/src/PokemonState.cpp
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

file(GLOB test_srcs "${PROJECT_SOURCE_DIR}/test/unit_test/*.cpp")
foreach(src ${test_srcs})
    get_filename_component(test_name ${src} NAME_WE)
    add_executable(${test_name}
        ${src}
    )
    target_link_libraries(${test_name}
        GoBattleSim
    )
    add_test(${test_name} ${test_name})
endforeach()

file(GLOB example_inputs "${PROJECT_SOURCE_DIR}/examples/input*.json")
foreach(example_input ${example_inputs})
    get_filename_component(fname ${example_input} NAME_WE)
    add_test("app_${fname}" gbs ${example_input} ${PROJECT_SOURCE_DIR}/examples/GBS_GAME_MASTER.json)
endforeach()
