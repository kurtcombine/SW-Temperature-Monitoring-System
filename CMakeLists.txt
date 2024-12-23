cmake_minimum_required(VERSION 3.10)
project(LcdSimulator C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)


find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(main global/global_mock.c ds18b20/onewire_DS18B20.c lcd/asciiLib.c lcd/lcd.c mem/mem.c main.c)
target_link_libraries(main m ${SDL2_LIBRARIES})
target_compile_definitions(main PRIVATE MOCKED_EMBEDDED)

add_executable(onewire_test global/global_mock.c ds18b20/onewire_DS18B20.c tests/onewire_test.c)
target_include_directories(onewire_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(onewire_test m)
target_compile_definitions(onewire_test PRIVATE MOCKED_EMBEDDED)

add_executable(onewire_mock_test global/global_mock.c tests/onewire_mock_test.c)
target_include_directories(onewire_mock_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(onewire_mock_test m)
target_compile_definitions(onewire_mock_test PRIVATE MOCKED_EMBEDDED)

add_executable(lcd_test global/global_mock.c lcd/asciiLib.c lcd/lcd.c tests/lcd_test.c)
target_include_directories(lcd_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(lcd_test ${SDL2_LIBRARIES})
target_compile_definitions(lcd_test PRIVATE MOCKED_EMBEDDED)


add_executable(mem_test global/global_mock.c mem/mem.c tests/mem_test.c)
target_include_directories(mem_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(mem_test PRIVATE MOCKED_EMBEDDED)

add_executable(onewire_lcd_test global/global_mock.c ds18b20/onewire_DS18B20.c lcd/asciiLib.c lcd/lcd.c tests/onewire_lcd_test.c)
target_include_directories(onewire_lcd_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(onewire_lcd_test m ${SDL2_LIBRARIES})
target_compile_definitions(onewire_lcd_test PRIVATE MOCKED_EMBEDDED)

add_executable(main_compile_test mock_compile_test/allmocked.c global/global_embed.c ds18b20/onewire_DS18B20.c lcd/asciiLib.c lcd/lcd.c mem/mem.c main.c)
target_include_directories(main_compile_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/mock_compile_test)
target_link_libraries(main_compile_test m)
target_compile_definitions(main_compile_test PRIVATE GCC_COMPILE_TEST)
