/****************************************************************
//Librairie MenuOledTPhys
//Auteur : Jude Levasseur
//Date : juin 2019
//version : 1.0.0
//Description: Permet de créer un menu déroulant de plusieurs 
//items sur affichage OLED SSD1306 128x64 pixels.
//***************************************************************/
/****************************************************************
//version : 1.0.1
//1.  Ajout de limitation du nombre de caractères d'une ligne afin
//    d'éviter les artéfacts de débordement sur ligne suivante.
//Par : Jude Levasseur
//Date: 06 août 2019
//***************************************************************/
/****************************************************************
//version : 1.1.0
//1.  Ajout d'une méthode (actualiserUnItem) afin de forcer
//    manuellement l'actualisation d'un item visible en cas 
//    d'une modification d'étiquette ou du tableau de texte
//    (cas d'un Item de type texte).
//Par : Jude Levasseur
//Date: 06 août 2019
//***************************************************************/
/****************************************************************
//version : 1.1.1
//1.  Aucune modification de fonctionnalité; seulement des 
//    correctifs sur les commentaires et la documentation.
//Par : Jude Levasseur
//Date: 11 août 2019
//***************************************************************/
/****************************************************************
//version : 1.2.0
//1.  Ajout de la rotation des choix texte d'un menu de type texte.
//2.  Ajout paramètre booléen aux méthodes d'ajout d'item afin de rendre un item non éditable.
//3.  Ajout d'un paramètre booléen à la méthode "setItemValeur" permettant
//    l'appel ou non de la fonction callback associée.
//4.  Correction position du souligné: ne s'applique qu'aux items de type numérique seulement.
//Par : Jude Levasseur
//Date: 14 août 2019
//***************************************************************/

#include "MenuOledTPhys.h"
//Constante pour le type d'affichage de la valeur courante de l'item
#define AFFICHENUMERIQUE 0 //Pour afficher la valeur numérique en tant que telle
#define AFFICHEBOOLEAN 1   //Pour afficher la valeur numérique en OFF (si = 0) ou ON (si !=0)
#define AFFICHETEXTE 2     //Pour afficher la valeur numérique en texte associé via un pointeur sur tableau de pointeurs
//Constante pour le nombre maximal de caractères maximum d'une ligne sur OLED
#define NB_MAX_CARACTERES_PAR_LIGNE 21

//Macros utilitaires
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
//Pour le constructeur Adafruit_SSD1306

//***************************************************************
// Déclaration des variables et constantes privées
//***************************************************************
bool _flagModeEdit;                                               //Variable indiquant qu'on est mode d'édition d'item
const int _posLigne[] = {12, 22, 32, 42};                         //Position verticale curseur pour chaque item sauf status et titre
const int16_t _nbLigneMenuOLED = sizeof(_posLigne) / sizeof(int); //Nombre de lignes consacrées aux items du menu dans l'affichage OLED
int16_t _menuItemActif;                                           //Pour indiquer l'item actif du menu
int16_t _precedent_menuItemActif;                                 //Pour indiquer l'item précédemment actif du menu
int16_t _noMenuItemPremiereLigneOLED;                             //Pour indiquer le no de l'item se retrouvant sur la première ligne du OLED
int16_t _nbMenuItem;                                              //Nombre d'items différents dans le menu
uint8_t _addI2C_OLED;                                             //Adresse de l'affichage I2C
int _posSouligne;                                                 //Position du souligné du digit en édition
const int POSSOULIGNEMAX = 8;                                     //Digit max: 0 pour 1er digit, 1 pour 2e digit, etc.
int _increment = 0;                                               //Incrément de la valeur courante de l'item en édition
bool _flagMenuOnOff;                                              //Variable indiquant si le menu est ON (true) ou OFF (false)
String _stringOne;                                                //Pour la manipulation de strings
String _stringTitre, _stringStatus;                               //Pour la conservation du Titre et Status

//Structure de la composition d'un item de menu
typedef struct menuItem
{
  const char *label;         //Pointeur sur une chaine pour le nom de l'item
  void (*callbackFct)(void); //Pointeur sur fonction callback
  int value;                 //Variable pour la valeur courante de l'item
  int minVal;                //Valeur minimale de la valeur courante
  int maxVal;                //Valeur maximale de la valeur courante
  int typeAffichage;         //0=affichage integer, 1=affichage boolean(ON ou OFF), 2=affichage texte via un pointeur sur tableau de pointeurs
  char **valTXT;             //Pointeur sur tableau de pointeurs de valeur texte pour affichage
  bool editable;             //Permettre ou non l'édition de l'item
} _menuItem_t;
_menuItem_t *_menuTab[_NB_MAX_ITEM]; //Tableau de pointeurs de l'ensemble du menu

