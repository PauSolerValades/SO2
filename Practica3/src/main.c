/**
 *
 * Practica 3 
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 

#include "red-black-tree.h"

#define MAXCHAR      100
#define MAGIC_NUMBER 0x01234567

/**
 * 
 *  Menu
 * 
 */

int menu() 
{
    char str[5];
    int opcio;

    printf("\n\nMenu\n\n");
    printf(" 1 - Creacio de l'arbre\n");
    printf(" 2 - Emmagatzemar arbre a disc\n");
    printf(" 3 - Llegir arbre de disc\n");
    printf(" 4 - Consultar informacio de l'arbre\n");
    printf(" 5 - Sortir\n\n");
    printf("   Escull opcio: ");

    fgets(str, 5, stdin);
    opcio = atoi(str); 

    return opcio;
}

char* retallar_strings(char* string){

    char* newString;
    int lenString;
    
    lenString = strlen(string) - 1;
    newString = malloc((lenString + 1) * sizeof(char));
    for(int i = 0; i <= lenString; i++) 
        newString[i] = string[i];
    newString[lenString] = 0;
    
    return newString;
    
}

void diccionari_arbre(rb_tree* tree, char* diccionari){
    
    /**
    * 
    * diccionari_arbre indexa les paraules del fitxer 'words' a l'arbre passat per paràmetre.
    * Arguments: rb_tree* tree
    * Retorn: void
    * 
    */
        
    FILE *fd;
    int ct = 0;
    char* auxWord; 
    char word[MAXCHAR];
    
    node_data *n_data;
  
    /* Obrim el fitxer per lectura */
    fd = fopen(diccionari, "r");

    if (!fd){
        printf("Could not open file: words in DICCIONARI_ARBRE\n");
        exit(1);
    }
    
    /* Aquesta es la funcio del fitxer extreu-paraules.c, adaptada per poder usar-la */

    while(fgets(word, MAXCHAR, fd) != NULL){

        /**
         * 
         * Gestionem la memoria dinamica de les paraules:
         * Com que fgets guarda word al stack, no podem usar-la directament, sino estariem usant la mateixa direccio 
         * de memoria tota la estona, cosa que faria petar l'arbre.
         * 
        */
        
        auxWord = retallar_strings(word); 

        /* Search if the key is in the tree */
        n_data = find_node(tree, auxWord); 

        if (n_data != NULL) {
        
            free(auxWord);  /* Com que no la estem fent servir, necessitem allibrerar la memoria */
            
        } else {

            /* If the key is not in the tree, allocate memory for the data
            * and insert in the tree */

            n_data = malloc(sizeof(node_data));
            
            /* This is the key by which the node is indexed in the tree */
            n_data->key = auxWord;
            
            /* This is additional information that is stored in the tree */
            n_data->num_times = 0;
            
            n_data->len = strlen(auxWord);
            
            /* We insert the node in the tree */
            insert_node(tree, n_data);
        }

        ct++;
    }
    
    //printf("Arbre creat amb %d paraules!\n", ct);
    
    fclose(fd);
}

void search_words(rb_tree* tree, char* filename){
    
    /**
     *
     * 'search_words' extreu les paraules del fitxer 'filename' i, per cada paraula,
     * recorre l'arbre comparant-la amb les keys dels nodes. Si troba una coincidencia
     * augmenta el num_times del node amb la key corresponent en 1.
     * Arguments:
     *  rb_tree* tree: arbre red_black_tree.c proporcionat a la pràctica
     *  char* filename: nom del fitxer que extraurem les paraules.
     * Retorn: void
     *
     */

    FILE *fp;
    char line[MAXCHAR], paraula[MAXCHAR];
    int i, j, is_word, len_line, apostrof = 39;
    node_data *temp;
    
    fp = fopen(filename, "r");

    if (!fp) {
        printf("Could not open file: %s in SEARCH_WORDS\n", filename);
        exit(1);
    }

    /* Search for the beginning of a candidate word */
    while(fgets(line, MAXCHAR, fp)){
        
        i = 0;
        len_line = strlen(line);
        
        while ((i < len_line) && (isspace(line[i]) || (ispunct(line[i])))) i++; 

        /* This is the main loop that extracts all the words */

        while (i < len_line)
        {
            j = 0;
            is_word = 1;

            /* Extract the candidate word including digits if they are present */

            do {

                if (isalpha(line[i]) || line[i] == apostrof)
                    paraula[j] = line[i];
                else 
                    is_word = 0;

                j++; i++;

                /* Check if we arrive to an end of word: space or punctuation character */

            } while ((i < len_line) && (!isspace(line[i])) && (!ispunct(line[i]) || i == apostrof));

            /* If word insert in list */

            if (is_word) {

                /* Put a '\0' (end-of-word) at the end of the string*/
                paraula[j] = 0;
                
                /* search 'paraula' in the tree and if found, increment 'num_times' */
                temp = find_node(tree, paraula);
                
                if (temp != NULL) {
                    (temp->num_times++);
                }
                
            }

            /* Search for the beginning of a candidate word */

            while ((i < len_line) && (isspace(line[i]) || (ispunct(line[i])))) i++; 

        }
    
    }
    
    fclose(fp);
    
}

