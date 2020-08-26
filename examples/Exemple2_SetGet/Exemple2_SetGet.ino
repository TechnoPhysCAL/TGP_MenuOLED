/*
Librairie MenuOLED - Exemple2_SetGet.ino

Programme de démonstration de l'interaction externe avec les valeurs courantes
des items du menu via les méthodes "getItemValeur" et "setItemValeur" et le terminal.
Ce programme commande 2 DEL à partir du menu. D'autres items du menu
ne servent qu'à montrer les fonctionnalités du menu sans rien commander.

Note: Ce programme est conçu pour la carte PROTOTPHYS 2V1.
      Ce programme utilise aussi la librairie Decodeur.
  
Auteur: Jude Levasseur
Date: 04 juillet 2019
Modification : CB, sept 2020
*/

#include <MenuOLED.h>                         //Pour utiliser la librairie MenuOLED
#include <Decodeur.h>                         //Pour décoder les messages du port série
const char nomProg[] = "Exemple2_SetGet.ino"; //Nom du programme pour transmission sur terminal

//Prototypes des fonctions pour callback du menu:
//Il faut déclarer les fonctions callback du menu avant leur utilisation dans la définition du menu
void ajusteLED1(void);    //Routine callback pour l'ajustement de la LED1
void ajusteLED2(void);    //Routine callback pour l'ajustement de la LED2
void callBackItemX(void); //Routine callback pour l'item X
void callBackItemY(void); //Routine callback pour l'item Y
void callBackItemZ(void); //Routine callback pour l'item Z

//Déclaration des variables utilisées pour le retour des numéros d'item du menu
int noItemLED1, noItemLED2, noItemX, noItemY, noItemZ;

//Déclaration du tableau de pointeurs de texte utile pour affichage en mode texte de l'item LED2
char *niveauLED2[] = {"Eteint", "Bas", "Moyen", "Fort"};
//Nombre d'éléments texte de niveauLED2[], nécessaire pour pour affichage en mode texte de l'item LED2
int nbChoixLED2 = sizeof(niveauLED2) / sizeof(niveauLED2[0]); //Calcul automatique (=4 pour le cas présent)

//Déclaration de l'instance monMenu du type MenuOLED
MenuOLED monMenu;

//Déclaration numéro de broches des Dels

//Paramètres PWM pour LED2
const int pinLED2 = 2;    //Broche D2 sur PROTOTPHYS 2V1
const int freq = 5000;    //Fréquence du PWM
const int ledChannel = 0; //Canal utilisé; choix entre 0 et 15
const int resolution = 8; //Résolution 8 bits; duty de 0 à 255

//Déclaration d'une variable Decodeur avec le port Serie
Decodeur monDecodeur(&Serial, ' ', ENTIER);

//Impression du protocole avec le terminal
void imprimeProtocole(void)
{
  Serial.println("--------------------------PROTOCOLE--------------------------");
  Serial.println("S n1 n2 : Ajuster valeur courante, n1= no.item et n2= valeur");
  Serial.println("G n1    : Obtenir valeur courante, n1= no.item              ");
  Serial.println("?       : Imprimer ce protocole                             ");
  Serial.println("------------------------------------------------------------");
}

