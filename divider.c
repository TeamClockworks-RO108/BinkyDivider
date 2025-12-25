#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/irq.h"
#include "quadrature.pio.h"   // PIO program (below)

// ---------------- CONFIG ----------------
#define ENC_A_PIN 2
#define ENC_B_PIN 3

#define OUT_A_PIN 6
#define OUT_B_PIN 7

#define DIVIDER 4   // Divide encoder by this amount
// ----------------------------------------

volatile int32_t encoder_count = 0;
volatile int32_t last_output_step = 0;
volatile uint8_t out_state = 0;

// Quadrature output sequence
const uint8_t quad_table[4][2] = {
    {0, 0},
    {1, 0},
    {1, 1},
    {0, 1}
};

// PIO interrupt handler
void pio_irq_handler() {
    while (!pio_sm_is_rx_fifo_empty(pio0, 0)) {
        int32_t delta = (int32_t)pio_sm_get(pio0, 0);
        encoder_count += delta;
    }
}

int main() {
    stdio_init_all();

    // Output pins
    gpio_init(OUT_A_PIN);
    gpio_init(OUT_B_PIN);
    gpio_set_dir(OUT_A_PIN, GPIO_OUT);
    gpio_set_dir(OUT_B_PIN, GPIO_OUT);

    // Load PIO program
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &quadrature_program);
    quadrature_program_init(pio, 0, offset, ENC_A_PIN);

    // Enable IRQ
    irq_set_exclusive_handler(PIO0_IRQ_0, pio_irq_handler);
    irq_set_enabled(PIO0_IRQ_0, true);
    pio_set_irq0_source_enabled(pio, PIO_INTR_SM0_RXNEMPTY, true);

    while (true) {
        int32_t step = encoder_count / DIVIDER;

        if (step != last_output_step) {
            if (step > last_output_step)
                out_state = (out_state + 1) & 3;
            else
                out_state = (out_state - 1) & 3;

            gpio_put(OUT_A_PIN, quad_table[out_state][0]);
            gpio_put(OUT_B_PIN, quad_table[out_state][1]);

            last_output_step = step;
        }

        tight_loop_contents();
    }
}
