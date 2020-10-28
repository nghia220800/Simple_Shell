#include "SimpleShell.h"
int numOfLines=0;
char** historyLines=NULL;


void shell_loop(){
  char *line;
  char **args;
  int status;
  historyLines=malloc(numOfLines*sizeof(char*));
  do {
    printf("SimpleShell> ");
    line = ss_readline();
    args = ss_splitline(line);
    status = ss_execute(args);
    free(line);
    free(args);
  } while (status);
  for (int i=0;i<numOfLines;i++){
    free(historyLines[i]);
  }
  free(historyLines);
}

char* ss_readline(){
    char *line = NULL;
    ssize_t bufsize = 0;
    int res=getline(&line, &bufsize, stdin);
    //getline returns -1 if eof is reached
    if ( res<0){
        if (feof(stdin)) {
          exit(EXIT_SUCCESS);
        } else  {
          perror("readline problem");
          exit(EXIT_FAILURE);
        }
    }else{
      //char* currentLine=malloc((strlen(line)+1)*sizeof(char));
      char* currentLine=strdup(line);
       if (numOfLines==0 ||strcmp(historyLines[numOfLines-1],currentLine)!=0){
        numOfLines++;
        historyLines=realloc(historyLines, numOfLines * sizeof(char*));
        historyLines[numOfLines-1]=currentLine;
      }
    }
    return line;
}


char **ss_splitline(char *line)
{
  int bufsize = TOKEN_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "ss: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, TOKEN_SEPERATOR);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += TOKEN_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "ss: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, TOKEN_SEPERATOR);
  }
  tokens[position] = NULL;
  return tokens;
}

int ss_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process sucess or notify error
    if (execvp(args[0], args) ==-1) {
      perror("ss launch error");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("ss fork error");
  } else {
    // Parent process
    do {
    //wait until child process is terminated normally or killed by signal
      wpid = waitpid(pid, &status, 0);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));

  }
  return 1;
}
/*Builtins commands*/
int ss_cd(char **args);
int ss_exit(char **args);
int ss_history(char **args);
int ss_latestcmd(char **args);

char *builtin_commands[] = {
  "cd",
  "exit",
  "history",
  "!!"
};

int (*builtin_func[]) (char **) = {
  &ss_cd,
  &ss_exit,
  &ss_history,
  &ss_latestcmd
};

int numOfBuiltins(){
    return sizeof(builtin_commands) / sizeof(char *);
}

int ss_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "You didnt specified any agrument");
  } else {
    if (chdir(args[1]) != 0) {
      perror("Change directory failed");
    }
  }
  return 1;
}

int ss_exit(char **args)
{
  return 0;
}

int ss_history(char **args){
  if (numOfLines==0){
    printf("No commands in history\n");
  }else{
    for (int i=0;i<numOfLines;i++){
      printf("%d. ",i+1);
      printf(historyLines[i]);
      // if (strcmp(historyLines[i+1],"!!\n")==0){
      //   printf("\n");
      // }
    }
  }
  return 1;
}

int ss_latestcmd(char **args){
  if (numOfLines==0){
    printf("No commands in history\n");
  }else{
    char* cL=strdup(historyLines[numOfLines-2]);
    return ss_execute(ss_splitline(cL));
  }
  return 1;
}
/*End of builtins command*/

int ss_execute(char **args)
{
  int i;
  //Entered empty string
  if (args[0] == NULL) {
    return 1;
  }

  for (i = 0; i < numOfBuiltins(); i++) {
    if (strcmp(args[0], builtin_commands[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return ss_launch(args);
}