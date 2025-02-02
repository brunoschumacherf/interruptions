#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pico/bootrom.h"
#include "interruptions.pio.h"

#define LED_RGB_GREEN_PIN 11
#define LED_RGB_BLUE_PIN 12
#define LED_RGB_RED_PIN 13
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6
#define NUM_LEDS 25
#define WS2812_PIN 7
#define DEBOUNCE_TIME_MS 400
#define LED_BRIGHTNESS 51

typedef struct {
    double green;
    double red;
    double blue;
} LedColor;

typedef LedColor LedMatrix[5][5];

uint32_t color_to_grb(double red, double green, double blue);
void update_led_matrix(LedMatrix config, PIO pio, uint sm);
void display_number(int number, PIO pio, uint sm);
void button_interrupt(uint gpio, uint32_t events);

volatile uint32_t last_press_a = 0;
volatile uint32_t last_press_b = 0;
volatile uint32_t now = 0;
volatile int current_number = 0;

PIO global_pio;
uint global_sm;


// Converte as intensidades RGB para o formato GRB exigido pelos LEDs WS2812g
uint32_t color_to_grb(double green, double red, double blue) {
    return ((uint8_t)(green * 255) << 16) |
           ((uint8_t)(red * 255) << 8) |
           ((uint8_t)(blue * 255));
}

// Atualiza a matriz de LEDs com base na configuração armazenada
void update_led_matrix(LedMatrix config, PIO pio, uint sm) {
    for (int row = 4; row >= 0; row--) {
        bool reverse = (row % 2 == 0); // Necessário para lidar com o layout serpentino da matriz de LEDs

        for (int col = (reverse ? 4 : 0);
             reverse ? (col >= 0) : (col < 5);
             reverse ? col-- : col++) {

            uint32_t color = color_to_grb(
                config[row][col].red,
                config[row][col].green,
                config[row][col].blue);
            pio_sm_put_blocking(pio, sm, color);
        }
    }
}

// Representações numéricas para exibição na matriz de LEDs
const uint8_t digit_patterns[10][5][5] = {
    [0] = {{0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 1, 0}},
    [1] = {{0, 0, 1, 0, 0}, {0, 1, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 1, 1, 1, 0}},
    [2] = {{0, 1, 1, 1, 0}, {0, 0, 0, 1, 0}, {0, 1, 1, 1, 0}, {0, 1, 0, 0, 0}, {0, 1, 1, 1, 0}},
    [3] = {{0, 1, 1, 1, 0}, {0, 0, 0, 1, 0}, {0, 0, 1, 1, 0}, {0, 0, 0, 1, 0}, {0, 1, 1, 1, 0}},
    [4] = {{0, 1, 0, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 1, 0}, {0, 0, 0, 1, 0}, {0, 0, 0, 1, 0}},
    [5] = {{0, 1, 1, 1, 0}, {0, 1, 0, 0, 0}, {0, 1, 1, 1, 0}, {0, 0, 0, 1, 0}, {0, 1, 1, 1, 0}},
    [6] = {{0, 1, 1, 1, 0}, {0, 1, 0, 0, 0}, {0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 1, 0}},
    [7] = {{0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 0, 0, 1, 0}, {0, 0, 0, 1, 0}, {0, 0, 0, 1, 0}},
    [8] = {{0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 1, 0}},
    [9] = {{0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 1, 0}, {0, 0, 0, 1, 0}, {0, 0, 0, 1, 0}}
};

// Atualiza a matriz para exibir um número de 0 a 9
void display_number(int number, PIO pio, uint sm) {
    LedMatrix matrix = {0};

    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 5; col++) {
            if (digit_patterns[number][row][col]) {
                matrix[row][col] = (LedColor){LED_BRIGHTNESS / 255.0, 0, 0};
            } else {
                matrix[row][col] = (LedColor){0, 0, 0};
            }
        }
    }
    update_led_matrix(matrix, pio, sm);
}

// Interrupção acionada pelo pressionamento dos botões
void button_interrupt(uint gpio, uint32_t events) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());

    if (gpio == BUTTON_A_PIN && (current_time - last_press_a) > DEBOUNCE_TIME_MS) {
        if (current_number < 9) {
            current_number++;
            printf("Incrementando: %d\n", current_number);
        }
        last_press_a = current_time;
    } 
    
    if (gpio == BUTTON_B_PIN && (current_time - last_press_b) > DEBOUNCE_TIME_MS) {
        if (current_number > 0) {
            current_number--;
            printf("Descrementando: %d\n", current_number);
        }
        last_press_b = current_time;
    }
}

// Inicializa o hardware do Raspberry Pi Pico e configura os periféricos
void initialize_hardware(PIO pio, uint *sm) {
    set_sys_clock_khz(128000, false); // Configura a frequência do clock do sistema
    stdio_init_all();

    uint offset = pio_add_program(pio, &ws2812_program);
    *sm = pio_claim_unused_sm(pio, true);
    ws2812_program_init(pio, *sm, offset, WS2812_PIN, 800000, false);

    global_pio = pio;
    global_sm = *sm;

    const uint rgb_pins[] = {LED_RGB_RED_PIN, LED_RGB_GREEN_PIN, LED_RGB_BLUE_PIN};
    for (int i = 0; i < 3; i++) {
        gpio_init(rgb_pins[i]);
        gpio_set_dir(rgb_pins[i], GPIO_OUT);
    }

    const uint button_pins[] = {BUTTON_A_PIN, BUTTON_B_PIN};
    for (int i = 0; i < 2; i++) {
        gpio_init(button_pins[i]);
        gpio_set_dir(button_pins[i], GPIO_IN);
        gpio_pull_up(button_pins[i]);
    }

    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, (gpio_irq_callback_t)button_interrupt);
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true, (gpio_irq_callback_t)button_interrupt);
}

int main() {
    stdio_init_all();
    printf("Iniciando....\n");
    
    PIO pio = pio0;
    uint sm = 0;
    initialize_hardware(pio, &sm);

    absolute_time_t last_blink = get_absolute_time();

    while (true) {
        now = to_ms_since_boot(get_absolute_time());

        if (absolute_time_diff_us(last_blink, get_absolute_time()) > 100000) {
            gpio_put(LED_RGB_RED_PIN, !gpio_get(LED_RGB_RED_PIN)); // Pisca o LED vermelho
            last_blink = get_absolute_time();
        }

        display_number(current_number, pio, sm); // Atualiza o número exibido

        sleep_ms(10);
    }
    
    return 0;
}
