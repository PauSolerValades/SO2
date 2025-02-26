/**
 *
 * Practica 5 - v1
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <sys/sysinfo.h>
#include <fcntl.h>


#include "red-black-tree.h"
#include "tree-to-mmap.h"
#include "dbfnames-mmap.h"

#define MAXCHAR      100
#define MAGIC_NUMBER 0x01234567
#define NUM_FILS     4

struct args_fil {
    char* data;
    rb_tree* tree;
};

struct args_fils {
    FILE* data;
    rb_tree* tree;
    int num_fitxers;
};

pthread_t fil;

pthread_mutex_t mutex_write = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_join = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_jj = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond;
int control=0, control_2=0;

/**
 * 
 *  Menu
 * 
 */

int menu() 
{
    char str[6];
    int opcio=0;

    printf("\n\nMenu\n\n");
    printf(" 1 - Creacio de l'arbre amb un fil\n");
    printf(" 2 - Creacio de l'arbre amb múltiples fils\n");
    printf(" 3 - Emmagatzemar arbre a disc\n");
    printf(" 4 - Llegir arbre de disc\n");
    printf(" 5 - Consultar informacio de l'arbre\n");
    printf(" 6 - Sortir\n\n");
    printf("   Escull opcio: ");

    if(fgets(str, 6, stdin))
        opcio = atoi(str); 

    return opcio;
}

char* retallar_strings(char* string){
    
    /*
     * Funció que usem després d'fgets per allocar l'string a memòria.
     * ARGUMENTS: char* string: string SENSE EL 0 AL FINAL.
     * RETURN: string en memòria dinàmica i amb el 0.
     */

    char* newString;
    int lenString;
    
    lenString = strlen(string) - 1;
    newString = malloc((lenString+1) * sizeof(char));
    for(int i = 0; i <= lenString; i++) 
        newString[i] = string[i];
    newString[lenString] = 0;
    
    return newString;
    
}

void diccionari_arbre(rb_tree* tree, char* word){
    
    /**
    * 
    * diccionari_arbre indexa les paraules del fitxer 'words' a l'arbre passat per paràmetre.
    * ARGUMENTS: rb_tree* tree
    * RETURN: void
    * 
    */
    
    char* auxWord;
    node_data *n_data;
        
    /*
    printf("Word: %s\n", word);
    
    for(int i = 0; i<=strlen(word); i++)
        printf("%c ", word[i]);
    
    printf("\n");
    */
    
    /* Search if the key is in the tree */
    if (find_node(tree, word) == NULL) {
        
        /* If the key is not in the tree, allocate memory for the data
        * and insert in the tree */

        auxWord = retallar_strings(word);
        
        n_data = malloc(sizeof(node_data));
        
        /* This is the key by which the node is indexed in the tree */
        n_data->key = auxWord;
        
        /* This is additional information that is stored in the tree */
        n_data->num_times = 0;
        
        n_data->len = strlen(auxWord);
        
        /* We insert the node in the tree */
        insert_node(tree, n_data);
        
    }
}

void search_words(rb_tree* tree, char* filename){
    
    /**
     *
     * 'search_words' extreu les paraules del fitxer 'filename' i, per cada paraula,
     * recorre l'arbre comparant-la amb les keys dels nodes. Si troba una coincidencia
     * augmenta el num_times del node amb la key corresponent en 1.
     * ARGUMENTS:
     *  rb_tree* tree: arbre red_black_tree.c proporcionat a la pràctica
     *  char* filename: nom del fitxer que extraurem les paraules.
     * RETURN: void
     *
     */

    FILE *fp;
    char line[MAXCHAR], paraula[MAXCHAR];
    int i, j, is_word, len_line, apostrof = 39;
    node_data* temp;
    
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
                    pthread_mutex_lock(&mutex_jj);
                    temp->num_times++;
                    pthread_mutex_unlock(&mutex_jj);
                }
                                        
            }

            /* Search for the beginning of a candidate word */

            while ((i < len_line) && (isspace(line[i]) || (ispunct(line[i])))) i++; 

        }
    
    }
    
    fclose(fp);
    
}

void update_arbre(node *current, node *current_copy) {
    
    /*
     * Funcio que actualitza recursivament un arbre a partir d'un altre passant
     * per parametre els roots dels dos.
     * ARGUMENTS: dos node*, els roots del dos arbres
     * RETURN: void
     */
    
    if (current == NULL){
        return;
    }
    else{
        if (current->left != NIL)
            update_arbre(current->left, current_copy->left);
        
        current_copy->data->len = current->data->len;
        current_copy->data->key = current->data->key;
        current_copy->data->num_times = current->data->num_times;
        
        if (current->right != NIL)
            update_arbre(current->right, current_copy->right);
    }
}

