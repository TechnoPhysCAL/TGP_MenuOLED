/*
Librairie TGP MenuOLED - Exemple1_Basic.ino

Programme de démonstration de l'utilisation de base de la librairie TGP MenuOLED.
Ce programme commande 2 DELs à partir du menu. D'autres items du menu
ne servent qu'à montrer les fonctionnalités du menu sans rien commander.

Note: Ce programme est conçu pour la carte PROTOTPHYS 2V1.

Résumé du fonctionnement du menu:
- Les boutons HAUT et BAS servent à changer d'item dans le menu.
- Le bouton SELECTION sert à entrer et sortir du mode d'édition de la valeur courante de l'item.
- En mode d'édition, les boutons HAUT et BAS servent à modifier la valeur courante de l'item.
- En mode d'édition d'un item de type NUMRERIQUE, les boutons GAUCHE et DROITE servent à
  choisir le digit à incrémenter ou décrémenter par les boutons HAUTS et BAS.
- À la sortie du mode d'édition par le bouton SELECTION, la fonction callback propre
  à l'item est automatiquement appelée.
  
Auteur: Jude Levasseur
Date: 04 juillet 2019
Modification : CB, sept 2020
*/

#include <MenuOLED.h>                        //Pour utiliser la librairie MenuOLED
const char nomProg[] = "Exemple1_Basic.ino"; //Nom du programme pour transmission sur terminal

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
char *niveauLED2[] = {"Éteint", "Bas", "Moyen", "Fort"};
//Nombre d'éléments texte de niveauLED2[], nécessaire pour pour affichage en mode texte de l'item LED2
int nbChoixLED2 = sizeof(niveauLED2) / sizeof(niveauLED2[0]); //Calcul automatique (=4 pour le cas présent)

//Déclaration de l'instance monMenu du type MenuOLED
MenuOLED monMenu;

void setup()
{
  Serial.begin(115200);    //Pour la communication série
  Serial.println(nomProg); //Transmission du nom du programme

  //Initialisation de l'OLED et des boutons
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
  monMenu.imprimeLigneTitreOLED("Exemple1_Basic");
  //Impression de la ligne Status
  monMenu.imprimeLigneStatusOLED("Sans erreur");
}

void loop()
{
  monMenu.refresh(); //Pour permettre le fonctionnement du menu
}

//Définition des fonctions callback propes à chacun des items du menu
void ajusteLED1()
{
  //Ajuste la DEL rouge la valeur courante de noItemLED1 du menu
  monMenu.rouge.set(monMenu.getItemValeur(noItemLED1) != 0);
}

void ajusteLED2()
{
  //Ajuste la luminoité de la DEL verte selon la valeur courante de noItemLED2 du menu
  int valeur = monMenu.getItemValeur(noItemLED2); //Lecture de la valeur courante

  if (valeur == 0)//Cas pour la valeur courante "0" correspondant à "Eteint"
  {
    monMenu.verte.set(false);
  } 
  else if (valeur == 1)//Cas pour la valeur courante "1" correspondant à "Bas"
  {
    monMenu.verte.set(true);
    monMenu.verte.setBrightness(8);
  } 
  else if (valeur == 2) //Cas pour la valeur courante "2" correspondant à "Moyen"
  {
    monMenu.verte.set(true);
    monMenu.verte.setBrightness(30);
   
  }
  else if (valeur == 3) //Cas pour la valeur courante "3" correspondant à "Fort"
  {
    monMenu.verte.set(true);
    monMenu.verte.setBrightness(100);
  }
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