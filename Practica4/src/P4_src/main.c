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
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/sysinfo.h>
#include <fcntl.h>
#include <time.h> 


#include "red-black-tree.h"
#include "tree-to-mmap.h"
#include "dbfnames-mmap.h"

#define MAXCHAR      100
#define MAGIC_NUMBER 0x01234567
#define NUM_PROCS    get_nprocs() /* No canviar */ 

/* 
 * MEMÒRIA COMPARTIDA PER ALS MÚTLIPLES FILLS (sinó la tenim no tots els fitxers poden saber aquestes dades)
 *  - int counter: compta el nombre de fitxers que hem obert
 *  - sem_t clau_counter: regula quan els fills canvien el nombre de fitxers vistos
 *  - sem_t clau_tree: regula quan els fills poden escriure a l'arbre.
 * 
 */

typedef struct shared_mem {
    sem_t clau_counter;
    sem_t clau_tree;
    int counter;
} shared_mem;

shared_mem *s;

/**
 * 
 *  Menu
 * 
 */

int menu() 
{
    char str[7];
    int opcio=0;

    printf("\n\nMenu\n\n");
    printf(" 1 - Creacio de l'arbre mapejat a memòria\n");
    printf(" 2 - Creacio de l'arbre amb un pare i un fill\n");
    printf(" 3 - Creacio de l'arbre amb un pare i múltiples fills\n");
    printf(" 4 - Emmagatzemar arbre a disc\n");
    printf(" 5 - Llegir arbre de disc\n");
    printf(" 6 - Consultar informacio de l'arbre\n");
    printf(" 7 - Sortir\n\n");
    printf("   Escull opcio: ");

    if(fgets(str, 7, stdin))
        opcio = atoi(str); 

    return opcio;
}

char* retallar_strings(char* string){

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
    * Arguments: rb_tree* tree
    * Retorn: void
    * 
    */
    char* auxWord;
    node_data *n_data;
    int ct = 0;
        
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
    
    
    //printf("Arbre creat amb %d paraules!\n", ct);
    
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
                    (temp->num_times++);
                }
                
            }

            /* Search for the beginning of a candidate word */

            while ((i < len_line) && (isspace(line[i]) || (ispunct(line[i])))) i++; 

        }
    
    }
    
    fclose(fp);
    
}

/*
 * 
 * Hem de fer el canvi en aquesta funció, la reanomenarem a practica4
 * 
 */
rb_tree* practica4(char* str1, char* str2)
{
    FILE *data, *diccionari;
    char* auxFilePath;
    char* mmap;
    char* mmap_data;
    char num[MAXCHAR];
    int num_fitxers = 2;

    /* L'arbre amb el diccionari s'ha de crear de la mateixa manera que a la practica 2 i 3 */
    
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
    
    char word[MAXCHAR];
    
    /* Omplim l'arbre amb les paraules del fitxer "diccionari" */
    while(fgets(word, MAXCHAR, diccionari) != NULL)
        diccionari_arbre(tree, word);
    
    fclose(diccionari);
    
    /* Mapejem l'arbre a memòria. */
    mmap = serialize_node_data_to_mmap(tree);
        
    /* Obrim el fitxer de fitxers */
    data = fopen(str2,"r"); /* obrim el fitxer amb tots els camins dels fitxers d'on extraurem les dades */

    if (!data) {
        printf("Could not open file: %s in MAIN\n", str2);
        delete_tree(tree);
        init_tree(tree);
        return tree;
    }
    
    /* Necessitem el nombre de fitxers que hi ha a llista.cfg per poder accedir a cadaun d'ells. */
    if(!fgets(num, MAXCHAR, data)){
        printf("Problem in lecture of NUM_FITXERS in PRACTICA4\n");
        delete_tree(tree);
        init_tree(tree);
        return tree;
    }
    
    num[strlen(num)] = '\0';
    num_fitxers = atoi(num);
    printf("%d\n", num_fitxers);
    
    fseek(data, 0, SEEK_SET);
    /*rewind(data);*/
    
    time_t c1 = clock();
    
    /* Carrega TOTS els fitxers de data al MMAP */
    mmap_data = dbfnames_to_mmap(data);
    
    fclose(data); /* tanca str2. Ja no el necessitem perque ja està mapat a memòria. */
    
    for(int fitxer=0; fitxer<num_fitxers; fitxer++){
        auxFilePath = get_dbfname_from_mmap(mmap_data, fitxer);
        search_words(tree, auxFilePath);
        
    }
    
    /* Quan acabem alliberem memòria??? */
    dbfnames_munmmap(mmap_data);
    
    /* Deserialitzem l'arbre del mmap. */
    deserialize_node_data_from_mmap(tree, mmap);
    
    c1 = clock() - c1;
    double temps = ((double) c1)/CLOCKS_PER_SEC;
    printf("Time: %f s\n", temps);
    
    return tree;

}

