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
- Ajout de nouveaux exemples C++, Python et R, incluant notamment *Map to Area*, l’exploration des sorties et la catégorisation des rendements.
- Réintégration du support **FMTExcel**.

### Modifié
- **Cassant :** harmonisation majeure des noms de classes, fichiers et en-têtes vers une convention uniforme (`FMTAction`, `FMTModel`, `FMTAreaParser`, etc.), avec refonte importante de la structure de l’API publique.
- Refonte de l’API R et des wrappers avec standardisation des noms en `camelCase`, corrections de casse et amélioration de la cohérence générale.
- Refonte importante de la gestion des exceptions avec meilleure encapsulation, nouvelles abstractions et amélioration de la propagation des erreurs.
- Refactorisation de l’infrastructure de journalisation et de gestion des exceptions du wrapper afin d’améliorer la robustesse, la récupération après erreur et la conservation des journaux.
- Introduction et maturation d’une infrastructure d’optimisation par lots (*batch* / *mini-batch*).
- Amélioration des algorithmes de recuit simulé (*Simulated Annealing*), incluant la gestion des taux d’annealing et du comportement d’optimisation.
- Mise à jour de la documentation automatique Python/R, de la documentation UI/Excel, des exemples et du README.
- Modernisation importante de l’infrastructure de compilation et de distribution :
  - compatibilité améliorée avec R 4.5, MSVC et vcpkg ;
  - intégration de **mimalloc** dans les compilations MSVC ;
  - simplification et nettoyage des scripts de génération et de publication.
- Amélioration de la génération, de l’exposition et de la distribution du changelog dans les livrables.

### Corrigé
- Correction de plusieurs problèmes de compilation et d’intégration des interfaces R, Python, Excel et Windows.
- Résolution de plusieurs blocages, pertes de journaux et plantages silencieux dans le wrapper, notamment lors de la journalisation, de la destruction des loggers et de la propagation des exceptions (#313 Adaptation Log et Exception Handler dans FMTWrapper).
- Correction de l’optimisation spatialement explicite lorsqu’aucune cache n’est disponible (#317 Optimisation spatialement Explicite non Fonctionnelle sans la cache).
- Correction de la gestion des voisinages et des contraintes d’adjacence dans les algorithmes d’ordonnancement spatial.
- Correction de l’optimisation du replanning.
- Correction du modèle de rendement basé sur les arbres de décision.
- Correction de la lecture d’une transition GCBM vide (#311 Lecture d'une transition GCBM vide).
- Correction des défaillances de rastérisation causées par des thèmes invalides (#326 Invalid Theme Missing Mask sous rastérisation CC).
- Correction des mécanismes de remplacement et de la nouvelle syntaxe de modélisation (#323 Nouvelle syntaxe, #322 rxreplace, #321 _replace, #316 Adaptation regex pour constante SQL).
- Correction de la prise en compte de la période de mise à jour (#331 Prise en compte de la période de mise à jour).
- Correction de plusieurs problèmes liés aux règles de patch, aux caractères d’échappement, au multithreading et aux interfaces utilisateur.
- Réduction significative de l’utilisation mémoire lors des étapes de présolve (#204 Too much memory used for presolve).
- Correction de nombreux problèmes de compilation, dépendances, documentation et scripts d’installation.

### Supprimé
- Retrait de la dépendance `magic_enum`.
- Suppression de plusieurs fichiers, en-têtes et variantes historiques devenus obsolètes à la suite de la normalisation de l’API.
- Suppression d’anciens scripts de compilation et configurations devenus redondants.
