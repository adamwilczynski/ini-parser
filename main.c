#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

char c;
int i, start_size, separator_index;

struct {
    char *char_array;
    int size;
} typedef str;

bool is_string_numerical(str s) {
    for (i = 0; i < s.size - 1; ++i) {
        if (!isdigit(s.char_array[i])) {
            return false;
        }
    }
    return true;
}

bool is_proper_identifier(str s) {
    for (i = 0; i < s.size - 1; ++i) {
        if (!isalnum(s.char_array[i]) & s.char_array[i] != '-') {
            return false;
        }
    }
    return true;
}

int first_index(const str s, const char target) {
    for (i = 0; i < strlen(s.char_array); ++i) {
        if (s.char_array[i] == target) {
            return i;
        }
    }
    return s.size;
}

bool is_key_value_pair(const str line) {
    return first_index(line, '=') != line.size;
};

str extract_str(str s, int start, int end) {
    str result;
    result.size = end - start + 1;
    result.char_array = malloc(sizeof(char) * result.size);  // on assumption that a space separates '=' from both sides
    strncpy(result.char_array, s.char_array + start, result.size - 1);
    result.char_array[result.size - 1] = '\0';
    return result;
}

bool is_header(const str line) {
    return line.char_array[0] == '[';
}

str get_header(const str line) {
    str header;
    header.size = strlen(line.char_array) - 2 + 1;
    header.char_array = malloc(sizeof(char) * header.size);

    for (i = 1; i < strlen(line.char_array) - 1; ++i) {
        header.char_array[i - 1] = line.char_array[i];
    }
    header.char_array[i - 1] = '\0';
    return header;
}

struct {
    str section;
    str key;
    str value;
} typedef item;

void print_item(const item item_to_print) {
    printf("[%s] %s = %s\n", item_to_print.section.char_array, item_to_print.key.char_array, item_to_print.value.char_array);
}

struct {
    item *item_array;
    int size;
} typedef dynamic_item_array;

int find_first_item_with_section(dynamic_item_array database, str section) {
    item current_item;
    for (i = 0; i < database.size; ++i) {
        current_item = database.item_array[i];
        if (strcmp(current_item.section.char_array, section.char_array) == 0) {
            return i;
        }
    }
    return database.size;
}

int find_item_index_by_key_and_value(dynamic_item_array database, int section_start_index, str key) {
    item current_item = database.item_array[section_start_index];
    str start_section = current_item.section;
    for (i = section_start_index; i < database.size & strcmp(current_item.section.char_array, start_section.char_array) == 0; ++i) {
        current_item = database.item_array[i];
        if (strcmp(current_item.key.char_array, key.char_array) == 0) {
            return i;
        }
    }
    return database.size;
}

int find_item_index(str selector, dynamic_item_array database) {
    separator_index = first_index(selector, '.');
    str section = extract_str(selector, 0, separator_index);
    str key = extract_str(selector, separator_index + 1, selector.size);

    int section_index = find_first_item_with_section(database, section);
    if (section_index == database.size) {
        printf("Failed to find section [%s]\n", section.char_array);
        return -1;
    }

    int item_index = find_item_index_by_key_and_value(database, section_index, key);
    if (item_index == database.size) {
        printf("Failed to find key \"%s\" in section [%s]\n", key.char_array, section.char_array);
        return -2;
    }
    return item_index;
}

