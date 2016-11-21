#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>

void main(){
/*- GLOBAL VARIALBES -------------------------------------------*/
char input[BUFSIZ];
char cwd[1024];
char cmd1[1024];
char cmd2[BUFSIZ];
char cmd3[BUFSIZ];
char cur;
char prev;
int cmdNum;
int cmdLength;
int cmdLengths[3];
int cmdStarts[3];
int cmdStart;
int inputType;
int done;
int badCmd;

/*- COMMAND CONSTANTS ----------------------------------------------*/
int SINGLE_COMMAND = 0;
int TWO_COMMANDS = 1;
int REDIRECT_STDOUT = 2;
int REDIRECT_STDIN = 3;
int REDIRECT_STDERR = 4;
int REDIRECT_BOTH = 5;
int PIPELINE_TWO = 6;
int PIPELINE_THREE = 7;
int IN_BACKGROUND = 8;
/*- GET CURRENT WORKING DIRECTORY+SET PATH ------------------------*/
if (getcwd(cwd, sizeof(cwd)) == NULL) {
printf("Error getting current directory. Exiting...\n");
}
char *name = "PATH";
char *val = "/home/ugrad/nedg/410/matrix-mult:/usr/bin";
int rewrite = 1;
if(setenv(name, val, rewrite))
printf("Cannot set environment");

/*SIGNAL LISTENER----------------------------------------------*/


/*- MAIN LOOP --------------------------------------------------*/
while(1){
printf("myshell>");
memset(input, EOF, BUFSIZ);
cmdLength = 0;
cmdStart = 0;
inputType = 0;

cmdLengths[0] = 0;
cmdLengths[1] = 0;
cmdLengths[2] = 0;
cmdStarts[0] = 0;
cmdStarts[1] = 0;
cmdStarts[2] = 0;

done = 0;
badCmd = 0;
fgets(input, BUFSIZ, stdin);
int i;
for (i = 0; i < BUFSIZ && !done; ++i) {
cur = input[i];

switch(cur) {
	case EOF:
		if (inputType == SINGLE_COMMAND ||
				inputType == IN_BACKGROUND) {
			memset(cmd1, '\0', sizeof(cmd1));
			strncpy(cmd1, input+cmdStart, cmdLength-2);

			cmdLengths[0] = cmdLength;
			cmdStarts[0] = cmdStart;
		} else if (inputType == PIPELINE_THREE) {
			memset(cmd3, '\0', sizeof(cmd1));
			strncpy(cmd3, input+cmdStart, cmdLength-2);

			cmdLengths[2] = cmdLength;
			cmdStarts[2] = cmdStart;
		} else {
			memset(cmd2, '\0', sizeof(cmd1));
			strncpy(cmd2, input+cmdStart, cmdLength-2);

			cmdLengths[1] = cmdLength;
			cmdStarts[1] = cmdStart;
		}

		done = 1;
		break;
	case ';':
		if (inputType != 0 || cmdLength == 0) {
			printf("Invalid command format.\n");
			badCmd = 1;
			done = 1;
			break;
		}

		memset(cmd1, '\0', sizeof(cmd1));
		strncpy(cmd1, input+cmdStart, cmdLength);


		cmdLengths[0] = cmdLength;
		cmdStarts[0] = cmdStart;

		cmdLength = 0;
		cmdStart = i + 1;
		inputType = TWO_COMMANDS;
		break;
	case '>':
		if (inputType != 0 || cmdLength == 0) {
			printf("Invalid command format.\n");
			badCmd = 1;
			done = 1;
			break;
		}

		if (prev == '2') inputType = REDIRECT_STDERR;
		else if (prev == '&') inputType = REDIRECT_BOTH;
		else inputType = REDIRECT_STDOUT;

		memset(cmd1, '\0', sizeof(cmd1));
		strncpy(cmd1, input+cmdStart, cmdLength);

		cmdLengths[0] = cmdLength;
		cmdStarts[0] = cmdStart;

		cmdLength = 0;
		cmdStart = i + 1;
		break;
	case '<':
		if (inputType != 0 || cmdLength == 0) {
			printf("Invalid command format.\n");
			badCmd = 1;
			done = 1;
			break;
		}

		inputType = REDIRECT_STDIN;

		memset(cmd1, '\0', sizeof(cmd1));
		strncpy(cmd1, input+cmdStart, cmdLength);

		cmdLengths[0] = cmdLength;
		cmdStarts[0] = cmdStart;

		cmdLength = 0;
		cmdStart = i + 1;
		break;
	case '|':
		if ((inputType != 0 && inputType != PIPELINE_TWO) ||
				cmdLength == 0) {
			printf("Invalid command format.\n");
			badCmd = 1;
			done = 1;
			break;
		}

		if (inputType == SINGLE_COMMAND) {
			memset(cmd1, '\0', sizeof(cmd1));
			strncpy(cmd1, input+cmdStart, cmdLength);

			cmdLengths[0] = cmdLength;
			cmdStarts[0] = cmdStart;
			inputType = PIPELINE_TWO;
		} else {
			memset(cmd2, '\0', sizeof(cmd1));
			strncpy(cmd2, input+cmdStart, cmdLength);

			cmdLengths[1] = cmdLength;
			cmdStarts[1] = cmdStart;
			inputType = PIPELINE_THREE;
		}

		cmdLength = 0;
		cmdStart = i + 1;
		break;
	case '&':
		if (input[i + 1] == '\n') {
			inputType = IN_BACKGROUND;	
		}
		break;
	case ' ':
		if (cmdLength == 0) ++cmdStart;
		break;		
	default:
		++cmdLength;
}

prev = input[i];

}


char *comm = cmd1;
char *comm2 = cmd2;
char *comm3 = cmd3;
char *arga[2] = {comm, NULL};
char *arga2[2] = {comm2, NULL};
char *arga3[2] = {comm3, NULL};
int fk;
int er;
int error = errno;
int dupval;
int dupval2;
int in;
int out;
int err;
int pipearr[2];
int pipes[4];
int status;
printf("input type: %d\n",inputType);
switch(inputType){
case 0:
	fk = fork();
	if(!fk){
		execvp(arga[0], arga);
		exit(error);
		//error = errno;
		//exit(error);
	}
	else{
		waitpid(fk, NULL, 0);
		break;
	}	
	/*	int status;
		wait(&status);

		if(WIFEXITED(status))
		printf("errcode = %d\n",WEXITSTATUS(status));
		}*/
case 1:
	fk = fork();
	if(!fk){
		execvp(arga[0], arga);
		exit(error);
	}
	else if(fk == -1){
		printf("cannot fork");
	}
	else{
		waitpid(fk, NULL, 0);
		fk = fork();
		if(!fk){
			arga[0] = cmd2;
			execvp(arga[0], arga);
			exit(error);
		}

		waitpid(fk, NULL, 0);
		break;
	}
case 2:
	out = open(cmd2, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
	dupval = dup(1);
	dup2(out,1);
	close(out);
	fk = fork();
	if(!fk){
		execvp(arga[0], arga);
		exit(error);
	}
	dup2(dupval, 1);
	break;
case 3:
	in = open(cmd2, O_RDONLY);
	dupval = dup(0);
	dup2(in, 0);
	close(in);
	fk = fork();
	if(!fk){
		execvp(arga[0], arga);
		exit(error);
	}
	dup2(dupval, 0);
	waitpid(fk, NULL, 0);
	break;
case 4:
	err = open(cmd2, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
	dupval = dup(2);
	dup2(err, 2);
	close(err);
	fk = fork();
	if(!fk){
		execvp(arga[0], arga);
		exit(error);
	}
	dup2(dupval, 2);
	waitpid(fk, NULL, 0);
	break;
case 5:
	out = open(cmd2, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
	err = open(cmd2, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
	dupval = dup(1);
	dupval2 = dup(2);
	dup2(out, 1);
	dup2(err, 2);
	close(out);
	close(err);
	fk = fork();
	if(!fk){
		execvp(arga[0], arga);
		exit(error);
	}
	dup2(dupval, 1);
	dup2(dupval2, 2);
	waitpid(fk, NULL, 0);
	break;
case 6:
	fk = fork();
	pipe(pipearr);

	if(!fk){
		dup2(pipearr[0], 0);
		close(pipearr[1]);
		execvp(arga2[0], arga2);
		exit(error);
	}
	else{
		fk = fork();
		if(!fk){
		dup2(pipearr[1], 2);
		close(pipearr[0]);
		execvp(arga[0], arga);
		exit(error);
		}
	}
	waitpid(fk, NULL, 0);
	break;
case 7:
	pipe(pipes);
	pipe(pipes+2);
	fk = fork();
	if(!fk){
		dup2(pipes[1], 2);

		close(pipes[0]);
		close(pipes[1]);
		close(pipes[2]);
		close(pipes[3]);

		execvp(arga[0], arga);
		exit(error);
	}
	else{
		fk = fork();
		if(!fk){
			dup2(pipes[0], 0);
			dup2(pipes[3], 2);

			close(pipes[0]);
			close(pipes[1]);
			close(pipes[2]);
			close(pipes[3]);
			
			execvp(arga2[0], arga2);
			exit(error);
		}
		else{
			fk = fork();
			if(!fk){
				dup2(pipes[2], 0);

				close(pipes[0]);
				close(pipes[1]);
				close(pipes[2]);
				close(pipes[3]);

				execvp(arga3[0], arga3);
			}
		}
	}
close(pipes[0]);
close(pipes[1]);
close(pipes[2]);
close(pipes[3]);

for(i = 0; i < 3; ++i){
	wait(&status);
	break;
case 8:
	fk = fork();
	if(!fk){
		execvp(arga[0], arga);
	}
	else
		break;
}
}

}
}
