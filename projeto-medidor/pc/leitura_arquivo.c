#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <asm/termios.h>

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("Uso: %s arquivo\n", argv[0]);
		return -1;
	}

	int fd;
	// Tenta abrir em apenas leitura.
	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		// Se abertura falhou, open retorna -1.
		// A função perror mostra na tela a razão da falho.
		perror("open");
		return -1;
	}

	// mudar o BAUD rate
	struct termios2 tio;
	ioctl(fd, TCGETS2, &tio);
	tio.c_cflag &= ~CBAUD;
	tio.c_cflag |= BOTHER;
	tio.c_ispeed = 9600;
	tio.c_ospeed = 9600;
	ioctl(fd, TCSETS2, &tio);

	for (;;) {
		char buffer[40];
		ssize_t count;

		// read retorna o número de bytes lidos, que é salvo na variável count.
		// Se for -1, indica que houve erro. Se for 0, indica que o arquivo acabou.
		count = read(fd, buffer, sizeof(buffer));

		switch (count) {
			case -1:
				perror("read");
				return -1;

			case 0:
				// Fim do arquivo, fecha e termina o programa.
				close(fd);
				return 0;

			default: {
				// Se chegou aqui, a leitura deu certo e você pode fazer alguma
				// coisa com os dados.
				int i, j;
				int k = 0;
		        bool comeco = 0;

				for (i = 0; i < count; i++) {

					if (buffer[i] == 255) {
						// O byte de começo de mensagem foi encontrado
						comeco = 1;
				    }

		            if (comeco) {
		            	k++;
		            	unsigned char T[8];
		            	T[k] = buffer[i];

		            	if (k == 7) {
		                	if (sum(T[1], T[2], T[3], T[4], T[5], T[6]) == T[7] ) {
			                	for (j = 1; j < 7; j++) {
			                    	printf("A tensão %d é %c.\n", j, convertBack(T[j]));
			                	}
		                	} else {
		                  		printf("A transmissão falhou.\n");
		                	}
		                	comeco = false;
		              	}
		            }
		        } // fim do for que avalia os dados recebidos
			} // fim do caso padrão
		} // fim do switch
	} // fim do loop principal
} // fim da função main

double convertBack(unsigned char T) {
	double Tensao = (double) (T)/100 + 1.74;
	return Tensao;
}

unsigned char sum(unsigned char T1, unsigned char T2, unsigned char T3, unsigned char T4, unsigned char T5, unsigned char T6) { // Faz a soma dos valores das tensões, e devolve uma char
	return (unsigned char) (T1+T2+T3+T4+T5+T6);
}
