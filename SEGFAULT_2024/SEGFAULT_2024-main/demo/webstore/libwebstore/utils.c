#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "utils.h"

#include "../../../src/refmem.h"

// To address a current bug with the refmem implementation as it
// doesnt check if the passed destructor function is a null pointer
static void dummy_destructor(__attribute_maybe_unused__ obj *o) {}

void print(char *str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        putchar(str[i]);
        i++;
    }
}

void println(char *str)
{
    print(str);
    print("\n");
}

bool not_empty(char *str)
{
    return strlen(str) > 0;
}

bool is_number(char *str)
{
    int len = strlen(str);
    int s_index = 0;
    bool is_num = false;

    if (str[0] == 45 && len > 1)
    {
        s_index = 1;
    }

    for (int i = s_index; i < len; i++)
    {
        char current = str[i];

        if (isdigit(current))
        {
            is_num = true;
        }
        else
        {
            is_num = false;
            break;
        }
    }
    return is_num;
}

static void clear_input_buffer()
{
    char c;
    do
    {
        c = getchar();

    } while (c != '\n' && c != EOF);
}

int read_string(char *buf, int buf_size)
{
    int i = 0;
    char c = getchar();

    while (c != '\n' && i < buf_size)
    {
        buf[i] = c;
        c = getchar();
        i++;
    }

    buf[i] = '\0';

    if (i >= buf_size)
    {
        clear_input_buffer();
    }

    return i;
}

static bool is_shelf(char *str)
{
    int len = strlen(str);
    int s_index = 0;
    bool is_shelf = false;
    while (s_index == 0)
    {
        if (str[0] >= 65 && str[0] <= 90)
        {
            s_index = 1;
        }
        else
        {
            printf("First char has to be [A-Z]\n");
            return is_shelf;
        }
    }
    for (int i = s_index; i < len; i++)
    {
        char current = str[i];

        if (isdigit(current))
        {
            is_shelf = true;
        }
        else
        {
            printf("Second char has to be [0-9]\n");
            is_shelf = false;
            break;
        }
    }

    return is_shelf;
}

static bool is_menu_option(char *input, char *valids)
{
    char *valid = valids;

    while (*valid && strlen(input) == 1)
    {
        if (*valid == *input)
        {
            return true;
        }
        valid++;
    }
    return false;
}

static answer_t ask_question(char *question, check_func check, convert_func convert)
{
    int buf_size = 255;
    char buf[buf_size];

    do
    {
        printf("%s", question);
        read_string(buf, buf_size);

    } while (!check(buf));

    return convert(buf);
}

static answer_t ascii_to_int(char *str)
{
    answer_t res;
    res.int_value = atoi(str);
    return res;
}

int ask_question_int(char *question)
{
    answer_t answer = ask_question(question, is_number, (convert_func) ascii_to_int);
    return answer.int_value; // svaret som ett heltal
}

char *duplicate_string(char *str)
{
    int len = strlen(str) + 1;

    char *new_str = allocate(len, dummy_destructor);
    if (!new_str)
    {
        return "";
    }
    retain(new_str);

    for (int i = 0; i < len; ++i)
    {
        new_str[i] = str[i];
    }

    return new_str;
}

static answer_t duplicate_string_answer(char *str) {
    answer_t res;
    res.string_value = duplicate_string(str);
    return res;
}

char *ask_question_string(char *question)
{
    return ask_question(question, not_empty, (convert_func) duplicate_string_answer).string_value;
}

char *ask_question_shelf(char *question)
{
    return ask_question(question, is_shelf, (convert_func) duplicate_string_answer).string_value;
}

char ask_question_menu(char *question, char *valids)
{
    char *str = ask_question(question, not_empty, (convert_func) duplicate_string_answer).string_value;
    if (is_menu_option(str, valids))
    {
        char res = *str;
        release(str);
        return toupper(res);
    }
    release(str);
    return ask_question_menu(question, valids);
}

bool ask_confirm(char *question, char a)
{
    char *result = ask_question_string(question);
    char confirm = toupper(*result);
    // printf("%c", confirm);

    a = toupper(a);
    if (a == confirm)
    {
        release(result);
        return true;
    }
    release(result);
    return false;
}
