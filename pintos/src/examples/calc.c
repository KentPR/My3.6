#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall.h>
#include <string.h>
#include "filesys/filesys.h"

const char alloweded_symbols[16] = {"0123456789+-%/*"};
const char file_name[17] = { "calc_history.txt" };

int open_file(void) {
	int fd = open(file_name);
	if (fd == -1) {
		filesys_create(file_name, 0);
		fd = open(file_name);
	}
	return fd;
}

void add_to_file(char *math_exp, char *result) {
	int i = 0, j = 0;
	int fd = open_file();
	char buffer[1024];
	int length_math = strlen(math_exp);
	int length_res = strlen(result);

	int reading_bytes = read(fd, buffer, sizeof(buffer));

	while (i < length_math) {
		buffer[reading_bytes] = math_exp[i];
		reading_bytes++;
		i++;
	}

	reading_bytes++;
	buffer[reading_bytes] = '=';
	reading_bytes++;

	while (j < length_res) {
		buffer[reading_bytes] = result[j];
		reading_bytes++;
		j++;
	}

	buffer[reading_bytes] = '\n';
	reading_bytes++;
	buffer[reading_bytes] = '\0';
	reading_bytes++;

	closing(fd);
	filesys_remove(file_name);
	filesys_create(file_name, reading_bytes);

	fd = open_file();
	write(fd, buffer, sizeof(buffer));
	closing(fd);
}

int check_arguments(int argc, char* argv) {
	switch (argc) {
	case 2:
		if (!strcmp(argv, "help") && !strcmp(argv, "history"))
			return 0;
		break;
	case 3:
		if (!strcmp(argv, "eval"))
			return 0;
		break;
	default:
		return -1;
		break;
	}
	return 1;
}

int check_math_exp(char* math_exp) {
	int length = strlen(math_exp);
	int sign_count = 0, num_count = 0;
	for (int i = 0; i < length; i++) {
		if ((math_exp[i] < '0' || math_exp[i] > '9') && math_exp[i] != '+'
			&& math_exp[i] != '-' && math_exp[i] != '*' && math_exp[i] != '/'
			&& math_exp[i] != '%')
			return 1;
	}

	for (int i = 0; i < length; i++) {
		if (math_exp[i] == '-' || math_exp[i] == '+' || math_exp[i] == '*'
			|| math_exp[i] == '%' || math_exp[i] == '/')
			sign_count++;
		else
			num_count++;
	}

	if (num_count == 0)
		return 2;
	if (sign_count != 1)
		return 2;

	return 0;
}

void help(void) {
	printf("HELP:\n");
	printf("Use ---calc help--- to show this message.\n");
	printf("Use ---calc eval 'math_expresion'--- to calculate.\n");
	printf("Alloweded symbols: %s\n", alloweded_symbols);
	printf("Alloweded expressions: a+b, a-b, a*b, a%b, a/b, -a.\n");
	printf("Use ---calc history--- to show history of calculations.\n");
}

int calculation(char* math_exp) {
	int i = 0, k = 0;
	int length = strlen(math_exp);
	char num1[10];
	char num2[10];
	int inum1;
	int inum2;
	int result;
	char sign;

	if (math_exp[0] == '-') {
		for (int j = 1; j < length; j++)
			num1[j - 1] = math_exp[j];
		inum1 = atoi(num1) * (-1);
		printf("Result: %d", inum1);
		return inum1;
	}
	else if (math_exp[0] == '+' || math_exp[0] == '*' || math_exp[0] == '%' || math_exp[0] == '/') {
		printf("Error: Math expression is unacceptable.\n");
		return 0;
	}
	else
	{
		for (i = 0; i < length; i++) {
			if (math_exp[i] != '+' && math_exp[i] != '-'
				&& math_exp[i] != '*' && math_exp[i] != '/'
				&& math_exp[i] != '%')
				num1[i] = math_exp[i];
			else
				break;
		}

		sign = math_exp[i];
		i++;

		for (k = 0; i < length; i++, k++) {
			num2[k] = math_exp[i];
		}
	}

	inum1 = atoi(num1);
	inum2 = atoi(num2);

	switch (sign) {
	case '+':
		result = inum1 + inum2;
		break;
	case '-':
		result = inum1 - inum2;
		break;
	case '%':
		result = inum1 % inum2;
		break;
	case '*':
		result = inum1 * inum2;
		break;
	case '/':
		if (inum2 == 0) {
			printf("Error: Division by zero.\n");
			return 0;
		}
		result = inum1 / inum2;
		break;
	}

	printf("Result: %d", result);
	return result;
}

void history(void) {
	char buffer[1024];
	char *save_ptr, *token;
	int reading;
	int fd = open_file();
	reading = read(fd, buffer, sizeof(buffer));
	if (reading == 0) {
		printf("Error: File %s is empty.\n", file_name);
		return;
	}
	printf("History:\n");
	//printf("%s\n", buffer);
	for (token = strtok_r(buffer, "\n", &save_ptr); token != NULL; token = strtok_r(NULL, "\n", &save_ptr)) {
		printf("%s\n", token);
	}
	closing(fd);
}

int 
main(int argc, const char* argv[])
{
	int check, action_num, result;
	char* action = argv[1];
	char* math_expression;
	char* ch_result[10];
	
	check = check_arguments(argc, action);
	switch (check)
	{
	case 0:
		printf("Error: Unacceptable command.\n");
		return 1;
		break;
	case -1:
		printf("Error: Unacceptable count of arguments.\n");
		return 1;
		break;
	default:
		break;
	}

	if (argc == 3) {
		math_expression = argv[2];
		check = check_math_exp(math_expression);
		if (check == 1) {
			printf("Error: Math expression has unacceptable symbols.\n");
			return 1;
		}
		if (check == 2) {
			printf("Error: Math expression is unacceptable.\n");
			return 1;
		}
	}

	if (strcmp(action, "help"))
		action_num = 1;
	else if (strcmp(action, "eval"))
		action_num = 2;
	else
		action_num = 3;

	switch (action_num)
	{
	case 1:
		help();
		return 0;
		break;
	case 2:
		result = calculation(math_expression);

		int v = 0;
		int n = result;
		while (n > 9) {
			ch_result[v++] = (n % 10) + '0';
			n = n / 10;
		}
		ch_result[v++] = n + '0';
		ch_result[v] = '\0';
		char t;
		for (int h = 0; h < v / 2; h++) {
			t = ch_result[h];
			ch_result[h] = ch_result[v - 1 - h];
			ch_result[v - 1 - h] = t;
		}
		
		add_to_file(math_expression, ch_result);
		return 0;
		break;
	case 3:
		history();
		return 0;
		break;
	}
}