//***************************************************************
// Constructeur
//***************************************************************
MenuOledTPhys::MenuOledTPhys() : ProtoTGP()
{
  _flagModeEdit = false;
  _menuItemActif = 0;
  _precedent_menuItemActif = 0;
  _noMenuItemPremiereLigneOLED = 0;
  _nbMenuItem = 0;
  _posSouligne = 0;
  _flagMenuOnOff = true;
  _addI2C_OLED = 0x3C; //Adresse par défaut
}

//***************************************************************************************
//Méthodes publiques
//***************************************************************************************
/****************************************************************
Méthode publique pour initialiser la communication I2C avec l'OLED
et les 5 boutons de navigation.
Inputs:
   uint8_t addI2C_OLED : adresse I2C du OLED
   int pinDROITE    : numéro broche pour le bouton de droite
   int pinHAUT      : numéro broche pour le bouton du haut
   int pinBAS       : numéro broche pour le bouton du bas
   int pinGAUCHE    : numéro broche pour le bouton de gauche
   int pinSELECTION : numéro broche pour le bouton de sélection
Output:
  Aucun.
Note: Cette fonction utilise les librairies "ClavierGenerique",
      "Adafruit_SSD1306" et "Adafruit_GFX".
****************************************************************/
void MenuOledTPhys::begin()
{
  ProtoTGP::begin();

  _menuItemActif = 0;               //Initialisation de l'item actif
  _precedent_menuItemActif = 0;     //Initialisation de l'item actif précédent
  _noMenuItemPremiereLigneOLED = 0; //initialisation du no de l'item sur la première ligne du OLED

  //Initialisation du clavier 5 boutons
  gauche().setLongPressDelay(1000);  //Pour ajuster le temps de l'état "MAINTENU"
  gauche().setLongPressInterval(50); //Pour ajuster le temps de l'état "MAINTENANT" (répétition)

  droite().setLongPressDelay(1000);  //Pour ajuster le temps de l'état "MAINTENU"
  droite().setLongPressInterval(50); //Pour ajuster le temps de l'état "MAINTENANT" (répétition)

  haut().setLongPressDelay(1000);  //Pour ajuster le temps de l'état "MAINTENU"
  haut().setLongPressInterval(50); //Pour ajuster le temps de l'état "MAINTENANT" (répétition)

  bas().setLongPressDelay(1000);  //Pour ajuster le temps de l'état "MAINTENU"
  bas().setLongPressInterval(50); //Pour ajuster le temps de l'état "MAINTENANT" (répétition)

  selection().setLongPressDelay(1000);  //Pour ajuster le temps de l'état "MAINTENU"
  selection().setLongPressInterval(50); //Pour ajuster le temps de l'état "MAINTENANT" (répétition)

  // Clear buffer OLED
  ecran().getAdafruit().clearDisplay();

  imprimeLigneTitreOLED("Titre");
  imprimeLigneStatusOLED("Status");
} //FIN MenuOledTPhys::initialiser

/****************************************************************
Méthode publique pour ajouter un item de type NUMERIQUE au menu
Inputs:
  const char *Etiquette : pointeur sur une chaine pour l'étiquette de l'item, 
  void(*callbackFct)()  : pointeur sur la fonction callback
  int ValeurInitiale    : valeur initiale de l'item
  int ValeurMin         : valeur minimale de l'item
  int ValeurMax         : valeur maximale de l'item
  bool editable         : true(defaut)=item éditable, false=item non éditable
  Output:
  int   : retourne le numéro d'identification de l'item; la numérotation débute à 0.
        : retourne -1 si les paramètres sont invalides pour créer ce type d'item
Note: 
****************************************************************/
int MenuOledTPhys::ajouterItemNumerique(const char *Etiquette, void (*callbackFct)(), int ValeurInitiale, int ValeurMin, int ValeurMax, bool editable)
{
  int returnVal = 0;
  //Test de validité des paramètres
  if (ValeurMax < ValeurMin)
  {
    returnVal = -1;
  }
  if (ValeurInitiale < ValeurMin)
  {
    returnVal = -1;
  }
  if (ValeurInitiale > ValeurMax)
  {
    returnVal = -1;
  }
  if (returnVal == 0)
  {
    returnVal = addItemMenu(Etiquette, callbackFct, ValeurInitiale, ValeurMin, ValeurMax, 0, NULL, editable);
  }
  return (returnVal);
} //FIN MenuOledTPhys::ajouterItemNumerique

