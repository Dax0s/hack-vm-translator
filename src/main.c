#include <stdio.h>
#include <stdlib.h>

#include "Parser.h"
#include "Utils.h"

/**
 * add
 * @SP
 * AM=M-1
 * D=M
 * @SP
 * AM=M-1
 * M=D+M
 *
 * @SP
 * M=M+1
 *
 * sub
 * neg
 * eq
 * gt
 * lt
 * and
 * or
 * not
 * pop segment i
 * push segment i
 * push pointer 0/1 -> *SP = THIS/THAT, SP++
 * [local, argument, this, that] -> [LCL, ARG, THIS, THAT]
 */
int main(void) {
    const char* command = "add local 2";
    char* cmd = ParseCommand(command);
    char* segment = ParseSegment(command);
    char* value = ParseValue(command);
    printf("%s %s %s\n", cmd, segment, value);

    free(cmd);
    return 0;
}