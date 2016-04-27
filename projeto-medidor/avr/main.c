/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 ThundeRatz
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*!
 * @file	main.c
 * @brief	ThundeRatz's LipoVoltageChecker Project Firmware.
 *
 * @author	ThundeRatz Robotics Team - POLI-USP: http://thunderatz.org/ \n
 *			Support email: contato@thunderatz.org
 *
 * @date	02 January 2016
 */

#include <avr/io.h>
#include <util/delay.h>

#include "uart.h"

////////////////////// Macros e Constantes 7 segmentos, nao mexam //////////////////////

//!	@name 7 segments displays macros.
//@{
#define aOff()		PORTB |= (1<<PB1)
#define aOn()		PORTB &= ~(1<<PB1)
#define bOff()		PORTB |= (1<<PB2)
#define bOn()		PORTB &= ~(1<<PB2)
#define cOff()		PORTB |= (1<<PB7)
#define cOn()		PORTB &= ~(1<<PB7)
#define dOff()		PORTD |= (1<<PD6)
#define dOn()		PORTD &= ~(1<<PD6)
#define eOff()		PORTD |= (1<<PD5)
#define eOn()		PORTD &= ~(1<<PD5)
#define fOff()		PORTD |= (1<<PD7)
#define fOn()		PORTD &= ~(1<<PD7)
#define gOff()		PORTB |= (1<<PB0)
#define gOn()		PORTB &= ~(1<<PB0)
#define dpOff()		PORTB |= (1<<PB6)
#define dpOn()		PORTB &= ~(1<<PB6)
#define sel1Off()	PORTD |= (1<<PD4)
#define sel1On()	PORTD &= ~(1<<PD4)
#define sel2Off()	PORTD |= (1<<PD3)
#define sel2On()	PORTD &= ~(1<<PD3)
#define sel3Off()	PORTD |= (1<<PD2)
#define sel3On()	PORTD &= ~(1<<PD2)
//@}

//!	@name Status LED macros.
//@{
#define ledOff()	PORTB &= ~(1<<PB3)
#define ledOn()		PORTB |= (1<<PB3)
#define ledToggle()	PORTB ^= (1<<PB3)
//@}

//!	@name 16-bit Timer 1 macros.
//@{
#define timerCount		TCNT1
#define startTimer()	TCCR1B |= (1<<CS12)
#define pauseTimer()	TCCR1B = 0
#define resetTimer()	TCNT1 = 0
//@}

//!	@name Constants.
//@{
#define LOW_VOLTAGE		102
#define ONES_COUNT		31250
//@}

////////////////////// @TODO Macros e Constantes Bluetooth //////////////////////


////////////////////// Prototipos 7 segmentos, nao mexam //////////////////////

//!	@name Functions prototypes.
//@{
void uCInit(void);
void adcInit(void);
unsigned int adcRead(unsigned char);
double getVoltage(unsigned char);
void sevenSegWrite(unsigned char, unsigned char, unsigned char);
void displayFor1S(unsigned char, unsigned char, unsigned char, unsigned char);
void getDigits(double, unsigned char*, unsigned char*, unsigned char*, unsigned char*);
//@}

////////////////////// @TODO Prototipos Bluetooth //////////////////////

char convert(float);
char sum(char, char, char, char, char, char);

////////////////////// Main //////////////////////

/*! \brief Main function.
 *
 *	The main function of the firmware initializes the microcontroller,
 *	as well as the peripherals connected to it; reads values of the ADC
 *	pins externally connected to Lipo battery cells through voltage dividers,
 *	and displays these values on 7 segments common anode displays.
 *
 *	\return Never return.
 *
 */

