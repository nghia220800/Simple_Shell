#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<time.h>
#include<stdbool.h>

#define TOKEN_BUFSIZE 64
#define TOKEN_SEPERATOR " \t\r\n\a"

void shell_loop();
char* ss_readline();
char** ss_splitline(char* line);
int ss_launch(char **args);
int ss_execute(char **args);
