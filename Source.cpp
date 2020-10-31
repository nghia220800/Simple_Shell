#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<sys/types.h>
char* history = nullptr;
//dem so phan tu trong mang lenh da nhap, lam de tien su dung ve sau
int parse_counter(char** parse)
{
	int i = 1;
	while (parse[i] != nullptr)
	{
		i++;
	}
	return i;
}

//xoa khoang trang va cac ki tu o cuoi chuoi (&) ma shell se khong hieu
void delete_unnecess(char* str, char c = '\0')
{
	int i, start = 0, end = strlen(str) - 1;
	if (c == '\0')
	{
		while ((str[start] == ' ' || str[start] == '\n' || str[start] == '\t' || str[start] == '\r') && start < end)
			start++;
		while (end >= start && (str[start] == ' ' || str[start] == '\n' || str[start] == '\t' || str[start] == '\r'))
			end--;
	}
	else end--;
	for (i = start; i < end-1; i++)
	{
		str[i - start] = str[i];
	}
	str[i - start] = '\0';
}

//check &
bool del_ampersand(char** parse)
{
	bool re = false;
	int n = parse_counter(parse);
	if (strcmp(parse[n - 1], "&") == 0)
	{
		re = true;
		parse[n - 1] = nullptr;
	}
	else
	{
		int len = strlen(parse[n - 1]);
		char tmp = parse[n - 1][len - 1];
		if (tmp == '&')
		{
			re = true;
			delete_unnecess(parse[n - 1], tmp);
		}
	}
	return re;
}

char* input_command()
{
	char* command = nullptr;
	size_t buffSize = 0;
	int ip = getline(&command, &buffSize, stdin);
	if (command[ip - 1] == '\n')
	{
		command[ip - 1] = '\0';
	}
	//luu cac cau lenh da nhap truoc
	if (strcmp(command, "!!") != 0 && command[0] != '\0')
	{
		if (history != nullptr) 
			free(history);
		history = (char*)malloc(strlen(command) + 1);
		strcpy(history, command);
	}
}

//ham dung strtok() de tach dong lenh thanh token dua tren delim
#define tk_buffsize 64
#define delim "\t\r\n\a"
char** parseSpace(char* line)
{
	int bufSize = tk_buffsize;
	int pos = 0;
	char* token;
	char** tokenArr = (char**)malloc(sizeof(char*) * bufSize);

	if (!tokenArr)
	{
		perror("Alloction error");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, delim);

	while (token != NULL)
	{
		tokenArr[pos] = token;
		pos++;

		if (pos >= bufSize)
		{
			bufSize += tk_buffsize;
			tokenArr = (char**)realloc(tokenArr, bufSize * sizeof(char*));
			if (!tokenArr)
			{
				perror("Allocation error");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, delim);

	}
	tokenArr[pos] = nullptr;

	return tokenArr;
}

//giong ham tren nhung dung cho pipe & redirect
#define pipeRedirect "|<>"
char** parsePipe(char* line)
{
	int bufSize = 2;
	char* pipe;
	char** pipeArr = (char**)malloc(bufSize * sizeof(char*) + 1);
	int pos = 0;

	pipe = strtok(line, pipeRedirect);
	delete_unnecess(pipe);
	pipeArr[0] = pipe;

	pipe = strtok(nullptr, "\n");
	delete_unnecess(pipe);
	pipeArr[1] = pipe;

	pipeArr[2] = nullptr;
	return pipeArr;
}

void execArgv(char** parse)
{
	bool found_amp = is_ampersand(parse);

	pid_t pid = fork();

	if (pid < 0)
	{
		perror("fork failed");
		exit(EXIT_FAILURE);
	}
	else if (pid == 0)
	{
		execvp(parse[0], parse);
		perror("exevcp failed");
		exit(EXIT_FAILURE);
	}
	//parent process
	else if (!found_amp)
	{
		waitpid(pid, NULL, 0);
	}

	return;
}
void main()
{

}