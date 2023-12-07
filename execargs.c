/*Programa execargs: el siguiente programa ejecuta los comandos que se le pasen como argumento esperando entre cada 
ejecución el número de segundos especificado en el primer argumento. En caso de no haber al menos dos argumentos
el programa finalizará con error. El primer argumento debe ser un número entero. El programa finalizará con error en
en caso de que alguno de los comandos no sea correcto*/

//gcc -Wall -Wshadow -Wvla -g -c execargs.c
//gcc -g -o execargs execargs.o
//Ejemplo: ./execargs 2 '/bin/echo hola' '/bin/ls -l' '/bin/echo adios'

//------------------------------------------------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

enum {
	ARGMAX = 20
};

long
checktime(char str[])
{
	long time;
	char *endptr;

	time = strtol(str, &endptr, 10);
	if (*endptr != '\0')
		errx(EXIT_FAILURE, "%s is not a valid number", str);
	if (time < 0)
		errx(EXIT_FAILURE, "Can't sleep a negative number of seconds");
	return time;
}

void
tokenargs(char *args[], char *str)
{
	int i = 0;
	char delim[] = " ";
	char *saveptr;
	char *token;

	token = strtok_r(str, delim, &saveptr);
	while (token != NULL) {
		args[i] = strdup(token);
		token = strtok_r(NULL, delim, &saveptr);
		i++;
	}

}

int
main(int argc, char *argv[])
{
	pid_t pid;
	long time;
	int i, j;
	int sts;
	char *args[ARGMAX];

	argc--;
	argv++;
	if (argc < 2)
		errx(EXIT_FAILURE, "Not enough arguments\n");
	time = checktime(argv[0]);
	argc--;
	argv++;
	for (i = 0; i < argc; i++) {
		if ((pid = fork()) == -1)
			err(EXIT_FAILURE, "Can't create a son");
		switch (pid) {
		case 0:
			for (j = 0; j < ARGMAX; j++) {
				args[j] = NULL;
			}

			tokenargs(args, argv[i]);
			if (execv(args[0], args) == -1)
				err(EXIT_FAILURE, "Can't exec");
			break;

		default:
			if (wait(&sts) < 0)
				errx(EXIT_FAILURE, "Wait failure");
			if (WIFEXITED(sts) && (WEXITSTATUS(sts) == 0))
				if (i == (argc - 1))
					continue;
				else
					sleep(time);

			else
				errx(EXIT_FAILURE, "Wait failure");
			fprintf(stderr, "I am the father\n");
			break;
		}
	}

	exit(EXIT_SUCCESS);
}
