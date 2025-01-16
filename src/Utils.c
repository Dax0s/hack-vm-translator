#include "Utils.h"

#include <stdio.h>
#include <stdlib.h>

int StrCmp(const char *s1, const char *s2)
{
    if (s1 == NULL || s2 == NULL) return 0;

    int i = 0;
    while (s1[i] != '\0')
    {
        if (s1[i] != s2[i]) return 0;
        i++;
    }

    if (s1[i] == s2[i]) return 1;
    return 0;
}

int StrLen(const char *s)
{
    if (s == NULL) return 0;
    int i;
    for (i = 0; s[i] != '\0'; i++) {}
    return i;
}

char* Clean(const char *s)
{
    if (s == NULL) return NULL;

    const int size = StrLen(s);
    char* p = malloc((size + 1) * sizeof(char));
    if (p == NULL) return NULL;

    int j = 0;
    for (int i = 0; i < size; i++)
    {
        if (s[i] != ' ' && s[i] != '\t' && s[i] != '\n' && s[i] != '\r')
        {
            p[j++] = s[i];
        }
    }
    p[j] = '\0';

    if (j == 0)
    {
        free(p);
        return NULL;
    }

    char* pNew = realloc(p, (j + 1) * sizeof(char));
    if (pNew == NULL)
    {
        free(p);
        return NULL;
    }

    return pNew;
}
