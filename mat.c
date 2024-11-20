#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define rows 50
#define cols 50
#define BUFFER_SIZE 1024

extern char *optarg;

typedef struct {
    int b_mode_binaire; // gere le type du fichier
    int c_mode_create;  // = 1 si demande de creation, 0 sinon
    int dimension;      // dimension de la matrice
    int a_mode_print;   // = 1 si demande l'affichage du fichier
    int d_fourni;       // = 1 si la dimension est precisee, 0 sinon
    char file_name[30]; // nom du fichier
    int f_fourni;       // = 1 si le nom du fichier est fourni
} option;

void genere_mat(int d, int m[d][d]) {
    srand(time(NULL));
    for(int i=0; i<d; i++)
        for(int j=0; j<d; j++)
            m[i][j] = rand()%100;
}
void print_matrice(int d, int m[d][d]) {
    for(int i=0; i<d; i++) {
        for(int j=0; j<d; j++)
            printf("%d ",m[i][j]);
        printf("\n");
    }
}

void save_file_bin(int d, int m[d][d], char *file) {
    int fb = open(file, O_WRONLY | O_CREAT | O_TRUNC);
    if(fb == -1) {
        perror("Erreur lors de l'ouverture du fichier binaire.");
        exit(-1);
    }
    size_t b = write(fb, m, sizeof(int) * d * d);
    if(b == -1) {
        perror("Erreur lors de l'ecriture sur le fichier binaire.");
    }
    
    close(fb);
}
void read_file_bin(int d, int m[d][d], char *file) {
    int fb = open(file, O_RDONLY);
    if(fb == -1) {
        perror("Erreur lors de l'ouverture du fichier binaire.");
        exit(-1);
    }
    size_t b = read(fb, m, sizeof(int) * d * d);
    if(b == -1) {
        perror("Erreur lors de l'ecriture sur le fichier binaire.");
    }
    
    close(fb);
}


void save_file_txt(int d, int m[d][d], char *file) {
    int ft = open(file, O_WRONLY | O_CREAT | O_TRUNC);
    if(ft == -1) {
        perror("Erreur lors de l'ouverture du ficher texte");
        exit(-1);
    }
    
    char ligne[1000];
    for(int i=0; i<d; i++) {
        int pos = 0;
        for(int j=0; j<d; j++) {
            pos += sprintf(ligne + pos, "%d ", m[i][j]);
        }
        ligne[pos++] = '\n';
        size_t b = write(ft, ligne, pos);
        if(b == -1) {
            perror("Erreur lors de l'ecriture sur le ficher texte");
            break;
        }
    }

    close(ft);
}
void read_file_txt(int d, int m[d][d], char *file) {
    int ft = open(file, O_RDONLY);
    if (ft == -1) {
        perror("Erreur lors de l'ouverture du fichier texte");
        return;
    }
    
    char buffer[BUFFER_SIZE];
    int row = 0, col = 0;
    int number = 0;
    int pos = 0;
    size_t b;
    
    while ((b = read(ft, buffer, BUFFER_SIZE)) > 0) {
        for (int i = 0; i < b; i++) {
            if (buffer[i] == ' ' || buffer[i] == '\n') {
                if (pos > 0) {
                    m[row][col++] = number;
                    number = 0;
                    pos = 0;
                    
                    if (buffer[i] == '\n' || col == d) {
                        row++;
                        col = 0;
                    }
                }
            } else if (buffer[i] >= '0' && buffer[i] <= '9') {
                number = number * 10 + (buffer[i] - '0');
                pos++;
            }
        }
    }
    
    if (b == -1) {
        perror("Erreur lors de la lecture du fichier texte");
    }
    
    close(ft);
}

void menu_help() {
    printf("Usage:\n");
    printf("-c 'Permet de demander la creation d'une matrice aleatoire'\n");
    printf("-a 'pour induquer au programme d'afficher le contenu du fichier'\n");
    printf("-d entier 'spécifie la dimension de la matrice exemple -d 10 permet d'indiquer que c'est une matrice 10x10'\n");
    printf("-b 'indique que c'est en mode binaire (mode par defaut)'\n");
    printf("-t 'indique le format du fichier est en mode texte'\n");
    printf("-f nomfichier 'specifie le nom du fichier'\n");
    printf("Les usages possibles du programmes:\n");
    printf("\t./matrix -c -d 10 -f fichiermatrice.bin\n");
    printf("\t./matrix -c -d 10 -f fichiermatrice.bin -t\n");
    printf("\t./matrix -a -d 10 -f fichiermatrice.bin -t\n");
}

int main(int argc, char *argv[]) {
    
    if(argc<4) {
        printf("Nombre d'argument insuffisant\n");
        menu_help();
        exit(1);
    }

    option op;
    int opt;
    op.b_mode_binaire = 1;
    op.a_mode_print = 0;
    op.d_fourni = 0;
    op.c_mode_create = 0;
    op.dimension = 10;
    op.f_fourni = 0;
    while((opt = getopt(argc, argv, ":cad:f:tb")) != -1) {
        switch(opt) {
            case 'd':
                op.d_fourni = 1;
                op.dimension = atoi(optarg);
                break;
            case 'c':
                op.c_mode_create = 1;
                break;
            case 'b':
                op.b_mode_binaire = 1;
                break;
            case 't':
                op.b_mode_binaire = 0;
                break;
            case 'f':
                strcpy(op.file_name, optarg);
                op.f_fourni = 1;
                break;
            case 'a':
                op.a_mode_print = 1;
                break;
            case ':':
                printf("L'option %c necessite un argument", opt);
                break;
            case '?':
                fprintf(stderr, "Option inconnue ou argument manquant\n");
                return 1;
        }
    }

    int Mat[op.dimension][op.dimension];
    if(op.f_fourni) {
        if(op.c_mode_create) {
            genere_mat(op.dimension, Mat);
            if(op.b_mode_binaire && strstr(op.file_name, "bin"))
                save_file_bin(op.dimension, Mat, op.file_name);
            else
                if(!op.b_mode_binaire && strstr(op.file_name, "txt"))
                    save_file_txt(op.dimension, Mat, op.file_name);
                else {
                    printf("Fichier incompatible au mode de creation !!!\n");
                    return 0;
                }
            printf("Matrice generee et sauvegardee dans %s avec succes.\n\n",op.file_name);
        }
        if(op.a_mode_print) {
            if(op.b_mode_binaire && strstr(op.file_name, "bin"))
                read_file_bin(op.dimension, Mat, op.file_name);
            else
                if(!op.b_mode_binaire && strstr(op.file_name, "txt"))
                    read_file_txt(op.dimension, Mat, op.file_name);
                else {
                    printf("Fichier incompatible au mode d'affichage !!!\n");
                    return 0;
                }
            printf("\tAffichage du fichier %s:\n\n", op.file_name);
            print_matrice(op.dimension, Mat);
        }
    }

    return 0;
}