/****************************************************************
Méthode publique pour ajouter un item de type ON-OFF au menu
Inputs:
  const char *Etiquette : pointeur sur une chaine pour l'étiquette de l'item, 
  void(*callbackFct)()  : pointeur sur la fonction callback
  int ValeurInitiale    : valeur initiale de l'item; 0 pour OFF et 1 pour ON
  bool editable         : true(defaut)=item éditable, false=item non éditable
  Output:
  int   : retourne le numéro d'identification de l'item; la numérotation débute à 0.
        : retourne -1 si les paramètres sont invalides pour créer ce type d'item
Note: 
****************************************************************/
int MenuOledTPhys::ajouterItemOnOff(const char *Etiquette, void (*callbackFct)(), int ValeurInitiale, bool editable)
{
  int returnVal = 0;
  //Test de validité des paramètres
  if (!(ValeurInitiale == 0 || ValeurInitiale == 1))
  {
    returnVal = -1;
  }
  if (returnVal == 0)
  {
    returnVal = addItemMenu(Etiquette, callbackFct, ValeurInitiale, 0, 1, 1, NULL, editable);
  }
  return (returnVal);
} //FIN MenuOledTPhys::ajouterItemOnOff

/****************************************************************
Méthode publique pour ajouter un item de type TEXTE au menu
Inputs:
  const char *Etiquette : pointeur sur une chaine pour l'étiquette de l'item, 
  void(*callbackFct)()  : pointeur sur la fonction callback
  int ValeurInitiale    : valeur initiale de l'item
  int nbChoix           : nombre de choix différents de texte
                        : devrait être égale à la dimension du tableau de chaines caractères
  char **choixTexte     : pointeur sur tableau de pointeurs de chaine de caractère pour les choix de texte à afficher
  bool editable         : true(defaut)=item éditable, false=item non éditable
  Output:
  int   : retourne le numéro d'identification de l'item; la numérotation débute à 0.
        : retourne -1 si les paramètres sont invalides pour créer ce type d'item
Note: Il est primordial que le paramètre "nbChoix" soit égal (ou inférieur) à la  
      dimension du tableau texte pointé par "char **choixTexte"
****************************************************************/
int MenuOledTPhys::ajouterItemTexte(const char *Etiquette, void (*callbackFct)(), int ValeurInitiale, int nbChoix, char **choixTexte, bool editable)
{
  int returnVal = 0;
  //Test de validité des paramètres
  if (ValeurInitiale < 0)
  {
    returnVal = -1;
  }
  if (nbChoix <= 0)
  {
    returnVal = -1;
  }
  if (ValeurInitiale > (nbChoix - 1))
  {
    returnVal = -1;
  }
  if (returnVal == 0)
  {
    nbChoix--;
    returnVal = addItemMenu(Etiquette, callbackFct, ValeurInitiale, 0, nbChoix, 2, choixTexte, editable);
  }
  return (returnVal);
} //FIN MenuOledTPhys::ajouterItemTexte

