/*
Anthony Leblanc
23/04/2022
Projet de RTOS, deuxième année d'ingénieur instrumentation à Sup Galilée
*/

#include <Arduino_FreeRTOS.h>
#include "queue.h"
#include "semphr.h"

#define POTENTIOMETER A0 // Le potentiomètre est branché sur la broche analogique A0
#define BUTTON_1 3 // Le bouton 1 est branché sur la broche numérique 3
#define BUTTON_2 4 // Le bouton 2 est branché sur la broche numérique 4
#define STR_MAX_SIZE 100
#define DECIMAL_PLACES 2

typedef struct _VALEUR_CAPTEURS {
  int analogique;
  int numerique;
  double tempsEnMillisecondes;
}VALEUR_CAPTEURS;

String double2string(double n, int ndec);

SemaphoreHandle_t  xMutex;
QueueHandle_t task_1Queue; // Création de la queue permettant d'envoyer la donnée  de la tâche 1
QueueHandle_t task_2Queue; // Création de la queue permettant d'envoyer la donnée  de la tâche 2
QueueHandle_t task_3Queue; // Création de la queue permettant d'envoyer la donnée  de la tâche 3
QueueHandle_t task_4Queue; // Création de la queue permettant d'envoyer la donnée  de la tâche 4

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // Initialisation du port série pour l'affichage

  xMutex = xSemaphoreCreateMutex();

  task_1Queue  = xQueueCreate(1,sizeof(int32_t)); // Initialisation de la queue de la taille d'une seule valeur
  task_2Queue  = xQueueCreate(1,sizeof(int32_t)); // Initialisation de la queue de la taille d'une seule valeur
  task_3Queue  = xQueueCreate(1,sizeof(VALEUR_CAPTEURS)); // Initialisation de la queue de la taille de la structure VALEUR_CAPTEURS
  task_4Queue  = xQueueCreate(1,sizeof(VALEUR_CAPTEURS)); // Initialisation de la queue de la taille de la structure VALEUR_CAPTEURS

  xTaskCreate(Task_1,
              "Potentiometer Manager",
              100,
              NULL,
              1,
              NULL
              ); // Création de la première tâche

  xTaskCreate(Task_2,
              "Buttons Manager",
              100,
              NULL,
              1,
              NULL
              ); // Création de la deuxième tâche

  xTaskCreate(Task_3,
              "Structure Manager",
              100,
              NULL,
              1,
              NULL
              ); // Création de la troisième tâche

  xTaskCreate(Task_4,
              "Print_1 Manager",
              100,
              NULL,
              1,
              NULL
              ); // Création de la quatrième tâche

  xTaskCreate(Task_5,
              "Print_1 Manager",
              100,
              NULL,
              1,
              NULL
              ); // Création de la cinquième tâche 
}

void Task_1(void *pvParameters) // Définition de la première tâche : 
{ /*
Récupère une valeur analogique sur l’entrée A0 qui est branchée avec un potentiomètre puis
l’envoie à la tâche 3 (valeur entre 0 et 1023)
*/

  pinMode(POTENTIOMETER,INPUT); // Définition de la broche du potentiomètre en entrée
  
  //BaseType_t qStatus;
  int32_t valueToSend;
  valueToSend =(int32_t)pvParameters;

  while(1)
  {
    valueToSend = analogRead(POTENTIOMETER); // Récupération de la valeur du potentiometre

    xQueueOverwrite( task_1Queue, &valueToSend ); // On ré-écrit la valeur dans la queue pour
                                                  // avoir la dernière dans la structure a la fin

    /*
    qStatus = xQueueSend(task_1Queue,&valueToSend,0); // On envoie la valeur dans la queue et on récupère
                                                      // l'état de la queue
    while(qStatus != pdPASS) {} // On attend que la valeur ai été lu avant de traité une nouvelle valeur
    */
  }
}

void Task_2(void *pvParameters) // Définition de la deuxième tâche : 
{/*
Récupère une valeur numérique qui est la résultante de l'addition des deux valeurs des deux
entrées numérique 3 et 4 qui sont branchées avec des boutons poussoirs en montage pull
down, puis envoie cette valeur numérique à la tâche 3 (valeur entre 0 et 2)
*/

  pinMode(BUTTON_1, INPUT); // Définition de la broche du bouton 1 en entrée
  pinMode(BUTTON_2, INPUT); // Définition de la broche du bouton 2 en entrée

  //BaseType_t qStatus;
  int32_t valueToSend;
  valueToSend =(int32_t)pvParameters;

  while(1)
  {
    valueToSend = digitalRead(BUTTON_1); // Lecture de l'état du bouton 1
    valueToSend += digitalRead(BUTTON_2); // Incrémentation avec l'état du bouton 2

    xQueueOverwrite( task_2Queue, &valueToSend ); // On ré-écrit la valeur dans la queue pour
                                                  // avoir la dernière dans la structure a la fin
    /*
    qStatus = xQueueSend(task_2Queue,&valueToSend,0); // On envoie la valeur dans la queue et on récupère
                                                      // l'état de la queue
    while(qStatus != pdPASS) {} // On attend que la valeur ai été lu avant de traité une nouvelle valeur
    */
  }
}

