# GPS-Arduino
Permet d'afficher les coordonnées ENU (East, North, Up) en mètres sur écran LCD en faisant référence 
à un point géographique identifié à partir de sa longitude et latitude. Ce programme permet de répondre à la question : A quelle distance en coordonnées cartésiennes je me situe par rapport à d'un point géographique de référence. 

### Matériel Requis 
* Un Arduino relié à 
* un écran LCD 162C-BA-BC
* un module GPS NEO6MV2 (chipset u-blox)


Ce code pour microcontroleur Arduino permet de calculer la 
position relative à un point géographique à la surface de la Terre. Les coordonnées sont exprimées en mètres dans un repère local.

### Définition du repère local
La position relative en mètres et coordonnées cartésiennes X,Y,Z est décrite dans un repère local où
* l'axe X est dirigé vers l'est,
* l'axe Y est dirigé vers le nord,
* l'axe Z est dirigé vers le nord.

Ce repère local, aussi appelé ENU (East, North, Up) correspond au repère dessiné en vert sur la figure ci-dessous.
Le point de coordonnées 0,0,0 dans le repère dit local correspond au point de référence.

![Repère ENU](ECEF_ENU_Longitude_Latitude_relationships.png)

### Traitements
L'obtention de la position relative dans ce repère local, nécessite  :
* d'avoir récupéré les coordonnées GPS (longitude, latitude) du point courant à partir d'un module GPS relié 
à l'Arduino via une connexion série à 9600 bits par seconde,
* d'avoir transformé les coordonnées GPS (longitude, latitude) du point courant en coordonnées cartésiennes dans un repère ECEF ("earth-centered, earth-fixed"),
* d'avoir convertit les coordonnées ECEF du point courant en coordonnées de type ENU (East, North, Up) depuis 
le point de référence

La position X,Y est affiché sur l'écran LCD connecté à l'arduino via une liaison série à 19200 bits par seconde.
