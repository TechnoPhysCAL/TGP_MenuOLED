/*
Librairie MenuOLED - Exemple5_AsDisplay.ino

Programme de démonstration de l'utilisation de la librairie MenuOLED
utilisant un item de menu en guise d'affichage seulement.

Le programme simule un système d'alarme de température. Lorsque la température dépasse un
certain seuil, l'alarme est mise ON et le demeure jusqu'à temp que le l'usager la mettre OFF.
La température est simulée par une rampe montante et descendnte entre 0oC et 100oC.

Le menu est composé de 3 items:
  1- Alarme :   Item de type booléen dont la fonction callback sert à controler l'état de la Del rouge.
                L'usager peut éditer cet item; il s'en sert entre autres pour éteindre l'alarme.
  2- Temp. °C : Cet item sert d'affichage pour la température; il n'est pas éditable par l'usager.
  3- Seuil °C : L'usager édite cet item pour ajuster le seuil de température déclenchant une alarme.

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
Date: 16 aout 2019
Modification : CB, jan. 2025
*/

#include <MenuOLED.h>                            //Pour utiliser la librairie MenuOLED
#include <BoutonPin.h>
#include <Ecran.h>	
#include <DelPin.h>
const char nomProg[] = "Exemple5_AsDisplay.ino"; //Nom du programme pour transmission sur terminal

//Prototypes des fonctions pour callback du menu:
//Il faut déclarer les fonctions callback du menu avant leur utilisation dans la définition du menu
void alarmeCallback(void); //Routine callback pour l'alarme
void TempCallback(void);   //Routine callback pour l'ajustement de la LED2
void seuilCallback(void);  //Routine callback pour l'item X

//Déclaration des variables utilisées pour le retour des numéros d'item du menu
int noItemAlarme, noItemTemp, noItemSeuil;

//Déclaration des instances de boutons et de l'écran
BoutonPin gauche(33);
BoutonPin droite(39);
BoutonPin haut(34);
BoutonPin bas(35);
BoutonPin selection(36);
Ecran ecran;

//Déclaration de l'instance monMenu du type MenuOLED
MenuOLED monMenu(&ecran,&gauche,&droite,&haut,&bas,&selection);

//Déclaration des instances de DELs
DelPin rouge(4);

//Déclaration de variables utilitaires
uint32_t lastMillis = 0;    //Pour retenir le temps
int temperatureSimulee = 0; //Variable pour la simulation de la mesure de température.
int incTemperature = 1;     //Variable pour l'incréemtation de la mesure de température.

void setup()
{
  Serial.begin(115200);    //Pour la communication série
  Serial.println(nomProg); //Transmission du nom du programme

  // Initialisation des boutons et de l'écran
  gauche.begin();
  droite.begin();
  haut.begin();
  bas.begin();
  selection.begin();
  ecran.begin();

  //Initialisation du menu
  monMenu.begin();

  rouge.begin(); //Initialisation de la DEL rouge

  //Paramètres de chaque type d'item du menu (voir "MenuOLED.h"):
  //Pour chaque item de type NUMERIQUE (ItemNumerique), les paramètres sont:
  //  - String Etiquette; pointeur sur une string,
  //  - void(*callbackFct)(); pointeur sur fonction callback
  //  - int ValeurInitiale; valeur initiale de l'item
  //  - int ValeurMin; valeur minimale de l'item
  //  - int ValeurMax, valeur maximale de l'item
  //  - bool editable, true(defaut)=item éditable, false=item non editable
  //Pour chaque item de type ON_OFF (ItemOnOff), les paramètres sont:
  //  - String Etiquette; pointeur sur une string,
  //  - void(*callbackFct)(); pointeur sur fonction callback
  //  - int ValeurInitiale; valeur initiale de l'item, 0=Off et 1=On
  //  - bool editable, true(defaut)=item éditable, false=item non editable
  //Pour chaque item de type TEXTE (ItemTexte), les paramètres sont:
  //  - String Etiquette; pointeur sur une string,
  //  - void(*callbackFct)(); pointeur sur fonction callback
  //  - int ValeurInitiale; valeur initiale de l'item
  //  - int nbChoix; nombre de choix différents de texte à afficher,
  //  - String *choixTexte; pointeur sur tableau de pointeurs de de chaines charactère pour les choix de texte à afficher
  //  - bool editable, true(defaut)=item éditable, false=item non éditable

  //Construction du menu par la définition de chacun des items
  noItemAlarme = monMenu.ajouterItemOnOff("Alarme   = ", &alarmeCallback, 0);
  noItemTemp = monMenu.ajouterItemNumerique("Temp. °C = ", &TempCallback, 0, -100, 200, ITEM_NON_EDITABLE); //Attention au dernier paramètre
  noItemSeuil = monMenu.ajouterItemNumerique("Seuil °C = ", &seuilCallback, 80, -20, 120);                  //Seuil ajustable entre -20 et 120; valeur intiale 80.

  //Impression de la ligne Titre
  monMenu.imprimeLigneTitreOLED("Exemple5_AsDisplay");
  //Impression de la ligne Status
  monMenu.imprimeLigneStatusOLED("Sans erreur");
}

void loop()
{
  // Rafraîchissement des boutons et de l'écran
  gauche.refresh();
  droite.refresh();
  haut.refresh();
  bas.refresh();
  selection.refresh();
  ecran.refresh();

  monMenu.refresh(); //Pour permettre le fonctionnement du menu

  rouge.refresh(); //Pour permettre le fonctionnement de la DEL rouge

  //Simulation rampe de température
  if (millis() - lastMillis > 200)
  {
    lastMillis = millis();
    temperatureSimulee += incTemperature;
    if (temperatureSimulee > 100)
    {
      temperatureSimulee = 100;
      incTemperature = -1;
    }
    if (temperatureSimulee < 0)
    {
      temperatureSimulee = 0;
      incTemperature = 1;
    }
    //Ajustement de l'item température
    monMenu.setItemValeur(noItemTemp, temperatureSimulee, SANS_APPEL_CALLBACK); //Ajustement de la valeur courante sans appel fct callback
  }
  //Verification pour alarme de température
  if (temperatureSimulee >= monMenu.getItemValeur(noItemSeuil))
  {
    monMenu.setItemValeur(noItemAlarme, 1, AVEC_APPEL_CALLBACK); //Ajustement alarme et appel callback pour allumer la Del
  }
}

//Définition des fonctions callback propes à chacun des items du menu
void alarmeCallback()
{
  //Ajuste la DEL rouge selon la valeur courante de l'alarme
  rouge.set(monMenu.getItemValeur(noItemAlarme) != 0);
}
void TempCallback()
{
  Serial.println("callback TempCallback"); //Pour signifier sur le terminal
}
void seuilCallback()
{
  Serial.println("callback seuilCallback"); //Pour signifier sur le terminal
}