rb_tree* practica2(char* str1, char* str2)
{
    FILE *data;
    char filename[MAXCHAR];
    char* auxFilePath;
    int control = 0;
  
    rb_tree *tree;
    tree = (rb_tree *) malloc(sizeof(rb_tree));
    init_tree(tree);
    
    /* Omplim l'arbre amb les paraules del fitxer "words" */
    diccionari_arbre(tree, str1);
    
    data = fopen(str2,"r"); /* obrim el fitxer amb tots els camins dels fitxers d'on extraurem les dades */

    if (!data) {
        printf("Could not open file: llista.cfg in MAIN\n");
        exit(1);
    }
    
    //printf("S'estan recorrent els fitxers, esperi si us plau.");
    
    while(fgets(filename, MAXCHAR, data)){
        /* Busquem les paraules al arbre de tots els diccionaris */
        if(control==0){
            control++; /* El primer element de llista.cfg és un int del nombre de fitxers que hi ha al document. Ens els saltem perque no hi podrem accedir */
        }else{
           
            /* Retallem l'string per poder-lo passar a la funcio i que llegeixi be el path */
            auxFilePath = retallar_strings(filename);
            
            search_words(tree, auxFilePath);
            
            free(auxFilePath);
        }
    }
        
    /* printejem el diccionari ordenat alfabèticament */
    print_tree_inorder(tree->root);
    
    fclose(data); /* tanca llista.cfg */
    
    return tree;

}

void guardar_arbre(char* filename, rb_tree* tree){

    FILE* fd;
    
    fd = fopen(filename, "w");
    
    if (!fd) {
        printf("Could not open file: %s in GUARDAR_ARBRE\n", filename);
        return;
    }
    
    int magic = MAGIC_NUMBER;
    int num_elements = tree->num_elements;
    
    /* Aixo ja es binari */
    fwrite(&magic, sizeof(int), 1, fd);
    fwrite(&tree, sizeof(int), 1, fd);
    
    fclose(fd);
    
}
/**
 * 
 *  Main procedure
 *
 *
 */

int main(int argc, char **argv)
{
    char str1[MAXCHAR], str2[MAXCHAR];
    int opcio;
    
    rb_tree *tree;
    tree = NULL;
    
    if (argc != 1)
        printf("Opcions de la linia de comandes ignorades\n");

    do {
        opcio = menu();
        printf("\n\n");

       /* Feu servir aquest codi com a pantilla */

        switch (opcio) {
            case 1:
                printf("Fitxer de diccionari de paraules: ");
                fgets(str1, MAXCHAR, stdin);
                str1[strlen(str1)-1]=0;

                printf("Fitxer de base de dades: ");
                fgets(str2, MAXCHAR, stdin);
                str2[strlen(str2)-1]=0;
                
                tree = practica2(str1, str2);

                printf("Fucking elements: %d\n", tree->num_elements);
                break;

            case 2:
                printf("Nom de fitxer en que es desara l'arbre: ");
                fgets(str1, MAXCHAR, stdin);
                str1[strlen(str1)-1]=0;

                if(tree !=NULL){
                    guardar_arbre(str1, tree);
                
                }else{ printf("L'arbre no ha estat creat.\n"); } 
                break;

            case 3:
                printf("Nom del fitxer que conte l'arbre: ");
                fgets(str1, MAXCHAR, stdin);
                str1[strlen(str1)-1]=0;

                /* Falta codi */

                break;

            case 4:
                printf("Paraula a buscar o polsa enter per saber la paraula que apareix mes vegades: ");
                fgets(str1, MAXCHAR, stdin);
                str1[strlen(str1)-1]=0;

                /* Falta codi */

                break;

            case 5:
                
                if(tree != NULL){
                    delete_tree(tree);
                    free(tree);
                    
                }else{ printf("L'arbre no ha estat creat.\n"); }

                break;

            default:
                printf("Opcio no valida\n");

        } /* switch */
    }
    while (opcio != 5);

    return 0;
}

