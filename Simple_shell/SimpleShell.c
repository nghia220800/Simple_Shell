#include "SimpleShell.h"
int numOfLines=0;
char** historyLines=NULL;

void shell_loop(){
  char *line;
  char **args;
  int type;
  char **args1;
  char **partition = malloc(2* sizeof(char*));
  int status; 
  historyLines=malloc(numOfLines*sizeof(char*));
  do {
    printf("SimpleShell> ");
    line = ss_readline();
    type = cmd_type(line);
    partition = parsePipeRedirect(line);

    args = ss_splitline(partition[0]);
    args1 = ss_splitline(partition[1]);

    if(type == 1) //out 1
    {
      out_redirect(args,args1);
    }
    else if(type == 2) //in 2
    {
      in_redirect(args,args1);
    }
    else if (type ==3) // pipe 3
    {
      execArgsPiped(args, args1);
    }
    else
    {
      status = ss_execute(args);
    }
    free(line);
    free(args);
    type = 0;
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
      printf("%s",historyLines[i]);
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


int cmd_type(char* line)
{
  for (int i = 0; i<strlen(line);i++)
  {
    if(line[i] == '|')
    {//pipe
      return 3;
    }
    if(line[i] == '>')
    { //out
      return 1;
    }
    else if(line[i] == '<')
    { // in
       return 2;
    }    
  }
  return 0;
}

void out_redirect(char **args, char** fileName)
{
  pid_t wpid;
  pid_t pid=fork();
  if (pid<0)
  {
    perror("fork failed");
    return;
  }
  if (pid==0)
  {
    int fd = open(fileName[0], O_CREAT| O_WRONLY, 0666);
    if (fd < 0)
  {
    perror("open error");
    return ;
  }
  
  if (dup2(fd,STDOUT_FILENO)<0)
  {
    perror("dup2 failed");
    return;
  }
  close(fd);
    if(execvp(args[0],args)<0)
    {
      perror("execvp failed."); 
      exit(0);   
    }
  }
  wpid = waitpid(pid, NULL, 0);
}

void in_redirect(char** args, char** fileName)
{
  pid_t wpid;
  pid_t pid = fork();
  if (pid<0)
  {
    perror("fork failed");
    return;
  }
  if (pid==0)
  {
    int fd=open(fileName[0],O_RDONLY,0666);
    if (fd<0)
    {
      perror("open failed");
      return ;
    }
    if (dup2(fd,STDIN_FILENO)<0)
    {
      perror("dup2 failed");
      return;
    }
    close(fd);
    if(execvp(args[0],args)<0)
    {
      perror("execvp failed");
      exit(EXIT_FAILURE);
    }
    }
    wpid = waitpid(pid, NULL, 0);
}  

//Parse Pipe and Redirect
char** parsePipeRedirect(char* line) 
{
  int bufsize = 2;
  char **tokens = (char**)malloc(bufsize * sizeof(char*));
  char *token;

  token = strtok(line, delimPipeRedirect);
  tokens[0] = token;
  token=strtok(NULL,"\n");
  tokens[1]= token;  
  return tokens;
}


void execArgsPiped(char** parsed, char** parsedpipe) 
{ 
    // 0 is read end, 1 is write end 
    int pipefd[2];  
    pid_t p1, p2; 
  
    if (pipe(pipefd) < 0) { 
        perror("\nPipe could not be initialized"); 
        return; 
    } 
    p1 = fork(); 
    if (p1 < 0) { 
        perror("\nCould not fork"); 
        return; 
    } 
  
    if (p1 == 0) { 
        // Child 1 executing.. 
        // It only needs to write at the write end 
        close(pipefd[0]); 
        dup2(pipefd[1], STDOUT_FILENO); 
        close(pipefd[1]); 
  
        if (execvp(parsed[0], parsed) < 0) { 
            perror("\nCould not execute command 1.."); 
            exit(0); 
        } 
    } else { 
        // Parent executing 
        p2 = fork(); 
  
        if (p2 < 0) { 
            perror("\nCould not fork"); 
            return; 
        } 
  
        // Child 2 executing.. 
        // It only needs to read at the read end 
        if (p2 == 0) { 
            close(pipefd[1]); 
            dup2(pipefd[0], STDIN_FILENO); 
            close(pipefd[0]); 
            if (execvp(parsedpipe[0], parsedpipe) < 0) { 
                perror("\nCould not execute command 2.."); 
                exit(0); 
            } 
        } 
    } 
    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(p1,NULL,0);
    waitpid(p2,NULL,0);
} 