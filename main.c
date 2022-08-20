#include <stdio.h>
#include <stdlib.h>
#include "abl.h"
#include "abl_compiler.h"

static char* read_file(const char* path) 
{
	assert(path != NULL);

	FILE* file = fopen(path, "rb");
	if (file == NULL)
	{
		fprintf(stderr, "failed to open file %s", path);
		exit(EXIT_FAILURE);
	}
	fseek(file, 0L, SEEK_END);
	size_t const size = ftell(file);
	rewind(file);
	char* buffer = (char*)malloc(size + 1);
	assert(buffer != NULL);
	fread(buffer, sizeof(char), size, file);
	buffer[size] = '\0';
	fclose(file);
	return buffer;
}

static void repl() 
{
	char line[1024];
	while (1)
	{
		printf("> ");
		if (!fgets(line, sizeof(line), stdin)) {
			puts("");
			break;
		}
		lexer lex;
		init_lexer(&lex, line);
		token tk;
		do {
			tk = lex_token(&lex);
			printf("tk : %d\n", tk.type);
		}
		while (tk.type != TK_EOF);
	}
}

int main(int argc, char** argv)
{
	//if (argc == 1)
	//{
	//	repl();
	//} 
	//else if (argc == 2)
	{
		//char* src = read_file(argv[1]);
		char* src = read_file("test.abl");
		compile(src, stdout);
		free(src);
	}
	return 0;
}