void print_arbre(node *current) {
    
    /*
     * Funcio que imprimeix l'arbre per pantalla fent un dfs in-order
     * ARGUMENTS: node*, la root de l'arbre a imprimir.
     * RETURN: void
     */
 
    if (current == NULL){
        return;
    }
    else{
        if (current->left != NIL)
            print_arbre(current->left);
        
        int len = current->data->len;
        char* key = current->data->key;
        int num_times = current->data->num_times;
        
        printf("Key: %s\t\t Len: %d\t Times: %d\n", key, len, num_times);
        
        if (current->right != NIL)
            print_arbre(current->right);
    }
}

void nodes_tree_inorder(node *current, FILE* fd) {
    
    /*
     * Funcio que imprimeix l'arbre en un fitxer fent un dfs in-order
     * ARGUMENTS: node*, la root de l'arbre a imprimir.
     * RETURN: void
     */
 
    if (current == NULL){
        return;
    }
    else{
        if (current->left != NIL)
            nodes_tree_inorder(current->left, fd);
        
        int len = current->data->len;
        char* key = current->data->key;
        int num_times = current->data->num_times;
        
        printf("Key: %s\t\t Len: %d\t Times: %d\n", key, len, num_times);
        
        fwrite(&len, sizeof(int), 1, fd);
        fwrite(key, len*sizeof(char), 1, fd);
        fwrite(&num_times, sizeof(int), 1, fd);
        
        
        if (current->right != NIL)
            nodes_tree_inorder(current->right, fd);
    }
}

void guardar_arbre(char* filename, rb_tree* tree){
    
    /*
     * Funcio que emmagatzema l'arbre a memoria
     * ARGUMENTS: l'arbre a guardar i el nom del fitxer on el volem guardar
     * RETURN: void
     */

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
    fwrite(&num_elements, sizeof(int), 1, fd);
    
    nodes_tree_inorder(tree->root, fd);
    
    fclose(fd);
    
}

rb_tree* recuperar_arbre(char* filename){
    
    /*
     * Funcio que recupera l'arbre de memoria
     * ARGUMENTS: el nom del fitxer on esta guardat l'arbre
     * RETURN: rb_tree*, l'abre que recuperem de memoria
     */
    
    FILE* fd;
    int magic, MAGIC, num_nodes, len_key=0, num_times=0;    
    
    
    rb_tree *tree;
    tree = (rb_tree *) malloc(sizeof(rb_tree));
    init_tree(tree);
    
    fd = fopen(filename, "r");
    
    if (!fd) {
        printf("Could not open file: %s in RECUPERAR_ARBRE\n", filename);
        return NULL;
    }
    
    if(fread(&magic, sizeof(int), 1, fd)){
    
         printf("MAGIC DEL FITXER: %d\n", magic);
         MAGIC = MAGIC_NUMBER;    

         if(magic != MAGIC){
         
            printf("Magic error");
            fclose(fd);
            return NULL;
        }
    }
    
    
    if(fread(&num_nodes, sizeof(int), 1, fd)){
        
        printf("Num nodes: %d\n",num_nodes);
        tree->num_elements = num_nodes;
        printf("%d\n", tree->num_elements);
    
    }
    
    for(int i = 0; i < num_nodes; i++){
        if(fread(&len_key, sizeof(int), 1, fd)){}

        char *key = malloc(sizeof(char)*len_key + 1);
        if(fread(key,sizeof(char)*len_key, 1, fd))
            key[len_key] = '\0';
        
        if(fread(&num_times, sizeof(int), 1, fd)){}
        
        node_data* tmp = malloc(sizeof(node_data));
        
        tmp->len = len_key;
        tmp->key = key;
        tmp->num_times = num_times;
        
        printf("Key: %s\t\t Len: %d\t Times: %d\n", key, len_key, num_times);
        
        insert_node(tree, tmp);
     }
    
    fclose(fd);
    
    return tree;
    
}

void top_1(rb_tree *tree){
 
    /*
     * Funcio que impreix el top 1 de paraules que apareixen mes vegades
     * ARGUMENTS: l'arbre a consultar
     * RETURN: void
     */
    
    FILE *fd;
    
    fd = fopen("tmp.txt", "w");
    
    print_tree_inorder_file(tree->root, fd);
    
    fclose(fd);
}

/**
 * 
 *  Metodes dels threads
 *
 */

