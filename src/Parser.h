#ifndef PARSER_H
#define PARSER_H

char* ParseNWord(const char* str, int n);
char* ParseCommand(const char* command);
char* ParseSegment(const char* command);
char* ParseValue(const char* command);

#endif //PARSER_H
