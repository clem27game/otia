
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

Variable variables[MAX_VARIABLES];
Array arrays[MAX_ARRAYS];
int variable_count = 0;
int array_count = 0;

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

// --- Nouvelles fonctions de gestion de fichiers (21-30) ---
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
}

// --- Nouvelles fonctions de console étendues (31-40) ---
void execute_console_advanced(const char* command, const char* content_line) {
    char temp_content[1024];
    strncpy(temp_content, content_line, sizeof(temp_content) - 1);
    temp_content[sizeof(temp_content) - 1] = '\0';
    char* trimmed_content = trim_whitespace(temp_content);

    if (strcmp(command, "Otia.console") == 0) {
        if (trimmed_content[0] == '\\') {
            printf("%s\n", trimmed_content + 1);
        }
    } else if (strcmp(command, "Otia.console.color") == 0) {
        char color[20], message[500];
        if (sscanf(trimmed_content, "\\%s %[^\n]", color, message) == 2) {
            if (strcmp(color, "rouge") == 0) printf("\033[31m%s\033[0m\n", message);
            else if (strcmp(color, "vert") == 0) printf("\033[32m%s\033[0m\n", message);
            else if (strcmp(color, "bleu") == 0) printf("\033[34m%s\033[0m\n", message);
            else if (strcmp(color, "jaune") == 0) printf("\033[33m%s\033[0m\n", message);
            else printf("%s\n", message);
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
}

// --- Nouvelles fonctions de variables étendues (41-50) ---
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
                }
            }
        }
    }
}

// --- Nouvelles fonctions d'array (51-55) ---
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
}

// --- Nouvelles fonctions système (56-60) ---
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
                fprintf(file, "%s", trimmed_content);
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

void execute_condition(const char* content_line) {
    char temp_content[1024];
    strncpy(temp_content, content_line, sizeof(temp_content) - 1);
    temp_content[sizeof(temp_content) - 1] = '\0';
    char* trimmed_content = trim_whitespace(temp_content);

    char var1[MAX_VAR_NAME], var2[MAX_VAR_NAME], operator[10];
    int val1, val2;

    if (sscanf(trimmed_content, "if %s %s %s", var1, operator, var2) == 3) {
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

        bool condition_met = false;
        if (strcmp(operator, "==") == 0) {
            condition_met = (val1 == val2);
        } else if (strcmp(operator, "!=") == 0) {
            condition_met = (val1 != val2);
        } else if (strcmp(operator, ">") == 0) {
            condition_met = (val1 > val2);
        } else if (strcmp(operator, "<") == 0) {
            condition_met = (val1 < val2);
        } else if (strcmp(operator, ">=") == 0) {
            condition_met = (val1 >= val2);
        } else if (strcmp(operator, "<=") == 0) {
            condition_met = (val1 <= val2);
        }

        printf("Condition '%s %s %s' (%d %s %d): %s\n", 
               var1, operator, var2, val1, operator, val2, 
               condition_met ? "VRAIE" : "FAUSSE");
    }
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

        // Détection des nouveaux blocs de commandes
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
        // Nouveaux blocs
        else if (strstr(trimmed_line, "Otia.file.copy {")) {
            strcpy(current_command, "Otia.file.copy");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.file.delete {")) {
            strcpy(current_command, "Otia.file.delete");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.file.size {")) {
            strcpy(current_command, "Otia.file.size");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.file.exists {")) {
            strcpy(current_command, "Otia.file.exists");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.file.append {")) {
            strcpy(current_command, "Otia.file.append");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.file.lines {")) {
            strcpy(current_command, "Otia.file.lines");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.console.color {")) {
            strcpy(current_command, "Otia.console.color");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.console.clear {")) {
            strcpy(current_command, "Otia.console.clear");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.console.beep {")) {
            strcpy(current_command, "Otia.console.beep");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.console.title {")) {
            strcpy(current_command, "Otia.console.title");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.variable.increment {")) {
            strcpy(current_command, "Otia.variable.increment");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.variable.decrement {")) {
            strcpy(current_command, "Otia.variable.decrement");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.variable.show {")) {
            strcpy(current_command, "Otia.variable.show");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.array.create {")) {
            strcpy(current_command, "Otia.array.create");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.array.set {")) {
            strcpy(current_command, "Otia.array.set");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.array.get {")) {
            strcpy(current_command, "Otia.array.get");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.array.sum {")) {
            strcpy(current_command, "Otia.array.sum");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.array.sort {")) {
            strcpy(current_command, "Otia.array.sort");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.system.time {")) {
            strcpy(current_command, "Otia.system.time");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.system.date {")) {
            strcpy(current_command, "Otia.system.date");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.system.random {")) {
            strcpy(current_command, "Otia.system.random");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.system.sleep {")) {
            strcpy(current_command, "Otia.system.sleep");
            in_block = true;
        } else if (strstr(trimmed_line, "Otia.system.exit {")) {
            strcpy(current_command, "Otia.system.exit");
            in_block = true;
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
            // Nouvelles fonctionnalités
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
