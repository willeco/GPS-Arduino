# GPS-Arduino
Permet d'afficher les coordonnées ENU (East, North, Up) en mètres sur écran LCD en faisant référence 
à un point géographique identifié à partir de sa longitude et latitude. 
## Matériel Requis 
* Un Arduino relié à 
* un écran LCD 162C BA BC
* un GPS NEO6MV2 par liaison série


Ce code pour microcontroleur Arduino permet de calculer la 
position relative en coordonnées cartésiennes X,Y,Z après avoir pris pour 
référence à un point géographique.

La position relative en mètres et coordonnées cartésiennes X,Y,Z est décrite dans un repère local où
* l'axe X est dirigé vers l'est,
* l'axe Y est dirigé vers le nord,
* l'axe Z est dirigé vers le nord.

Ce repère local, aussi appelé ENU (East, North, Up) correspond au repère en vert sur la figure suivante.
Le point de coordonnées 0,0 dans le repère dit local correspond au point de référence.

![Repère ENU](ECEF_ENU_Longitude_Latitude_relationships.png)


L'obtention de la position relative dans ce repère local, nécessite  :

* d'avoir récupéré les coordonnées GPS  (longitude, latitude) à partir d'un module GPS relié 
à l'arduino via une connexion série à 9600 bits par seconde,
* d'avoir transformé les coordonnées GPS (longitude, latitude) en coordonnées cartésienne 
de type X,Y,Z dans un repère ECEF ("earth-centered, earth-fixed"),
* d'avoir convertit les coordonnées X,Y,Z en coordonnées de type ENU (East, North, Up) depuis 
le point de référence

La position X,Y peut ensuite être affiché sur l'écran LCD connecté à l'arduino via une liaison série à 19200 bits par seconde.