int main(void) {
	unsigned char i, cellCount, ten, unit, tenth, hundredth;
	unsigned int adcValue = LOW_VOLTAGE;
	double cells[7] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

	// Microcontroller initialization.
	uCInit();

	// Checks the maximum number of cells connected (2 to 6).
	for (i = cellCount = 0; i < 6 && adcValue >= LOW_VOLTAGE; i++) {
		adcValue = adcRead(i);
		if (adcValue > LOW_VOLTAGE)
			cellCount++;
	}

	// Displays the cell count.
	displayFor1S(cellCount, 'c', 'E', 0);

	i = 0;

	// Status LED on.
	ledOn();

	// Infinite loop.
	while (1) {
		////////////////////// @TODO Incorporar codigo Bluetooth nesse loop //////////////////////
		i++;

		// Display the cell number to be displayed.
		displayFor1S('N', 'o', i, 2);

		// cells[i] = voltage from cell "i" referenced to GND.
		// cells[0] = voltage accross cell "i".
		cells[i] = getVoltage(i);
		cells[0] = cells[i] - cells[i - 1];

		// Displays the voltage accross cell "i".
		getDigits(cells[0], &ten, &unit, &tenth, &hundredth);
		displayFor1S(unit, tenth, hundredth, 1);

		// Displays the battery voltage and resets the display count.
		if (i >= cellCount) {
			displayFor1S('A', 'L', 'L', 0);

			getDigits(cells[cellCount], &ten, &unit, &tenth, &hundredth);

			if (ten)
				displayFor1S(ten, unit, tenth, 2);
			else
				displayFor1S(unit, tenth, hundredth, 1);

			i = 0;
			cells[0] = 0.0;
		}
	}

	// Although never reached, returning "0" keeps the compiler from complaining.
	return 0;
}

////////////////////// @TODO Funcoes Bluetooth //////////////////////

char convert(float Tensao) {	// Calcula a distância(em incrementos de 0.01) da tensão até o valor limite, e converte em char
	int TC=(Tensao-1.74)*100;
	return TC;
}

char sum(char T1, char T2, char T3, char T4, char T5, char T6) { // Faz a soma dos valores das tensões, e devolve uma char
	return (T1+T2+T3+T4+T5+T6);
}

////////////////////// Funcoes 7 segmentos, nao mexam //////////////////////

/*! \brief Microcontroller initialization.
 *
 *	Initializes PORTB and PORTD as output pins, except for pins: PB4, PB5, PD0
 *	(RXD) and PD1 (TXD), and initializes the ADC.
 *
 *	\return void
 *
 */

void uCInit() {
	DDRD |= (1<<PD7) | (1<<PD6) | (1<<PD5) | (1<<PD4) | (1<<PD3) | (1<<PD2);
	DDRB |= (1<<PB7) | (1<<PB6) | (1<<PB3) | (1<<PB2) | (1<<PB1) | (1<<PB0);

	adcInit();
	uart_init();
}

/*! \brief ADC initialization.
 *
 *	Enables ADC with highest division factor (128), using AVCC as voltage reference.
 *
 *	\return void
 *
 */

void adcInit() {
	ADCSRA |= (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);

	ADMUX |= (1<<REFS0);
}

/*! \brief Reads analog value from specified channel.
 *
 *	Returns the analog reading from ADC channels 0 to 5 (specified
 *	by channel).
 *
 *	\param channel ADC channel to be read.
 *
 *	\return Analog reading from specified channel.
 *
 */

unsigned int adcRead(unsigned char channel) {
	channel &= 0x07;
	ADMUX = (ADMUX & 0xF8) | channel;
	_delay_ms(5);

	ADCSRA |= (1<<ADSC);
	while (ADCSRA & (1<<ADSC));

	return ADC;
}

/*! \brief Reads voltage from specified battery cell.
 *
 *	Returns the voltage (referenced to GND) from battery cell (specified).
 *
 *	\param cell Cell voltage to be read.
 *
 *	\return Voltage from specified cell referenced to GND.
 *
 */

double getVoltage(unsigned char cell) {
	unsigned int adcValue;
	double cellVoltage, multiplier;

	adcValue = adcRead(cell - 1);

	switch (cell) {
		case 1:
			multiplier = (0.0 + 2.2) / 2.2;
			break;
		case 2:
			multiplier = (2.2 + 2.2) / 2.2;
			break;
		case 3:
			multiplier = (3.9 + 2.2) / 2.2;
			break;
		case 4:
			multiplier = (5.6 + 2.2) / 2.2;
			break;
		case 5:
			multiplier = (8.2 + 2.2) / 2.2;
			break;
		case 6:
			multiplier = (10. + 2.2) / 2.2;
			break;
		default:
			multiplier = 0.0;
	}

	cellVoltage = multiplier * adcValue * 5 / 1024;

	return cellVoltage;
}

/*! \brief Writes specified character to one display.
 *
 *	Selects one of the displays (1, 2 or 3) and writes a character
 *	(0 - 9, 'A', 'c', 'E', 'L', 'n' or 'o'), with (1) or without (0)
 *	decimal point.
 *
 *	\param display Selected display.
 *	\param digit Character to be written.
 *	\param dp Presence of decimal point.
 *
 *	\return void
 *
 */

