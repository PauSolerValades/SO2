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

#include "red-black-tree.h"

#define MAXVALUE 10
#define MAXCHAR 100 /*El fem servir com a limit temporal per recollir les paraules del fitxer.*/


/**
 *
 *  Main function. This function is an example that shows
 *  how the binary tree works.
 *
 */

void process_line(char *line)
{
    int i, j, is_word, len_line;
    char paraula[MAXCHAR];

    i = 0;

    len_line = strlen(line);

    /* Search for the beginning of a candidate word */

    while ((i < len_line) && (isspace(line[i]) || (ispunct(line[i])))) i++; 

    /* This is the main loop that extracts all the words */

    while (i < len_line)
    {
        j = 0;
        is_word = 1;

        /* Extract the candidate word including digits if they are present */

        do {

            if (isalpha(line[i])) //lugar donde poner apostrofes
                paraula[j] = line[i];
            else 
                is_word = 0;

            j++; i++;

            /* Check if we arrive to an end of word: space or punctuation character */

        } while ((i < len_line) && (!isspace(line[i])) && (!ispunct(line[i])));

        /* If word insert in list */

        if (is_word) {

            /* Put a '\0' (end-of-word) at the end of the string*/
            paraula[j] = 0;

            /* Print found word. Lugar dónde ponde el malloc */
            
            printf("%s\n", paraula);
        }

        /* Search for the beginning of a candidate word */

        while ((i < len_line) && (isspace(line[i]) || (ispunct(line[i])))) i++; 

    } /* while (i < len_line) */
}

int main(int argc, char **argv)
{
  FILE *fd;
  int maxnum, ct = 0;
  char word[MAXCHAR];

  rb_tree *tree;
  node_data *n_data;

  if (argc != 2)
  {
    printf("Usage: %s maxnum\n", argv[0]);
    exit(1);
  }

  maxnum = atoi(argv[1]); /* Converteix string a int */

  printf("Test with red-black-tree\n");

  /* Allocate memory for tree */
  tree = (rb_tree *) malloc(sizeof(rb_tree));

  /* Initialize the tree */
  init_tree(tree);
  
  /* Obrim el fitxer per lectura */
  fd = fopen("words", "r");
  
    if (!fd){
        printf("Could not open file\n");
        exit(1);
    }
    
    /* Aquesta es la funcio del fitxer extreu-paraules.c, on hi insertem el que volem vamos */
  char* auxWord; 
  int lenWord;
  while(fgets(word, MAXCHAR, fd) != NULL && ct < maxnum){
    
    /* Gestionem la memoria dinamica de les paraules:
        Com que fgets guarda word al stack, no podem usar-la directament, sino estariem usant la mateixa direccio de memoria tota la estona, cosa que faria petar l'arbre
     */
    
    lenWord = strlen(word); 
    printf("%d\n", lenWord);
    auxWord = malloc((lenWord+1)*sizeof(char)); /* Guardem un char mes per saber a on acaba l'string. TODO: mirar si canviant char per size of word*/
    for(int i=0; i<lenWord;i++) 
        auxWord[i] = word[i];
    auxWord[lenWord]= 0;
    
    
    
    /* Search if the key is in the tree1 */
    n_data = find_node(tree, auxWord); 
    

    if (n_data != NULL) {

        printf("La paraula ja era a l'arbre\n");
      /* If the key is in the tree increment 'num' */
      n_data->num_times++;
      
      /* Com que no la estem fent servir, necessitem allibrerar la memoria. */
      free(auxWord);
      
    } else {

      /* If the key is not in the tree, allocate memory for the data
       * and insert in the tree */

      n_data = malloc(sizeof(node_data));
      
      /* This is the key by which the node is indexed in the tree */
      n_data->key = auxWord;
      
      /* This is additional information that is stored in the tree */
      n_data->num_times = 1;
      
      printf("Key: %s\n", n_data->key);

      /* We insert the node in the tree */
      insert_node(tree, n_data);
    }
    
    ct++;
    
  }
  
  fclose(fd);
  
  /*TOTS ELS SEGMENTATIONS VENEN DE NO SABER GESTIONAR STRINGS LOCO Xd*/
  printf("Surto del bucle\n");
  
  printf("Root: %s", (tree->root)->data->key);
  
  /* We now dump the information of the tree to screen */

  ct = 0;
    /*
  for(a = 1; a <= MAXVALUE; a++)
  {
    n_data = find_node(tree, a);

    if (n_data) { 
      printf("El numero %d apareix %d cops a l'arbre.\n", a, n_data->num_times);
      ct += n_data->num_times;
    }
  }

  printf("Nombre total que vegades que s'ha accedit a l'arbre: %d\n", ct);
  
  */
    
  /* Delete the tree */
  delete_tree(tree);
  free(tree);

  return 0;
}

