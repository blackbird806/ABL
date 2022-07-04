#include <stdio.h>
#include <stdlib.h>
#include "abl.h"

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
	buffer[size] = '\0';
	fclose(file);
	return buffer;
}

static void repl() 
{
	char line[1024];
	while (1)
	{
		puts("> ");
		if (!fgets(line, sizeof(line), stdin)) {
			puts("");
			break;
		}
	}
}

int main(int argc, char** argv)
{
	return 0;
	if (argc == 1) 
	{
		repl();
	} 
	else if (argc == 2)
	{
		char* src = read_file(argv[1]);	
		free(src);
	}

	return 0;
}
