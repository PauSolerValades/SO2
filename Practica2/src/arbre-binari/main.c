/**
 *
 * Main file 
 * 
 * This file is an example that uses the red-black-tree.c functions.
 *
 * Lluis Garrido, July 2019.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>  


#include "red-black-tree.h"

#define MAXVALUE 10
#define MAXCHAR 100 /*El fem servir com a limit temporal per recollir les paraules del fitxer.*/

void diccionari_arbre(rb_tree* tree, int maxnum){
    
    /* diccionari arbre omple l'arbre amb les paraules del fitxer diccionari (el que ja teniem a la practica) */  
    FILE *fd;
    int ct = 0, lenWord;
    char* auxWord; 
    char word[MAXCHAR];
    
    node_data *n_data;
  
    /* Obrim el fitxer per lectura */
    fd = fopen("words", "r");

    if (!fd){
        printf("Could not open file: words in DICCIONARI_ARBRE\n");
        exit(1);
    }

    printf("Omplint l'arbre amb el diccionari...\n");
    
    /* Aquesta es la funcio del fitxer extreu-paraules.c, on hi insertem el que volem vamos */

    while(fgets(word, MAXCHAR, fd) != NULL /*&& ct < maxnum*/){

        /* 
         * Gestionem la memoria dinamica de les paraules:
         * Com que fgets guarda word al stack, no podem usar-la directament, sino estariem usant la mateixa direccio 
         * de memoria tota la estona, cosa que faria petar l'arbre
         * 
        */

        lenWord = strlen(word) - 1; 
        //printf("%d\n", lenWord);
        
        auxWord = malloc((lenWord + 1) * sizeof(char)); /* Guardem un char mes per saber a on acaba l'string. TODO: mirar si canviant char per size of word*/
        for(int i = 0; i <= lenWord; i++) 
            auxWord[i] = word[i];
        auxWord[lenWord]= 0;

        /* Search if the key is in the tree1 */
        n_data = find_node(tree, auxWord); 


        if (n_data != NULL) {

            printf("La paraula ja era a l'arbre\n");
        
            /* n_data->num_times++; If the key is in the tree increment 'num'*/
            free(auxWord);  /* Com que no la estem fent servir, necessitem allibrerar la memoria. */
            
        } else {

            /* If the key is not in the tree, allocate memory for the data
            * and insert in the tree */

            n_data = malloc(sizeof(node_data));
            
            /* This is the key by which the node is indexed in the tree */
            n_data->key = auxWord;
            
            /* This is additional information that is stored in the tree */
            n_data->num_times = 0;
            
            //printf("Key: %s\n", n_data->key);
            //printf("len_key: %ld\n", strlen(n_data->key));

            /* We insert the node in the tree */
            insert_node(tree, n_data);
        }

        ct++;
    }
    
    printf("Arbre creat amb %d paraules!\n", ct);
    
    fclose(fd);
}

void search_words(rb_tree* tree, char* filename){
    
    /* search_words ARA extreu les paraules del fitxer filename i les printeja
    HA DE: crear nodes per bucar-los al arbre que se li passa per parametre
    Es la funció que extreu les paraules sense separar els números.   
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
                
                temp = find_node(tree, paraula);
                
                if (temp != NULL) {
                    (temp->num_times++);
                    
                    //printf("%s\n", paraula);
                    //printf("%d\n", find_node(tree, paraula)->num_times);
                }
                
                
            }

            /* Search for the beginning of a candidate word */

            while ((i < len_line) && (isspace(line[i]) || (ispunct(line[i])))) i++; 

        } /* while (i < len_line) */
    
    }
    
    fclose(fp);
    
}


int main(int argc, char **argv)
{
    FILE *data;
    char filename[MAXCHAR];
    char* auxFilePath; 
    int maxnum, /*numFitxers,*/ control = 0, lenFilePath;
    
    if (argc != 2)
    {
        printf("Usage: %s maxnum\n", argv[0]);
        exit(1);
    }
  
    rb_tree *tree;
    tree = (rb_tree *) malloc(sizeof(rb_tree));
    init_tree(tree);

    maxnum = atoi(argv[1]); /* Converteix string a int */

    /* Omplim l'arbre amb les paraules del fitxer "words" */
    diccionari_arbre(tree, maxnum);
    
    data = fopen("llista.cfg","r"); /* obrim el fitxer amb tots els camins dels fitxers d'on extraurem les dades */

    if (!data) {
        printf("Could not open file: llista.cfg in MAIN\n");
        exit(1);
    }
    
    
    while(fgets(filename, MAXCHAR, data)){
        /* Busquem les paraules al arbre de tots els diccionaris */
        if(control==0){
            //numFitxers = atoi(filename); /* Es possible que aixo falli uwu */
            control++; /* El primer element de llista.cfg és un int del nombre de fitxers que hi ha al document. ens els saltem perque no hi podrem accedir */
        }else{
            /* Retallem l'string per poder-lo passar a la funcio i que llegeixi be el path */
            lenFilePath = strlen(filename) - 1; 
            //printf("Len: %d\n", lenFilePath);
            auxFilePath = malloc((lenFilePath + 1)*sizeof(char)); /* Guardem un char mes per saber a on acaba l'string. TODO: mirar si canviant char per size of word*/
            for(int i=0; i<=lenFilePath;i++) 
                auxFilePath[i] = filename[i];
            auxFilePath[lenFilePath]= 0;
            
            //printf("Len: %ld\n", strlen(auxFilePath));
            //printf("File %s\n", auxFilePath);
            
            search_words(tree, auxFilePath);
            
            free(auxFilePath);
        }
    }
    
//     print_tree_inorder(tree->root);
    
    fclose(data); /* tanca llista.cfg */
    /* Delete the tree */
    delete_tree(tree);
    free(tree);

    return 0;
}


