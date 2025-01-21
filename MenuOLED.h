#ifndef __MenuOLED_H__
#define __MenuOLED_H__

#include "Bouton.h"
#include "Adafruit_SSD1306.h"

#define _NB_MAX_ITEM 128          //Nombre maximale d'item dans le menu
#define ITEM_NON_EDITABLE false   //Pour méthodes d'ajout d'item
#define ITEM_EDITABLE true        //Pour méthodes d'ajout d'item
#define SANS_APPEL_CALLBACK false //Pour méthode setItemValeur
#define AVEC_APPEL_CALLBACK true  //Pour méthode setItemValeur

/******************************************************************************
* Definitions
******************************************************************************/
class MenuOLED
{
public:
    MenuOLED(Adafruit_SSD1306* adafruit_ss1306, Bouton* gauche, Bouton* droite, Bouton* haut, Bouton* bas, Bouton* selection);
    void begin();                                                                                                                                   //Pour initialiser
    int ajouterItemNumerique(String Etiquette, void (*callbackFct)(), int ValeurInitiale, int ValeurMin, int ValeurMax, bool editable = true); //Pour ajouter un item de type Numérique au menu
    int ajouterItemOnOff(String Etiquette, void (*callbackFct)(), int ValeurInitiale, bool editable = true);                                   //Pour ajouter un item de type On-Off au menu, 0=OFF, 1=ON
    int ajouterItemTexte(String Etiquette, void (*callbackFct)(), int ValeurInitiale, int nbChoix, String* choixTexte, bool editable = true);   //Pour ajouter un item de type texte au menu;
    void refresh(void);                                                                                                                             //Pour rafraichir le menu
    void imprimeLigneTitreOLED(String TitreEtiquette);                                                                                               //Fonction pour imprimer la ligne de Titre
    void imprimeLigneStatusOLED(String StatusEtiquette);                                                                                             //Fonction pour imprimer la ligne de Status
    int getItemValeur(int noItem);                                                                                                                  //Fonction pour obtenir la valeur courante d'un item de menu
    void setItemValeur(int noItem, int valeur, bool callfctback = true);                                                                            //Fonction pour ajuster la valeur courante d'un item de menu
    void setMenuOn(void);                                                                                                                           //Fonction pour mettre ON le menu.
    void setMenuOff(void);                                                                                                                          //Fonction pour mettre OFF le menu.
    bool getMenuOnOff(void);                                                                                                                        //Fonction pour obtenir l'état du menu; soit ON ou OFF.
    int getNbItems(void);                                                                                                                           //Fonction pour obtenir le nombre d'items du menu.
    void actualiserUnItem(int noItem);                                                                                                              //Fonction pour forcer manuellement l'actualisation d'un item (ajout de la version 1.1.0)

private:
    int addItemMenu(String Label, void (*callbackFct)(), int ValeurInitiale, int ValeurMin, int ValeurMax, int typeAffichage, String* choixTexte, bool editable);
    void updateMenuOLED();                          //Mise à jour du menu Oled
    void heartbeat(void);                           //Fonction pour la mise à jur du heartbeat
    void printItemMenuOLED(int16_t menuItem);       //Fonction pour imprimer une ligne du menu sur OLED
    void printAllItemsMenuOLED(void);               //Fonction pour imprimer toutes lignes du menu sur OLED
    void limitePosSouligne();                       //Fonction pour limiter la position du souligné d'édition au nombre de digits affichés
    int16_t add_and_saturate(int16_t x, int16_t y); //Fonction pour éviter le débordement 16 bits lors d'une somme
    void restoreTitreOLED(void);                    //Fonction pour restaurer le Titre sur OLED
    void restoreStatusOLED(void);                   //Fonction pour restaurer le Status sur OLED
    Bouton* gauche;
    Bouton* droite;
    Bouton* haut;
    Bouton* bas;
    Bouton* selection;                    //Déclaration des boutons
    Adafruit_SSD1306* ecran;
};
#endif
