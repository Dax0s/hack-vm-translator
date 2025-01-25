#ifndef PARSER_H
#define PARSER_H

char* ParseNWord(const char* str, int n);

char* ParseOp(const char* command);

char* ParseSegment(const char* command);

char* ParseValue(const char* command);

char* ParseCommand(const char* command, const char* filename, int lineNum, int* eqNum, int* gtNum, int* ltNum, int* returnAddrIndex, char** currentFuction);

#endif //PARSER_H
