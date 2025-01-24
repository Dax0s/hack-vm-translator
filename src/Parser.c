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

char* ParseArithmeticCommand(const char* command, int* eqNum, int* gtNum, int* ltNum)
{
    const char* op = ParseOp(command);
    if (op == NULL) return NULL;

    if (StrCmp(op, "add"))
    {
        const char* tmp = "@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nM=D+M\n";
        char* p = malloc((StrLen(tmp) + 1) * sizeof(char));

        sprintf(p, "%s", tmp);

        free((void*) op);
        return p;
    }
    if (StrCmp(op, "sub"))
    {
        const char* tmp = "@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nM=M-D\n";
        char* p = malloc((StrLen(tmp) + 1) * sizeof(char));

        sprintf(p, "%s", tmp);

        free((void*) op);
        return p;
    }
    if (StrCmp(op, "neg"))
    {
        const char* tmp = "@SP\nA=M-1\nM=-M\n";
        char* p = malloc((StrLen(tmp) + 1) * sizeof(char));

        sprintf(p, "%s", tmp);

        free((void*) op);
        return p;
    }
    if (StrCmp(op, "eq"))
    {
        const char* tmp = "@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nD=D-M\n@EQ%d\nD;JEQ\n@SP\nA=M-1\nM=0\n@END_EQ%d\n0;JMP\n(EQ%d)\n@SP\nA=M-1\nM=-1\n(END_EQ%d)\n";
        char* p = malloc((StrLen(tmp) - 8 + IntLength(*eqNum) + 1) * sizeof(char));

        sprintf(p, tmp, *eqNum, *eqNum, *eqNum, *eqNum);
        (*eqNum)++;

        free((void*) op);
        return p;
    }
    if (StrCmp(op, "gt"))
    {
        const char* tmp = "@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nD=M-D\n@GT%d\nD;JGT\n@SP\nA=M-1\nM=0\n@END_GT%d\n0;JMP\n(GT%d)\n@SP\nA=M-1\nM=-1\n(END_GT%d)\n";
        char* p = malloc((StrLen(tmp) - 8 + IntLength(*gtNum) + 1) * sizeof(char));

        sprintf(p, tmp, *gtNum, *gtNum, *gtNum, *gtNum);
        (*gtNum)++;

        free((void*) op);
        return p;
    }
    if (StrCmp(op, "lt"))
    {
        const char* tmp = "@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nD=M-D\n@LT%d\nD;JLT\n@SP\nA=M-1\nM=0\n@END_LT%d\n0;JMP\n(LT%d)\n@SP\nA=M-1\nM=-1\n(END_LT%d)\n";
        char* p = malloc((StrLen(tmp) - 8 + IntLength(*ltNum) + 1) * sizeof(char));

        sprintf(p, tmp, *ltNum, *ltNum, *ltNum, *ltNum);
        (*ltNum)++;

        free((void*) op);
        return p;
    }
    if (StrCmp(op, "and"))
    {
        const char* tmp = "@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nM=D&M\n";
        char* p = malloc((StrLen(tmp) + 1) * sizeof(char));

        sprintf(p, "%s", tmp);

        free((void*) op);
        return p;
    }
    if (StrCmp(op, "or"))
    {
        const char* tmp = "@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nM=D|M\n";
        char* p = malloc((StrLen(tmp) + 1) * sizeof(char));

        sprintf(p, "%s", tmp);

        free((void*) op);
        return p;
    }
    if (StrCmp(op, "not"))
    {
        const char* tmp = "@SP\nA=M-1\nM=!M\n";
        char* p = malloc((StrLen(tmp) + 1) * sizeof(char));

        sprintf(p, "%s", tmp);

        free((void*) op);
        return p;
    }

    free((void*) op);
    return NULL;
}

char* PushSegmentString(const char* segment, const char* value)
{
    const char* tmp = "@%s\nD=M\n@%s\nD=D+A\nA=D\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
    char* p = malloc((StrLen(value) + StrLen(tmp) - 4 + StrLen(segment) + 1) * sizeof(char));

    sprintf(p, tmp, segment, value);
    free((void*) value);
    return p;
}

char* PopSegmentString(const char* segment, const char* value)
{
    const char* tmp = "@%s\nD=M\n@%s\nD=D+A\n@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n";
    char* p = malloc((StrLen(value) + StrLen(tmp) - 4 + StrLen(segment) + 1) * sizeof(char));

    sprintf(p, tmp, segment, value);
    free((void*) value);
    return p;
}