/****************************************************************
Méthode publique pour actualiser le menu en prenant en compte l'état des boutons.
Cette fonction gère entre autres la navigation à travers le menu ainsi
que l'édition des items par les boutons.
Inputs:
  Aucun
Output:
  Aucun
Note: Cette fonction suit une logique "state machine" basée principalement 
      sur l'état du flag "_flagModeEdit". Au besoin, elle fait appel à la
      fonction "updateMenuOLED()" pour la mise à jour de l'affichage.
****************************************************************/
void MenuOledTPhys::refresh()
{
  //Mise à jour des boutons
  ProtoTGP::refresh();
  //Mise à jour du heartbeat
  heartbeat();
  // Rafraichissement de l'affichage s'il n'est pas OFF
  if (_flagMenuOnOff == true)
  {
    //Premièrement, on vérifie si on est en mode d'édition
    /*****************State en mode d'édition************************/
    if (_flagModeEdit == true)
    {
      //On vérifie d'abors si on doit sortir du mode d'édition
      if (selection().isPressed() /*|| flagRemote == true*/)
      {
        _flagModeEdit = false;
        _increment = 0;
        _posSouligne = 0;
        updateMenuOLED();                           //Pour retirer le char du mode edition
        (*_menuTab[_menuItemActif]->callbackFct)(); //On execute le callback à la fin de l'édition
      }
      //On execute le mode d'édition de la valeur courante selon les états de boutons
      else
      {
        //1er - On ajuste le souligné du digit selon boutons DROITE et GAUCHE pour l'édition de valeur numérique
        if (droite().isPressed() && _menuTab[_menuItemActif]->typeAffichage == AFFICHENUMERIQUE)
        { //Correction version 1.2.0
          _posSouligne--;
          limitePosSouligne();
          updateMenuOLED();
        }
        else if (gauche().isPressed() && _menuTab[_menuItemActif]->typeAffichage == AFFICHENUMERIQUE)
        { //Correction version 1.2.0
          _posSouligne++;
          limitePosSouligne();
          updateMenuOLED();
        }
        //2e - On ajuste l'incrément de la valeur courante

        if (haut().isPressed() || haut().isLongPressed())
        {
          if (_menuTab[_menuItemActif]->typeAffichage == AFFICHENUMERIQUE)
          {
            _increment = pow(10, _posSouligne);
          }
          else
          {
            _increment = 1;
          }
        }
        else if (bas().isPressed() || bas().isLongPressed())
        {
          if (_menuTab[_menuItemActif]->typeAffichage == AFFICHENUMERIQUE)
          {
            _increment = -pow(10, _posSouligne);
          }
          else
          {
            _increment = -1;
          }
        }
        else
          _increment = 0;
        //3e - Si l'incréement est non nul, on l'ajoute à la valeur courante et on met à jour l'item
        if (_increment != 0)
        {
          int buf;
          if (INT_MAX > INT16_MAX)
          {
            buf = _menuTab[_menuItemActif]->value + _increment;
          }
          else
          {
            buf = add_and_saturate(_menuTab[_menuItemActif]->value, _increment); //Pour éviter le débordement 16 bits
          }
          //Limitation minimale
          if (buf < _menuTab[_menuItemActif]->minVal)
          {
            if (_menuTab[_menuItemActif]->typeAffichage == AFFICHETEXTE)
            { //Pour un item de type texte, on fait une rotation des valeurs (ajout version 1.2.0)
              buf = _menuTab[_menuItemActif]->maxVal;
            }
            else
            {
              buf = _menuTab[_menuItemActif]->minVal; //Pour les autres types, on limite à la valeur minimale
            }
          }
          //Limitation maximale
          if (buf > _menuTab[_menuItemActif]->maxVal)
          {
            if (_menuTab[_menuItemActif]->typeAffichage == AFFICHETEXTE)
            { //Pour un item de type texte, on fait une rotation des valeurs (ajout version 1.2.0)
              buf = _menuTab[_menuItemActif]->minVal;
            }
            else
            {
              buf = _menuTab[_menuItemActif]->maxVal; //Pour les autres types, on limite à la valeur maximale
            }
          }
          //Écriture nouvelle valeur
          if (buf != _menuTab[_menuItemActif]->value)
          {
            _menuTab[_menuItemActif]->value = buf;
            limitePosSouligne();
            updateMenuOLED();
          }
        }
      }
    } /********************Fin state en mode d'édition *********************/

    //Cas où l'on n'est pas en édition
    /********************State en mode navigation *********************/
    else
    {
      //On vérifie d'abors si on doit aller en mode d'édition
      if (selection().isPressed() && _menuTab[_menuItemActif]->editable == ITEM_EDITABLE)
      { //Ajout bool editable version 1.2.0
        _flagModeEdit = true;
        updateMenuOLED(); //Pour afficher le char du mode edition
      }
      //On execute la navigation dans le menu selon l'état des boutons
      else if (haut().isPressed() || haut().isLongPressed())
      {
        _precedent_menuItemActif = _menuItemActif; //Pour signifier un changement de menu actif
        if (_menuItemActif > 0)
        {
          _menuItemActif--;
          updateMenuOLED();
        }
      }
      else if (bas().isPressed() || bas().isLongPressed())
      {
        _precedent_menuItemActif = _menuItemActif; // Pour signifier un changement d'item actif
        if (_menuItemActif < _nbMenuItem - 1)
        {
          _menuItemActif++;
          updateMenuOLED();
        }
      }
    } /********************FIN State en mode navigation *********************/
  }
} // FIN MenuOledTPhys::rafraichir()

