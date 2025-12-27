#ifndef CS50_H
#define CS50_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <float.h>

// String type
typedef char *string;

// Input functions
string get_string(const char *format, ...);
char get_char(const char *format, ...);
int get_int(const char *format, ...);
long get_long(const char *format, ...);
float get_float(const char *format, ...);
double get_double(const char *format, ...);

#ifdef CS50_IMPLEMENTATION

#include <stdarg.h>

// Buffer for reading input
#define CS50_BUFFER_SIZE 1024

static void print_prompt(const char *format, va_list args) {
    if (format != NULL) {
        vprintf(format, args);
    }
}

string get_string(const char *format, ...) {
    va_list args;
    va_start(args, format);
    print_prompt(format, args);
    va_end(args);

    char *buffer = NULL;
    size_t capacity = 0;
    size_t size = 0;
    int c;

    while ((c = fgetc(stdin)) != '\n' && c != EOF) {
        if (size + 1 >= capacity) {
            capacity = capacity == 0 ? 32 : capacity * 2;
            char *temp = realloc(buffer, capacity);
            if (temp == NULL) {
                free(buffer);
                return NULL;
            }
            buffer = temp;
        }
        buffer[size++] = (char)c;
    }

    if (size == 0 && c == EOF) {
        free(buffer);
        return NULL;
    }

    char *result = realloc(buffer, size + 1);
    if (result == NULL) {
        free(buffer);
        return NULL;
    }
    result[size] = '\0';
    return result;
}

char get_char(const char *format, ...) {
    while (1) {
        va_list args;
        va_start(args, format);
        print_prompt(format, args);
        va_end(args);

        string line = get_string(NULL);
        if (line == NULL) {
            return '\0';
        }
        if (strlen(line) == 1) {
            char c = line[0];
            free(line);
            return c;
        }
        free(line);
        printf("Retry: ");
    }
}

int get_int(const char *format, ...) {
    while (1) {
        va_list args;
        va_start(args, format);
        print_prompt(format, args);
        va_end(args);

        string line = get_string(NULL);
        if (line == NULL) {
            return INT_MAX;
        }

        char *endptr;
        long n = strtol(line, &endptr, 10);
        if (*endptr == '\0' && n >= INT_MIN && n <= INT_MAX) {
            free(line);
            return (int)n;
        }
        free(line);
        printf("Retry: ");
    }
}

long get_long(const char *format, ...) {
    while (1) {
        va_list args;
        va_start(args, format);
        print_prompt(format, args);
        va_end(args);

        string line = get_string(NULL);
        if (line == NULL) {
            return LONG_MAX;
        }

        char *endptr;
        long n = strtol(line, &endptr, 10);
        if (*endptr == '\0') {
            free(line);
            return n;
        }
        free(line);
        printf("Retry: ");
    }
}

float get_float(const char *format, ...) {
    while (1) {
        va_list args;
        va_start(args, format);
        print_prompt(format, args);
        va_end(args);

        string line = get_string(NULL);
        if (line == NULL) {
            return FLT_MAX;
        }

        char *endptr;
        float f = strtof(line, &endptr);
        if (*endptr == '\0') {
            free(line);
            return f;
        }
        free(line);
        printf("Retry: ");
    }
}

double get_double(const char *format, ...) {
    while (1) {
        va_list args;
        va_start(args, format);
        print_prompt(format, args);
        va_end(args);

        string line = get_string(NULL);
        if (line == NULL) {
            return DBL_MAX;
        }

        char *endptr;
        double d = strtod(line, &endptr);
        if (*endptr == '\0') {
            free(line);
            return d;
        }
        free(line);
        printf("Retry: ");
    }
}

#endif // CS50_IMPLEMENTATION

#endif // CS50_H
