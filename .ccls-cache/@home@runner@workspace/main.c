#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h> // Pour isspace

// --- Fonctions utilitaires ---

// Supprime les espaces et retours à la ligne en début et fin de chaîne
char* trim_whitespace(char* str) {
    char* end;

    // Supprime les espaces de début
    while (isspace((unsigned char)*str)) {
        str++;
    }

    if (*str == 0) { // Chaîne vide ou uniquement des espaces
        return str;
    }

    // Supprime les espaces de fin
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }
    *(end + 1) = 0; // Termine la chaîne

    return str;
}

// Extrait le contenu entre des délimiteurs (ex: '(', ')') ou '{', '}'
char* extract_delimited_content(const char* line, char start_char, char end_char) {
    const char* start = strchr(line, start_char);
    if (!start) return NULL;
    start++; // Passe le caractère de début

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

// --- Fonctions pour les opérations mathématiques ---
void execute_mathematics(const char* content_line) {
    // Supprime les espaces autour de l'opérateur et des opérandes
    char temp_content[1024];
    strncpy(temp_content, content_line, sizeof(temp_content) - 1);
    temp_content[sizeof(temp_content) - 1] = '\0';
    char* trimmed_content = trim_whitespace(temp_content);

    int a, b;
    char op;
    char operation_type[10]; // "add", "sub", "mul", "div"

    // Recherche de l'opérateur et extraction des opérandes
    // Exemple: "add = 4 + 5"
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
        } else {
            fprintf(stderr, "Erreur: Opération mathématique ou syntaxe invalide: %s\n", trimmed_content);
        }
    } else {
        fprintf(stderr, "Erreur: Syntaxe mathématique Otia.mathematique invalide: %s\n", trimmed_content);
    }
}

// --- Fonctions pour la gestion de fichiers ---

void create_file(const char* filename) {
    // Le nom de fichier est passé avec '\' au début, nous devons l'ignorer
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
    // Le nom de fichier est passé avec '\' au début, nous devons l'ignorer
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

    // Cherche le début du nom de fichier après '\' et le début du contenu entre parenthèses
    char* filename_start = strchr(arg_line, '\\');
    char* content_open_paren = strchr(arg_line, '(');

    if (filename_start && content_open_paren && content_open_paren > filename_start) {
        // Extrait le nom du fichier (entre '\' et '(')
        size_t filename_len = content_open_paren - (filename_start + 1);
        filename_part = (char*)malloc(filename_len + 1);
        strncpy(filename_part, filename_start + 1, filename_len);
        filename_part[filename_len] = '\0';

        // Extrait le contenu à écrire (entre parenthèses)
        content_part = extract_delimited_content(arg_line, '(', ')');

        if (filename_part && content_part) {
            // Supprime les guillemets simples autour du contenu si présents
            char* trimmed_content = trim_whitespace(content_part);
            if (trimmed_content[0] == '\'' && trimmed_content[strlen(trimmed_content) - 1] == '\'') {
                trimmed_content[strlen(trimmed_content) - 1] = '\0';
                memmove(trimmed_content, trimmed_content + 1, strlen(trimmed_content));
            }

            FILE* file = fopen(filename_part, "w"); // "w" pour écraser le contenu
            if (file) {
                fprintf(file, "%s", trimmed_content);
                printf("Contenu écrit dans '%s' avec succès.\n", filename_part);
                fclose(file);
            } else {
                perror("Erreur lors de l'écriture dans le fichier");
            }
        } else {
            fprintf(stderr, "Erreur: Syntaxe d'écriture de fichier invalide (nom de fichier ou contenu manquant).\n");
        }
    } else {
        fprintf(stderr, "Erreur: Syntaxe d'écriture de fichier invalide: %s\n", arg_line);
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
    char current_command[256] = ""; // Ex: "Otia.mathematique"
    bool in_block = false; // Vrai si on est dans un bloc de commande (après '{')

    while (fgets(line, sizeof(line), file) != NULL) {
        char* trimmed_line = trim_whitespace(line);

        if (strlen(trimmed_line) == 0) {
            continue; // Ignorer les lignes vides
        }

        // Détection du début d'un bloc de commande
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
        }
        // Détection de la fin d'un bloc
        else if (strcmp(trimmed_line, "}") == 0) {
            in_block = false;
            strcpy(current_command, ""); // Réinitialiser la commande
        }
        // Exécution du contenu à l'intérieur d'un bloc
        else if (in_block) {
            if (strcmp(current_command, "Otia.mathematique") == 0) {
                execute_mathematics(trimmed_line);
            } else if (strcmp(current_command, "Otia.create.fichier") == 0) {
                // Le nom de fichier est la ligne trimée, y compris le '\' initial
                create_file(trimmed_line);
            } else if (strcmp(current_command, "Otia.read.fichier") == 0) {
                // Le nom de fichier est la ligne trimée, y compris le '\' initial
                read_file(trimmed_line);
            } else if (strcmp(current_command, "Otia.write.fichier") == 0) {
                // Cette fonction doit analyser la ligne complète pour le nom du fichier et le contenu
                write_file(trimmed_line);
            } else {
                fprintf(stderr, "Avertissement: Ligne ignorée dans un bloc non géré: %s\n", trimmed_line);
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