/****************************************************************
Méthode publique pour imprimer la ligne réservée au TITRE du menu.
Inputs:
  char* TitreEtiquette  : pointeur sur la chaine de caractère du titre
Output:
  Aucun
Note: 
****************************************************************/
void MenuOledTPhys::imprimeLigneTitreOLED(char *TitreEtiquette)
{
  _stringTitre = TitreEtiquette; //Pour conserver le titre
  restoreTitreOLED();
} //FIN MenuOledTPhys::imprimeLigneTitreOLED

/****************************************************************
Méthode publique pour imprimer la ligne réservée au STATUS du menu.
Inputs:
  char* StatusEtiquette  : pointeur sur la chaîne de caractère du status
Output:
  Aucun
Note: 
****************************************************************/
void MenuOledTPhys::imprimeLigneStatusOLED(char *StatusEtiquette)
{
  _stringStatus = StatusEtiquette; //Pour conserver le status
  restoreStatusOLED();
} //FIN MenuOledTPhys::imprimeLigneStatusOLED

/****************************************************************
Méthode publique pour obtenir la valeur courante d'un item de menu.
Inputs:
  int noItem  : numéro de l'item de menu attribué par les méthodes d'ajout d'items
Output:
  int         : retour de la valeur courante
              : retourne "0" si paramètre invalide
Note: 
****************************************************************/
//Fonction pour obtenir la valeur courante d'un item de menu
int MenuOledTPhys::getItemValeur(int noItem)
{
  if (noItem >= 0 && noItem < _nbMenuItem)
  {
    return (_menuTab[noItem]->value);
  }
  else
  {
    return (0);
  }
} // FIN MenuOledTPhys::getItemValeur

/****************************************************************
Méthode publique pour ajuster la valeur courante d'un item de menu.
Inputs:
  int noItem        : numéro de l'item de menu attribué par les méthodes d'ajout d'items
  int valeur        : valeur courante à imposer à l'item
  bool callfctback  : true(defaut)= avec appel de la fonction callback, false= sans appel de la fonction callback
Output:
  Aucun
Note: La valeur est limitée selon les limites de l'item.
      La fonction callback est appelée.
****************************************************************/
void MenuOledTPhys::setItemValeur(int noItem, int valeur, bool callfctback)
{
  if (noItem >= 0 && noItem < _nbMenuItem)
  {
    if (valeur < _menuTab[noItem]->minVal)
    {
      valeur = _menuTab[noItem]->minVal;
    } //Limitation minimale
    if (valeur > _menuTab[noItem]->maxVal)
    {
      valeur = _menuTab[noItem]->maxVal;
    } //Limitation maximale
    _menuTab[noItem]->value = valeur;
    printItemMenuOLED(noItem);
    if (callfctback == AVEC_APPEL_CALLBACK)
    { //Ajout condition version 1.2.0
      (*_menuTab[noItem]->callbackFct)();
    }
  }
} // FIN MenuOledTPhys::setItemValeur

/****************************************************************
Méthode publique pour obtenir l'état du menu; soit ON ou OFF.
Inputs:
  Aucun
Output:
  bool  : "false" pour menu OFF et "true" pour menu ON
Note: 
****************************************************************/
bool MenuOledTPhys::getMenuOnOff(void)
{
  return (_flagMenuOnOff);
} //FIN MenuOledTPhys::getMenuOnOff

/****************************************************************
Méthode publique pour mettre ON le menu. Au retour de l’affichage 
par cette méthode, le menu est reconstitué tel qu’il était 
avant sa désactivation. 
Inputs:
  Aucun
Output:
  Aucun
Note: 
****************************************************************/
void MenuOledTPhys::setMenuOn(void)
{
  if (_flagMenuOnOff == false)
  {
    _flagMenuOnOff = true;
    //ecran().initializeScreen(); // initialize with the I2C addr et faire reset
    ecran().getAdafruit().clearDisplay();
    restoreTitreOLED();      //Pour restaurer le Titre sur OLED
    printAllItemsMenuOLED(); //Pour restaurer l'affichage du menu
    restoreStatusOLED();     //Pour restaurer le Status sur OLED
  }
} //FIN MenuOledTPhys::setMenuOn

