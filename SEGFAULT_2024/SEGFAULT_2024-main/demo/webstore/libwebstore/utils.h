#pragma once
#include <stdbool.h>

typedef union
{
    int int_value;
    float float_value;
    char *string_value;
    char char_value;
} answer_t;

typedef bool (*check_func)(char *);
typedef answer_t (*convert_func)(char *);

int read_string(char *buf, int buf_siz);

bool is_number(char *str);

int ask_question_int(char *question);

/**
 * @brief ask the user to enter a string
 * @param question the message to print before reading input
 * @return a null terminated string compatible with the refmem library
 * and one retention that needs to be manually released.
 */
char *ask_question_string(char *question);

/**
 * @brief ask the user to enter a string and also checks that the
 * entered string is a valid self before accepting the input
 * @param question the message to print before reading input
 * @return a null terminated string compatible with the refmem library
 * and one retention that needs to be manually released.
 */
char *ask_question_shelf(char *question);

/**
 * @brief ask the user for a selection of a menu option char
 * @param question the message to print before reading input
 * @return a char with the selected option.
 */
char ask_question_menu(char *question, char *valids);

void print(char *str);

void println(char *str);

bool not_empty(char *str);

bool ask_confirm(char *question, char a);

/**
 * @brief Duplicates a string
 * @param str the string to duplicate
 * @return a refmem compatible heap allocated string with one retention
 * passed on to the caller of this function.
 */
char *duplicate_string(char *str);
