# MiniDisplay_UI

Permet l'affichage et l'edition de parametres organisés sous forme de pages sur des ecran oled ou lcd communiquant via un bus i2c.
La navigation entre les pages et les paramètres est assurée pas soit 3 entrées digitales (lecture uniquement), soit 5 entrées digitales (lecture / edition).
- Plateforme supportée : Bela.
- Utilise la librairie [u8g2](https://github.com/olikraus/u8g2) adaptée par [Giulio Moro pour etre utilisable sur Bela](https://github.com/giuliomoro/O2O)
- exemple de page affichée sur un ecran oled :
<picture>
  <source media="(prefers-color-scheme: dark)" srcset="https://github.com/PascalFaivre/MiniDisplay_UI/blob/master/doc/exscreen.jpg">
  <source media="(prefers-color-scheme: light)" srcset="https://github.com/PascalFaivre/MiniDisplay_UI/blob/master/doc/exscreen.jpg">
  <img alt="exemple de page sur écran oled" src="https://github.com/PascalFaivre/MiniDisplay_UI/blob/master/doc/exscreen.jpg">
</picture>

## Fonctionnalités :
- dimensions de l'ecran (en caractères) configurable (la taille de la font est déterminé par ce parametre)
- ajout / suppression de pages
- ajout / suppression de parametres. la valeur d'un parametre est de type double. il peut etre affiché en décimal, entier ou booleen
- navigation à 3 touches en lecture et a 5 touches en lecture et edition
- edition du parametre en cours de selection via l'interface de navigation
- liaison possible d'un parametre a une valeur externe via la fourniture d'un pointeur sur celle-ci a la creation du parametre
- gestion dynamique du rafraichissement de l'écran (est rafraichit uniquements quand des changements ont eu lieu sur la fenetre d'affichage (valeur, selection, page))
    

## Utilisation :
    voir exemple : render.cpp


## TODO :
- type de page : Information ( pas de parametres mais juste des lignes de texte)
- type de parametre : liste déroulante
- type de page : Boot  ( page auto définie par un fichier présent dans le dossier project de bela contenant les différents projets bootable. 
                                      Sur une selection et validation d'un de ceux-ci bela redemarrera sur le project en question)
- type de page : Bargraph (la valeur des parametres est presenté en bargraph vertical)
- type de page : Trill_Setup ( page auto définie permetant de modifier les reglages des capteurs trill (prescaler, threshold...) present sur le systeme)
- utilisation possible d'un capteur trill pour l'edition de parametres
- intégration à Faust2bela
    
    
    
## Notes :
- Actuellements 3 types d'écrans sont supportés :
    "U8G2_SH1106_128X64_NONAME_F_HW_I2C_LINUX", "U8G2_SSD1306_128X64_NONAME_F_HW_I2C_LINUX", "U8G2_SSD1309_128X64_NONAME2_F_HW_I2C_LINUX".
    il est facile d'en ajouter car la librairie u8g2 en supporte enormément.
- Le rafraichisement du buffer de l'ecran via i2c est très lent (env 30ms pour un ecran de 128x64 pixels) 
    donc si vous utilisez des capteurs en i2c il est preferable que les ecrans et les capteurs soient sur un bus i2c différent.
 