/****************************************************************
Méthode publique pour mettre OFF le menu.
Inputs:
  Aucun
Output:
  Aucun
Note: 
****************************************************************/
void MenuOledTPhys::setMenuOff(void)
{
  if (_flagMenuOnOff == true)
  {
    _flagMenuOnOff = false;
    ecran().getAdafruit().clearDisplay();
  }
} //FIN MenuOledTPhys::setMenuOff

/****************************************************************
Méthode publique pour obtenir le nombre d'items dans le menu.
Inputs:
  Aucun
Output:
  int  : nombre d'items dans le menu
Note: 
****************************************************************/
int MenuOledTPhys::getNbItems(void)
{
  return (_nbMenuItem);
} // FIN MenuOledTPhys::getNbItems

/****************************************************************
  Méthode publique pour forcer manuellement l'actualisation d'un item.
  Inputs:
    int noItem  : numéro de l'item de menu attribué par les méthodes d'ajout d'items
  Output:
    Aucun
  Note: Nécessaire pour actualiser un item visible sur OLED dont l'étiquette
        ou le tableau de texte (pour itemTexte) ont été modifiés.
  ****************************************************************/
void MenuOledTPhys::actualiserUnItem(int noItem)
{
  if (noItem >= 0 && noItem < _nbMenuItem)
  {
    printItemMenuOLED(noItem);
  }
} // FIN MenuOledTPhys::void actualiserUnItem(int noItem)

//****FIN des Méthodes publiques*********************************************************

//***************************************************************************************
//Méthodes privées
//**************************************************************************************
/****************************************************************
Fonction universelle pour l'ajout d'un item au menu. Elle permet
un code universel des différents types d'Item; NUMERIQUE, ON-OFF ou TEXTE
Inputs:
  const char *Label     : pointeur sur une chaine pour l'étiquette de l'item, 
  void(*callbackFct)()  : pointeur sur la fonction callback
  int ValeurInitiale    : valeur initiale de l'item
  int ValeurMin         : valeur minimale de l'item
  int ValeurMax         : valeur maximale de l'item
  int typeAffichage     : 0= type NUMERIQUE, 1= type ON-OFF et 2= type TEXTE 
  char **choixTexte     : pointeur sur tableau de pointeurs de chaine de charactère pour les choix de texte à afficher
Output:
  int   : retourne le numéro d'identification de l'item; la numérotation débute à 0.
Note:
****************************************************************/
int MenuOledTPhys::addItemMenu(const char *Label, void (*callbackFct)(), int ValeurInitiale, int ValeurMin, int ValeurMax, int typeAffichage, char **choixTexte, bool editable)
{
  //Création d'un nouvel Item de menu
  int noItem = _nbMenuItem;
  _menuTab[noItem] = new _menuItem_t;
  //Populer le nouvel Item
  _menuTab[noItem]->label = Label;
  _menuTab[noItem]->callbackFct = callbackFct;
  _menuTab[noItem]->value = ValeurInitiale;
  _menuTab[noItem]->minVal = ValeurMin;
  _menuTab[noItem]->maxVal = ValeurMax;
  _menuTab[noItem]->typeAffichage = typeAffichage;
  _menuTab[noItem]->valTXT = &choixTexte[0];
  _menuTab[noItem]->editable = editable;
  _nbMenuItem++;             //Incremente le nombre d'items
  printItemMenuOLED(noItem); //Pour imprimer le présent Item
  return (noItem);           //retourne le numéro de l'item
} //FIN MenuOledTPhys::addItemMenu