void Task_3(void *pvParameters) // Définition de la troisième tâche : 
{/*
Reçoit les deux valeurs des tâches 1 et 2 puis les mets dans une structure en ajoutant la
valeur de la fonction millis()
struct valeurCapteurs {
int analogique;
int numérique;
double tempsEnMillisecondes;
};
Une fois la structure remplie, cette dernière doit être envoyée à la tâche 4.
*/

  //BaseType_t qStatus;
  VALEUR_CAPTEURS *valeurCapteurs = NULL;
  valeurCapteurs = (VALEUR_CAPTEURS*)malloc(1*sizeof(VALEUR_CAPTEURS)); // Allocation dynamique de mémoire pour sauvegarder la structure

  unsigned long myTime = millis();
  unsigned long taskTime;

  while(1)
  {
    myTime = millis();
    taskTime = myTime;

    // Récupération des données des Queues.
    xQueueReceive(task_1Queue, &valeurCapteurs->analogique,( TickType_t ) 10);
    xQueueReceive(task_2Queue, &valeurCapteurs->numerique, (TickType_t) 10);

    // Calcul du temps d'exécution de la tâche
    taskTime = millis();
    valeurCapteurs->tempsEnMillisecondes = taskTime - myTime;

    xQueueOverwrite( task_3Queue, valeurCapteurs ); // On ré-écrit la valeur dans la queue pour
                                                  // avoir la dernière dans la structure a la fin
  }
}

void Task_4(void *pvParameters) // Définition de la deuxième tâche : 
{/*
Cette tâche reçoit la valeur de la structure et utilise le port série pour l’afficher, ensuite envoie
cette structure à la tâche 5.
*/

  VALEUR_CAPTEURS *valeurCapteurs = NULL;
  valeurCapteurs = (VALEUR_CAPTEURS*)malloc(1*sizeof(VALEUR_CAPTEURS));
  String pcStringToPrint = "";
  String buffer = "";

  while(1)
  {
    xQueueReceive(task_3Queue, valeurCapteurs,( TickType_t ) 10);

    // Préparation de la chaîne de caractères pour l'affichage
    pcStringToPrint = String(valeurCapteurs->analogique);
    pcStringToPrint += ":";
    pcStringToPrint += String(valeurCapteurs->numerique);
    pcStringToPrint += ":" ;
    pcStringToPrint += double2string(valeurCapteurs->tempsEnMillisecondes, DECIMAL_PLACES);

    // Gestion du port série
    // https://www.freertos.org/a00122.html
    if( xMutex != NULL )
    {
        /* See if we can obtain the semaphore.  If the semaphore is not
        available wait 10 ticks to see if it becomes free. */
        if( xSemaphoreTake( xMutex, ( TickType_t ) 10 ) == pdTRUE )
        {
            /* We were able to obtain the semaphore and can now access the
            shared resource. */

            Serial.println(pcStringToPrint);

            /* We have finished accessing the shared resource.  Release the
            semaphore. */
            xSemaphoreGive( xMutex );
        }
        else
        {
            /* We could not obtain the semaphore and can therefore not access
            the shared resource safely. */
        }
    }

    xQueueOverwrite( task_4Queue, valeurCapteurs ); // On ré-écrit la valeur dans la queue pour
                                                  // avoir la dernière dans la structure a la fin
  }
}

void Task_5(void *pvParameters) // Définition de la deuxième tâche : 
{/*
Cette tâche doit transformer la valeur du temps dans la structure en minutes, ensuite elle doit
afficher cette nouvelle structure à travers le port série.
*/

  VALEUR_CAPTEURS *valeurCapteurs = NULL;
  valeurCapteurs = (VALEUR_CAPTEURS*)malloc(1*sizeof(VALEUR_CAPTEURS));
  String pcStringToPrint = "";
  double buffer = 0.0;
  double timeEnMinutes = 0;

  while(1)
  {
    xQueueReceive(task_4Queue, valeurCapteurs,( TickType_t ) 10);

    // Préparation de la chaîne de caractères pour l'affichage
    pcStringToPrint = String(valeurCapteurs->analogique);
    pcStringToPrint += ":";
    pcStringToPrint += String(valeurCapteurs->numerique);
    pcStringToPrint += ":" ;
    pcStringToPrint += double2string(valeurCapteurs->tempsEnMillisecondes * 1.6666666666667E-5, 
                                      DECIMAL_PLACES + 5);

    // Gestion du port série
    // https://www.freertos.org/a00122.html
    if( xMutex != NULL )
    {
        /* See if we can obtain the semaphore.  If the semaphore is not
        available wait 10 ticks to see if it becomes free. */
        if( xSemaphoreTake( xMutex, ( TickType_t ) 10 ) == pdTRUE )
        {
            /* We were able to obtain the semaphore and can now access the
            shared resource. */

            Serial.println(pcStringToPrint);

            /* We have finished accessing the shared resource.  Release the
            semaphore. */
            xSemaphoreGive( xMutex );
        }
        else
        {
            /* We could not obtain the semaphore and can therefore not access
            the shared resource safely. */
        }
    }
  }
}
/*
void printer(const char* pcString) 
{ // https://microcontrollerslab.com/arduino-freertos-mutex-tutorial-priority-inversion-priority-inheritance/
  // take mutex
  xSemaphoreTake(xMutex, portMAX_DELAY);
  Serial.println(pcString); // send string to serial monitor
  xSemaphoreGive(xMutex); // release mutex
}
*/

String double2string(double n, int ndec) 
{ //https://askcodez.com/convertir-le-type-double-en-chaine-de-type-dans-le-sketch-arduino.html
    String r = "";

    int v = n;
    r += v;     //whole number part
    r += '.';   //decimal point
    int i;
    for (i=0;i<ndec;i++) {
        //iterate through each decimal digit for 0..ndec 
        n -= v;
        n *= 10; 
        v = n;
        r += v;
    }

    return r;
}

void loop() {
  // put your main code here, to run repeatedly:

}
