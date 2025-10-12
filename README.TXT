This folder will include all of the 'to be used in the future as a base for all of my project' c files.

IDEE: 
      Allouer sur la stack = MAL pcq on maitrise pas ca donc on precise a compile-time une taille
                             pour un array qui sera automatiquement push sur NOTRE stack
      Mais comment ? Meta programming ? 
      EG:  typedef struct Person
           {
              char name[@:SIZE]; -> ici SIZE est stocke qqpart jsp ou et sera utilise automatiquement a chaque fois qu'on accede
           }
