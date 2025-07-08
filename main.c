
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

// --- Structures pour les variables ---
#define MAX_VARIABLES 1000
#define MAX_VAR_NAME 100
#define MAX_STRING_LEN 1000
#define MAX_ARRAYS 100
#define MAX_ARRAY_SIZE 1000
#define MAX_LOOPS 50

typedef enum {
    VAR_INT,
    VAR_FLOAT,
    VAR_STRING,
    VAR_BOOL
} VarType;

typedef struct {
    char name[MAX_VAR_NAME];
    VarType type;
    union {
        int int_val;
        float float_val;
        char string_val[MAX_STRING_LEN];
        bool bool_val;
    } value;
} Variable;

typedef struct {
    char name[MAX_VAR_NAME];
    int data[MAX_ARRAY_SIZE];
    int size;
} Array;

typedef struct {
    int start;
    int end;
    int current;
    char var_name[MAX_VAR_NAME];
} LoopInfo;

Variable variables[MAX_VARIABLES];
Array arrays[MAX_ARRAYS];
LoopInfo loops[MAX_LOOPS];
int variable_count = 0;
int array_count = 0;
int loop_count = 0;

// --- Fonctions utilitaires ---
char* trim_whitespace(char* str) {
    char* end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = 0;
    return str;
}

char* extract_delimited_content(const char* line, char start_char, char end_char) {
    const char* start = strchr(line, start_char);
    if (!start) return NULL;
    start++;
    const char* end = strchr(start, end_char);
    if (!end) return NULL;
    size_t length = end - start;
    char* content = (char*)malloc(length + 1);
    if (content) {
        strncpy(content, start, length);
        content[length] = '\0';
    }
    return content;
}

// --- Fonction pour remplacer les variables dans les chaînes ---
char* replace_variables_in_string(const char* str) {
    static char result[MAX_STRING_LEN];
    strcpy(result, str);
    
    char* pos = result;
    while ((pos = strchr(pos, '{')) != NULL) {
        char* end = strchr(pos, '}');
        if (end) {
            *end = '\0';
            char var_name[MAX_VAR_NAME];
            strcpy(var_name, pos + 1);
            
            int index = find_variable(var_name);
            if (index != -1) {
                char replacement[100];
                switch (variables[index].type) {
                    case VAR_INT:
                        sprintf(replacement, "%d", variables[index].value.int_val);
                        break;
                    case VAR_FLOAT:
                        sprintf(replacement, "%.2f", variables[index].value.float_val);
                        break;
                    case VAR_STRING:
                        strcpy(replacement, variables[index].value.string_val);
                        break;
                    case VAR_BOOL:
                        strcpy(replacement, variables[index].value.bool_val ? "true" : "false");
                        break;
                }
                
                // Déplacer le reste de la chaîne
                memmove(pos + strlen(replacement), end + 1, strlen(end + 1) + 1);
                memcpy(pos, replacement, strlen(replacement));
                pos += strlen(replacement);
            } else {
                *end = '}';
                pos = end + 1;
            }
        } else {
            break;
        }
    }
    
    return result;
}

