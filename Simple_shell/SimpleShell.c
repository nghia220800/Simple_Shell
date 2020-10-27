#include "SimpleShell.h"

void shell_loop(){
  char *line;
  char **args;
  int status;

  do {
    printf("SimpleShell> ");
    line = ss_readline();
    args = ss_splitline(line);
    status = ss_execute(args);

    free(line);
    free(args);
  } while (status);
}

char* ss_readline(){
    char *line = NULL;
    ssize_t bufsize = 0;

    //getline returns -1 if eof is reached
    if (getline(&line, &bufsize, stdin) == -1){
        if (feof(stdin)) {
        exit(EXIT_SUCCESS);
        } else  {
        perror("readline problem");
        exit(EXIT_FAILURE);
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
    fprintf(stderr, "lsh: allocation error\n");
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
        fprintf(stderr, "lsh: allocation error\n");
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
    if (execvp(args[0], args) == -1) {
      perror("ss launch error");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh fork error");
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

char *builtin_commands[] = {
  "cd",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &ss_cd,
  &ss_exit
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