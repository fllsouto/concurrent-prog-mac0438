#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#define _GNU_SOURCE_


int main(int argc, char* argv[]) {

	int fd[2];
	static char msg[BUFSIZ];
	if (argc != 2) {
		fprintf(stderr, "Usage: %s message\n", argv[0]);
		exit(1);
	}
	if (pipe(fd) == -1) {
		fprintf(stderr, "Cannot create pipe!\n");
		exit(1);
	}
	switch (fork()) {
		case -1:
			fprintf(stderr, "Cannot fork!\n");
			exit(1);

		case 0:
			close(fd[1]);
			if (read(fd[0], msg, BUFSIZ) != -1) {
				printf("Message received by child: %s\n", msg);
				fflush(stdout);
			}
			break;
		default:
			close(fd[0]);
			if (write(fd[1], argv[1], (unsigned)strlen(argv[1])) != -1) {
				printf("Message sent by parent: %s\n", argv[1]);
				fflush(stdout);
			}
	}

	return 0;
}