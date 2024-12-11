#include "PIN_LPC17xx.h"
#include "LPC17xx.h"
#include "GPIO_LPC17xx.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#define DS18B20_SCRATCHPAD_SIZE 9
#define DS18B20_ROM_CODE_SIZE 8
#define DS18B20_SKIP_ROM 0xCC
#define DS18B20_READ_ROM 0x33
#define DS18B20_SEARCH_ROM 0xF0
#define DS18B20_CONVERT_T 0x44
#define DS18B20_READ_SCRATCHPAD 0xBE

volatile uint32_t myTick;

void uart_send_string(const char *string)
{
    for (int i = 0; i < strlen(string); i++)
    {
        while (!(LPC_UART0->LSR & (1 << 5)))
            ;
        LPC_UART0->THR = string[i];
    }
}

void SysTick_Handler(void) {myTick++;}

void delay(uint32_t t) {myTick = 0;while (myTick < t);} // FIXME

// ------------------------------------------
// Funkcje niskopoziomowe dla magistrali 1-Wire
// ------------------------------------------

int wire_reset(void)
{
    // Przytrzymaj liniÄ™ na niskim poziomie przez 480 Âµs
    GPIO_PinWrite(0, 0, 0);
    delay(480);
    // PuL›Ä‡ liniÄ™ (ustaw na 1), poczekaj 70 Âµs
    GPIO_PinWrite(0, 0, 1);
    delay(70);
    // SprawdLs stan linii - jeL›li czujnik jest obecny, linia bÄ™dzie w stanie niskim
    int rc = GPIO_PinRead(0, 0);
    // Dodatkowe opAlLsnienie, aby zakoL„czyÄ‡ slot resetu
    delay(410);
    return (rc == 0) ? 1 : 0;
}

void write_bit(int value)
{
    if (value)
    {
        // WysL‚anie bitu '1'
        GPIO_PinWrite(0, 0, 0);
        delay(6);
        GPIO_PinWrite(0, 0, 1);
        delay(64);
    }
    else
    {
        // WysL‚anie bitu '0'
        GPIO_PinWrite(0, 0, 0);
        delay(60);
        GPIO_PinWrite(0, 0, 1);
        delay(10);
    }
}

int read_bit(void)
{
    int rc;
    GPIO_PinWrite(0, 0, 0);
    delay(6);
    GPIO_PinWrite(0, 0, 1);
    delay(9);
    rc = GPIO_PinRead(0, 0);
    delay(55);
    return rc;
}

void wire_write(uint8_t byte)
{
    for (int i = 0; i < 8; i++)
    {
        write_bit(byte & 0x01);
        byte >>= 1;
    }
}

uint8_t wire_read(void)
{
    uint8_t value = 0;
    for (int i = 0; i < 8; i++)
    {
        value >>= 1;
        if (read_bit())
            value |= 0x80;
    }
    return value;
}


/**/
// ------------------------------------------
// Funkcje do obliczania CRC danych z DS18B20
// ------------------------------------------
uint8_t byte_crc(uint8_t crc, uint8_t byte)
{
    for (int i = 0; i < 8; i++)
    {
        uint8_t b = crc ^ byte;
        crc >>= 1;
        if (b & 0x01)
            crc ^= 0x8c;
        byte >>= 1;
    }
    return crc;
}

uint8_t wire_crc(const uint8_t *data, int len)
{
    uint8_t crc = 0;
    for (int i = 0; i < len; i++)
        crc = byte_crc(crc, data[i]);
    return crc;
}
/***/
static int wire_send_cmd(const uint8_t *rom_code, uint8_t cmd)
{
    if (wire_reset() != 1)
        return 0;

    // JeL›li brak rom_code, to SKIP_ROM
    if (!rom_code)
    {
        wire_write(DS18B20_SKIP_ROM);
    }
    else
    {
        wire_write(0x55);
        for (int i = 0; i < DS18B20_ROM_CODE_SIZE; i++)
           wire_write(rom_code[i]);
    }
    wire_write(cmd);
    return 1;
}

/**
 * @brief wire_start_measure
 * Rozpoczyna pomiar temperatury w DS18B20, wysyL‚ajÄ…c komendÄ™ CONVERT_T.
 * JeL›li rom_code == NULL, komenda dotyczy jedynego czujnika na linii.
 */
int wire_start_measure(const uint8_t *rom_code)
{
    return wire_send_cmd(rom_code, DS18B20_CONVERT_T);
}

/**
 * @brief wire_read_scratchpad
 * Odczytuje 9 bajtAlw scratchpada z czujnika DS18B20 i sprawdza CRC.
 * Scratchpad zawiera zmierzonÄ… temperaturÄ™ w bajtach 0 i 1.
 */
static int wire_read_scratchpad(const uint8_t *rom_code, uint8_t *scratchpad)
{
    if (wire_send_cmd(rom_code, DS18B20_READ_SCRATCHPAD) != 1)
        return 0;

    for (int i = 0; i < DS18B20_SCRATCHPAD_SIZE; i++)
        scratchpad[i] = wire_read();

    uint8_t crc = wire_crc(scratchpad, DS18B20_SCRATCHPAD_SIZE - 1);
    return (scratchpad[DS18B20_SCRATCHPAD_SIZE - 1] == crc);
}

/**
 * @brief wire_get_temp
 * Odczytuje temperaturÄ™ z DS18B20.
 * ZakL‚ada, LLe wczeL›niej wysL‚aliL›my CONVERT_T i odczekaliL›my wymagany czas (750ms) na konwersjÄ™.
 * Zwraca temperaturÄ™ jako wartoL›Ä‡ caL‚kowitÄ… w Â°C (np. 25 dla 25.0Â°C).
 */