void sigusr(int signal){
    // Function to control la merda 
}

void child(char* str2, rb_tree* tree){

    FILE* data;
    char num[MAXCHAR];
    char* auxFilePath;
    char* mmap_data;
    int num_fitxers;
    
    /* Obrim el fitxer de fitxers */
    data = fopen(str2,"r"); /* obrim el fitxer amb tots els camins dels fitxers d'on extraurem les dades */

    if (!data) {
        printf("Could not open file: %s in MAIN\n", str2);
        delete_tree(tree);
        init_tree(tree);
        exit(1);
    }
    
    /* Necessitem el nombre de fitxers que hi ha a llista.cfg per poder accedir a cadaun d'ells. */
    if(!fgets(num, MAXCHAR, data)){
        printf("Problem in lecture of NUM_FITXERS in PRACTICA4\n");
        delete_tree(tree);
        init_tree(tree);
        exit(1);
    }
    
    num[strlen(num)] = '\0';
    num_fitxers = atoi(num);
    printf("%d\n", num_fitxers);
    
    fseek(data, 0, SEEK_SET);
    /*rewind(data);*/
    
    /* Carrega TOTS els fitxers de data al MMAP */
    mmap_data = dbfnames_to_mmap(data);
    
    fclose(data); /* tanca str2. Ja no el necessitem perque ja està mapat a memòria. */
    
    for(int fitxer=0; fitxer<num_fitxers; fitxer++){
        auxFilePath = get_dbfname_from_mmap(mmap_data, fitxer);
        search_words(tree, auxFilePath);
    }
    
    /* Quan acabem alliberem el mmap de la memòria compartida */
    dbfnames_munmmap(mmap_data);
    
    kill(getppid(), SIGUSR1);
    
    exit(0);
}



rb_tree* crear_arbre_fill(char* str1, char* str2)
{
    FILE *diccionari;
    char word[MAXCHAR];
    char* mmap;
    pid_t id;

    /* L'arbre amb el diccionari s'ha de crear de la mateixa manera que a la practica 2 i 3 */
    
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
    
    /* Mapejem l'arbre a memòria. */
    mmap = serialize_node_data_to_mmap(tree);
    
    
    /* COMENÇA LA GENERACIÓ DE FILL(S): EL PARTO */
    time_t c1 = clock();
    
    /* Definim les funcions que es cridaran quan una senyal sigui llençada o rebuda */
    signal(SIGUSR1,sigusr); /* Fill a pare */
    
    id = fork();
    
    if(id != 0)
        pause();
    else
        child(str2, tree);
    
    /* Deserialitzem l'arbre del mmap. */
    deserialize_node_data_from_mmap(tree, mmap);
    
    c1 = clock() - c1;
    double temps = ((double) c1)/CLOCKS_PER_SEC;
    printf("Time: %f s\n", temps);
    
    return tree;
}


