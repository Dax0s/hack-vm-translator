#include "Parser.h"

#include <stdlib.h>

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

char* ParseCommand(const char* command)
{
    char* op = ParseOp(command);

    if (op == NULL) return NULL;

    if (StrCmp(op, "add"))
    {
        free(op);
        return "@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nM=D+M\n";
    }
    if (StrCmp(op, "sub"))
    {
        free(op);
        return "@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nM=M-D\n";
    }
    if (StrCmp(op, "neg"))
    {
        free(op);
        return "@SP\nA=M-1\nM=-M\n";
    }
    if (StrCmp(op, "eq"))
    {
        free(op);
        return "@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nD=D-M\n@EQ1\nD;JEQ\n@SP\nA=M-1\nM=0\n@END1\n0;JMP\n(EQ1)\n@SP\nA=M-1\nM=1\n(END1)\n";
    }
    if (StrCmp(op, "gt"))
    {
        free(op);
        return "@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nD=M-D\n@GT1\nD;JGT\n@SP\nA=M-1\nM=0\n@END1\n0;JMP\n(GT1)\n@SP\nA=M-1\nM=1\n(END1)\n";
    }
    if (StrCmp(op, "lt"))
    {
        free(op);
        return "@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nD=M-D\n@GT1\nD;JLT\n@SP\nA=M-1\nM=0\n@END1\n0;JMP\n(GT1)\n@SP\nA=M-1\nM=1\n(END1)\n";
    }
    if (StrCmp(op, "and"))
    {
        free(op);
        return "@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nM=D&M\n";
    }
    if (StrCmp(op, "or"))
    {
        free(op);
        return "@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nM=D|M\n";
    }
    if (StrCmp(op, "not"))
    {
        free(op);
        return "@SP\nA=M-1\nM=!M\n";
    }
}