str process_expression(str a, str b, char operator) {
    str result;

    if (is_string_numerical(a) & is_string_numerical(b)) {
        long long int a_numerical, b_numerical;
        sscanf(a.char_array, "%lld", &a_numerical);
        sscanf(b.char_array, "%lld", &b_numerical);
        switch (operator) {
            case '+':
                sprintf(result.char_array, "%lli", a_numerical + b_numerical);
                break;
            case '-':
                sprintf(result.char_array, "%lli", a_numerical - b_numerical);
                break;
            case '*':
                sprintf(result.char_array, "%lli", a_numerical * b_numerical);
                break;
            case '/':
                sprintf(result.char_array, "%lli", a_numerical / b_numerical);
                break;
            default:
                printf("Invalid operator %c", operator);
                result.size = -1;
                return result;
        }
        result.size = strlen(result.char_array);
        return result;
    }

    if ((operator == '+') & !is_string_numerical(a) & !is_string_numerical(b)) {
        result.size = a.size + b.size - 1;
        result.char_array = malloc(result.size);
        strncpy(result.char_array , a.char_array, a.size - 1);
        strncpy(result.char_array + a.size - 1, b.char_array, b.size);
        return result;
    }

    printf("Invalid operation: %s %c %s", a.char_array, operator, b.char_array);
}


int main(int argc, char *argv[]) {
    FILE *file;
    file = fopen(argv[1], "r");

    if (file == NULL) {
        printf("Error occurred when opening a file: %s", argv[1]);
        return 1;
    }

    str line;
    start_size = 1;
    line.char_array = malloc(sizeof(char) * start_size);
    line.char_array[0] = '\0';
    line.size = start_size;

    dynamic_item_array database;
    database.item_array = malloc(sizeof(item));
    database.size = 0;

    str current_header;
    item current_line_item;
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
//            printf("%d %s\n", is_header(line), line.char_array);
            if (is_header(line)) {
                current_header = get_header(line);
            } else if (is_key_value_pair(line)) {
                separator_index = first_index(line, '=');
                current_line_item.key = extract_str(line, 0, first_index(line, '=') - 1);
                current_line_item.value = extract_str(line, separator_index + 2, strlen(line.char_array));
                current_line_item.section = current_header;

                if (!is_proper_identifier(current_line_item.key)) {
                    printf("%s is an invalid identifier", current_line_item.key.char_array);
                    return -1;
                } else if (!is_proper_identifier(current_line_item.value)) {
                    printf("%s is an invalid identifier", current_line_item.value.char_array);
                    return -1;
                } else if (!is_proper_identifier(current_line_item.section)) {
                    printf("%s is an invalid identifier", current_line_item.section.char_array);
                    return -1;
                }

                database.item_array = realloc(database.item_array, sizeof(item) * ++database.size);
                database.item_array[database.size - 1] = current_line_item;
            }

//            printf("%s\n",line.char_array);
            line.size = 1;
            line.char_array[line.size - 1] = '\0';
        } else if (strlen(line.char_array) + 2 >= line.size) {
            line.char_array = realloc(line.char_array, sizeof(char) * ++line.size);
            line.char_array[line.size - 2] = c;
            line.char_array[line.size - 1] = '\0';
        }
    }

    if (strcmp(argv[2], "expression") != 0) {
        // Search Query
        str selector;
        selector.char_array = argv[2];
        selector.size = strlen(selector.char_array) + 1;

        int item_index = find_item_index(selector, database);
        if (item_index < 0) {
            return -1;
        }
        print_item(database.item_array[item_index]);
    } else {
        // Query
        str query, a, b;
        query.char_array = argv[3];
        query.size = strlen(argv[3]);

        separator_index = first_index(query, ' ');
        if (separator_index == query.size) {
            printf("Invalid expression");
            return -1;
        }

        a = extract_str(query, 0, separator_index);
        char operator = query.char_array[separator_index + 1];
        b = extract_str(query, separator_index + 3, query.size);

        int a_index, b_index;
        a_index = find_item_index(a, database);
        b_index = find_item_index(b, database);
        if (a_index < 0 | b_index < 0) {
            return -1;
        }

        str a_value, b_value;
        a_value = database.item_array[a_index].value;
        b_value = database.item_array[b_index].value;
        str result = process_expression(a_value, b_value, operator);
        if (result.size < 0) {
            return -1;
        }
        printf("%s %c %s = %s", a_value.char_array, operator, b_value.char_array, result.char_array);
    }



    return 0;
}
