# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)
project(interruptions C CXX ASM)

pico_sdk_init()

add_executable(interruptions main.c )

# Criar diretório para arquivos gerados
file(MAKE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/generated)

# Definir caminho do arquivo PIO
set(PIO_FILE ${CMAKE_CURRENT_LIST_DIR}/interruptions.pio)

# Se o arquivo PIO não existir, cria um automaticamente
if (NOT EXISTS ${PIO_FILE})
    file(WRITE ${PIO_FILE} 
        ".pio_version 0 // only requires PIO version 0\n\n"
        ".program ws2812\n"
        ".side_set 1\n\n"
        ".define public T1 3\n"
        ".define public T2 3\n"
        ".define public T3 4\n\n"
        ".wrap_target\n"
        "bitloop:\n"
        "    out x, 1       side 0 [T3 - 1] \n"
        "    jmp !x do_zero side 1 [T1 - 1] \n"
        "do_one:\n"
        "    jmp  bitloop   side 1 [T2 - 1] \n"
        "do_zero:\n"
        "    nop            side 0 [T2 - 1] \n"
        ".wrap\n\n"
        "% c-sdk {\n"
        "#include \"hardware/clocks.h\"\n\n"
        "static inline void ws2812_program_init(PIO pio, uint sm, uint offset, uint pin, float freq, bool rgbw) {\n\n"
        "    pio_gpio_init(pio, pin);\n"
        "    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);\n\n"
        "    pio_sm_config c = ws2812_program_get_default_config(offset);\n"
        "    sm_config_set_sideset_pins(&c, pin);\n"
        "    sm_config_set_out_shift(&c, false, true, rgbw ? 32 : 24);\n"
        "    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);\n\n"
        "    int cycles_per_bit = ws2812_T1 + ws2812_T2 + ws2812_T3;\n"
        "    float div = clock_get_hz(clk_sys) / (freq * cycles_per_bit);\n"
        "    sm_config_set_clkdiv(&c, div);\n\n"
        "    pio_sm_init(pio, sm, offset, &c);\n"
        "    pio_sm_set_enabled(pio, sm, true);\n"
        "}\n"
        "%}\n"
    )
endif()

# Gerar cabeçalho PIO
pico_generate_pio_header(interruptions ${PIO_FILE} OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)

target_sources(interruptions PRIVATE main.c)
target_link_libraries(interruptions PRIVATE pico_stdlib hardware_pio)

pico_add_extra_outputs(interruptions)
