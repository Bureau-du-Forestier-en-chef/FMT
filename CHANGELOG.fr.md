## [v1.3.0] - 2026-09-11 (e5a606bc)

### Ajouté
- Ajout d'une conversion explicite UTF-8 vers les chaînes système dans l’interface R (`_convertToSystemString`), améliorant la compatibilité des caractères accentués et internationaux.
- Ajout d’une génération automatisée des stubs Python et du packaging associé afin de simplifier la distribution et l’utilisation de FMT depuis Python.
- Ajout de scénarios, jeux de données SQLite et tests démontrant l’utilisation de requêtes SQL directement dans les fichiers de rendements, incluant la prise en charge des constantes.
- Ajout d’un fichier `CMakePresets.json` permettant la configuration automatique des environnements de compilation et une meilleure intégration avec les IDE modernes.

### Modifié
- Amélioration de la compatibilité avec les anciennes versions de GDAL/PROJ lors des opérations spatiales, notamment pour la gestion des couches OGR, des projections et des opérations de rastérisation.
- Amélioration de la portabilité de `FMTObject` et de l’initialisation de GDAL afin de renforcer la compatibilité multi-plateforme.
- Optimisation des performances de `FMTSemodel::postSolve`, des parcours de graphes et des mécanismes de cache afin de réduire le temps de traitement des modèles spatiaux.
- Renforcement du moteur de parsing :
  - meilleure prise en charge des constantes dans les requêtes SQL ;
  - gestion plus robuste de `FOREACH` et `*INCLUDE` ;
  - amélioration du traitement insensible à la casse ;
  - amélioration de la lecture des fichiers de calendrier (`schedule`) avec prise en compte des constantes ;
  - amélioration de la détection des dépendances récursives dans les rendements complexes.
- Amélioration de l’infrastructure interne :
  - ajout d’API de récupération des références de projection GDAL ;
  - ajout d’opérations de filtrage sur les masques ;
  - optimisation de l’accès au cache des rendements.
- Modernisation de l’infrastructure de compilation et de développement :
  - prise en charge des presets CMake ;
  - intégration des configurations d’IDE via `.gitignore` ;
  - suppression de scripts de compilation historiques devenus obsolètes.
- Mise à jour de la documentation, des README, de la couverture de tests, des exemples et du processus de génération du changelog et des messages de commit.

### Corrigé
- Correction d’un plantage de l’interface utilisateur causé par la gestion des pointeurs dans le système de journalisation.
- Correction du parseur de constantes utilisé dans les requêtes SQL.
- Correction de l’issue **#338 SQL Query**, rétablissant la résolution correcte des variables et constantes dans les requêtes SQL.
- Correction de l’issue **#201 Incapable de faire de la rastérisation**, grâce à une gestion plus robuste des projections GDAL/OGR lors des opérations de rastérisation et de reprojection.
- Correction et optimisation de `FMTSemodel::postSolve`, améliorant à la fois la stabilité et les performances des traitements spatiaux.
- Correction de la compilation lorsque le support Mosek est désactiv