## [Non publié] - 2026-09-17 (8fb920c1)

### Ajouté

- Migration de nouveaux services métier vers `FMTWrapperCore`, incluant les domaines Operating Area et Area Variability, afin de poursuivre l'isolation de la logique applicative du wrapper .NET.
- Automatisation de la construction de la pile COIN-OR sous RTools, avec prise en charge des solveurs GLPK, CLP et de l'intégration MOSEK pour les environnements R.
- Renforcement du flux de génération et de validation des messages de commit, avec centralisation de la gestion des erreurs et amélioration de la robustesse du processus.

### Modifié

- Refactorisation du module des contraintes : les classes liées aux bornes et aux spécifications ont été séparées dans des modules dédiés afin d'améliorer la maintenabilité du code (#202).
- Poursuite de la migration de l'architecture vers `FMTWrapperCore` avec nettoyage du code, ajustements du contrôleur et consolidation des tests unitaires.
- Renommage de la macro d'export du Core vers `FMT_WRAPPER_CORE_EXPORT` pour uniformiser l'API exportée.
- Mise à jour de la documentation Doxygen, du changelog intégré et des exemples R afin de refléter la nouvelle architecture et la nomenclature courante.
- Migration de l'espace de noms du Core de `FMTWrapperCore` vers `FMTWrapper::Backend` dans l'ensemble du Core, des tests et du wrapper. La bibliothèque, la cible CMake et le dossier `FMTWrapperCore` demeurent inchangés.
- Adaptation du wrapper .NET et des tests afin d'utiliser systématiquement le nouvel espace de noms `FMTWrapper::Backend`.

### Corrigé

