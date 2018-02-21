/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

/* Compare two strings ignoring case.
 * strcasecmp is not in the C standard. However, it's defined by
 * 4.4BSD, POSIX.1-2001. So we use our own
 */
int strcmp_nocase(const char *s1, const char *s2)
{
    const unsigned char *us1 = (const unsigned char *)s1,
            *us2 = (const unsigned char *)s2;

    while (tolower(*us1) == tolower(*us2++)) {
        if (*us1++ == '\0')
            return (0);
    }
    return (tolower(*us1) - tolower(*--us2));
}

/* Strip whitespace from the end of a string */
void rtrim(char* s)
{
    size_t len = 0;
    if (!s) return;
    len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1]))
        len--;
    s[len] = '\0';
}

/* Return the component after final slash */
/*  "/tmp/x"  -> "x"  */
/*  "/tmp/"   -> ""   */
const char* extract_filename(const char* filepath)
{
    const char* s = strrchr(filepath, get_dir_separator_char());
    if (!s) return filepath;
    else    return s + 1;
}

/* Returns an array of strings the last of which is NULL */
char** string_split(char* inputString, const char* delimiter)
{
    char** result = NULL;
    char* p = inputString;
    char* lastDelimiter = NULL;
    char* aToken = NULL;
    size_t numTokens = 0;
    size_t strLength = 0;
    size_t index = 0;
    char delimiterChar = 0;

    DebugAssert(inputString);
    DebugAssert( delimiter && (strlen(delimiter)==1) );
    delimiterChar = delimiter[0];
    while (*p) {
        const char ctmp = *p;
        if (ctmp == delimiterChar) {
            ++numTokens;
            lastDelimiter = p;
        }
        p++;
    }
    strLength = strlen(inputString);
    if (lastDelimiter < (inputString + strLength - 1)) {
        ++numTokens; /* there is a trailing token */
    }
    ++numTokens; /* terminating NULL string to mark the end */

    result = (char**)malloc(numTokens * sizeof(char*));
    Assert(result);

    /* Start tokenizing */
    aToken = strtok(inputString, delimiter);
    while (aToken) {
        Assert(index < numTokens);
        *(result + index++) = strdup(aToken);
        aToken = strtok(NULL, delimiter);
    }
    Assert(index == numTokens - 1);
    *(result + index) = NULL;

    return result;
}

/* Return 0 if can convert input to an integer, 1 otherwise */
int string_to_long(const char* input, long* output)
{
    const int base = 10;
    char *endptr;
    errno = 0;
    long val = 0;

    if (!input) return 1;

    val = strtol(input, &endptr, base);
    if ( (errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) ||
         (errno != 0 && val == 0) )
    {
        //perror("strtol");
        return 1;
    }
    if (endptr == input) {
        //fprintf(stderr, "No digits were found. EXIT_FAILURE\n");
        return 1;
    }
    *output = val;
    return 0; // OK
}
