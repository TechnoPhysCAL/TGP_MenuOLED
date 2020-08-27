/*
Librairie MenuOLED - Exemple3_MenuOnOff.ino

Programme de démonstration de mise ON et OFF du menu afin de libérer l'affichage OLED
aux besoins du programme principal. À l'aide du dernier item du menu, l'usager 
décide de mettre OFF le menu. Une fois le menu OFF, le programme principal utilise 
l'affichage pour une autre tâche tout en vérifiant l'appuie simultanée et soutenue
des boutons GAUCHE et DROITE afin restaurer le menu.

Fondamentalement, ce programme commande 2 DELs à partir du menu. D'autres items du menu
ne servent qu'à montrer les fonctionnalités du menu sans rien commander.

Note: Ce programme est conçu pour la carte PROTOTPHYS 2V1.
  
Auteur: Jude Levasseur
Date: 05 juillet 2019
Modification : CB, sept 2020
*/

#include <MenuOLED.h>                            //Pour utiliser la librairie MenuOLED
const char nomProg[] = "Exemple3_MenuOnOff.ino"; //Nom du programme pour transmission sur terminal

//Prototypes des fonctions pour callback du menu:
//Il faut déclarer les fonctions callback du menu avant leur utilisation dans la définition du menu
void ajusteLED1(void);            //Routine callback pour l'ajustement de la LED1
void ajusteLED2(void);            //Routine callback pour l'ajustement de la LED2
void callBackItemX(void);         //Routine callback pour l'item X
void callBackItemY(void);         //Routine callback pour l'item Y
void callBackItemZ(void);         //Routine callback pour l'item Z
void callBackItemMenuOnOff(void); //Routine callback pour l'item MenuOnOff

//Déclaration des variables utilisées pour le retour des numéros d'item du menu
int noItemLED1, noItemLED2, noItemX, noItemY, noItemZ, noItemMenuOnOff;

//Déclaration des pointeurs de texte utile pour affichage en mode texte de l'item LED2
char *niveauLED2[] = {"Eteint", "Bas", "Moyen", "Fort"};
//Nombre d'éléments texte de niveauLED2[], nécessaire pour pour affichage en mode texte de l'item LED2
int nbChoixLED2 = sizeof(niveauLED2) / sizeof(niveauLED2[0]); //Calcul automatique (=4 pour le cas présent)

//Déclaration de l'instance monMenu du type MenuOLED
MenuOLED monMenu;

//Initialisation du message local pour restaurer le menu
void initMessage()
{
  monMenu.ecran.clearDisplay();
  monMenu.ecran.setCursor(0, 12);
  monMenu.ecran.print("Maintenir les boutons");
  monMenu.ecran.setCursor(0, 22);
  monMenu.ecran.print("GAUCHE et DROITE");
  monMenu.ecran.setCursor(0, 32);
  monMenu.ecran.print("pour retour au menu.");
  monMenu.ecran.display();
}
bool flagInitMessage = false;  //Flag pour initialisation du message local
uint32_t oldmillis = millis(); //Pour retenir le temps
void afficheTemps()
{
  monMenu.ecran.fillRect(0, 48, monMenu.ecran.width(), 10, BLACK); //Pour effacer la ligne
  monMenu.ecran.setCursor(0, 48);
  monMenu.ecran.print("Temps = ");
  monMenu.ecran.print((millis() / 1000) % 3600); //Compte les secondes durant une heure
}

void setup()
{
  Serial.begin(115200);    //Pour la commucation série
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
  noItemLED2 = monMenu.ajouterItemTexte("LED 2  = ", &ajusteLED2, 0, nbChoixLED2, &niveauLED2[0]);
  noItemX = monMenu.ajouterItemNumerique("Item X = ", &callBackItemX, 0, 0, 512);
  noItemY = monMenu.ajouterItemNumerique("Item Y = ", &callBackItemY, 0, -500000, 500000);
  noItemZ = monMenu.ajouterItemNumerique("Item Z = ", &callBackItemZ, 0, -10, 50);
  noItemMenuOnOff = monMenu.ajouterItemOnOff("Menu   = ", &callBackItemMenuOnOff, 1);

  //Impression de la ligne Titre
  monMenu.imprimeLigneTitreOLED("Exemple3_MenuOnOff");
  //Impression de la ligne Status
  monMenu.imprimeLigneStatusOLED("Sans erreur");
}

void loop()
{
  monMenu.refresh(); //Pour permettre le fonctionnement du menu et de la plateforme ProtoTPhys

  //On verifie si le menu est OFF
  if (monMenu.getMenuOnOff() == 0)
  {
    //On verifie si les boutons GAUCHE t DROITE sont maintenus
    if (monMenu.gauche.isOnLongPress() && monMenu.droite.isOnLongPress())
    {
      flagInitMessage = false; //Remise à false de l'initialisation message local
      //On restaure le menu
      monMenu.setItemValeur(noItemMenuOnOff, 1); //Remise à ON de l'item Menu
      monMenu.setMenuOn();                       //Restauration du menu
    }
    //On fait fonctionner l'affichage local
    else
    {
      //On initialise l'affichage avec le message pour restaurer le menu
      if (flagInitMessage == false)
      {
        flagInitMessage = true;
        initMessage();
      }
      else
      {
        //On imprime le temps en guise d'utilisation de l'affichage local à tous les secondes
        if (millis() - oldmillis >= 1000)
        {
          oldmillis = millis();
          afficheTemps();
        }
      }
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
void callBackItemMenuOnOff()
{
  if (monMenu.getItemValeur(noItemMenuOnOff) == 0)
  {                       //Si la valeur courante est mise à OFF
    monMenu.setMenuOff(); //On met OFF le menu
  }
}