- Correction de l'export des symboles du Core pour les builds partagés.
- Correction d'un problème de parser affectant les tables d'actions et la comparaison des thèmes (#341).
- Correction de divers problèmes apparus durant la phase finale de migration du Core vers une architecture modulaire.
- Amélioration de la récupération et de l'affichage des descriptions d'erreurs dans l'interface (#336).
- Correction et mise à jour de la génération du Change Log intégré à l'application (#328).
- Correction d'un problème empêchant la réinterprétation du scénario ROOT (#244).
- Ajout et validation du support GLPK dans les environnements ciblés (#276).
- Stabilisation des composants de journalisation et de gestion des avertissements lors des travaux de migration du Core.

### Cassant : Modifié

- **Cassant :** l'API C++ publique du Core utilise désormais l'espace de noms `FMTWrapper::Backend` à la place de `FMTWrapperCore`. Tout code client faisant référence à `FMTWrapperCore::*` doit être adapté.

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
- Correction de la compilation lorsque le support Mosek est désactivé

## [v1.2.0] - 2026-08-20 (26e42c7a)

### Ajouté
- Prise en charge du nouveau mot-clé `_SHIFT` pour la modélisation et la définition de scénarios, avec exemples associés.
- Ajout du support des fichiers **Parquet** grâce à la mise à jour de GDAL.
- Ajout du support et de fonctionnalités complémentaires pour le solveur **GLPK**, incluant des tests dédiés.
- Exposition des solveurs disponibles dans les interfaces publiques, ainsi que de nouvelles fonctionnalités liées à la gestion des solveurs.
- Exposition de `FMTmask::decompose` dans l’interface Python.
- Exposition des thèmes statiques dans l’interface R.
- Ajout de nouvelles possibilités de gestion des exceptions, notamment `FMTExceptionHandler::getErrorsToIgnore`.
- Ajout d’un mécanisme de récupération du logger et du gestionnaire d’exceptions dans l’interface utilisateur après un incident, permettant de conserver le contexte et les journaux existants.
- Ajout d’une conversion explicite UTF-8 vers les chaînes système dans l’interface R (`_convertToSystemString`), améliorant la compatibilité des caractères accentués et internationaux.
- Ajout de nouveaux exemples C++, Python et R, incluant notamment *Map to Area*, l’exploration des sorties, la catégorisation des rendements ainsi que des scénarios démontrant l’utilisation de requêtes SQL, de bases de données SQLite et de constantes dans les fichiers de rendements.
- Réintégration du support **FMTExcel**.

### Modifié
- **Cassant :** harmonisation majeure des noms de classes, fichiers et en-têtes vers une convention uniforme (`FMTAction`, `FMTModel`, `FMTAreaParser`, etc.), avec refonte importante de la structure de l’API publique.
- Refonte de l’API R et des wrappers avec standardisation des noms en `camelCase`, corrections de casse et amélioration de la cohérence générale.
- Refonte importante de la gestion des exceptions avec meilleure encapsulation, nouvelles abstractions et amélioration de la propagation des erreurs.
- Refactorisation de l’infrastructure de journalisation et de gestion des exceptions du wrapper afin d’améliorer la robustesse, la récupération après erreur et la conservation des journaux.
- Introduction et maturation d’une infrastructure d’optimisation par lots (*batch* / *mini-batch*).
- Amélioration des algorithmes de recuit simulé (*Simulated Annealing*), incluant la gestion des taux d’annealing et du comportement d’optimisation.
- Amélioration de la compatibilité avec les anciennes versions de GDAL/PROJ lors des opérations spatiales, notamment pour la gestion des couches OGR, des projections et des opérations de rastérisation.
- Optimisation des performances de `FMTSemodel::postSolve`, des mécanismes de cache et de certains parcours internes du graphe afin de réduire le temps de traitement des modèles spatiaux.

- Renforcement du moteur de parsing :
  - meilleure prise en charge des constantes dans les requêtes SQL ;
  - gestion plus robuste de `FOREACH` et `*INCLUDE` ;
  - amélioration du traitement insensible à la casse pour les constantes, variables et directives ;
  - amélioration de la lecture des fichiers d’horaires (`schedule`) avec prise en compte des constantes ;
  - amélioration de la détection des dépendances récursives dans les rendements complexes.
- Mise à jour de la documentation automatique Python/R, de la documentation UI/Excel, des exemples, du README, de la couverture de tests et du changelog.
- Modernisation importante de l’infrastructure de compilation et de distribution :
  - compatibilité améliorée avec R 4.5, MSVC et vcpkg ;
  - intégration de **mimalloc** dans les compilations MSVC ;
  - simplification et nettoyage des scripts de génération et de publication.
- Amélioration de la génération, de l’exposition et de la distribution du changelog dans les livrables.

### Corrigé
- Correction de plusieurs problèmes de compilation et d’intégration des interfaces R, Python, Excel et Windows.
- Résolution de plusieurs blocages, pertes de journaux et plantages silencieux dans le wrapper, notamment lors de la journalisation, de la destruction des loggers et de la propagation des exceptions (#313 Adaptation Log et Exception Handler dans FMTWrapper).
- Correction d’un plantage de l’interface utilisateur causé par la gestion des pointeurs dans le système de journalisation.
- Correction de l’optimisation spatialement explicite lorsqu’aucune cache n’est disponible (#317 Optimisation spatialement Explicite non Fonctionnelle sans la cache).
- Correction et optimisation de `FMTSemodel::postSolve`, améliorant la stabilité et les performances des traitements spatiaux.
- Correction de la gestion des voisinages et des contraintes d’adjacence dans les algorithmes d’ordonnancement spatial.
- Correction de l’optimisation du replanning.
- Correction du modèle de rendement basé sur les arbres de décision.
- Correction de la lecture d’une transition GCBM vide (#311 Lecture d'une transition GCBM vide).
- Correction des défaillances de rastérisation causées par des thèmes invalides (#326 Invalid Theme Missing Mask sous rastérisation CC).
- Correction de l’issue **#201 Incapable de faire de la rastérisation**, grâce à une gestion plus robuste des projections GDAL/OGR lors des opérations de rastérisation et de reprojection.
- Correction des mécanismes de remplacement et de la nouvelle syntaxe de modélisation (#323 Nouvelle syntaxe, #322 rxreplace, #321 _replace, #316 Adaptation regex pour constante SQL).
- Correction du parseur de constantes utilisé dans les requêtes SQL et résolution de l’issue **#338 SQL Query**, rétablissant la résolution correcte des variables et constantes dans les requêtes SQL.
- Correction de la prise en compte de la période de mise à jour (#331 Prise en compte de la période de mise à jour).
- Correction de plusieurs problèmes liés aux règles de patch, aux caractères d’échappement, au multithreading et aux interfaces utilisateur.
- Correction de la compilation lorsque le support Mosek est désactivé.
- Correction de plusieurs problèmes liés aux gestionnaires d’exceptions et au traitement des avertissements.
- Réduction significative de l’utilisation mémoire lors des étapes de présolve (#204 Too much memory used for presolve).
- Correction de nombreux problèmes de compilation, dépendances, documentation et scripts d’installation.

### Supprimé
- Retrait de la dépendance `magic_enum`.
- Suppression d’un composant de cache obsolète dans l’interface utilisateur, simplifiant l’implémentation interne.
- Suppression de plusieurs fichiers, en-têtes et variantes historiques devenus obsolètes à la suite de la normalisation de l’API.
- Suppression d’anciens scripts de compilation et configurations devenus redondants.