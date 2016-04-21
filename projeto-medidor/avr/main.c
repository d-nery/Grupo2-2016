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

#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#include "uart.h"

char convert(float Tensao) {	// Calcula a distância(em incrementos de 0.01) da tensão até o valor limite, e converte em char
	int TC=(Tensao-1.74)*100;
	return TC;
}

char sum(char T1, char T2, char T3, char T4, char T5, char T6) { // Faz a soma dos valores das tensões, e devolve uma char
	return (T1+T2+T3+T4+T5+T6);
}

int main() {
	float Tensao1, Tensao2, Tensao3, Tensao4, Tensao5, Tensao6;
	char T1, T2, T3, T4, T5, T6;

	uart_init();

	for (;;) {
		char start=255;
		T1=convert(Tensao1);//Converte os valores float recebidos em variáveis
		T2=convert(Tensao2);//do tipo char, que vão de 0 até 255
		T3=convert(Tensao3);//Nesse caso, o programa está dando que as variáveis
		T4=convert(Tensao4);//não foram inicializadas, porque não sei como vamos 
		T5=convert(Tensao5);//receber os dados
		T6=convert(Tensao6);//

		uart_putchar(start);
		uart_putchar(T1);
		uart_putchar(T2);
		uart_putchar(T3);
		uart_putchar(T4);
		uart_putchar(T5);
		uart_putchar(T6);
		uart_putchar(sum(T1, T2, T3, T4, T5, T6));

		_delay_ms(2000); // falta sincronizar
	}

	cli();
	sleep_cpu();
}