rb_tree* crear_arbre_fills(char* str1, char* str2)
{
    FILE *data, *diccionari;
    char word[MAXCHAR], num[MAXCHAR];
    char* mmap_arbre;
    char* mmap_data;
    char* auxFilePath;
    pid_t id;
    int num_fitxers, max_fills, temp, status = 0;

    /* L'arbre amb el diccionari s'ha de crear de la mateixa manera que a la practica 2 i 3 */
    
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
    
    /* Mapejem l'arbre a memòria. */
    mmap_arbre = serialize_node_data_to_mmap(tree);
    
    /* Obrim el fitxer de fitxers */
    data = fopen(str2,"r"); /* obrim el fitxer amb tots els camins dels fitxers d'on extraurem les dades */

    if (!data) {
        printf("Could not open file: %s in MAIN\n", str2);
        delete_tree(tree);
        init_tree(tree);
        return tree;
    }
    
    /* Necessitem el nombre de fitxers que hi ha a llista.cfg per poder accedir a cadaun d'ells. */
    if(!fgets(num, MAXCHAR, data)){
        printf("Problem in lecture of NUM_FITXERS in PRACTICA4\n");
        delete_tree(tree);
        init_tree(tree);
        return tree;
    }
    
    num[strlen(num)] = '\0';
    num_fitxers = atoi(num);
    printf("%d\n", num_fitxers);
    
    fseek(data, 0, SEEK_SET); /* Tornem a posar el punter al principi del fitxer per generar el mmap de data */
    /*rewind(data);*/
    
    /* Carrega TOTS els fitxers de data al MMAP */
    mmap_data = dbfnames_to_mmap(data);
    
    fclose(data); /* tanca str2. Ja no el necessitem perque ja està mapat a memòria. */
    
    
    /* 
     *
     * 
     * 
     * COMENÇA LA GENERACIÓ DE FILL(S): EL PARTO 
     * 
     * 
     * 
     */
    
    time_t c1 = clock();
    
    s = mmap(NULL, sizeof(shared_mem), PROT_READ | PROT_WRITE,
             MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    /* Inicialitzem el semàfor i la memòria compartida */
    s->counter = 0;
    sem_init(&s->clau_counter, 1, 1);
    sem_init(&s->clau_tree, 1, 1);
    
    max_fills = NUM_PROCS;
    
    for(int i=0; i<max_fills; i++){
        id = fork();
    
        if(id == 0){
        
            /*printf("[son] pid %d from [parent] %d pid\n",getpid(), getppid());*/
            while(1){
                
                /* RACE CONDITIONS */
                sem_wait(&s->clau_counter); // lock
                temp = s->counter;
                s->counter++;
                sem_post(&s->clau_counter); // unlock
                
                
                if(temp < num_fitxers){
                    auxFilePath = get_dbfname_from_mmap(mmap_data, temp);
                    
                    
                    /*
                    printf("Filename mmap: %s\n", auxFilePath);
                    printf("Counter: %d \n", s->counter);
                    */
                    
                    sem_wait(&s->clau_tree);
                    search_words(tree, auxFilePath);
                    sem_post(&s->clau_tree);
                    
                }else{ /* Tots els fitxers estan llegits */
                    break;
                }
                
            }
            exit(1);
        }
    }

    while ((wait(&status)) > 0); /* Esperem a que tots els processos s'acabin */

    /* Deserialitzem l'arbre del mmap i desmapejem la memòria compartida i els fitxers */
    munmap(s, sizeof(shared_mem));
    dbfnames_munmmap(mmap_data);
    deserialize_node_data_from_mmap(tree, mmap_arbre);
    
    c1 = clock() - c1;
    double temps = ((double) c1)/CLOCKS_PER_SEC;
    printf("Time: %f s\n", temps);
    
    return tree;
}

void nodes_tree_inorder(node *current, FILE* fd) {
 
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

void guardar_arbre(char* filename, char* diccionari, rb_tree* tree){

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
 
    FILE *fd;
    
    fd = fopen("tmp.txt", "w");
    
    print_tree_inorder_file(tree->root, fd);
    
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
    char str1[MAXCHAR], str2[MAXCHAR], diccionari[MAXCHAR];
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
                
                tree = practica4(str1, str2);

                diccionari[strlen(str1)-1]=0;
                strcpy(diccionari, str1);
                
                printf("Elements: %d\n", tree->num_elements);
                break;

            case 2:
                printf("Fitxer de diccionari de paraules: ");
                if(fgets(str1, MAXCHAR, stdin))
                    str1[strlen(str1)-1]=0;

                printf("Fitxer de base de dades: ");
                if(fgets(str2, MAXCHAR, stdin))
                    str2[strlen(str2)-1]=0;
                
                tree = crear_arbre_fill(str1, str2);

                diccionari[strlen(str1)-1]=0;
                strcpy(diccionari, str1);
                
                printf("Elements: %d\n", tree->num_elements);
                break;
                
            case 3:
                printf("Fitxer de diccionari de paraules: ");
                if(fgets(str1, MAXCHAR, stdin))
                    str1[strlen(str1)-1]=0;

                printf("Fitxer de base de dades: ");
                if(fgets(str2, MAXCHAR, stdin))
                    str2[strlen(str2)-1]=0;
                
                tree = crear_arbre_fills(str1, str2);

                diccionari[strlen(str1)-1]=0;
                strcpy(diccionari, str1);
                
                printf("Elements: %d\n", tree->num_elements);
                break;
                
            case 4:
                printf("Nom de fitxer en que es desara l'arbre: ");
                if(fgets(str1, MAXCHAR, stdin))
                    str1[strlen(str1)-1]=0;
                
                if(tree !=NULL){
                    guardar_arbre(str1, diccionari, tree);
                
                }else{ printf("L'arbre no ha estat creat.\n"); } 
                break;
                
            case 5:
                printf("Nom del fitxer que conte l'arbre: ");
                if(fgets(str1, MAXCHAR, stdin))
                    str1[strlen(str1)-1]=0;

                if(tree != NULL){
                    delete_tree(tree);
                    free(tree);                
                }
                

                tree = recuperar_arbre(str1);

                break;

            case 6:
                 if(tree != NULL){
                        
                    printf("Paraula a buscar o polsa enter per saber la paraula que apareix mes vegades: ");
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

            case 7:
                
                if(tree != NULL){
                    delete_tree(tree);
                    free(tree);
                    
                }

                break;

            default:
                printf("Opcio no valida\n");

        } /* switch */
    }
    while (opcio != 7);

    return 0;
}
