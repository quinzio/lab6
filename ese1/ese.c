#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/wait.h>
#include <ctype.h>
#define N 9
#define R 0
#define W 1

int main(int argc, char*argv[])
{
	pid_t pidc1, pidc2;
	int status;
	int i;
	int piped[2];

	pipe(piped);
	pidc1 = fork();
	if (pidc1) {
		//parent
		pidc2 = fork();
		if (pidc2) {
			//parent
			close(piped[R]);
			close(piped[W]);
			waitpid(pidc1, &status, 0); 
			waitpid(pidc2, &status, 0); 
		}
		else {
			//child 2
			int n;
			char str1[1024];
			char str2[1024];
			close(piped[W]);
			for(;;) {
				read(piped[R], &n, sizeof(int));
				read(piped[R], str1, n);
				str1[strlen(str1)+1] = 0;	
				i=0;
				do {
					str2[i] = toupper(str1[i]);
					i++;
				}
				while (str2[i-1] != 0);
				fprintf(stdout, "Received %s", str2);
				if(strncmp(str1, "end\n", 4) ==0) break;
			}
			close(piped[R]);
		}
	}
	else {
		//child 1
		char str1[1024];
		int n;
		close(piped[R]);
		for(;;) {
			fgets(str1, 1024, stdin); // \n at the end
			n = strlen(str1);
			write(piped[W], &n, sizeof(int));
			write(piped[W], str1, n);
			if (strncmp(str1, "end\n", 4) == 0) {
				break;
			}
		}
		close(piped[W]);
	} 
	return 0;
}
