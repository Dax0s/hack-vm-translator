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
    char* p = ParseNWord(command, 3);
    if (p == NULL) return NULL;

    if (ParseInt(p, NULL)) return p;

    return p;
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

char* PushSegmentString(const char* segment, const char* value)
{
    const char* tmp = "@%s\nD=M\n@%s\nD=D+A\nA=D\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
    char* p = malloc((StrLen(value) + StrLen(tmp) - 4 + StrLen(segment) + 1) * sizeof(char));

    sprintf(p, tmp, segment, value);
    return p;
}

char* PopSegmentString(const char* segment, const char* value)
{
    const char* tmp = "@%s\nD=M\n@%s\nD=D+A\n@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n";
    char* p = malloc((StrLen(value) + StrLen(tmp) - 4 + StrLen(segment) + 1) * sizeof(char));

    sprintf(p, tmp, segment, value);
    return p;
}

char* ParsePushCommand(const char* command, const int n)
{
    const char* segment = ParseSegment(command);
    if (segment == NULL)
    {
        fprintf(stderr, "Segment is missing from push command in line %d\n", n);
        exit(1);
    }
    const char* value = ParseValue(command);
    if (value == NULL)
    {
        free((void*) segment);
        fprintf(stderr, "Number is missing from push command in line %d\n", n);
        exit(1);
    }

    if (StrCmp(segment, "constant"))
    {
        // 28 is the lenght of the asm command
        char* p = malloc((StrLen(value) + 28 + 1) * sizeof(char));
        sprintf(p, "@%s\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", value);

        free((void*) segment);
        free((void*) value);

        return p;
    }
    if (StrCmp(segment, "local"))
    {
        return PushSegmentString("LCL", value);
    }
    if (StrCmp(segment, "argument"))
    {
        return PushSegmentString("ARG", value);
    }
    if (StrCmp(segment, "this"))
    {
        return PushSegmentString("THIS", value);
    }
    if (StrCmp(segment, "that"))
    {
        return PushSegmentString("THAT", value);
    }
    if (StrCmp(segment, "temp"))
    {
        int num;
        ParseInt(value, &num);
        num += 5;

        const char* tmp = "@%d\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
        char* p = malloc((StrLen(tmp) - 2 + IntLength(num) + 1) * sizeof(char));

        sprintf(p, tmp, num);
        return p;
    }
    if (StrCmp(segment, "pointer"))
    {
        const char* tmp = "@%s\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
        char* p = malloc((StrLen(tmp) - 2 + 4 + 1) * sizeof(char));
        if (StrCmp(value, "0"))
        {
            sprintf(p, tmp, "THIS");
        }
        else
        {
            sprintf(p, tmp, "THAT");
        }

        return p;
    }
    if (StrCmp(segment, "static"))
    {

    }

    free((void*) segment);
    free((void*) value);
    fprintf(stderr, "Unrecognized push command '%s' in line %d\n", command, n);
    exit(1);
}

char* ParsePopCommand(const char* command, const int n)
{
    const char* segment = ParseSegment(command);
    if (segment == NULL)
    {
        fprintf(stderr, "Segment is missing from pop command in line %d\n", n);
        exit(1);
    }
    const char* value = ParseValue(command);
    if (value == NULL)
    {
        free((void*) segment);
        fprintf(stderr, "Number is missing from pop command in line %d\n", n);
        exit(1);
    }

    if (StrCmp(segment, "local"))
    {
        return PopSegmentString("LCL", value);
    }
    if (StrCmp(segment, "argument"))
    {
        return PopSegmentString("ARG", value);
    }
    if (StrCmp(segment, "this"))
    {
        return PopSegmentString("THIS", value);
    }
    if (StrCmp(segment, "that"))
    {
        return PopSegmentString("THAT", value);
    }
    if (StrCmp(segment, "temp"))
    {
        int num;
        ParseInt(value, &num);
        num += 5;

        const char* tmp = "@SP\nAM=M-1\nD=M\n@%d\nM=D\n";
        char* p = malloc((StrLen(tmp) - 2 + IntLength(num) + 1) * sizeof(char));

        sprintf(p, tmp, num);
        return p;
    }
    if (StrCmp(segment, "pointer"))
    {
        const char* tmp = "@SP\nAM=M-1\nD=M\n@%s\nM=D\n";
        char* p = malloc((StrLen(tmp) - 2 + 4 + 1) * sizeof(char));
        if (StrCmp(value, "0"))
        {
            sprintf(p, tmp, "THIS");
        }
        else
        {
            sprintf(p, tmp, "THAT");
        }

        return p;
    }
    if (StrCmp(segment, "static"))
    {

    }

    free((void*) segment);
    free((void*) value);
    fprintf(stderr, "Unrecognized pop command '%s' in line %d\n", command, n);
    exit(1);
}

char* ParseCommand(const char* command, const char* filename, const int lineNum, int* staticNum)
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
        char* p = ParsePushCommand(command, lineNum);
        if (p == NULL) return NULL;

        return p;
    }

    if (StrCmp(op, "pop"))
    {
        char* p = ParsePopCommand(command, lineNum);
        if (p == NULL) return NULL;

        return p;

    }

    free((void*) op);
    fprintf(stderr, "Unrecognized command '%s' in line %d\n", command, lineNum);
    exit(1);
}