/****************************************************************
Fonction pour rafraichir l'impression d'un item présent sur l'affichage OLED.
Inputs:
  int16_t menuItem  : numéro de l'item dans l'ensemble du menu 
Output:
  Aucun
Note: l'impression n'a lieu que si l'item est présent sur le OLED
****************************************************************/
void MenuOledTPhys::printItemMenuOLED(int16_t menuItem)
{
  //Calcul de la position de l'item sur l'affichage OLED
  int noLigne = menuItem - _noMenuItemPremiereLigneOLED;
  //On imprime seulement si l'item est dans l'écran actuel
  if (noLigne >= 0 && noLigne < _nbLigneMenuOLED && menuItem < _nbMenuItem && _flagMenuOnOff == true)
  {
    //Premier caractère
    if (menuItem == _menuItemActif)
    {
      _stringOne = String(">");
    }
    else
    {
      _stringOne = String(" ");
    }
    //Ajout étiquette et valeur courante
    _stringOne += String(_menuTab[menuItem]->label);
    if (_menuTab[menuItem]->typeAffichage == AFFICHENUMERIQUE)
    {
      //Affichage de la valeur en type entier
      _stringOne += String(_menuTab[menuItem]->value);
    }
    else if (_menuTab[menuItem]->typeAffichage == AFFICHEBOOLEAN)
    {
      //Affichage de valeur en boolean: 0->OFF
      if (_menuTab[menuItem]->value == 0)
      {
        _stringOne += String("OFF");
      }
      else
      {
        _stringOne += String("ON");
      }
    }
    else if (_menuTab[menuItem]->typeAffichage == AFFICHETEXTE)
    {
      //Affichage via tableau de texte
      char **pi = _menuTab[menuItem]->valTXT;
      _stringOne += String(pi[_menuTab[menuItem]->value]);
    }
    //Il faut limiter la ligne au nombre de caractères de l'affichage moins 1 (pour caractère édition)
    // Jude: A faire
    if (_stringOne.length() > (NB_MAX_CARACTERES_PAR_LIGNE - 1))
    {
      _stringOne = _stringOne.substring(0, (NB_MAX_CARACTERES_PAR_LIGNE - 1));
    }

    //Ajout caractère du mode édition
    if (_flagModeEdit == true && menuItem == _menuItemActif)
    {
      _stringOne = _stringOne + String("<");
    }
    //Imprime l'item
    ecran().getAdafruit().setTextSize(1);
    ecran().getAdafruit().setTextColor(WHITE);
    ecran().getAdafruit().invertDisplay(false);
    ecran().getAdafruit().fillRect(0, _posLigne[noLigne], ecran().width(), 10, BLACK); //Pour effacer la ligne
    ecran().getAdafruit().setCursor(0, _posLigne[noLigne]);
    ecran().getAdafruit().print(_stringOne);

    //Imprime souligné du digit en édition
    if (_flagModeEdit == true && menuItem == _menuItemActif && _menuTab[menuItem]->typeAffichage == 0)
    {
      limitePosSouligne();
      int longueur = _stringOne.length();
      _stringOne = String("");
      for (int i = 0; i < (longueur - (_posSouligne + 2)); i++)
      {
        _stringOne += String(" ");
      }
      _stringOne += String("_");
      ecran().getAdafruit().setCursor(0, _posLigne[noLigne] + 2);
      ecran().getAdafruit().print(_stringOne);
    }
    ecran().getAdafruit().display();
  }
} //MenuOledTPhys::printItemMenuOLED

/****************************************************************
Fonction pour imprimer tous les items présents sur l'affichage OLED.
Inputs:
  Aucun 
Output:
  Aucun
Note: 
****************************************************************/
void MenuOledTPhys::printAllItemsMenuOLED(void)
{
  for (int i = 0; i < _nbLigneMenuOLED; i++)
  {
    printItemMenuOLED(i + _noMenuItemPremiereLigneOLED);
  }
} //FIN MenuOledTPhys::printAllItemsMenuOLED

/****************************************************************
Fonction pour actualiser les items devant être présents dans l'affichage OLED
Inputs:
  Aucun
Output:
  Aucun
Note: 
****************************************************************/
void MenuOledTPhys::updateMenuOLED()
{
  //Vérifier s'il faut déplacer (dérouler) les items sur le OLED
  if (_menuItemActif > (_noMenuItemPremiereLigneOLED + _nbLigneMenuOLED - 1))
  {
    //Il faut décaler vers le bas
    _noMenuItemPremiereLigneOLED = _menuItemActif - _nbLigneMenuOLED + 1;
    //On imprime toutes les lignes
    printAllItemsMenuOLED();
  }
  else if (_menuItemActif < (_noMenuItemPremiereLigneOLED))
  {
    //Il faut décaler vers le haut
    _noMenuItemPremiereLigneOLED = _menuItemActif;
    //On imprime toutes les lignes
    printAllItemsMenuOLED();
  }
  //On vérifie les lignes qui ont changé d'état d'édition devant être mises à jour
  else if (_menuItemActif != _precedent_menuItemActif)
  {
    printItemMenuOLED(_menuItemActif);
    printItemMenuOLED(_precedent_menuItemActif);
  }
  else
  {
    printItemMenuOLED(_menuItemActif);
  }
} //FIN MenuOledTPhys::updateMenuOLED()

