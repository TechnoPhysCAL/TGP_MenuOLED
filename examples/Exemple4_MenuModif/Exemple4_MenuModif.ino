/*
Librairie MenuOLED - Exemple4_MenuModif.ino

Modification d'un programme de démonstration de l'utilisation de base de la librairie MenuOLED 

En plus de commander les 2 DELs à partir du menu. L'Item Y du menu sert à modifier
la structure de l'itmeX ((voir "callBackItemY()")):
  - L'ajustement à "0" de la valeur courante de l'item Y modifie le premier 
    elément texte de l'item X pour "A".
  - L'ajustement à "1" de la valeur courante de l'item Y modifie le premier
    elément texte de l'item X pour le nombre "123.4567"; plusieurs façons de faire sont décrites.
  - L'ajustement à "2" de la valeur courante de l'item Y modifie l'étiquette
    de l'item Z pour "coco = ".
  - L'ajustement à "3" de la valeur courante de l'item Y crée une rotation des
    éléments texte pour l'item X.

 De plus, on utilise les fonctionnalités ajoutées à version 1.1.0, soit:
  - La troncation d'un texte trop long à afficher sur une ligne de l'OLED. Ceci est démontré
    par un des éléments texte de l'itemLED2 (voir "*niveauLED2[]").
  - L'utilisation de la nouvelle méthode "actualiserUnItem" pour forcer
    le rafraichissement de l'affichage d'un item (voir "callBackItemY()").

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
Date: 11 aout 2019
Modification : CB, sept 2020
*/

#include <MenuOLED.h>                            //Pour utiliser la librairie MenuOLED
const char nomProg[] = "Exemple4_MenuModif.ino"; //Nom du programme pour transmission sur terminal

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
char *niveauLED2[] = {"Eteint_blablablabla", "Bas", "Moyen", "Fort"}; //Premier élément trop long pour affichage; il sera tronqué.
//Nombre d'éléments texte de niveauLED2[], nécessaire pour pour affichage en mode texte de l'item LED2
int nbChoixLED2 = sizeof(niveauLED2) / sizeof(niveauLED2[0]); //Calcul automatique (=4 pour le cas présent)

//Déclaration du tableau de pointeurs de texte utile pour affichage en mode texte de l'item X
char *xText[] = {"A", "B", "C", "D"};
//Nombre d'éléments texte de niveauLED2[], nécessaire pour pour affichage en mode texte de l'item LED2
int nbChoix_itemX = sizeof(xText) / sizeof(xText[0]); //Calcul automatique

//Variable pour le l'étquette de l'Item Z; pouvant ainsi être modifiable
char itemZ_Etiquette[20] = "Item Z = ";

//Déclaration de l'instance monMenu du type MenuOLED
MenuOLED monMenu;

void setup()
{
  Serial.begin(115200);    //Pour la communication série
  Serial.println(nomProg); //Transmission du nom du programme

  //Initialisation du menu
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
  noItemX = monMenu.ajouterItemTexte("Item X = ", &callBackItemX, 0, nbChoix_itemX, &xText[0]);
  noItemY = monMenu.ajouterItemNumerique("Item Y = ", &callBackItemY, 0, -500000, 500000);
  noItemZ = monMenu.ajouterItemNumerique(itemZ_Etiquette, &callBackItemZ, 0, -10, 50); //L'étiquette est référencée à une variable; elle peut donc être modifiée.

  //Impression de la ligne Titre
  monMenu.imprimeLigneTitreOLED("Exemple4_MenuModif");
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
  int valeur = monMenu.getItemValeur(noItemY); //Lecture de la valeur courante
  if (valeur == 0)
  {
    //Cas pour la valeur courante "0"
    ///Modification du texte de l'item X pour valeur 0 de l'item Y; plusieurs possibilités
    xText[0] = "A";
    monMenu.setItemValeur(noItemX, 0); //Pour mettre à jour l'affichage de la ligne d'item
  }
  else if (valeur == 1)
  {
    //Cas pour la valeur courante "1"
    //Modification du texte de l'item X pour valeur 1 de l'item Y; plusieurs possibilités

    //Premièrement on crée (ou utilise) une chaine de caractères pour y met le contenu à afficher
    float fbuf = 123.4567;
    static char cbuf[10]; //Doit être static (ou déclaré en global) afin de conserver la référence
    //snprintf(cbuf, sizeof(cbuf), "%f", fbuf);   //OK
    sprintf(cbuf, "%.5f", fbuf); //OK
    //sprintf(cbuf,"123.4567");                   //OK
    //snprintf(cbuf, sizeof(cbuf), "123.4567");   //OK

    //Deuxièmement on transfert la référence de la chaine de caractère au bon endroit dans le tableau de pointeurs de chaine de caractère
    //xText[0] = cbuf;                            //OK, la référence de cbuf est transmise en pointeur no. 0 au tableau xTexT[]
    xText[0] = &cbuf[0]; //OK, équivalent à "xText[0] = cbuf;", mais plus explicite
    //xText[0] = "123.4567";                      //OK, le compilateur transfert automatiquement la référence de cette string en pointeur no. 0 au tableau xTexT[]
    monMenu.setItemValeur(noItemX, 0); //Pour mettre à jour l'affichage de la ligne d'item
  }
  else if (valeur == 2)
  {
    //Cas pour la valeur courante "2"
    //Modification de l'étiquette de l'item X pour valeur 2 de l'item Y
    sprintf(itemZ_Etiquette, "Coco = "); //Modification de la variable caractère utilisée pour cette étiquette
    monMenu.actualiserUnItem(noItemZ);   //Pour forcer le rafraichissement de cette ligne sans modification de la valeur courante
  }
  else if (valeur == 3)
  {
    //Cas pour la valeur courante "3"
    //Rotation des valeurs texte de l'item X
    char *ptrbuf;
    ptrbuf = xText[0];                 //On conserve le pointeur de l'élément 0
    xText[0] = xText[1];               //Décalage du pointeur 1 dans pointeur 0
    xText[1] = xText[2];               //Décalage du pointeur 2 dans pointeur 1
    xText[2] = xText[3];               //Décalage du pointeur 3 dans pointeur 2
    xText[3] = ptrbuf;                 //Récupération du pointeur 0 dans pointeur 3
    monMenu.actualiserUnItem(noItemX); //Pour forcer le rafraichissement de cette ligne sans modification de la valeur courante
  }
}
void callBackItemZ()
{
  Serial.println("callback callBackItemZ()"); //Pour signifier sur le terminal
}