int wire_get_temp(const uint8_t *rom_code)
{
	
    uint8_t scratchpad[DS18B20_SCRATCHPAD_SIZE];
    int16_t temp;

    // Odczyt scratchpada z czujnika
    if (!wire_read_scratchpad(rom_code, scratchpad))
        return 85; // Zwracamy 85 jako bL‚Ä…d, jeL›li CRC nie pasuje

    // Pierwsze 2 bajty to temperatura w skali 1/16 Â°C
    memcpy(&temp, &scratchpad[0], sizeof(temp));

    // Przeliczamy na Â°C dzielÄ…c przez 16
    return temp / 16;
}


void wire_get_romcode(uint8_t *romcode) {
	// assert: tylko jedno urzadzenie na linii
	  if (!wire_reset()) {
			romcode[0] = 0xDE;
			romcode[1] = 0xAD;
			romcode[2] = 0xDE;
			romcode[3] = 0xAD;
			return;
		}
    wire_write(DS18B20_READ_ROM);
		for (int i = 0; i < DS18B20_ROM_CODE_SIZE; i++)
					romcode[i] = wire_read();
	
}

int wire_search_tree(uint8_t *romcodes, int maxn) {
		// returns number of found romcodes
		
	  if (!wire_reset()) return 0;
		wire_write(DS18B20_SEARCH_ROM);
		char current_state[DS18B20_ROM_CODE_SIZE*8];
		char branch_state[DS18B20_ROM_CODE_SIZE*8];
		memset(current_state, 0, DS18B20_ROM_CODE_SIZE*8);
		memset(branch_state, 0, DS18B20_ROM_CODE_SIZE*8);
		int i = 0;
	  int found_roms = 0;
		int branches_left = 0;
	  for(int i = 0; i <  DS18B20_ROM_CODE_SIZE*8; i++) {
			char bit = read_bit();
			char negated_bit = read_bit();
			if(bit == negated_bit) {
				// not branch
				current_state[i] = bit;
				branch_state[i] = 0;
			} else {
				if(bit != 0) {//jest zle - wyszlismy poza drzewo
					} else {
				// branch
				current_state[i] = 0;
				branch_state[i] = 1;
				++branches_left;
					}
			}
			write_bit(0);
		}
		/*
		
				if(i == DS18B20_ROM_CODE_SIZE*8 - 1) {
						if(branches_left == 0) { // ostatni bit, ostatni rom
							// TODO add:
							++found_roms;
							break;
					} else { // ostatni bit, trzeba wrócic
							// TODO add:
							++found_roms;
						for(; branch_state[i] != 1; --i);
						branch_state[i] = 0;
						--branches_left;
						current_state[i] = 1;
						continue;
					}
				}
			i++;*/
		return branches_left;
}

// UART0 9600baudrate
// piny 0.2 0.3
// 1wire data
// piny 0.0

int main()
{
    SysTick_Config(SystemCoreClock / 1000000); // systick co 1µs

    PIN_Configure(0, 2, 1, 2, 0); // Funkcja pinu jako TXD2
    PIN_Configure(0, 3, 1, 0, 0); // Funkcja pinu jako RXD2
    LPC_UART0->LCR = 3 | (1 << 7); // 8N1, DLAB = 1 LLeby ustawiÄ‡ baud
    LPC_UART0->DLL = 163;           // Ustawienia baud rate (np. 115200 baud, zaleLLne od czÄ™stotliwoL›ci zegara)
    LPC_UART0->DLM = 0;
    LPC_UART0->LCR = 3; // DLAB = 0 (koniec konfiguracji)
	

    // Konfiguracja pinu P0.0 jako linia 1-Wire (GPIO)
    // Linia ta musi mieÄ‡ rezystor podciÄ…gajÄ…cy (ok. 4.7 kÎ©) do VDD
    PIN_Configure(0, 0, 0, 0, 1);

    // Sprawdzenie obecnoL›ci czujnika - reset linii 1-Wire
    if (!wire_reset()) {
        uart_send_string("Brak czujnika DS18B20 na magistrali!\n\r");
    }
		uint8_t romcode[DS18B20_ROM_CODE_SIZE];
		

    // GL‚Alwna pÄ™tla programu
		int i = 0;
    while (1)
    {
        if (wire_start_measure(NULL))
        {
            // DS18B20 potrzebuje do 750 ms na dokonanie pomiaru przy maksymalnej rozdzielczoL›ci
            delay(750 * 1000);

            // Odczytaj temperaturÄ™
            int temp = wire_get_temp(NULL);
            char str[50];
            sprintf(str, "[%d] Temperatura: %d C\n\r", i++, temp);
            uart_send_string(str);
					
						wire_get_romcode(romcode);
						char buf[2+6];
						for(int i = 0; i < 8; i++) {
							sprintf(buf, "%02hhx", romcode[i]);
							uart_send_string(buf);
						}
						uart_send_string("\r\n");
        }
        else
        {
            uart_send_string("Brak czujnika DS18B20 na magistrali!\n\r");
        }
					char str[50];
				sprintf(str, "Liczba urzadzen: %d\n\r", wire_search_tree(NULL, 0));
					uart_send_string(str);
				

        // Poczekaj 1 sekundÄ™ (1s = 1000000 Âµs) przed kolejnym pomiarem
        delay(100);
    }
}
