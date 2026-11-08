# Eressea Tools

Outils C++ historiques pour manipuler les rapports de tours Eressea, vérifier des ordres et afficher des informations sur les régions. La solution Visual Studio comprend dix projets (bibliothèques et exécutables).

## Outils

| Projet / exécutable | Rôle |
|---|---|
| `CheckOrders` | Vérifie la syntaxe des ordres Eressea et leur cohérence avec les données du rapport de tour. |
| `CRDiff` | Compare des caractéristiques et ressources de régions entre deux rapports `.cr`, affiche les écarts et peut recopier les valeurs de référence dans le rapport traité. |
| `CRFaction` | Modifie des factions d’un rapport : anonymisation et suppression facultative des factions sans unité. |
| `CRFind` | Parcourt les lignes de voyage du rapport pour signaler les unités ou navires mentionnés comme voyageurs invisibles, mais absents du rapport. |
| `CRID` | Convertit les identifiants Eressea fournis en argument de base 36 vers leur représentation décimale. |
| `CRSaveOrders` | Extrait les ordres confirmés d’un rapport `.cr` et les écrit dans des fichiers texte, pour les factions configurées ou spécifiées. Peut produire un fichier par faction et limiter l’extraction aux régions sélectionnées. `-h` ou `-?` affiche son aide. |
| `CRSave` (`CRSel`) | Filtre un rapport `.cr` sur une sélection de régions (coordonnées ou fichier `.sel`), peut déplacer l’origine des coordonnées et supprimer des batailles. |
| `EresseaStats` | Application MFC qui présente les régions d’un rapport dans un tableau, avec filtres, tri et affichage facultatif des différences. |
| `CRHelpers` | Bibliothèque commune de lecture, modification et écriture des rapports Eressea, utilisée par les outils. |
| `CRTools` | Bibliothèque de parsing et de structures de données des rapports, partagée par les autres projets. |

Le dossier `RegionStats` contient une ancienne source MFC, mais ce projet n’est pas inclus dans `Eressea.slnx`.

## Prérequis

- Windows et Visual Studio avec la charge de travail **Développement Desktop en C++**.
- Les outils de compilation MSVC **v145**, le SDK Windows et les composants MFC nécessaires aux projets.
- Les projets ciblent **Win32 (x86)** et proposent les configurations **Debug** et **Release**.

## Compilation

Ouvrir `Eressea.slnx` dans Visual Studio, choisir la configuration et la plateforme `Win32`, puis lancer **Générer la solution**. Depuis un terminal développeur Visual Studio, par exemple :

```powershell
msbuild .\Eressea.slnx /m /t:Build /p:Configuration=Debug /p:Platform=Win32
```

Remplacer `Debug` par `Release` pour compiler la configuration optimisée.

## Données d’exécution

Les outils qui analysent ou modifient un rapport nécessitent un rapport de tour `.cr` et, selon l’outil, le fichier de hiérarchie `.crh` (souvent nommé `eressea.crh`). Ces données de jeu ne sont pas fournies dans le dépôt. `CRSaveOrders` peut aussi utiliser la configuration `magellan.ini` pour trouver les factions et mots de passe autorisés.

Les rapports `.cr`, les fichiers d’ordres générés et les données locales de Visual Studio sont exclus du suivi Git. Ne publiez pas de rapports ou de mots de passe de jeu.

## Licence

Le dépôt ne définit pas de licence globale. Des avis de licence distincts figurent dans certains fichiers ; vérifiez-les avant toute réutilisation ou redistribution.