/****************************************************************
Fonction pour gérer le "heartbeat" sur l'affichage OLED
Inputs:
  Aucun
Output:
  Aucun
Note: 
****************************************************************/
void MenuOledTPhys::heartbeat(void)
{
  static uint32_t oldmillis;
  static char color = WHITE;
  if ((millis() - oldmillis) > 1000u)
  {
    oldmillis = millis();
    if (color == WHITE)
    {
      color = BLACK;
    }
    else
    {
      color = WHITE;
    }
    ecran().getAdafruit().fillRect(ecran().width() - 3, ecran().height() - 3, 3, 3, color); //Petit carré à droite au bas de l'écran
    ecran().getAdafruit().display();
  }
} //FIN MenuOledTPhys::heartbeat

/****************************************************************
Fonction pour limiter la position du souligné d'édition au nombre de digits affichés
Inputs:
  Aucun
Output:
  Aucun
Note: Cette fonction affecte la variable "_posSouligne" qui est utilisé 
      pour ajuster l'incrementation de la valeur courante d'un item
****************************************************************/
void MenuOledTPhys::limitePosSouligne(void)
{
  //Limitation position souligné selon le nombre de digits
  int bufVal = _menuTab[_menuItemActif]->value;
  int nbDigit = 0;
  while (bufVal / 10 != 0)
  {
    nbDigit++;
    bufVal = bufVal / 10;
  }
  _posSouligne = MIN(_posSouligne, nbDigit);
  _posSouligne = MAX(_posSouligne, 0);              //Limitation au digit unité
  _posSouligne = MIN(_posSouligne, POSSOULIGNEMAX); //Limitation au digit maximal
} //FIN MenuOledTPhys::limitePosSouligne

/****************************************************************
Fonction pour éviter le débordement 16 bits lors d'une somme 
Inputs:
  int16_t x : valeur à laquelle on additionne un second nombre 
  int16_t y : second nombre de l'addition
Output:
  int16_t : résultat de l'addition saturé à 16 bits
Note: 
****************************************************************/
int16_t MenuOledTPhys::add_and_saturate(int16_t x, int16_t y)
{
  int32_t z = (int32_t)x + y;
  if (z > INT16_MAX)
  {
    z = INT16_MAX;
  }
  else if (z < INT16_MIN)
  {
    z = INT16_MIN;
  }
  return (int16_t)z;
} //FIN MenuOledTPhys::add_and_saturate

/****************************************************************
Fonction pour restaurer le Titre sur OLED
Inputs:
  char* TitreEtiquette  : pointeur sur la chaîne de caractère du titre
Output:
  Aucun
Note: 
****************************************************************/
void MenuOledTPhys::restoreTitreOLED()
{
  ecran().getAdafruit().setTextSize(1);
  ecran().getAdafruit().setTextColor(WHITE);
  ecran().getAdafruit().invertDisplay(false);
  ecran().getAdafruit().fillRect(0, 0, ecran().width(), 10, BLACK); //Pour effacer le titre et la ligne en dessous
  ecran().getAdafruit().setCursor(0, 0);
  ecran().getAdafruit().print(_stringTitre);
  ecran().getAdafruit().drawLine(0, 9, ecran().width() - 1, 9, WHITE); //Trace ligne en-dessous de l'entête
  ecran().getAdafruit().display();
} //FIN MenuOledTPhys::restoreTitreOLED

/****************************************************************
Fonction pour restaurer le Status sur OLED
Inputs:
  Aucun
Output:
  Aucun
Note: 
****************************************************************/
void MenuOledTPhys::restoreStatusOLED()
{
  ecran().getAdafruit().setTextSize(1);
  ecran().getAdafruit().setTextColor(WHITE);
  ecran().getAdafruit().invertDisplay(false);
  ecran().getAdafruit().fillRect(0, ecran().height() - 7, ecran().width() - 4, 10, BLACK); //Pour effacer la ligne sauf Heartbeat
  ecran().getAdafruit().setCursor(0, ecran().height() - 7);
  ecran().getAdafruit().print(_stringStatus);
  ecran().getAdafruit().drawLine(0, ecran().height() - 9, ecran().width() - 1, ecran().height() - 9, WHITE); //Trace ligne en-dessous de l'entête
  ecran().getAdafruit().display();
} //FIN MenuOledTPhys::restoreStatusOLED()
