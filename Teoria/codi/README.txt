
-----------------------------------------------------------
Exemples en què s'accedeix per lectura a variables globals
-----------------------------------------------------------

* Executar fitxer_genera.c Aixo permet generar fitxer.data per fer proves amb 
  l'executable lectura_fitxer.c

* Executar fitxer_lectura. Observar que amb 1 fil es llegeixen tantes linies
  com es demanen, però amb dos o més fils es poden llegir més línies de
  les que realment es demanen. El problema és a la condició del while 

   while (nelem < N)
   {
        pthread_mutex_lock(&mutex);
        fscanf(fitxer, "%d", &i);
        nelem++;
        pthread_mutex_unlock(&mutex);

        total_local += i;
   }

   La variable nelem és una variable global que es modificada per (altres) fils
   en un altre lloc. No fer aquests tipus de codi ja que pot fer que el codi
   funcioni de forma incorrecta. 

   Una solució que no funciona és la següent. Per què? Podeu trobar una forma
   en què l'aplicació queda bloquejada?

   while (1)    // BUCLE INFINIT
   {
        pthread_mutex_lock(&mutex);
        fscanf(fitxer, "%d", &i);
        nelem++;
        if (nelem >= N)  // SI HE ACABAT SURTO DEL FIL
            return ((void *) 0);
        pthread_mutex_unlock(&mutex);

        total_local += i;
    }

   Per proposar una solució que funcioni es recomana fer servir una variable local 
   a la condició del while.

* Conclusió: compte a l'hora d'accedir a variables globals, encara que sigui per
  lectura! 