char* ParsePushCommand(const char* command, const char* filename, const int n)
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
        const char* tmp = "@%s\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
        char* p = malloc((StrLen(tmp) - 2 + StrLen(value) + 1) * sizeof(char));
        sprintf(p, tmp, value);

        free((void*) segment);
        free((void*) value);

        return p;
    }
    if (StrCmp(segment, "local"))
    {
        free((void*) segment);
        return PushSegmentString("LCL", value);
    }
    if (StrCmp(segment, "argument"))
    {
        free((void*) segment);
        return PushSegmentString("ARG", value);
    }
    if (StrCmp(segment, "this"))
    {
        free((void*) segment);
        return PushSegmentString("THIS", value);
    }
    if (StrCmp(segment, "that"))
    {
        free((void*) segment);
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

        free((void*) segment);
        free((void*) value);
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

        free((void*) segment);
        free((void*) value);
        return p;
    }
    if (StrCmp(segment, "static"))
    {
        const char* tmp = "@%s.%s\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
        const char* filenameWithoutExtension = FilenameWithoutExtension(filename);
        char* p = malloc((StrLen(tmp) - 4 + StrLen(filenameWithoutExtension) + StrLen(value) + 1) * sizeof(char));

        sprintf(p, tmp, filenameWithoutExtension, value);

        free((void*) segment);
        free((void*) value);
        free((void*) filenameWithoutExtension);
        return p;
    }

    free((void*) segment);
    free((void*) value);
    fprintf(stderr, "Unrecognized push command '%s' in line %d\n", command, n);
    exit(1);
}

char* ParsePopCommand(const char* command, const char* filename, const int n)
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
        free((void*) segment);
        return PopSegmentString("LCL", value);
    }
    if (StrCmp(segment, "argument"))
    {
        free((void*) segment);
        return PopSegmentString("ARG", value);
    }
    if (StrCmp(segment, "this"))
    {
        free((void*) segment);
        return PopSegmentString("THIS", value);
    }
    if (StrCmp(segment, "that"))
    {
        free((void*) segment);
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

        free((void*) segment);
        free((void*) value);
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

        free((void*) segment);
        free((void*) value);
        return p;
    }
    if (StrCmp(segment, "static"))
    {
        const char* tmp = "@SP\nAM=M-1\nD=M\n@%s.%s\nM=D\n";
        const char* filenameWithoutExtension = FilenameWithoutExtension(filename);
        char* p = malloc((StrLen(tmp) - 4 + StrLen(filenameWithoutExtension) + StrLen(value) + 1) * sizeof(char));

        sprintf(p, tmp, filenameWithoutExtension, value);

        free((void*) segment);
        free((void*) value);
        free((void*) filenameWithoutExtension);
        return p;
    }

    free((void*) segment);
    free((void*) value);
    fprintf(stderr, "Unrecognized pop command '%s' in line %d\n", command, n);
    exit(1);
}

char* ParseBranchingCommand(const char* command)
{
    const char* op = ParseOp(command);
    if (op == NULL) return NULL;

    const char* label = ParseNWord(command, 2);
    if (label == NULL) return NULL;

    char* p = NULL;
    if (StrCmp(op, "label"))
    {
        const char* tmp = "(%s)\n";
        p = malloc((StrLen(tmp) - 2 + StrLen(label) + 1) * sizeof(char));

        sprintf(p, tmp, label);

    }
    else if (StrCmp(op, "goto"))
    {
        const char* tmp = "@%s\n0;JMP\n";
        p = malloc((StrLen(tmp) - 2 + StrLen(label) + 1) * sizeof(char));

        sprintf(p, tmp, label);
    }
    else if (StrCmp(op, "if-goto"))
    {
        const char* tmp = "@SP\nAM=M-1\nD=M\n@%s\nD;JNE\n";
        p = malloc((StrLen(tmp) - 2 + StrLen(label) + 1) * sizeof(char));

        sprintf(p, tmp, label);
    }

    free((void*) op);
    free((void*) label);
    return p;
}

char* ParseCommand(const char* command, const char* filename, const int lineNum, int* eqNum, int* gtNum, int* ltNum)
{
    char* parsedArithmeticCommand = ParseArithmeticCommand(command, eqNum, gtNum, ltNum);
    if (parsedArithmeticCommand != NULL)
    {
        return parsedArithmeticCommand;
    }

    char* parsedBranchingCommand = ParseBranchingCommand(command);
    if (parsedBranchingCommand != NULL)
    {
        return parsedBranchingCommand;
    }

    const char* op = ParseOp(command);
    if (op == NULL) return NULL;
    if (StrCmp(op, "push"))
    {
        free((void*) op);

        char* p = ParsePushCommand(command, filename, lineNum);
        if (p == NULL) return NULL;

        return p;
    }

    if (StrCmp(op, "pop"))
    {
        free((void*) op);

        char* p = ParsePopCommand(command, filename, lineNum);
        if (p == NULL) return NULL;

        return p;

    }

    free((void*) op);
    fprintf(stderr, "Unrecognized command '%s' in line %d\n", command, lineNum);
    exit(1);
}
