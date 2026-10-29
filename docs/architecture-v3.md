# Documentation technique — Bras robotique V3

Ce document complète le [README](../README.md). Les valeurs ci-dessous reflètent des essais ou des informations historiques, **pas une validation définitive des performances**.

## État actuel

| Axe PCA9685 | Articulation | État |
|---|---|---|
| 0 | Base | Testée |
| 1 | Épaule | Testée |
| 2 | Coude | Testé |
| 3 | Inclinaison du poignet | Intégrée et testée |
| 4 | Rotation du poignet (MF90) | Installée, mouvement au repos à corriger |
| 5+ | Pince | Non réalisée |

Les *six fonctions visées* correspondent aux cinq axes ci-dessus **plus la pince**. Elles ne sont pas six axes opérationnels à ce jour.

## Commande électronique

- ESP32 Dev Board ; programme en **C++ / environnement Arduino**.
- Contrôleur **PCA9685** : I²C `0x40`, fréquence `50 Hz`.
- SDA : ESP32 GPIO21 → PCA9685 SDA.
- SCL : ESP32 GPIO22 → PCA9685 SCL.
- ESP32 3V3 → VCC logique ; masses reliées.
- Alimentation externe des servos : **6 V**, limitation de courant réglée à **3 A pour les essais**. Cette limite ne constitue pas le dimensionnement final.
- Joystick alimenté en **3,3 V** : X → GPIO34 ; Y → GPIO35 ; bouton SW → GPIO32 ; masse commune.

### Joystick et logiciel

Le joystick sert à commander une **vitesse de déplacement relative** plutôt qu'une position absolue. Les trois modes du programme de travail sont :
1. Base et épaule
2. Coude et inclinaison du poignet
3. Rotation du poignet, seconde direction réservée à la pince

Le firmware comprend également une logique de sauvegarde des positions *commandées* en mémoire non volatile grâce à `Preferences.h`. Il ne mesure pas les positions physiques réelles. La validation sur la machine complète reste à terminer.

### Plages PWM du programme de travail

| Axe | Limites dans le fichier fourni |
|---|---|
| Base | 500–2500 µs |
| Épaule | 1000–2000 µs |
| Coude | 500–2500 µs |
| Inclinaison poignet | 700–2300 µs |
| Rotation poignet | 500–2500 µs, non validée |

**Attention :** ces plages ont été explorées pendant la calibration et ne sont pas des limites sûres universelles. Vérifier physiquement chaque articulation, en l'absence de risque de collision ou de surcharge, avant toute utilisation.

## Mécanique et impression 3D

La V3 a été modélisée sous **Fusion 360** et imprimée principalement en **PETG** sur une **Bambu Lab A1** (buse 0,4 mm, profil 0,20 mm). Les paramètres discutés incluent quatre à cinq parois, un remplissage typique de 15 à 20 %, des motifs Cubic/Gyroid et des supports adaptés par pièce. Certaines pièces ont nécessité des révisions en raison de supports cassés ou de passages de câbles trop étroits.

Un défaut majeur est la **reprise des efforts de flexion et radiaux par les arbres des servomoteurs** lorsque les articulations ne sont supportées que d'un côté. La base présente aussi des à-coups de type **stick-slip**, liés aux frottements entre pièces imprimées. Les pistes d'amélioration incluent des axes passifs, des roulements, un guidage bilatéral et une réduction des masses en porte-à-faux.

## Références à confirmer

- Servos principaux Miuzei : référence potentielle **MS24-F / type DS3218**, puissance annoncée variable selon les descriptions (20/25 kg·cm). Contrôler l'étiquette avant de publier une fiche chiffrée.
- Poignet : **Miuzei MF90**, essayé sur le canal 4.
- Dimensions, masse totale, charge utile, amplitudes physiques et consommation maximale : **non mesurées / non validées**.

## Évolutions prévues, non réalisées

Pince robotique ; ESP32-CAM au poignet ; perception et reconnaissance d'objets ; automatisation ; intégration à OSIRIS ; puis éventuelle V4 avec roulements, transmissions, pièces métalliques et électronique dédiée.

La version V2 utilisait un modèle 3D provenant d'Internet, **pas une conception CAO personnelle**. Son modèle d'origine ne doit pas être redistribué sans contrôle des droits.