void sevenSegWrite(unsigned char display, unsigned char digit, unsigned char dp) {
	switch (display) {
		case 1:
			sel1On(); sel2Off(); sel3Off();
			break;
		case 2:
			sel1Off(); sel2On(); sel3Off();
			break;
		case 3:
			sel1Off(); sel2Off(); sel3On();
			break;
		default:
			sel1Off(); sel2Off(); sel3Off();
	}

	switch (digit) {
		case 0:
			aOn(); bOn(); cOn(); dOn(); eOn(); fOn(); gOff();
			break;
		case 1:
			aOff(); bOn(); cOn(); dOff(); eOff(); fOff(); gOff();
			break;
		case 2:
			aOn(); bOn(); cOff(); dOn(); eOn(); fOff(); gOn();
			break;
		case 3:
			aOn(); bOn(); cOn(); dOn(); eOff(); fOff(); gOn();
			break;
		case 4:
			aOff(); bOn(); cOn(); dOff(); eOff(); fOn(); gOn();
			break;
		case 5:
			aOn(); bOff(); cOn(); dOn(); eOff(); fOn(); gOn();
			break;
		case 6:
			aOff(); bOff(); cOn(); dOn(); eOn(); fOn(); gOn();
			break;
		case 7:
			aOn(); bOn(); cOn(); dOff(); eOff(); fOff(); gOff();
			break;
		case 8:
			aOn(); bOn(); cOn(); dOn(); eOn(); fOn(); gOn();
			break;
		case 9:
			aOn(); bOn(); cOn(); dOff(); eOff(); fOn(); gOn();
			break;
		case 'A':
			aOn(); bOn(); cOn(); dOff(); eOn(); fOn(); gOn();
			break;
		case 'c':
			aOff(); bOff(); cOff(); dOn(); eOn(); fOff(); gOn();
			break;
		case 'E':
			aOn(); bOff(); cOff(); dOn(); eOn(); fOn(); gOn();
			break;
		case 'L':
			aOff(); bOff(); cOff(); dOn(); eOn(); fOn(); gOff();
			break;
		case 'N':
			aOn(); bOn(); cOn(); dOff(); eOn(); fOn(); gOff();
			break;
		case 'o':
			aOff(); bOff(); cOn(); dOn(); eOn(); fOff(); gOn();
			break;
		default:
			aOff(); bOff(); cOff(); dOff(); eOff(); fOff(); gOff();
	}

	if (dp)
		dpOn();
	else
		dpOff();
}

/*! \brief Displays message on all 3 displays for 1s.
 *
 *	Writes 3 valid characters and 1 decimal point specified by position
 *	(0 = no decimal point; 1, 2, 3 = decimal point on display 1, 2 or 3)
 *	for 1 second.
 *
 *	\param digit1 Character on display 1.
  * 	\param digit2 Character on display 2.
 *	\param digit3 Character on display 3.
 *	\param dpPosition Position of decimal point.
 *
 *	\return void
 *
 */

void displayFor1S(unsigned char digit1, unsigned char digit2, unsigned char digit3,
				  unsigned char dpPosition) {
	resetTimer();
	startTimer();
	while (timerCount < ONES_COUNT) {
		sevenSegWrite(1, digit1, (unsigned char) (dpPosition == 1));
		_delay_ms(3);

		sevenSegWrite(2, digit2, (unsigned char) (dpPosition == 2));
		_delay_ms(3);

		sevenSegWrite(3, digit3, (unsigned char) (dpPosition == 3));
		_delay_ms(3);
	}
	pauseTimer();
}

/*! \brief Get 0 - 9 digits from double value.
 *
 *	Gets individual digits (ten, unit, tenth and hundredth) from specified
 *	floating point value. Automatically rounds up the value, if necessary.
 *
 *	\param value Floating point number.
 *	\param ten Ten digit from specified value.
 *	\param unit Unit digit from specified value.
 *	\param tenth Tenth digit from specified value.
 *	\param hundredth Hundredth digit from specified value.
 *
 *	\return void
 *
 */

void getDigits(double value, unsigned char* ten, unsigned char* unit,
			   unsigned char* tenth, unsigned char* hundredth) {
	unsigned char thousandth;
	unsigned int intValue;

	intValue = (unsigned int) (value * 1000);

	thousandth = intValue % 10;
	intValue /= 10;
	if (thousandth > 5)
		intValue++;

	*hundredth = intValue % 10;
	intValue /= 10;

	*tenth = intValue % 10;
	intValue /= 10;

	*unit = intValue % 10;
	intValue /= 10;

	*ten = intValue % 10;
	intValue /= 10;
}
