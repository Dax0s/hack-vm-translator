#ifndef UTILS_H
#define UTILS_H

int StrCmp(const char *s1, const char *s2);
int StrLen(const char *s);
char* Clean(const char *s);

/**
 * Parses int from string
 * @param s string to pass the int from
 * @param parsedInt variable to store parsed int
 * @return 0 - parsing failed 1 - parsing succeeded
 */
int ParseInt(const char *s, int* parsedInt);
int IntLength(int value);

#endif //UTILS_H
