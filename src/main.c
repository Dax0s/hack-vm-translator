#include <stdio.h>
#include <stdlib.h>

#include "Parser.h"
#include "Utils.h"

int main(const int argc, char **argv) {
    if (argc != 2)
    {
        printf("Usage: ./VMTranslator <input_file>\n");
        return 1;
    }

    FILE* input = fopen(argv[1], "r");
    if (input == NULL)
    {
        printf("Could not open file %s\n", argv[1]);
        return 1;
    }
    const char* outputFileName = OutputFileName(argv[1]);
    FILE* output = fopen(outputFileName, "w");

    char line[256];
    int lineNum = 1;
    int eqNum = 0;
    int gtNum = 0;
    int ltNum = 0;
    int returnAddrIndex = 0;
    char* currentFunction = NULL;
    const char* filenameWithoutExtension = FilenameWithoutExtension(argv[1]);
    while (fgets(line, sizeof(line), input))
    {
        const char* clean = Clean(line);
        if (clean != NULL && !(clean[0] == '/' && clean[1] == '/'))
        {
            const int len = StrLen(clean);
            if (clean[len - 1] != '\n')
            {
                fprintf(output, "// %s\n", clean);
            }
            else
            {
                fprintf(output, "// %s", clean);
            }

            const char* parsedCommand = ParseCommand(clean, filenameWithoutExtension, lineNum++, &eqNum, &gtNum, &ltNum, &returnAddrIndex, &currentFunction);
            fprintf(output, "%s", parsedCommand);
            free((void*) parsedCommand);
        }

        free((void*) clean);
    }

    free(currentFunction);
    free((void*) outputFileName);
    free((void*) filenameWithoutExtension);
    fclose(input);
    fclose(output);

    return 0;
}
