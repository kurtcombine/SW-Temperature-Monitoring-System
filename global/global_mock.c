#ifdef MOCKED_EMBEDDED
#include "global.h"
#include <unistd.h>
#include <signal.h>
#include <bits/types/sigset_t.h>
#include <bits/sigaction.h>

void global_setup() {}

void delay_us(int us) {
    sigset_t sigset;
    sigset_t oldset;
    sigfillset(&sigset);
    pthread_sigmask(SIG_BLOCK, &sigset, &oldset);
    usleep(us);
    pthread_sigmask(SIG_SETMASK, &oldset, NULL);
}

void ___LCD_render();

void delay_ms(int ms) {
    ___LCD_render();
    delay_us(ms * 1000);
}

void DEBUG(uint8_t *str) {
    printf("%s", str);
}

void DEBUG_addr(const uint8_t addr[8]) {
    if(addr == NULL) {
        printf("NULL");
        return;
    }
    printf("%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], addr[6], addr[7]);
}
void DEBUG_int(int x) {
    printf("%d", x);
}
void DEBUG_float(double x) {
    printf("%f", x);
}

void DEBUG_halt() {
    exit(1);
}
#endif
