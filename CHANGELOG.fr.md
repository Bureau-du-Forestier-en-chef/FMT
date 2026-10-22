## [Non publié] - 2026-10-20 (013913b2)

### Ajouté

- Ajout d’une architecture de services dans `FMTWrapperCore`, avec un contrôleur central et des cas d’utilisation dédiés à la planification, aux requêtes de modèles, aux scénarios, aux sessions, aux transformations et aux opérations spatiales (#340 Créer un controleur de Larman et restructurer Wrapper et WrapperCore).
- Migration des fonctionnalités de gestion des unités d’aménagement, de variabilité des superficies, de rastérisation, de sélection et de planification vers des composants portables de `FMTWrapperCore`.
- Ajout d’un système d’événements typés pour transmettre les journaux, avertissements, erreurs et résultats entre le Core et les interfaces.
- Ajout de scénarios de validation de l’interface couvrant notamment la planification, la rastérisation, les transformations, l’optimisation spatialement explicite et la replanification.
- Ajout de suites de tests automatisés pour les interfaces Python et R, lancées comme tests système de CTest.
- Ajout d’une construction automatisée de la pile COIN-OR pour RTools, incluant CLP, GLPK et l’intégration de MOSEK (#335 Compiler Osi avec Mosek+Clp+Glpk pour Rtools, #276 Support for GLPK).
- Ajout d’une documentation détaillée sur l’architecture, les normes de programmation, la compilation, les tests et les règles de contribution du projet.
- Ajout de modèles GitHub structurés pour les rapports de bogues et les demandes de fonctionnalités.
- Ajout de trois niveaux de tests dans `Tests/`, `Unit`, `Integration` et `System`, à côté des bancs de performance de `Performance`. Chaque test porte une étiquette de niveau, et des presets de test lancent un niveau à la fois : `ctest --preset unit`, `integration`, `system`, `system-private`, `performance` ou `all-public` (#350 [FEATURE] Add comprehensive unit tests for the Core namespace).
- Ajout d’un résumé à la fin de chaque passage de tests, à l’installation comme par `tools/tests/RunTests.bat <preset>` : tests réussis, en échec, ignorés et désactivés par niveau, puis la liste des échecs et des tests ignorés avec leur raison.
- Ajout de GoogleTest aux dépendances (`vcpkg.json`) et des premiers tests unitaires du Core, sur `FMTOperator`, `FMTBounds` et `FMTSpec`. Sans GoogleTest, les niveaux unitaire et d’intégration ne sont pas inscrits, et tout le reste se compile et s’exécute comme avant.
- Ajout des premiers tests d’intégration, dans `Tests/Integration/Parser` : allers-retours de `FMTModelParser::write`, de `writeToProject` et de `FMTScheduleParser`, qui comparent le modèle relu au modèle écrit, puis les deux écritures fichier par fichier. Ils ont révélé cinq défauts d’écriture, chacun gardé comme cas désactivé jusqu’à sa correction :
  - #379 [BUG] Un objectif `_PENALTY` sans signe est réécrit `y_PENALTY`
  - #380 [BUG] `_DISCOUNTFACTOR` est réécrit en série temporelle de ses paramètres, et son écriture plante quand son bloc `*YT` n’a pas d’autre yield
  - #381 [BUG] Un output qui mêle sources d’inventaire et d’action écrit ses sorties cachées, que chaque réécriture répète
  - #382 [BUG] Une contrainte `_RANDOM(<output>)` est réécrite sans `_RANDOM`
  - #383 [BUG] `FMTYields::operator==` compare les adresses des gestionnaires : deux copies d’un modèle sont inégales, et `writeToProject` écrit toujours le `._yld` d’un scénario
- Exportation par `FMTlib` de 18 classes du Core qu’un programme lié à la DLL, comme un test unitaire, ne pouvait pas utiliser : les comparateurs `FMTActionComparator`, `FMTActualDevelopmentComparator`, `FMTMaskComparator`, `FMTOutputNodeValueComparator`, `FMTOutputNodeOriginComparator`, `FMTOutputSourceComparator`, `FMTThemeComparator` et `FMTTransitionComparator`, `FMTOutputNodeHasher`, `FMTYieldRequest`, `FMTExpression`, `FMTFunctionCall`, `FMTYieldsCache`, `FMTYieldDevelopment`, et les modèles de rendement `FMTYieldModel`, `FMTYieldModelRandom`, `FMTYieldModelTsla` et `FMTYieldModelUnitCoverage` (#350 [FEATURE] Add comprehensive unit tests for the Core namespace).
- Ajout de tests unitaires des expressions et des gestionnaires de rendements du Core, construits en code sans modèle : `FMTOperator`, `FMTExpression`, `FMTFunctionCall`, `FMTData`, `FMTAgeYieldHandler`, `FMTTimeYieldHandler` et `FMTComplexYieldHandler` (#350 [FEATURE] Add comprehensive unit tests for the Core namespace). Ils fixent les comportements connus de #346 et de #347, et ont révélé trois défauts, dont les deux premiers sont gardés comme cas désactivés jusqu’à leur correction :
  - #386 [BUG] Un moins qui suit un opérateur est lu comme une soustraction dans `_EQUATION`
  - #388 [BUG] Les yields d’un bloc `*YT` lisent tous la période de départ du premier
  - #387 [BUG] En colonnes (`_CP`), chaque ligne d’un bloc `*YT` retarde d’une période les valeurs des lignes suivantes
- Ajout de tests unitaires des bornes, des spécifications, des contraintes, des sorties et des rendements du Core, construits en code sans modèle : `FMTBounds` par `FMTAgeBounds`, `FMTYldBounds`, `FMTLockBounds` et `FMTPerBounds`, `FMTSpec`, `FMTConstraint`, `FMTOutput` et `FMTYields`, avec les surcharges, `getAge` et `getEndPoint` des gestionnaires de rendements, et d’un test d’intégration de `FMTTransitionParser` qui fixe le comportement connu de #358 (#350 [FEATURE] Add comprehensive unit tests for the Core namespace). Ils montrent #379, #382 et #383 plus près de leur cause, et ont révélé quatre défauts, chacun gardé comme cas désactivé jusqu’à sa correction :
  - [BUG] `_MAX` part de 0 : le maximum de yields tous négatifs vaut 0 (issue à ouvrir)
  - [BUG] `FMTYields::getFromFactor` multiplie le taux et les années d’un `_DISCOUNTFACTOR` au lieu de sa valeur (issue à ouvrir)
  - [BUG] `FMTYields::getFromFactor` ne multiplie aucun yield quand on n’en nomme aucun, alors que sa documentation dit tous (issue à ouvrir)
  - [BUG] `_ENDPOINT` provoque une violation d’accès dès que sa source sort de ses bornes (issue à ouvrir)
- Ajout de tests système sur le modèle public `TWD_land` : lecture des scénarios (rendements, sorties, opérabilité, transitions, croissance), erreurs de lecture et leurs codes d’exception, scénarios lus ensemble, statistiques de planification et rejeu du calendrier optimal. Une fixture vérifie qu’aucun test ne modifie `Examples/Models`, et les défauts connus restent inscrits comme tests désactivés.
- Ajout de vérifications à `WriteAndReadProject`, `SolveAndWriteSchedules`, `CreateModel` et `UpdateNonSpatialModel`, qui vérifient maintenant ce qu’ils écrivent, et réparation de la vérification morte d’`AddYieldHandlers`.
- Ajout de bancs de performance et d’allocation avec leurs bornes attendues, et de `tools/performance/Measure_Performance.bat`, qui mesure un changement en comparant des passages d’avant et d’après (#349 [FEATURE] Add performance and allocation benchmarks).
- Ajout de `tools/tests/RunPrivateTests.bat`, qui vérifie que les modèles privés sont accessibles, puis lance la campagne de tests privée et son résumé.
- Ajout de `tools/testMigration/Update_PrivateTests.bat`, qui déplace et renomme une liste locale de tests privés (`BFECtests.csv`), et des tables des anciens et des nouveaux noms de tests (`Tests/Support/RenamedTests.csv`, `Tests/Support/RenamedTestCases.csv`).

### Modifié

- **Cassant :** renommage de l’espace de noms public C++ du Core, de `FMTWrapperCore` vers `FMTWrapper::Backend`. Le code client utilisant `FMTWrapperCore::*` doit être adapté.
- **Cassant :** `WITHOUT_TESTS` devient une option lue par sa valeur partout : `-DWITHOUT_TESTS=OFF` ou `false` configure maintenant les tests, alors qu’il les retirait de la compilation racine. `ON` ou `true` les retire toujours.
- Restructuration de l’interface et du wrapper afin de déléguer les traitements métier au contrôleur et aux services de `FMTWrapperCore`, réduisant la logique propre à l’interface (#340 Créer un controleur de Larman et restructurer Wrapper et WrapperCore).
- Division de `FMTBounds.hpp` en classes spécialisées pour les bornes d’âge, de verrouillage, de période et de rendement, ainsi que pour les spécifications (#202 Diviser le fichier FMTbounds.hpp en 6 fichier 1 par classe).
- Réorganisation du système d’installation CMake en modules distincts pour les composants d’exécution, Python, R et les tests.
- Amélioration de la détection et de la configuration des solveurs OSI, MOSEK, CLP et GLPK dans les environnements pris en charge.
- Mise à jour des exemples R pour utiliser la nomenclature courante de l’interface et consolidation de leur validation automatisée.
- Renforcement du générateur de messages de commit avec une validation centralisée, une meilleure gestion des erreurs et des instructions plus robustes.
- Mise à jour de la configuration Doxygen et de la documentation de l’API pour refléter la nouvelle architecture.
- Clarification des exigences de performance et de mémoire, notamment la préallocation et la réutilisation des ressources dans les traitements intensifs et multithreads.
- `cmake --install` ne lance plus par défaut que les tests publics ; la variable de cache `INSTALL_TEST_SUITE` choisit `public`, `all` ou `none`. La campagne privée se lance à part, avant une version. L’installation ne lance plus elle-même les suites Python et R : CTest les lance avec les autres tests.
- Déplacement des tests C++ dans `Tests/System/Cpp`, avec les sous-dossiers `FMTWrapperCore` et `Excel`, et des suites Python et R dans `Tests/System/Python` et `Tests/System/R` ; `Examples/C++` n’existe plus.
- Renommage des exécutables de test C++ d’après l’action qu’ils exercent (`testScenarioReading` devient `ReadScenario`, `doplanning` devient `DoPlanning`), et nom Given/When/Then pour chaque test public. Les noms CTest suivent la forme `<Niveau>.<Suite>.<Cas>`.
- Déplacement des scripts de la racine du dépôt dans `tools/` (`build`, `tests`, `changelog`, `commitMessage`), répertoriés dans `tools/README.md`.
- Réécriture des règles de test de `Documentation/CodingStandards.md` (trois niveaux de tests, Arrange / Act / Assert, noms Given/When/Then) et ajout de consignes Clean Code ; `Documentation/Architecture.md` consigne désormais la dette d’architecture que révèlent les tests, et la portabilité des tests et des chaînes d’intégration.
- `FMTlib` n’est plus lié à mimalloc, qu’il ne chargeait jamais : les exécutables utilisent le tas de l’environnement d’exécution C et, avec MSVC, `-DWITH_MIMALLOC=ON` lie mimalloc en premier dans chaque exécutable.
- Les bancs de performance compilés avec MinGW ont leurs propres bornes (`Tests/Performance/performance-mingw.csv`) et tournent avec `OMP_NUM_THREADS=32` (#373 [BUG] Paquet R (Rtools45) : OpenBLAS compilé avec OpenMP engage environ 65 Mo par processeur logique au chargement de FMT).
- Le générateur de changelog fusionne désormais une section non publiée existante au lieu d’en ajouter une seconde.

### Obsolète

- `WITHOUT_TESTING` est obsolète au profit de `WITHOUT_TESTS` : il retire toujours les tests, avec un avertissement.

### Corrigé

- **Cassant :** `_DISCOUNTFACTOR` utilise maintenant le taux qu’il lit, au lieu du nombre d’années par période, et l’exposant de `HALF` devient la période moins 0,5. Les modèles qui utilisent `_DISCOUNTFACTOR` obtiennent de nouveaux facteurs d’actualisation (#357 `_DISCOUNTFACTOR` n’utilise pas le taux lu).
- `_DISCOUNTFACTOR` lit maintenant un taux décimal, comme `0.04` ou `4.5%`, et une constante, au lieu de tronquer le premier et de s’arrêter sur la seconde (#374 [BUG] _DISCOUNTFACTOR tronque un taux décimal et refuse une constante).
- Inclusion de `actionsmapping.json` dans le paquet Python afin de rétablir le fonctionnement des distributions publiées (#365 Build FMT release 1.3.0 non fonctionnel).
- Correction de la détection de la fonctionnalité ONNX Runtime par `hasFeature("ONNXRUNTIME")` dans les compilations utilisant `FMTWITHONNXR` (#362 hasFeature("ONNXRUNTIME") rend toujours faux).
- Correction de la comparaison des thèmes dans le constructeur des tables d’actions et des valeurs par défaut associées (#341 ; de trop dans un if).
- Correction de calculs de modèles et de l’exécution des tests de l’interface R.
- Amélioration de la récupération et de l’affichage des descriptions d’erreurs dans l’interface (#336 Description des erreurs dans l’interface).
- Correction de la génération et de l’intégration du changelog dans l’application (#328 Change Log).
- Correction d’un problème empêchant la réinterprétation du scénario `ROOT` (#244 On ne peut pas réinterpréter le scénario ROOT!).
- Correction de l’export des symboles de `FMTWrapperCore` pour les bibliothèques partagées.
- Stabilisation de la journalisation, du traitement des avertissements et du cache de modèles pendant la migration vers la nouvelle architecture.
- Correction de la durée de vie du journal de `FMTLpSolver`, qui pouvait faire planter un programme qui copie un modèle résolu : chaque solveur garde maintenant sa propre copie du journal.
- Correction de la compilation MinGW avec RTools 4.5 : `FindOSI` ne réutilise plus les chemins COIN-OR mis en cache par une configuration plus ancienne, `build-coin-stack.sh` reçoit le git que trouve `FindOSI`, et le harnais des bancs de performance cherche les fonctions de processeur de Windows 10 à l’exécution.
- Correction de `CMakePresets.json`, que CMake refusait parce que le preset de compilation `release-mam` renvoyait à un preset de configuration inexistant.
- Correction du générateur de changelog, qui corrompait son invite quand les données insérées contenaient l’un de ses jetons.

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