void *fil_fn(void *arg){
    
    /*
     * Funcio del thread quan nomes en tenim un, obre el fitxer d'on extraurem les
     * dades, copia l'arbre del pare en un arbre propi del thread i l'actualitza
     * amb les dades adients. Despres, tanca el fitxer
     * ARGUMENTS: void *arg, en aquest cas es un struct definit a l'inici del programa
     * RETURN: void*
     */
    
    FILE* data;
    int control = 0;
    char filename[MAXCHAR];

    struct args_fil *arguments = (struct args_fil *) arg;
    
    data = fopen(arguments->data,"r"); /* obrim el fitxer amb tots els camins dels fitxers d'on extraurem les dades */

    if (!data) {
        return NULL;
    }
    
    rb_tree *tree_fil;
    tree_fil = (rb_tree *) malloc(sizeof(rb_tree));
    init_tree(tree_fil);
    
    tree_fil->root = arguments->tree->root;
    tree_fil->num_elements = arguments->tree->num_elements;
    
    while(fgets(filename, MAXCHAR, data)){
        if(control==0){
            control++;
            
        }else{
           
            filename[strlen(filename)-1] = 0;
            control++;
            search_words(tree_fil, filename);
            
        }
    }
    
    fclose(data);
    
    return NULL;
}

rb_tree* crear_arbre_fil(char* str1, char* str2){
    
    /*
     * Funcio que indexa les paraules del diccionari i llavors crea un fil secundari 
     * perque sigui aquest el que faci el search_words dels fitxers. Despres, torna a
     * unir el fil secundari al principal
     * ARGUMENTS: char* str1, el diccionari, i char* str2, el fitxer amb els noms dels
     * fitxers a llegir
     * RETURN: rb_tree*
     */
    
    FILE *diccionari;
    char word[MAXCHAR];
    int err;
    void *tret;
    struct args_fil *arguments;
    
    /* Reservem memoria per l'struct que li passarem al fil */
    arguments = malloc(sizeof(struct args_fil));

    rb_tree *tree;
    tree = (rb_tree *) malloc(sizeof(rb_tree));
    init_tree(tree);
    
    /* Obrim el diccionari que ens passin */
    diccionari = fopen(str1, "r");
    
    if (!diccionari) {
        printf("Could not open file: %s in MAIN\n", str1);
        fclose(diccionari);
        return tree;
    }
    
    /* Omplim l'arbre amb les paraules del fitxer "diccionari" */
    while(fgets(word, MAXCHAR, diccionari) != NULL)
        diccionari_arbre(tree, word);
    
    fclose(diccionari);
    
    
    /* Creem els fils */
    
    arguments->data = str2;
    arguments->tree = tree;
    
    err = pthread_create(&fil, NULL, fil_fn, (void *) arguments);
    if (err != 0) {
        printf("Error al crear el fil secundari.\n");
        exit(1);
    }
    
    /* Ajuntem els fils */
    
    pthread_join(fil, &tret);
    
    return tree;
}

void *fils_fn(void *arg){
    
    /*
     * Funcio dels fils secundaris, crea un arbre pel fil en el que es trobi, i copia
     * el passat per parametre, aleshores mitjançant un comptador, agafa el fitxer que
     * li pertoca recorrer en l'fgets i en compta el numero de vegades que apareix
     * cada paraula. Despres, actualitza l'arbre del pare amb les dades noves.
     * Tant l'augment del comptador com l'update de l'arbre equeeixen de monitors per
     * evitar problemes entre fils
     * ARGUMENTS: struct d'on traiem el fitxer, l'arbre i el nombre de fitxers
     * RETURN: void
     */
    
    int tmp;
    char filename[MAXCHAR];
    struct args_fils *arguments = (struct args_fils *) arg;
    
    rb_tree *tree_fil;
    
    tree_fil = (rb_tree *) malloc(sizeof(rb_tree));
    
    init_tree(tree_fil);
    
    tree_fil->root = arguments->tree->root;
    tree_fil->num_elements = arguments->tree->num_elements;
  
    while(1){
        
        pthread_mutex_lock(&mutex_write);
        
        tmp = control_2;
        control_2++;
        if(fgets(filename, MAXCHAR, arguments->data) == NULL){}
        
        printf("ID: %ld Filename: %s Control: %d \n", syscall(SYS_gettid), filename, control);
        
        pthread_mutex_unlock(&mutex_write);
        
        if(tmp < arguments->num_fitxers){ 
            filename[strlen(filename)-1]=0;
            
            search_words(tree_fil, filename);
            
        }else{
            break; 
        }

    }
    
    print_arbre(tree_fil->root);
    
    pthread_mutex_lock(&mutex_join);
    update_arbre(arguments->tree->root, tree_fil->root);
    pthread_mutex_unlock(&mutex_join);

    return ((void *) 0); 
    
}

