#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

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

char* input_comm()
{
	char* command = nullptr;
	size_t* buffSize = 0;
	int ip = std::getline(&command, &buffSize, stdin);
}
void main()
{

}