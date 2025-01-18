#include "Parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Utils.h"

char* ParseNWord(const char* str, const int n)
{
    if (n < 1) return NULL;
    if (str == NULL) return NULL;

    const int size = StrLen(str);
    char* p = malloc((size + 1) * sizeof(char));
    if (p == NULL) return NULL;

    int j = 0;
    int word = 0;
    int space = 0;
    for (int i = 0; i < size + 1; i++)
    {
        if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n' || str[i] == '\r' || str[i] == '\0')
        {
            if (word > space) space++;

            if (word == n)
            {
                p[j] = '\0';

                char* clean = Clean(p);
                free(p);
                if (clean == NULL) return NULL;

                return clean;
            }
        }
        else
        {
            if (word == space) word++;

            if (word == n)
            {
                p[j++] = str[i];
            }
        }
    }

    return NULL;
}

char* ParseOp(const char* command)
{
    return ParseNWord(command, 1);
}

char* ParseSegment(const char* command)
{
    return ParseNWord(command, 2);
}

char* ParseValue(const char* command)
{
    return ParseNWord(command, 3);
}

char* ParseArithmeticCommand(const char* command)
{
    const char* op = ParseOp(command);
    if (op == NULL) return NULL;

    if (StrCmp(op, "add"))
    {
        free((void*) op);
        return "@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nM=D+M\n";
    }
    if (StrCmp(op, "sub"))
    {
        free((void*) op);
        return "@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nM=M-D\n";
    }
    if (StrCmp(op, "neg"))
    {
        free((void*) op);
        return "@SP\nA=M-1\nM=-M\n";
    }
    if (StrCmp(op, "eq"))
    {
        free((void*) op);
        return "@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nD=D-M\n@EQ1\nD;JEQ\n@SP\nA=M-1\nM=0\n@END1\n0;JMP\n(EQ1)\n@SP\nA=M-1\nM=1\n(END1)\n";
    }
    if (StrCmp(op, "gt"))
    {
        free((void*) op);
        return "@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nD=M-D\n@GT1\nD;JGT\n@SP\nA=M-1\nM=0\n@END1\n0;JMP\n(GT1)\n@SP\nA=M-1\nM=1\n(END1)\n";
    }
    if (StrCmp(op, "lt"))
    {
        free((void*) op);
        return "@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nD=M-D\n@GT1\nD;JLT\n@SP\nA=M-1\nM=0\n@END1\n0;JMP\n(GT1)\n@SP\nA=M-1\nM=1\n(END1)\n";
    }
    if (StrCmp(op, "and"))
    {
        free((void*) op);
        return "@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nM=D&M\n";
    }
    if (StrCmp(op, "or"))
    {
        free((void*) op);
        return "@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nM=D|M\n";
    }
    if (StrCmp(op, "not"))
    {
        free((void*) op);
        return "@SP\nA=M-1\nM=!M\n";
    }

    return NULL;
}

char* ParseCommand(const char* command)
{
    const char* op = ParseOp(command);
    if (op == NULL) return NULL;

    const char* parsedArithmeticCommand = ParseArithmeticCommand(command);
    if (parsedArithmeticCommand != NULL)
    {
        const int len = StrLen(parsedArithmeticCommand);
        char* p = malloc((len + 1) * sizeof(char));

        strcpy(p, parsedArithmeticCommand);

        return p;
    }


    if (StrCmp(op, "push"))
    {
        const char* segment = ParseSegment(command);

        if (StrCmp(segment, "constant"))
        {

        }
    }
    // if (StrCmp(op, "pop"))
    // {
    //
    // }

    free(op);
    fprintf(stderr, "Unrecognized command '%s' in line n\n", command);
    exit(1);
}