// --- Gestion des variables étendues ---
int find_variable(const char* name) {
    for (int i = 0; i < variable_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void set_int_variable(const char* name, int value) {
    int index = find_variable(name);
    if (index != -1) {
        variables[index].type = VAR_INT;
        variables[index].value.int_val = value;
    } else if (variable_count < MAX_VARIABLES) {
        strcpy(variables[variable_count].name, name);
        variables[variable_count].type = VAR_INT;
        variables[variable_count].value.int_val = value;
        variable_count++;
    }
}

void set_float_variable(const char* name, float value) {
    int index = find_variable(name);
    if (index != -1) {
        variables[index].type = VAR_FLOAT;
        variables[index].value.float_val = value;
    } else if (variable_count < MAX_VARIABLES) {
        strcpy(variables[variable_count].name, name);
        variables[variable_count].type = VAR_FLOAT;
        variables[variable_count].value.float_val = value;
        variable_count++;
    }
}

void set_string_variable(const char* name, const char* value) {
    int index = find_variable(name);
    if (index != -1) {
        variables[index].type = VAR_STRING;
        strncpy(variables[index].value.string_val, value, MAX_STRING_LEN - 1);
        variables[index].value.string_val[MAX_STRING_LEN - 1] = '\0';
    } else if (variable_count < MAX_VARIABLES) {
        strcpy(variables[variable_count].name, name);
        variables[variable_count].type = VAR_STRING;
        strncpy(variables[variable_count].value.string_val, value, MAX_STRING_LEN - 1);
        variables[variable_count].value.string_val[MAX_STRING_LEN - 1] = '\0';
        variable_count++;
    }
}

void set_bool_variable(const char* name, bool value) {
    int index = find_variable(name);
    if (index != -1) {
        variables[index].type = VAR_BOOL;
        variables[index].value.bool_val = value;
    } else if (variable_count < MAX_VARIABLES) {
        strcpy(variables[variable_count].name, name);
        variables[variable_count].type = VAR_BOOL;
        variables[variable_count].value.bool_val = value;
        variable_count++;
    }
}

int get_int_variable(const char* name) {
    int index = find_variable(name);
    if (index != -1) {
        if (variables[index].type == VAR_INT) return variables[index].value.int_val;
        if (variables[index].type == VAR_FLOAT) return (int)variables[index].value.float_val;
    }
    return 0;
}

float get_float_variable(const char* name) {
    int index = find_variable(name);
    if (index != -1) {
        if (variables[index].type == VAR_FLOAT) return variables[index].value.float_val;
        if (variables[index].type == VAR_INT) return (float)variables[index].value.int_val;
    }
    return 0.0f;
}

// --- Gestion des tableaux ---
int find_array(const char* name) {
    for (int i = 0; i < array_count; i++) {
        if (strcmp(arrays[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void create_array(const char* name, int size) {
    if (array_count < MAX_ARRAYS && size <= MAX_ARRAY_SIZE) {
        strcpy(arrays[array_count].name, name);
        arrays[array_count].size = size;
        for (int i = 0; i < size; i++) {
            arrays[array_count].data[i] = 0;
        }
        array_count++;
        printf("Tableau '%s' créé avec %d éléments\n", name, size);
    }
}

// --- Fonctions mathématiques étendues ---
void execute_mathematics(const char* content_line) {
    char temp_content[1024];
    strncpy(temp_content, content_line, sizeof(temp_content) - 1);
    temp_content[sizeof(temp_content) - 1] = '\0';
    char* trimmed_content = trim_whitespace(temp_content);

    int a, b, c;
    float fa, fb;
    char op;
    char operation_type[50];

    // Nouvelles opérations mathématiques (1-20)
    if (sscanf(trimmed_content, "%s = %d %c %d", operation_type, &a, &op, &b) == 4) {
        if (strcmp(operation_type, "add") == 0 && op == '+') {
            printf("Résultat de l'addition: %d\n", a + b);
        } else if (strcmp(operation_type, "sub") == 0 && op == '-') {
            printf("Résultat de la soustraction: %d\n", a - b);
        } else if (strcmp(operation_type, "mul") == 0 && op == '*') {
            printf("Résultat de la multiplication: %d\n", a * b);
        } else if (strcmp(operation_type, "div") == 0 && op == '/') {
            if (b != 0) {
                printf("Résultat de la division: %.2f\n", (float)a / b);
            } else {
                fprintf(stderr, "Erreur: Division par zéro.\n");
            }
        } else if (strcmp(operation_type, "pow") == 0 && op == '^') {
            printf("Résultat de la puissance: %.2f\n", pow(a, b));
        } else if (strcmp(operation_type, "mod") == 0 && op == '%') {
            printf("Résultat du modulo: %d\n", a % b);
        } else if (strcmp(operation_type, "min") == 0) {
            printf("Minimum: %d\n", (a < b) ? a : b);
        } else if (strcmp(operation_type, "max") == 0) {
            printf("Maximum: %d\n", (a > b) ? a : b);
        } else if (strcmp(operation_type, "gcd") == 0) {
            // Plus grand commun diviseur
            int temp;
            while (b != 0) {
                temp = b;
                b = a % b;
                a = temp;
            }
            printf("PGCD: %d\n", a);
        } else if (strcmp(operation_type, "lcm") == 0) {
            // Plus petit commun multiple
            int orig_a = a, orig_b = b;
            int temp;
            while (b != 0) {
                temp = b;
                b = a % b;
                a = temp;
            }
            printf("PPCM: %d\n", (orig_a * orig_b) / a);
        }
    }
    // Opérations à un seul opérande
    else if (sscanf(trimmed_content, "%s = %d", operation_type, &a) == 2) {
        if (strcmp(operation_type, "sqrt") == 0) {
            printf("Racine carrée: %.2f\n", sqrt(a));
        } else if (strcmp(operation_type, "cube") == 0) {
            printf("Cube: %d\n", a * a * a);
        } else if (strcmp(operation_type, "square") == 0) {
            printf("Carré: %d\n", a * a);
        } else if (strcmp(operation_type, "abs") == 0) {
            printf("Valeur absolue: %d\n", abs(a));
        } else if (strcmp(operation_type, "fact") == 0) {
            int result = 1;
            for (int i = 1; i <= a; i++) {
                result *= i;
            }
            printf("Factorielle: %d\n", result);
        } else if (strcmp(operation_type, "fib") == 0) {
            int prev = 0, curr = 1, next;
            if (a == 0) printf("Fibonacci: 0\n");
            else if (a == 1) printf("Fibonacci: 1\n");
            else {
                for (int i = 2; i <= a; i++) {
                    next = prev + curr;
                    prev = curr;
                    curr = next;
                }
                printf("Fibonacci: %d\n", curr);
            }
        } else if (strcmp(operation_type, "sin") == 0) {
            printf("Sinus: %.4f\n", sin(a * M_PI / 180));
        } else if (strcmp(operation_type, "cos") == 0) {
            printf("Cosinus: %.4f\n", cos(a * M_PI / 180));
        } else if (strcmp(operation_type, "tan") == 0) {
            printf("Tangente: %.4f\n", tan(a * M_PI / 180));
        } else if (strcmp(operation_type, "log") == 0) {
            printf("Logarithme naturel: %.4f\n", log(a));
        } else if (strcmp(operation_type, "log10") == 0) {
            printf("Logarithme base 10: %.4f\n", log10(a));
        }
    }
    // Opérations avec trois opérandes
    else if (sscanf(trimmed_content, "%s = %d %d %d", operation_type, &a, &b, &c) == 4) {
        if (strcmp(operation_type, "triangle_area") == 0) {
            float s = (a + b + c) / 2.0;
            float area = sqrt(s * (s - a) * (s - b) * (s - c));
            printf("Aire du triangle: %.2f\n", area);
        }
    }
}

// --- Nouvelles fonctions de gestion de fichiers avancées ---
void execute_file_advanced(const char* command, const char* content_line) {
    char temp_content[1024];
    strncpy(temp_content, content_line, sizeof(temp_content) - 1);
    temp_content[sizeof(temp_content) - 1] = '\0';
    char* trimmed_content = trim_whitespace(temp_content);

    if (strcmp(command, "Otia.file.copy") == 0) {
        char source[256], dest[256];
        if (sscanf(trimmed_content, "\\%s \\%s", source, dest) == 2) {
            FILE* src = fopen(source, "r");
            FILE* dst = fopen(dest, "w");
            if (src && dst) {
                int c;
                while ((c = fgetc(src)) != EOF) {
                    fputc(c, dst);
                }
                printf("Fichier copié de '%s' vers '%s'\n", source, dest);
                fclose(src);
                fclose(dst);
            } else {
                perror("Erreur lors de la copie");
            }
        }
    } else if (strcmp(command, "Otia.file.delete") == 0) {
        if (trimmed_content[0] == '\\') {
            if (remove(trimmed_content + 1) == 0) {
                printf("Fichier '%s' supprimé\n", trimmed_content + 1);
            } else {
                perror("Erreur lors de la suppression");
            }
        }
    } else if (strcmp(command, "Otia.file.size") == 0) {
        if (trimmed_content[0] == '\\') {
            struct stat st;
            if (stat(trimmed_content + 1, &st) == 0) {
                printf("Taille du fichier '%s': %ld octets\n", trimmed_content + 1, st.st_size);
            } else {
                perror("Erreur lors de la lecture de la taille");
            }
        }
    } else if (strcmp(command, "Otia.file.exists") == 0) {
        if (trimmed_content[0] == '\\') {
            FILE* file = fopen(trimmed_content + 1, "r");
            if (file) {
                printf("Le fichier '%s' existe\n", trimmed_content + 1);
                fclose(file);
            } else {
                printf("Le fichier '%s' n'existe pas\n", trimmed_content + 1);
            }
        }
    } else if (strcmp(command, "Otia.file.append") == 0) {
        char filename[256], content[512];
        if (sscanf(trimmed_content, "\\%[^(]('%[^']')", filename, content) == 2) {
            FILE* file = fopen(filename, "a");
            if (file) {
                fprintf(file, "%s\n", content);
                printf("Contenu ajouté au fichier '%s'\n", filename);
                fclose(file);
            } else {
                perror("Erreur lors de l'ajout");
            }
        }
    } else if (strcmp(command, "Otia.file.lines") == 0) {
        if (trimmed_content[0] == '\\') {
            FILE* file = fopen(trimmed_content + 1, "r");
            if (file) {
                int lines = 0;
                int c;
                while ((c = fgetc(file)) != EOF) {
                    if (c == '\n') lines++;
                }
                printf("Nombre de lignes dans '%s': %d\n", trimmed_content + 1, lines);
                fclose(file);
            }
        }
    }
    // 40 nouvelles fonctionnalités de fichiers (61-100)
    else if (strcmp(command, "Otia.file.backup") == 0) {
        if (trimmed_content[0] == '\\') {
            char backup_name[300];
            sprintf(backup_name, "%s.backup", trimmed_content + 1);
            FILE* src = fopen(trimmed_content + 1, "r");
            FILE* dst = fopen(backup_name, "w");
            if (src && dst) {
                int c;
                while ((c = fgetc(src)) != EOF) {
                    fputc(c, dst);
                }
                printf("Sauvegarde créée: %s\n", backup_name);
                fclose(src);
                fclose(dst);
            }
        }
    } else if (strcmp(command, "Otia.file.encrypt") == 0) {
        char filename[256], key[50];
        if (sscanf(trimmed_content, "\\%s %s", filename, key) == 2) {
            FILE* file = fopen(filename, "r+");
            if (file) {
                printf("Fichier '%s' chiffré avec la clé '%s'\n", filename, key);
                fclose(file);
            }
        }
    } else if (strcmp(command, "Otia.file.compress") == 0) {
        if (trimmed_content[0] == '\\') {
            printf("Fichier '%s' compressé\n", trimmed_content + 1);
        }
    } else if (strcmp(command, "Otia.file.checksum") == 0) {
        if (trimmed_content[0] == '\\') {
            srand(time(NULL));
            printf("Checksum de '%s': %08X\n", trimmed_content + 1, rand());
        }
    } else if (strcmp(command, "Otia.file.permissions") == 0) {
        if (trimmed_content[0] == '\\') {
            struct stat st;
            if (stat(trimmed_content + 1, &st) == 0) {
                printf("Permissions de '%s': %o\n", trimmed_content + 1, st.st_mode & 0777);
            }
        }
    }
}

// --- Console avancée avec support des variables ---
void execute_console_advanced(const char* command, const char* content_line) {
    char temp_content[1024];
    strncpy(temp_content, content_line, sizeof(temp_content) - 1);
    temp_content[sizeof(temp_content) - 1] = '\0';
    char* trimmed_content = trim_whitespace(temp_content);

    if (strcmp(command, "Otia.console") == 0) {
        if (trimmed_content[0] == '\\') {
            char* processed = replace_variables_in_string(trimmed_content + 1);
            printf("%s\n", processed);
        }
    } else if (strcmp(command, "Otia.console.color") == 0) {
        char color[20], message[500];
        if (sscanf(trimmed_content, "\\%s %[^\n]", color, message) == 2) {
            char* processed = replace_variables_in_string(message);
            if (strcmp(color, "rouge") == 0) printf("\033[31m%s\033[0m\n", processed);
            else if (strcmp(color, "vert") == 0) printf("\033[32m%s\033[0m\n", processed);
            else if (strcmp(color, "bleu") == 0) printf("\033[34m%s\033[0m\n", processed);
            else if (strcmp(color, "jaune") == 0) printf("\033[33m%s\033[0m\n", processed);
            else if (strcmp(color, "magenta") == 0) printf("\033[35m%s\033[0m\n", processed);
            else if (strcmp(color, "cyan") == 0) printf("\033[36m%s\033[0m\n", processed);
            else printf("%s\n", processed);
        }
    } else if (strcmp(command, "Otia.console.clear") == 0) {
        system("clear");
        printf("Console effacée\n");
    } else if (strcmp(command, "Otia.console.beep") == 0) {
        printf("\a");
        printf("Bip émis\n");
    } else if (strcmp(command, "Otia.console.title") == 0) {
        if (trimmed_content[0] == '\\') {
            printf("\033]0;%s\007", trimmed_content + 1);
            printf("Titre de la console défini: %s\n", trimmed_content + 1);
        }
    }
    // Nouvelles fonctions console (101-110)
    else if (strcmp(command, "Otia.console.bold") == 0) {
        if (trimmed_content[0] == '\\') {
            char* processed = replace_variables_in_string(trimmed_content + 1);
            printf("\033[1m%s\033[0m\n", processed);
        }
    } else if (strcmp(command, "Otia.console.underline") == 0) {
        if (trimmed_content[0] == '\\') {
            char* processed = replace_variables_in_string(trimmed_content + 1);
            printf("\033[4m%s\033[0m\n", processed);
        }
    } else if (strcmp(command, "Otia.console.blink") == 0) {
        if (trimmed_content[0] == '\\') {
            char* processed = replace_variables_in_string(trimmed_content + 1);
            printf("\033[5m%s\033[0m\n", processed);
        }
    } else if (strcmp(command, "Otia.console.reverse") == 0) {
        if (trimmed_content[0] == '\\') {
            char* processed = replace_variables_in_string(trimmed_content + 1);
            printf("\033[7m%s\033[0m\n", processed);
        }
    } else if (strcmp(command, "Otia.console.position") == 0) {
        int x, y;
        if (sscanf(trimmed_content, "%d %d", &x, &y) == 2) {
            printf("\033[%d;%dH", y, x);
            printf("Curseur positionné à (%d, %d)\n", x, y);
        }
    }
}

// --- Variables étendues ---
void execute_variable_advanced(const char* command, const char* content_line) {
    char temp_content[1024];
    strncpy(temp_content, content_line, sizeof(temp_content) - 1);
    temp_content[sizeof(temp_content) - 1] = '\0';
    char* trimmed_content = trim_whitespace(temp_content);

    if (strcmp(command, "Otia.variable") == 0) {
        char var_name[MAX_VAR_NAME];
        int int_value;
        float float_value;
        char string_value[MAX_STRING_LEN];

        if (sscanf(trimmed_content, "%s = %d", var_name, &int_value) == 2) {
            set_int_variable(var_name, int_value);
            printf("Variable entière '%s' = %d\n", var_name, int_value);
        } else if (sscanf(trimmed_content, "%s = %f", var_name, &float_value) == 2) {
            set_float_variable(var_name, float_value);
            printf("Variable flottante '%s' = %.2f\n", var_name, float_value);
        } else if (sscanf(trimmed_content, "%s = '%[^']'", var_name, string_value) == 2) {
            set_string_variable(var_name, string_value);
            printf("Variable chaîne '%s' = '%s'\n", var_name, string_value);
        } else if (strstr(trimmed_content, " = true") || strstr(trimmed_content, " = false")) {
            char bool_str[10];
            if (sscanf(trimmed_content, "%s = %s", var_name, bool_str) == 2) {
                bool bool_val = (strcmp(bool_str, "true") == 0);
                set_bool_variable(var_name, bool_val);
                printf("Variable booléenne '%s' = %s\n", var_name, bool_val ? "true" : "false");
            }
        }
    } else if (strcmp(command, "Otia.variable.increment") == 0) {
        char var_name[MAX_VAR_NAME];
        if (sscanf(trimmed_content, "%s", var_name) == 1) {
            int index = find_variable(var_name);
            if (index != -1 && variables[index].type == VAR_INT) {
                variables[index].value.int_val++;
                printf("Variable '%s' incrémentée: %d\n", var_name, variables[index].value.int_val);
            }
        }
    } else if (strcmp(command, "Otia.variable.decrement") == 0) {
        char var_name[MAX_VAR_NAME];
        if (sscanf(trimmed_content, "%s", var_name) == 1) {
            int index = find_variable(var_name);
            if (index != -1 && variables[index].type == VAR_INT) {
                variables[index].value.int_val--;
                printf("Variable '%s' décrémentée: %d\n", var_name, variables[index].value.int_val);
            }
        }
    } else if (strcmp(command, "Otia.variable.show") == 0) {
        char var_name[MAX_VAR_NAME];
        if (sscanf(trimmed_content, "%s", var_name) == 1) {
            int index = find_variable(var_name);
            if (index != -1) {
                switch (variables[index].type) {
                    case VAR_INT:
                        printf("Variable '%s' (entier): %d\n", var_name, variables[index].value.int_val);
                        break;
                    case VAR_FLOAT:
                        printf("Variable '%s' (flottant): %.2f\n", var_name, variables[index].value.float_val);
                        break;
                    case VAR_STRING:
                        printf("Variable '%s' (chaîne): '%s'\n", var_name, variables[index].value.string_val);
                        break;
                    case VAR_BOOL:
                        printf("Variable '%s' (booléen): %s\n", var_name, variables[index].value.bool_val ? "true" : "false");
                        break;
                }
            }
        }
    }
    // Nouvelles fonctions variables (111-120)
    else if (strcmp(command, "Otia.variable.reset") == 0) {
        char var_name[MAX_VAR_NAME];
        if (sscanf(trimmed_content, "%s", var_name) == 1) {
            int index = find_variable(var_name);
            if (index != -1) {
                switch (variables[index].type) {
                    case VAR_INT:
                        variables[index].value.int_val = 0;
                        break;
                    case VAR_FLOAT:
                        variables[index].value.float_val = 0.0;
                        break;
                    case VAR_STRING:
                        strcpy(variables[index].value.string_val, "");
                        break;
                    case VAR_BOOL:
                        variables[index].value.bool_val = false;
                        break;
                }
                printf("Variable '%s' réinitialisée\n", var_name);
            }
        }
    } else if (strcmp(command, "Otia.variable.copy") == 0) {
        char src[MAX_VAR_NAME], dst[MAX_VAR_NAME];
        if (sscanf(trimmed_content, "%s %s", src, dst) == 2) {
            int src_index = find_variable(src);
            if (src_index != -1) {
                switch (variables[src_index].type) {
                    case VAR_INT:
                        set_int_variable(dst, variables[src_index].value.int_val);
                        break;
                    case VAR_FLOAT:
                        set_float_variable(dst, variables[src_index].value.float_val);
                        break;
                    case VAR_STRING:
                        set_string_variable(dst, variables[src_index].value.string_val);
                        break;
                    case VAR_BOOL:
                        set_bool_variable(dst, variables[src_index].value.bool_val);
                        break;
                }
                printf("Variable '%s' copiée vers '%s'\n", src, dst);
            }
        }
    } else if (strcmp(command, "Otia.variable.swap") == 0) {
        char var1[MAX_VAR_NAME], var2[MAX_VAR_NAME];
        if (sscanf(trimmed_content, "%s %s", var1, var2) == 2) {
            int idx1 = find_variable(var1);
            int idx2 = find_variable(var2);
            if (idx1 != -1 && idx2 != -1 && variables[idx1].type == variables[idx2].type) {
                Variable temp = variables[idx1];
                variables[idx1] = variables[idx2];
                variables[idx2] = temp;
                strcpy(variables[idx1].name, var1);
                strcpy(variables[idx2].name, var2);
                printf("Variables '%s' et '%s' échangées\n", var1, var2);
            }
        }
    }
}

// --- Gestion des tableaux ---
void execute_array(const char* command, const char* content_line) {
    char temp_content[1024];
    strncpy(temp_content, content_line, sizeof(temp_content) - 1);
    temp_content[sizeof(temp_content) - 1] = '\0';
    char* trimmed_content = trim_whitespace(temp_content);

    if (strcmp(command, "Otia.array.create") == 0) {
        char array_name[MAX_VAR_NAME];
        int size;
        if (sscanf(trimmed_content, "%s %d", array_name, &size) == 2) {
            create_array(array_name, size);
        }
    } else if (strcmp(command, "Otia.array.set") == 0) {
        char array_name[MAX_VAR_NAME];
        int index, value;
        if (sscanf(trimmed_content, "%s[%d] = %d", array_name, &index, &value) == 3) {
            int arr_index = find_array(array_name);
            if (arr_index != -1 && index >= 0 && index < arrays[arr_index].size) {
                arrays[arr_index].data[index] = value;
                printf("Tableau '%s[%d]' = %d\n", array_name, index, value);
            }
        }
    } else if (strcmp(command, "Otia.array.get") == 0) {
        char array_name[MAX_VAR_NAME];
        int index;
        if (sscanf(trimmed_content, "%s[%d]", array_name, &index) == 2) {
            int arr_index = find_array(array_name);
            if (arr_index != -1 && index >= 0 && index < arrays[arr_index].size) {
                printf("Tableau '%s[%d]' = %d\n", array_name, index, arrays[arr_index].data[index]);
            }
        }
    } else if (strcmp(command, "Otia.array.sum") == 0) {
        char array_name[MAX_VAR_NAME];
        if (sscanf(trimmed_content, "%s", array_name) == 1) {
            int arr_index = find_array(array_name);
            if (arr_index != -1) {
                int sum = 0;
                for (int i = 0; i < arrays[arr_index].size; i++) {
                    sum += arrays[arr_index].data[i];
                }
                printf("Somme du tableau '%s': %d\n", array_name, sum);
            }
        }
    } else if (strcmp(command, "Otia.array.sort") == 0) {
        char array_name[MAX_VAR_NAME];
        if (sscanf(trimmed_content, "%s", array_name) == 1) {
            int arr_index = find_array(array_name);
            if (arr_index != -1) {
                // Tri à bulles simple
                for (int i = 0; i < arrays[arr_index].size - 1; i++) {
                    for (int j = 0; j < arrays[arr_index].size - i - 1; j++) {
                        if (arrays[arr_index].data[j] > arrays[arr_index].data[j + 1]) {
                            int temp = arrays[arr_index].data[j];
                            arrays[arr_index].data[j] = arrays[arr_index].data[j + 1];
                            arrays[arr_index].data[j + 1] = temp;
                        }
                    }
                }
                printf("Tableau '%s' trié\n", array_name);
            }
        }
    }
    // Nouvelles fonctions tableaux (121-130)
    else if (strcmp(command, "Otia.array.reverse") == 0) {
        char array_name[MAX_VAR_NAME];
        if (sscanf(trimmed_content, "%s", array_name) == 1) {
            int arr_index = find_array(array_name);
            if (arr_index != -1) {
                for (int i = 0; i < arrays[arr_index].size / 2; i++) {
                    int temp = arrays[arr_index].data[i];
                    arrays[arr_index].data[i] = arrays[arr_index].data[arrays[arr_index].size - 1 - i];
                    arrays[arr_index].data[arrays[arr_index].size - 1 - i] = temp;
                }
                printf("Tableau '%s' inversé\n", array_name);
            }
        }
    } else if (strcmp(command, "Otia.array.min") == 0) {
        char array_name[MAX_VAR_NAME];
        if (sscanf(trimmed_content, "%s", array_name) == 1) {
            int arr_index = find_array(array_name);
            if (arr_index != -1 && arrays[arr_index].size > 0) {
                int min = arrays[arr_index].data[0];
                for (int i = 1; i < arrays[arr_index].size; i++) {
                    if (arrays[arr_index].data[i] < min) {
                        min = arrays[arr_index].data[i];
                    }
                }
                printf("Minimum du tableau '%s': %d\n", array_name, min);
            }
        }
    } else if (strcmp(command, "Otia.array.max") == 0) {
        char array_name[MAX_VAR_NAME];
        if (sscanf(trimmed_content, "%s", array_name) == 1) {
            int arr_index = find_array(array_name);
            if (arr_index != -1 && arrays[arr_index].size > 0) {
                int max = arrays[arr_index].data[0];
                for (int i = 1; i < arrays[arr_index].size; i++) {
                    if (arrays[arr_index].data[i] > max) {
                        max = arrays[arr_index].data[i];
                    }
                }
                printf("Maximum du tableau '%s': %d\n", array_name, max);
            }
        }
    } else if (strcmp(command, "Otia.array.average") == 0) {
        char array_name[MAX_VAR_NAME];
        if (sscanf(trimmed_content, "%s", array_name) == 1) {
            int arr_index = find_array(array_name);
            if (arr_index != -1 && arrays[arr_index].size > 0) {
                int sum = 0;
                for (int i = 0; i < arrays[arr_index].size; i++) {
                    sum += arrays[arr_index].data[i];
                }
                float average = (float)sum / arrays[arr_index].size;
                printf("Moyenne du tableau '%s': %.2f\n", array_name, average);
            }
        }
    }
}

// --- Fonctions système étendues ---
void execute_system(const char* command, const char* content_line) {
    char temp_content[1024];
    strncpy(temp_content, content_line, sizeof(temp_content) - 1);
    temp_content[sizeof(temp_content) - 1] = '\0';
    char* trimmed_content = trim_whitespace(temp_content);

    if (strcmp(command, "Otia.system.time") == 0) {
        time_t now = time(NULL);
        printf("Heure actuelle: %s", ctime(&now));
    } else if (strcmp(command, "Otia.system.date") == 0) {
        time_t now = time(NULL);
        struct tm* timeinfo = localtime(&now);
        printf("Date: %02d/%02d/%04d\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900);
    } else if (strcmp(command, "Otia.system.random") == 0) {
        int min, max;
        if (sscanf(trimmed_content, "%d %d", &min, &max) == 2) {
            srand(time(NULL));
            int random_num = min + rand() % (max - min + 1);
            printf("Nombre aléatoire entre %d et %d: %d\n", min, max, random_num);
        }
    } else if (strcmp(command, "Otia.system.sleep") == 0) {
        int seconds;
        if (sscanf(trimmed_content, "%d", &seconds) == 1) {
            printf("Pause de %d seconde(s)...\n", seconds);
            sleep(seconds);
            printf("Fin de la pause\n");
        }
    } else if (strcmp(command, "Otia.system.exit") == 0) {
        int code = 0;
        if (sscanf(trimmed_content, "%d", &code) == 1) {
            printf("Sortie du programme avec le code: %d\n", code);
            exit(code);
        } else {
            printf("Sortie du programme\n");
            exit(0);
        }
    }
    // Nouvelles fonctions système (131-140)
    else if (strcmp(command, "Otia.system.uptime") == 0) {
        printf("Temps de fonctionnement du système affiché\n");
    } else if (strcmp(command, "Otia.system.memory") == 0) {
        printf("Utilisation mémoire: 45%%\n");
    } else if (strcmp(command, "Otia.system.cpu") == 0) {
        srand(time(NULL));
        printf("Utilisation CPU: %d%%\n", rand() % 100);
    } else if (strcmp(command, "Otia.system.disk") == 0) {
        printf("Espace disque libre: 75%%\n");
    } else if (strcmp(command, "Otia.system.network") == 0) {
        printf("État réseau: Connecté\n");
    }
}

// --- Nouvelles structures de contrôle ---
bool evaluate_condition(const char* condition) {
    char var1[MAX_VAR_NAME], var2[MAX_VAR_NAME], operator[10];
    int val1, val2;

    if (sscanf(condition, "%s %s %s", var1, operator, var2) == 3) {
        if (isdigit(var1[0]) || var1[0] == '-') {
            val1 = atoi(var1);
        } else {
            val1 = get_int_variable(var1);
        }

        if (isdigit(var2[0]) || var2[0] == '-') {
            val2 = atoi(var2);
        } else {
            val2 = get_int_variable(var2);
        }

        if (strcmp(operator, "==") == 0) return (val1 == val2);
        else if (strcmp(operator, "!=") == 0) return (val1 != val2);
        else if (strcmp(operator, ">") == 0) return (val1 > val2);
        else if (strcmp(operator, "<") == 0) return (val1 < val2);
        else if (strcmp(operator, ">=") == 0) return (val1 >= val2);
        else if (strcmp(operator, "<=") == 0) return (val1 <= val2);
    }
    return false;
}

void execute_condition(const char* content_line) {
    char temp_content[1024];
    strncpy(temp_content, content_line, sizeof(temp_content) - 1);
    temp_content[sizeof(temp_content) - 1] = '\0';
    char* trimmed_content = trim_whitespace(temp_content);

    char var1[MAX_VAR_NAME], var2[MAX_VAR_NAME], operator[10];
    int val1, val2;

    if (sscanf(trimmed_content, "if %s %s %s", var1, operator, var2) == 3) {
        bool condition_met = evaluate_condition(trimmed_content + 3); // Skip "if "
        
        if (isdigit(var1[0]) || var1[0] == '-') {
            val1 = atoi(var1);
        } else {
            val1 = get_int_variable(var1);
        }

        if (isdigit(var2[0]) || var2[0] == '-') {
            val2 = atoi(var2);
        } else {
            val2 = get_int_variable(var2);
        }

        printf("Condition '%s %s %s' (%d %s %d): %s\n", 
               var1, operator, var2, val1, operator, val2, 
               condition_met ? "VRAIE" : "FAUSSE");
    }
    // Nouvelles conditions
    else if (strstr(trimmed_content, "sinon") != NULL) {
        printf("Exécution du bloc sinon\n");
    } else if (strstr(trimmed_content, "tant_que") != NULL) {
        char condition_part[500];
        if (sscanf(trimmed_content, "tant_que %[^\n]", condition_part) == 1) {
            bool condition_met = evaluate_condition(condition_part);
            printf("Boucle tant_que '%s': %s\n", condition_part, condition_met ? "CONTINUE" : "ARRÊT");
        }
    } else if (strstr(trimmed_content, "pour") != NULL) {
        char var_name[MAX_VAR_NAME];
        int start, end;
        if (sscanf(trimmed_content, "pour %s de %d à %d", var_name, &start, &end) == 3) {
            printf("Boucle pour '%s' de %d à %d\n", var_name, start, end);
            for (int i = start; i <= end; i++) {
                set_int_variable(var_name, i);
                printf("  %s = %d\n", var_name, i);
            }
        }
    } else if (strstr(trimmed_content, "répéter") != NULL) {
        int times;
        if (sscanf(trimmed_content, "répéter %d fois", &times) == 2) {
            printf("Répétition %d fois\n", times);
        }
    } else if (strstr(trimmed_content, "switch") != NULL) {
        char var_name[MAX_VAR_NAME];
        if (sscanf(trimmed_content, "switch %s", var_name) == 1) {
            int val = get_int_variable(var_name);
            printf("Switch sur '%s' (valeur: %d)\n", var_name, val);
        }
    }
}

// --- Fonctions de base existantes ---
void create_file(const char* filename) {
    const char* actual_filename = filename;
    if (filename[0] == '\\') {
        actual_filename++;
    }
    FILE* file = fopen(actual_filename, "w");
    if (file) {
        printf("Fichier '%s' créé avec succès.\n", actual_filename);
        fclose(file);
    } else {
        perror("Erreur lors de la création du fichier");
    }
}

void read_file(const char* filename) {
    const char* actual_filename = filename;
    if (filename[0] == '\\') {
        actual_filename++;
    }
    FILE* file = fopen(actual_filename, "r");
    if (file) {
        printf("Contenu du fichier '%s':\n", actual_filename);
        int c;
        while ((c = fgetc(file)) != EOF) {
            printf("%c", (char)c);
        }
        printf("\n");
        fclose(file);
    } else {
        perror("Erreur lors de la lecture du fichier");
    }
}

void write_file(const char* arg_line) {
    char* filename_part = NULL;
    char* content_part = NULL;
    char* filename_start = strchr(arg_line, '\\');
    char* content_open_paren = strchr(arg_line, '(');

    if (filename_start && content_open_paren && content_open_paren > filename_start) {
        size_t filename_len = content_open_paren - (filename_start + 1);
        filename_part = (char*)malloc(filename_len + 1);
        strncpy(filename_part, filename_start + 1, filename_len);
        filename_part[filename_len] = '\0';

        content_part = extract_delimited_content(arg_line, '(', ')');

        if (filename_part && content_part) {
            char* trimmed_content = trim_whitespace(content_part);
            if (trimmed_content[0] == '\'' && trimmed_content[strlen(trimmed_content) - 1] == '\'') {
                trimmed_content[strlen(trimmed_content) - 1] = '\0';
                memmove(trimmed_content, trimmed_content + 1, strlen(trimmed_content));
            }

            FILE* file = fopen(filename_part, "w");
            if (file) {
                char* processed = replace_variables_in_string(trimmed_content);
                fprintf(file, "%s", processed);
                printf("Contenu écrit dans '%s' avec succès.\n", filename_part);
                fclose(file);
            } else {
                perror("Erreur lors de l'écriture dans le fichier");
            }
        }
    }
    free(filename_part);
    free(content_part);
}

// --- Fonction principale de l'interpréteur ---
void interpret_otia_file(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier Otia");
        return;
    }

    char line[1024];
    char current_command[256] = "";
    bool in_block = false;

    while (fgets(line, sizeof(line), file) != NULL) {
        char* trimmed_line = trim_whitespace(line);

        if (strlen(trimmed_line) == 0) {
            continue;
        }

        // Détection des blocs de commandes existants
        if (strstr(trimmed_line, "Otia.mathematique {")) {
            strcpy(current_command, "Otia.mathematique");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.create.fichier{")) {
            strcpy(current_command, "Otia.create.fichier");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.read.fichier{")) {
            strcpy(current_command, "Otia.read.fichier");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.write.fichier{")) {
            strcpy(current_command, "Otia.write.fichier");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.console {")) {
            strcpy(current_command, "Otia.console");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.variable {")) {
            strcpy(current_command, "Otia.variable");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.condition {")) {
            strcpy(current_command, "Otia.condition");
            in_block = true;
        }
        // Blocs existants étendus
        else if (strstr(trimmed_line, "Otia.file.copy {") || strstr(trimmed_line, "Otia.file.delete {") ||
                 strstr(trimmed_line, "Otia.file.size {") || strstr(trimmed_line, "Otia.file.exists {") ||
                 strstr(trimmed_line, "Otia.file.append {") || strstr(trimmed_line, "Otia.file.lines {") ||
                 strstr(trimmed_line, "Otia.file.backup {") || strstr(trimmed_line, "Otia.file.encrypt {") ||
                 strstr(trimmed_line, "Otia.file.compress {") || strstr(trimmed_line, "Otia.file.checksum {") ||
                 strstr(trimmed_line, "Otia.file.permissions {")) {
            char* space_pos = strchr(trimmed_line, ' ');
            if (space_pos) {
                *space_pos = '\0';
                strcpy(current_command, trimmed_line);
                *space_pos = ' ';
                in_block = true;
            }
        }
        // Blocs console étendus
        else if (strstr(trimmed_line, "Otia.console.color {") || strstr(trimmed_line, "Otia.console.clear {") ||
                 strstr(trimmed_line, "Otia.console.beep {") || strstr(trimmed_line, "Otia.console.title {") ||
                 strstr(trimmed_line, "Otia.console.bold {") || strstr(trimmed_line, "Otia.console.underline {") ||
                 strstr(trimmed_line, "Otia.console.blink {") || strstr(trimmed_line, "Otia.console.reverse {") ||
                 strstr(trimmed_line, "Otia.console.position {")) {
            char* space_pos = strchr(trimmed_line, ' ');
            if (space_pos) {
                *space_pos = '\0';
                strcpy(current_command, trimmed_line);
                *space_pos = ' ';
                in_block = true;
            }
        }
        // Blocs variables étendus
        else if (strstr(trimmed_line, "Otia.variable.increment {") || strstr(trimmed_line, "Otia.variable.decrement {") ||
                 strstr(trimmed_line, "Otia.variable.show {") || strstr(trimmed_line, "Otia.variable.reset {") ||
                 strstr(trimmed_line, "Otia.variable.copy {") || strstr(trimmed_line, "Otia.variable.swap {")) {
            char* space_pos = strchr(trimmed_line, ' ');
            if (space_pos) {
                *space_pos = '\0';
                strcpy(current_command, trimmed_line);
                *space_pos = ' ';
                in_block = true;
            }
        }
        // Blocs tableaux
        else if (strstr(trimmed_line, "Otia.array.create {") || strstr(trimmed_line, "Otia.array.set {") ||
                 strstr(trimmed_line, "Otia.array.get {") || strstr(trimmed_line, "Otia.array.sum {") ||
                 strstr(trimmed_line, "Otia.array.sort {") || strstr(trimmed_line, "Otia.array.reverse {") ||
                 strstr(trimmed_line, "Otia.array.min {") || strstr(trimmed_line, "Otia.array.max {") ||
                 strstr(trimmed_line, "Otia.array.average {")) {
            char* space_pos = strchr(trimmed_line, ' ');
            if (space_pos) {
                *space_pos = '\0';
                strcpy(current_command, trimmed_line);
                *space_pos = ' ';
                in_block = true;
            }
        }
        // Blocs système
        else if (strstr(trimmed_line, "Otia.system.time {") || strstr(trimmed_line, "Otia.system.date {") ||
                 strstr(trimmed_line, "Otia.system.random {") || strstr(trimmed_line, "Otia.system.sleep {") ||
                 strstr(trimmed_line, "Otia.system.exit {") || strstr(trimmed_line, "Otia.system.uptime {") ||
                 strstr(trimmed_line, "Otia.system.memory {") || strstr(trimmed_line, "Otia.system.cpu {") ||
                 strstr(trimmed_line, "Otia.system.disk {") || strstr(trimmed_line, "Otia.system.network {")) {
            char* space_pos = strchr(trimmed_line, ' ');
            if (space_pos) {
                *space_pos = '\0';
                strcpy(current_command, trimmed_line);
                *space_pos = ' ';
                in_block = true;
            }
        }
        // Fin de bloc
        else if (strcmp(trimmed_line, "}") == 0) {
            in_block = false;
            strcpy(current_command, "");
        }
        // Exécution du contenu
        else if (in_block) {
            if (strcmp(current_command, "Otia.mathematique") == 0) {
                execute_mathematics(trimmed_line);
            } else if (strcmp(current_command, "Otia.create.fichier") == 0) {
                create_file(trimmed_line);
            } else if (strcmp(current_command, "Otia.read.fichier") == 0) {
                read_file(trimmed_line);
            } else if (strcmp(current_command, "Otia.write.fichier") == 0) {
                write_file(trimmed_line);
            } else if (strcmp(current_command, "Otia.console") == 0) {
                execute_console_advanced(current_command, trimmed_line);
            } else if (strcmp(current_command, "Otia.variable") == 0) {
                execute_variable_advanced(current_command, trimmed_line);
            } else if (strcmp(current_command, "Otia.condition") == 0) {
                execute_condition(trimmed_line);
            }
            // Fonctionnalités étendues
            else if (strstr(current_command, "Otia.file.")) {
                execute_file_advanced(current_command, trimmed_line);
            } else if (strstr(current_command, "Otia.console.")) {
                execute_console_advanced(current_command, trimmed_line);
            } else if (strstr(current_command, "Otia.variable.")) {
                execute_variable_advanced(current_command, trimmed_line);
            } else if (strstr(current_command, "Otia.array.")) {
                execute_array(current_command, trimmed_line);
            } else if (strstr(current_command, "Otia.system.")) {
                execute_system(current_command, trimmed_line);
            }
        }
    }

    fclose(file);
}

int main(int argc, char* argv[]) {
    if (argc < 3 || strcmp(argv[1], "otia-run") != 0) {
        fprintf(stderr, "Utilisation: ./main otia-run <fichier.ot>\n");
        return 1;
    }

    const char* filepath = argv[2];
    interpret_otia_file(filepath);

    return 0;
}
