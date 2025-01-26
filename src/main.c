#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#include "Parser.h"
#include "Utils.h"

int main(const int argc, char **argv) {
    if (argc != 2)
    {
        printf("Usage: ./VMTranslator <input_file/input_dir>\n");
        return 1;
    }

    struct dirent *de;
    DIR *dr = opendir(argv[1]);

    if (dr == NULL)
    {
        FILE* input = fopen(argv[1], "r");
        if (input == NULL)
        {
            printf("Could not open file or directory by name %s\n", argv[1]);
            return 1;
        }
        const char* outputFileName = OutputFileName(argv[1]);
        FILE* output = fopen(outputFileName, "w");
        free((void*) outputFileName);

        char line[256];
        int lineNum = 1;
        int eqNum = 0;
        int gtNum = 0;
        int ltNum = 0;
        int returnAddrIndex = 0;
        char* currentFunction = NULL;
        const char* filenameWithoutExtension = FilenameWithoutExtension(argv[1]);

        fprintf(output, "// Bootstrap code\n");
        fprintf(output, "@256\nD=A\n@0\nM=D\n");
        const char* bootstrapCommand = ParseCommand("call Sys.init 0", filenameWithoutExtension, lineNum++, &eqNum, &gtNum, &ltNum, &returnAddrIndex, NULL);
        fprintf(output, "%s", bootstrapCommand);
        free((void*) bootstrapCommand);
        fprintf(output, "// End of bootstrap code\n");

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
        free((void*) filenameWithoutExtension);
        fclose(input);
        fclose(output);
    }
    else
    {
        char* cleanDir = malloc((StrLen(argv[1]) + 1) * sizeof(char));
        strcpy(cleanDir, argv[1]);
        if (cleanDir == NULL) return 1;
        if (cleanDir[StrLen(cleanDir) - 1] == '/' || cleanDir[StrLen(cleanDir) - 1] == '\\')
        {
            cleanDir[StrLen(cleanDir) - 1] = '\0';
        }

        char* outputFilename = malloc((StrLen(cleanDir) + 4 + 1) * sizeof(char));
        sprintf(outputFilename, "%s.asm", cleanDir);
        FILE* output = fopen(outputFilename, "w");
        if (output == NULL)
        {
            fprintf(stderr, "Could not open an output file by name %s", outputFilename);

            free(cleanDir);
            free(outputFilename);
            return 1;
        }

        free(outputFilename);

        int lineNum = 1;
        int eqNum = 0;
        int gtNum = 0;
        int ltNum = 0;
        int returnAddrIndex = 0;
        char* currentFunction = NULL;

        fprintf(output, "// Bootstrap code\n");
        fprintf(output, "@256\nD=A\n@0\nM=D\n");
        const char* bootstrapCommand = ParseCommand("call Sys.init 0", cleanDir, lineNum++, &eqNum, &gtNum, &ltNum, &returnAddrIndex, NULL);
        fprintf(output, "%s", bootstrapCommand);
        free((void*) bootstrapCommand);
        fprintf(output, "// End of bootstrap code\n");

        while ((de = readdir(dr)) != NULL)
        {
            const char* filename = de->d_name;
            const int inLen = StrLen(filename);
            if (!(inLen > 3 && filename[inLen - 3] == '.' && filename[inLen - 2] == 'v' && filename[inLen - 1] == 'm'))
                continue;

            // + 1 to account for '/'
            char* fullFilename = malloc((StrLen(cleanDir) + StrLen(filename) + 1 + 1) * sizeof(char));
            sprintf(fullFilename, "%s/%s", cleanDir, filename);
            FILE* input = fopen(fullFilename, "r");
            if (input == NULL)
            {
                continue;
            }

            free(fullFilename);

            char line[256];
            const char* filenameWithoutExtension = FilenameWithoutExtension(filename);
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

            free((void*) filenameWithoutExtension);
            fclose(input);
        }
        if (currentFunction != NULL) free(currentFunction);
        free(cleanDir);
        fclose(output);
    }

    return 0;
}
