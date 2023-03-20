#include <stdio.h>
#include <string.h>
# include <ctype.h>

int first_occurrence_index(const char text[100], char target) {
    for (int i = 0; i < strlen(text); ++i) {
        if (text[i] == target) {
            return i;
        }
    }
    return strlen(text);
}

int is_section_header(const char text[100]) {
    return text[0] == '[' && text[strlen(text) - 1];
}

int is_key_value_pair(const char line[100]) {
    return first_occurrence_index(line, '=') != strlen(line);
}

int is_numerical(const char text[100]) {
    for (int i = 0; i < strlen(text); ++ i) {
        if (!isdigit(text[i])) {
            return 0;
        }
    }
    return 1;
}

int is_valid_identifier(const char text[100]) {
    for (int i = 0; i < strlen(text); ++i) {
        char c = text[i];
        if (!isalnum(c) && c != '-') {
            return 0;
        }
    }
    return 1;
}

struct section {
    char key_list[100][100];
    char value_list[100][100];
};


int main(int argc, char *argv[]) {
    char filename[100], selector[100], selector_header_name[100], selector_key[100];
    strcpy(filename, argv[1]);
    strcpy(selector, argv[2]);
    printf("File: %s\n", filename, selector);

    int selector_separator_index = first_occurrence_index(selector, '.');
    strncpy(selector_header_name, selector, selector_separator_index);
    selector_header_name[selector_separator_index] = '\0';
    int characters_to_select_after_separator = strlen(selector) - selector_separator_index - 1;
    strncpy(selector_key, selector + selector_separator_index + 1, characters_to_select_after_separator);
    selector_key[characters_to_select_after_separator] = '\0';

    printf("Selector: %s\nSelector Header: %s\nSelector Key: %s\n\n", selector, selector_header_name, selector_key);

    FILE *file;
    file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file.");
        return 1;
    }

    char line[100];

    char current_header_name[100];
    char key[100];
    char value[100];

    int i;
    int selection_found_flag = 0;

    struct section section_list[100];
    int section_list_current_index = -1;

    char current_key_list[100][100];
    int key_list_current_index = 0;
    char current_value_list[100][100];
    int value_list_current_index = 0;

    while (fgets(line, sizeof(line), file)) {
//        printf("%s", line);

        if (is_section_header(line)) {
            if (section_list_current_index >= 0) {
                struct section current_section;
                memcpy(current_section.key_list, current_key_list, 100 * 100 * sizeof(char));
                memcpy(current_section.value_list,  current_value_list, 100 * 100 * sizeof(char));
                section_list[section_list_current_index] = current_section;

                key_list_current_index = 0;
                value_list_current_index = 0;
            }
            ++section_list_current_index;

            strcpy(current_header_name, "");
            for (i = 1; i < strlen(line) - 1; ++i) {
                current_header_name[i - 1] = line[i];
            }
            current_header_name[i - 2] = '\0';
            continue;
        }

        int equal_sign_index = first_occurrence_index(line, '=');
        if (is_key_value_pair(line)) {
            strcpy(key, "");
            strcpy(value, "");
            for (i = 0; i < equal_sign_index - 1; ++i) {
                key[i] = line[i];
            }
            key[i] = '\0';
            for (i = 0; i < strlen(line) - (equal_sign_index + 2); ++i) {
                value[i] = line[i + equal_sign_index + 2];
            }
            value[i] = '\0';

//            printf("Line: %sKey: %s\nValue: %s\n\n", line, key, value);
//            printf("Current: %s Selector: %s\n", current_header_name, selector_header_name);
            if (strcmp(current_header_name, selector_header_name) == 0) {
                selection_found_flag = 1;
                if (strcmp(key, selector_key) == 0) {
                    printf("Value found: %s\n", value);
                    return 0;
                }
            }
        }
    }
    fclose(file);

    if (!selection_found_flag) {
        printf("Failed to find section %s", selector_header_name);
        return 1;
    } else {
        printf("Failed to find key %s in section %s", selector_key, selector_header_name);
        return 1;
    }
}
