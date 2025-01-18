#ifdef MOCKED_EMBEDDED
#include <math.h>
#include <time.h>
#include "onewire_DS18B20.h"

#define SEED 0
#define DEVS 16
#define ADDR_ERROR 0.5
#define CONV_ERROR 0.2
#define WIRE_ERROR 0.2
#define DATA_ERROR 0.2
#define TEMP_STDEV 0.5
#define CHANGE_RESOLUTION_ERROR 0.2

typedef struct {
    uint8_t *devs;
    bool *connected;
    double *temperature;
    size_t n;
    int i;
    int k;
} OneWireSimulator;

OneWireSimulator ow_sim;

double ___random() {
    return (double)rand() / RAND_MAX;
}

void ___random_addr(uint8_t *addr) {
    for(int i = 0; i < 7; i++) { addr[i] = rand(); }
    addr[7] = onewire_crc8(addr, 7);
}
int ___compare_addrs(const void *a, const void *b) {
    return memcmp((const uint8_t *)a, (const uint8_t *)b, 8);
}

void ___create_simulator(size_t n) {
    ow_sim.n = n;
    ow_sim.devs = malloc(n * 8 * sizeof(uint8_t *));
    ow_sim.connected = malloc(n * sizeof(bool));
    ow_sim.temperature = malloc(n * sizeof(double));
    ow_sim.i = 0;
    ow_sim.k = 12;

    for(size_t i = 0; i < n; i++) {
        ___random_addr(ow_sim.devs + i * 8);
        ow_sim.connected[i] = true;
        ow_sim.temperature[i] = (double)(rand() % (DS18B20_TEMP_MAX - DS18B20_TEMP_MIN) + DS18B20_TEMP_MIN);
    }

    // sort addresses
    qsort(ow_sim.devs, n, 8 * sizeof(uint8_t), ___compare_addrs);
}

// --- Connection Check ---
bool ___is_connected(size_t i) {
    ow_sim.connected[i] = ___random() > WIRE_ERROR;
    return ow_sim.connected[i];
}

// --- Gaussian Random Number Generator ---
double ___gaussian_random() {
    double sum = 0.0;
    for(int i = 0; i < 12; i++) { sum += ___random(); }
    return sum - 6.0;
}

// --- Get Temperature ---
double ___get_temperature(uint8_t *addr) {
    for(size_t i = 0; i < ow_sim.n; i++) {
        if(memcmp(addr, ow_sim.devs + i * 8, 8) == 0) {
            if(!ow_sim.connected[i]) { return 999.0; }
            double temp = ow_sim.temperature[i] + TEMP_STDEV * ___gaussian_random();
            double factor = pow(2, (8 - ow_sim.k));
            return round(temp / factor) * factor;
        }
    }
    return 999.0;
}

void onewire_DS18B20_cleanup() {
    free(ow_sim.devs);
    free(ow_sim.connected);
    free(ow_sim.temperature);
}

void onewire_reset_search() {
    ow_sim.i = 0;
}
bool onewire_search(uint8_t *new_addr, bool search_mode) {
    for(size_t i = ow_sim.i; i < ow_sim.n; i++) {
        if(___is_connected(i)) {
            memcpy(new_addr, ow_sim.devs + i * 8, 8);
            if(___random() < ADDR_ERROR) { new_addr[rand() % 8] ^= 1 << (rand() % 8); }
            ow_sim.i = i + 1;
            return true;
        }
    }
    onewire_reset_search();
    return false;
}

void onewire_setup() {}

void onewire_DS18B20_setup() {
    ___create_simulator(DEVS);
    srand(SEED);
    // also update onewire_mock_test.c
}

bool onewire_DS18B20_read_scratchpad(uint8_t addr[8]) {
    float temp = ___get_temperature(addr);
    DS18B20__presence = temp != 999.0 && ___random() > WIRE_ERROR;
    if(!DS18B20__presence) return false;

    int16_t DS18B20__raw = temp / 0.0625;
    DS18B20__data[0] = DS18B20__raw & 0xFF;
    DS18B20__data[1] = DS18B20__raw >> 8;

    for(int i = 2; i < 8; i++) { DS18B20__data[i] = rand(); }

    switch(ow_sim.k) {
        case 9: DS18B20__data[4] = DS18B20__data[4] & ~0x60 | 0x00; break;
        case 10: DS18B20__data[4] = DS18B20__data[4] & ~0x60 | 0x20; break;
        case 11: DS18B20__data[4] = DS18B20__data[4] & ~0x60 | 0x40; break;
        default: DS18B20__data[4] = DS18B20__data[4] & ~0x60 | 0x60; break;
    }

    DS18B20__data[8] = onewire_crc8(DS18B20__data, 8);

    if(___random() < DATA_ERROR) DS18B20__data[rand() % 9] ^= 1 << (rand() % 8);

    if(onewire_crc8(DS18B20__data, 8) != DS18B20__data[8]) return false;
    return true;
}

bool onewire_DS18B20_convert(uint8_t addr[8]) {
    DS18B20__presence = ___get_temperature(addr) != 999.0 && ___random() > WIRE_ERROR;
    if(!DS18B20__presence) return false;
    delay_ms(DS18B20__conversionTime);
    if(___random() < CHANGE_RESOLUTION_ERROR) ow_sim.k = rand() % 4 + 9;
    return true;
}
#endif
