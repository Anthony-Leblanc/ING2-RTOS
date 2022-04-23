# ING2-RTOS

Vous trouverez le sujet dans ce répertoire. Il s'agit d'un projet pour se familiariser avec FreeRTOS et surtout sur la gestion de la programmation par thread.

Pour réaliser le projet j'ai réalisé simplement les tâches comme demandé dans le sujet, j'ai opté pour la solution la plus simple (dans mon esprit) pour les réaliser.
J'ai donc créé une file pour chaque donnée qu'il fallait transmettre de la taille de 1 donnée. J'ai utilisé un sémaphore pour permettre le partage du port série permettant l'affichage à l'écran.
J'ai choisi de faire le moins de traitement possible lorsque le sémaphore est pris pour éviter de le monopoliser inutilement.

J'ai fait le choix de ne pas mettre de délais même si l'affichage cela rend compliqué la lecture de l'affichage car ce n'était pas demandé dans le cahier des charges.
