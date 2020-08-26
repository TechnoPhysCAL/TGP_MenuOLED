#ifndef __MENUOLEDTPHYS_H__
#define __MENUOLEDTPHYS_H__

#include "ProtoTGP.h"

#define OLED_RESET -1    //Si pas nécessaire, il faut alors mettre <= 0
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define _NB_MAX_ITEM 128          //Nombre maximale d'item dans le menu
#define ITEM_NON_EDITABLE false   //Pour méthodes d'ajout d'item
#define ITEM_EDITABLE true        //Pour méthodes d'ajout d'item
#define SANS_APPEL_CALLBACK false //Pour méthode setItemValeur
#define AVEC_APPEL_CALLBACK true  //Pour méthode setItemValeur

//Les états détectables des boutons (voir Bouton.h) sont:
// RELACHE,     (= 0) pour détecter l'état stable relaché du bouton
// RELACHANT,   (= 1) pour détecter le front lors du relachement du bouton
// ENFONCE,     (= 2) pour détecter l'état stable enfoncé du bouton
// ENFONCANT,   (= 3) pour détecter le front lors de l'enfoncement du bouton
// MAINTENU,    (= 6) pour détecter l'état continuellement enfoncé du bouton
// MAINTENANT,  (= 7) pour détecter le début de l'état continuellement enfoncé du bouton
enum clavierProtoTphys
{
    DROITE = 0,
    HAUT,
    BAS,
    GAUCHE,
    SELECTION
}; //Label pour les adressage des 5 boutons

/******************************************************************************
* Definitions
******************************************************************************/
class MenuOledTPhys : public ProtoTGP
{
public:
    MenuOledTPhys();
    void begin();                                                                                                                                   //Pour initialiser
    int ajouterItemNumerique(const char *Etiquette, void (*callbackFct)(), int ValeurInitiale, int ValeurMin, int ValeurMax, bool editable = true); //Pour ajouter un item de type Numérique au menu
    int ajouterItemOnOff(const char *Etiquette, void (*callbackFct)(), int ValeurInitiale, bool editable = true);                                   //Pour ajouter un item de type On-Off au menu, 0=OFF, 1=ON
    int ajouterItemTexte(const char *Etiquette, void (*callbackFct)(), int ValeurInitiale, int nbChoix, char **choixTexte, bool editable = true);   //Pour ajouter un item de type texte au menu;
    void refresh(void);                                                                                                                             //Pour rafraichir le menu
    void imprimeLigneTitreOLED(char *TitreEtiquette);                                                                                               //Fonction pour imprimer la ligne de Titre
    void imprimeLigneStatusOLED(char *StatusEtiquette);                                                                                             //Fonction pour imprimer la ligne de Status
    int getItemValeur(int noItem);                                                                                                                  //Fonction pour obtenir la valeur courante d'un item de menu
    void setItemValeur(int noItem, int valeur, bool callfctback = true);                                                                            //Fonction pour ajuster la valeur courante d'un item de menu
    void setMenuOn(void);                                                                                                                           //Fonction pour mettre ON le menu.
    void setMenuOff(void);                                                                                                                          //Fonction pour mettre OFF le menu.
    bool getMenuOnOff(void);                                                                                                                        //Fonction pour obtenir l'état du menu; soit ON ou OFF.
    int getNbItems(void);                                                                                                                           //Fonction pour obtenir le nombre d'items du menu.
    void actualiserUnItem(int noItem);                                                                                                              //Fonction pour forcer manuellement l'actualisation d'un item (ajout de la version 1.1.0)

private:
    int addItemMenu(const char *Label, void (*callbackFct)(), int ValeurInitiale, int ValeurMin, int ValeurMax, int typeAffichage, char **choixTexte, bool editable);
    void updateMenuOLED();                          //Mise à jour du menu Oled
    void heartbeat(void);                           //Fonction pour la mise à jur du heartbeat
    void printItemMenuOLED(int16_t menuItem);       //Fonction pour imprimer une ligne du menu sur OLED
    void printAllItemsMenuOLED(void);               //Fonction pour imprimer toutes lignes du menu sur OLED
    void limitePosSouligne();                       //Fonction pour limiter la position du souligné d'édition au nombre de digits affichés
    int16_t add_and_saturate(int16_t x, int16_t y); //Fonction pour éviter le débordement 16 bits lors d'une somme
    void restoreTitreOLED(void);                    //Fonction pour restaurer le Titre sur OLED
    void restoreStatusOLED(void);                   //Fonction pour restaurer le Status sur OLED
};
#endif