rb_tree* crear_arbre_fils(char* str1, char* str2){
    
    /*
     * Funcio que indexa les paraules del diccionari i llavors crea n fils secundaris 
     * perque siguin aquests els que faci el search_words dels fitxers. Despres, torna a
     * unir els fils secundaris al principal
     * ARGUMENTS: char* str1, el diccionari, i char* str2, el fitxer amb els noms dels
     * fitxers a llegir
     * RETURN: rb_tree*
     */
    
    FILE *diccionari, *data;
    char word[MAXCHAR], num[MAXCHAR];
    int num_fitxers;
    pthread_t fils[NUM_FILS];
    struct args_fils *arguments;

    rb_tree *tree;
    tree = (rb_tree *) malloc(sizeof(rb_tree));
    init_tree(tree);
    
    /* Obrim el diccionari que ens passin */
    diccionari = fopen(str1, "r");
    
    if (!diccionari) {
        printf("Could not open file: %s in MAIN\n", str1);
        fclose(diccionari);
        return tree;
    }
         
    /* Omplim l'arbre amb les paraules del fitxer "diccionari" */
    while(fgets(word, MAXCHAR, diccionari) != NULL)
        diccionari_arbre(tree, word);
    
    fclose(diccionari);
    
    data = fopen(str2, "r"); /* obrim el fitxer amb tots els camins dels fitxers d'on extraurem les dades */

    if (!data) {
        printf("Could not open file: %s in MAIN\n", str2);
        return NULL;
    }
    
    if(!fgets(num, MAXCHAR, data)){
        printf("Problem in lecture of NUM_FITXERS in PRACTICA4\n");
        return NULL;
    }
    
    num[strlen(num)] = '\0';
    num_fitxers = atoi(num);
    printf("%d\n", num_fitxers);
    
    for(int i = 0; i < NUM_FILS; i++) {
        arguments = malloc(sizeof(struct args_fils));
        arguments->data = data;
        arguments->tree = tree;
        arguments->num_fitxers = num_fitxers;
    
        pthread_create(&(fils[i]), NULL, fils_fn, (void *) arguments);
    }
        
    for(int i = 0; i < NUM_FILS; i++) {
        pthread_join(fils[i], NULL);
    }
    
    fclose(data);
    
    return tree;
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
    
    node_data* n_data;
    
    if (argc != 1)
        printf("Opcions de la linia de comandes ignorades\n");

    do {
        opcio = menu();
        printf("\n\n");

       /* Feu servir aquest codi com a pantilla */

        switch (opcio) {
            case 1:
                printf("Fitxer de diccionari de paraules: ");
                if(fgets(str1, MAXCHAR, stdin))
                    str1[strlen(str1)-1]=0;

                printf("Fitxer de base de dades: ");
                if(fgets(str2, MAXCHAR, stdin))
                    str2[strlen(str2)-1]=0;
                
                tree = crear_arbre_fil(str1, str2);
                
                printf("Elements: %d\n", tree->num_elements);
                break;
                
            case 2:
                printf("Fitxer de diccionari de paraules: ");
                if(fgets(str1, MAXCHAR, stdin))
                    str1[strlen(str1)-1]=0;

                printf("Fitxer de base de dades: ");
                if(fgets(str2, MAXCHAR, stdin))
                    str2[strlen(str2)-1]=0;
                
                tree = crear_arbre_fils(str1, str2);
                
                printf("Elements: %d\n", tree->num_elements);
                break;
                
            case 3:
                printf("Nom de fitxer en que es desara l'arbre: ");
                if(fgets(str1, MAXCHAR, stdin))
                    str1[strlen(str1)-1]=0;
                
                if(tree !=NULL){
                    guardar_arbre(str1, tree);
                
                }else{ printf("L'arbre no ha estat creat.\n"); } 
                break;
                
            case 4:
                printf("Nom del fitxer que conte l'arbre: ");
                if(fgets(str1, MAXCHAR, stdin))
                    str1[strlen(str1)-1]=0;

                if(tree != NULL){
                    delete_tree(tree);
                    free(tree);                
                }
                

                tree = recuperar_arbre(str1);

                break;

            case 5:
                 if(tree != NULL){
                        
                    printf("Paraula a buscar o polsa enter per saber la paraula que apareix més vegades: ");
                    if(fgets(str1, MAXCHAR, stdin))
                        str1[strlen(str1)-1]=0;

                    if(strlen(str1)){
                        /* és tan petit que no fa falta usar una funció. */
                        
                        n_data = find_node(tree, str1);
                        
                        if(n_data != NULL)
                            printf("'%s' surt %d vegades", str1, n_data->num_times);
                        else
                            printf("La paraula no apareix al diccionari...");
                        
                    }else{
                        
                        top_1(tree);
                        
                        if(system("cat tmp.txt | sort -nr | head -n1")){}
                    }
                
                }else{ printf("L'arbre no ha estat creat.\n"); }
                

                break;

            case 6:
                
                if(tree != NULL){
                    delete_tree(tree);
                    free(tree);
                }

                break;

            default:
                printf("Opcio no vàlida\n");

        } /* switch */
    }
    while (opcio != 6);

    return 0;
}
