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

		default:
			{
				// Se chegou aqui, a leitura deu certo e você pode fazer alguma
				// coisa com os dados.
				int i, j;
        bool comeco = false;

				for (i = 0; i < count; i++) {

				    if (buffer[i] == 255) {
              comeco = true;
              int k = 0;
            }

            if (comeco) {
              k++;
              char T[8];
              T[k] = buffer[i];
              if (k == 7) {
                if (T[1] + T[2] + T[3] + T[4] + T[5] + T[6] == T[7] ) {
                  for (j = 1; j < 7; j++) {
                    printf("A tensão %d é %c.\n", j, T[j]);
                  }
                } else {
                  printf("A transmissão falhou.\n");
                }
                comeco = false;
              }
            }
        }
			}
		}
	}
}