void setup()
{
  Serial.begin(115200);

  Serial.println(nomProg); //Transmission du nom du programme

  //Configuration PWM fonctionnalités pour LED2 (spécifique à ESP32)
  ledcSetup(ledChannel, freq, resolution);
  //Attribution du canal PWM à la broche pour LED2 (spécifique à ESP32)
  ledcAttachPin(pinLED2, ledChannel);
  //Initialisation du duty cycle du PWM (spécifique à ESP32)
  ledcWrite(ledChannel, 0); //duty Cycle de 0

  //Initialisation du menu OLED
  monMenu.begin();

  //Paramètres de chaque type d'item du menu (voir "MenuOLED.h"):
  //Pour chaque item de type NUMERIQUE (ItemNumerique), les paramètres sont:
  //  - char *Etiquette; pointeur sur une string,
  //  - void(*callbackFct)(); pointeur sur fonction callback
  //  - int ValeurInitiale; valeur initiale de l'item
  //  - int ValeurMin; valeur minimale de l'item
  //  - int ValeurMax, valeur maximale de l'item
  //  - bool editable, true(defaut)=item éditable, false=item non editable
  //Pour chaque item de type ON_OFF (ItemOnOff), les paramètres sont:
  //  - char *Etiquette; pointeur sur une string,
  //  - void(*callbackFct)(); pointeur sur fonction callback
  //  - int ValeurInitiale; valeur initiale de l'item, 0=Off et 1=On
  //  - bool editable, true(defaut)=item éditable, false=item non editable
  //Pour chaque item de type TEXTE (ItemTexte), les paramètres sont:
  //  - char *Etiquette; pointeur sur une string,
  //  - void(*callbackFct)(); pointeur sur fonction callback
  //  - int ValeurInitiale; valeur initiale de l'item
  //  - int nbChoix; nombre de choix différents de texte à afficher,
  //  - char **choixTexte; pointeur sur tableau de pointeurs de de chaines charactère pour les choix de texte à afficher
  //  - bool editable, true(defaut)=item éditable, false=item non éditable

  //Construction du menu par la définition de chacun des items
  noItemLED1 = monMenu.ajouterItemOnOff("LED 1  = ", &ajusteLED1, 0);
  noItemLED2 = monMenu.ajouterItemTexte("LED 2  = ", &ajusteLED2, 0, nbChoixLED2, &niveauLED2[0]); //Attention au dernier paramètre
  noItemX = monMenu.ajouterItemNumerique("Item X = ", &callBackItemX, 0, 0, 512);
  noItemY = monMenu.ajouterItemNumerique("Item Y = ", &callBackItemY, 0, -500000, 500000);
  noItemZ = monMenu.ajouterItemNumerique("Item Z = ", &callBackItemZ, 0, -10, 50);

  //Impression de la ligne Titre
  monMenu.imprimeLigneTitreOLED("Exemple2_SetGet");
  //Impression de la ligne Status
  monMenu.imprimeLigneStatusOLED("Sans erreur");

  //Impression du protocole avec le terminal
  imprimeProtocole();
}

void loop()
{
  monMenu.refresh(); //Pour permettre le fonctionnement du menu
  //Vérification d'une commande du termianl
  if (monDecodeur.available())
  {
    char commande = monDecodeur.getCommand();
    switch (commande)
    {
    case 'G':
    case 'g':
    {
      if (monDecodeur.getArgCount() > 0)
      {
        int item = monDecodeur.getArg(0);
        if (item >= 0 && item < monMenu.getNbItems())
        {
          Serial.print("La valeur courante de l'item ");
          Serial.print(item);
          Serial.print(" est : ");
          Serial.println(monMenu.getItemValeur(item));
        }
        else
        {
          Serial.println("Numero Item non valide!");
        }
      }
      else
      {
        Serial.println("Commande non valide!");
      }
    }
    break;
    case 'S':
    case 's':
    {
      if (monDecodeur.getArgCount() > 1)
      {
        int item = monDecodeur.getArg(0);
        int valeur = monDecodeur.getArg(1);
        if (item >= 0 && item < monMenu.getNbItems())
        {
          monMenu.setItemValeur(item, valeur);
        }
        else
        {
          Serial.println("Numero Item non valide!");
        }
      }
      else
      {
        Serial.println("Commande non valide!");
      }
    }
    break;
    case '?':
      imprimeProtocole();
      break;
    default:
      Serial.println("Commande non valide!");
      break;
    }
  }
}

//Définition des fonctions callback propes à chacun des items du menu
void ajusteLED1()
{
  //Ajuste la DEL rouge la valeur courante de noItemLED1 du menu
  monMenu.rouge.set(monMenu.getItemValeur(noItemLED1) != 0);
}
void ajusteLED2()
{
  //Ajuste PWM de LED2 selon la valeur courante de noItemLED2 du menu
  int valeur = monMenu.getItemValeur(noItemLED2); //Lecture de la valeur courante
  if (valeur == 0)
  {
    ledcWrite(ledChannel, 0);
  } //Cas pour la valeur courante "0" correspondant à "Eteint"
  else if (valeur == 1)
  {
    ledcWrite(ledChannel, 20);
  } //Cas pour la valeur courante "1" correspondant à "Bas"
  else if (valeur == 2)
  {
    ledcWrite(ledChannel, 80);
  } //Cas pour la valeur courante "2" correspondant à "Moyen"
  else if (valeur == 3)
  {
    ledcWrite(ledChannel, 255);
  } //Cas pour la valeur courante "3" correspondant à "Fort"
}
void callBackItemX()
{
  Serial.println("callback callBackItemX()"); //Pour signifier sur le terminal
}
void callBackItemY()
{
  Serial.println("callback callBackItemY()"); //Pour signifier sur le terminal
}
void callBackItemZ()
{
  Serial.println("callback callBackItemZ()"); //Pour signifier sur le terminal
}