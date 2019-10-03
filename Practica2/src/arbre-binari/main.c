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

int main(int argc, char **argv)
{
  FILE *fd;
  int a, maxnum, ct = 0, strLen;
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
  /*
    strLen = strlen(word)-1;
    char wordNais[strLen];
    *wordNais[strLen] = word;
        */

  while(fgets(word, MAXCHAR, fd) != NULL && ct < maxnum){
    
    /* Search if the key is in the tree */
    n_data = find_node(tree, *word); 
    
    /*printf("%s, %d",word,ct);*/

    if (n_data != NULL) {

      /* If the key is in the tree increment 'num' */
      n_data->num_times++;
      
    } else {

      /* If the key is not in the tree, allocate memory for the data
       * and insert in the tree */

      n_data = malloc(sizeof(node_data));
      
      /* This is the key by which the node is indexed in the tree */
      n_data->key = *word;
      
      /* This is additional information that is stored in the tree */
      n_data->num_times = 1;

      /* We insert the node in the tree */
      insert_node(tree, n_data);
    }
    
    ct++;
    
  }
  
  /*TOTS ELS SEGMENTATIONS VENEN DE NO SABER GESTIONAR STRINGS LOCO Xd*/
  printf("%s", (tree->root)->data->key);
  
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
  
  /* Delete the tree */
  delete_tree(tree);
  free(tree);

  return 0;
}

