# État des tests FMT sans `T:\`

> Fichier de suivi du chantier « plus de tests exécutables sans les données privées ». Il est
> mis à jour **à la fin de chaque lot**. Une session qui démarre à froid doit pouvoir savoir
> d'ici quoi faire, sans relire l'historique.
>
> Le dépôt est public : ce fichier ne cite ni chemin `T:\` ni modèle privé. Les tests BFEC
> y sont désignés par le nom de leur exécutable.

## 1. Objectif, règles et patron

### Objectif

Protéger d'abord **la chaîne que tout modèle traverse** (lecture, objets du cœur, graphe,
optimisation, résultats, écriture) par des tests inscrits dans `basetests.csv`, qui tournent
sur le modèle public `Examples/Models/TWD_land`, sans `T:\`, en une minute environ.

### Décisions de Gabriel (2026-09-16)

- **Filets retenus** : alerte quand une ligne CSV ne nomme aucune cible ; isolation des
  exécutions ; code de sortie 77 = « Skipped ».
- **Écartés pour l'instant** : labels `base`/`bfec`, `TIMEOUT`, mesure de couverture, hook
  anti-U+FFFD.
- **Règles retenues** : tout test a été vu rouge ; un bug corrigé arrive avec un test public.
- **Solveur (2026-09-17)** :
  - les nouveaux tests privilégient MOSEK, le solveur de production, et retombent sur CLP
    quand FMT est compilé sans MOSEK ;
  - les tests existants gardent leur solveur (règle 6).
- **Priorité par criticité, pas par couverture** : une couverture de lignes ne dit pas ce qui
  est critique. On protège d'abord les fonctions qui construisent les modèles.
- **Harnais** : un en-tête commun écrit maison, sans dépendance (lot 1).

### Rôles

- **Gabriel** : compile, reconfigure CMake, lance la suite BFEC, commite.
- **Claude** : écrit les tests et ce fichier, fait les contrôles statiques, puis **lance la
  suite base après le build de Gabriel** :

```
& "C:/Program Files/Microsoft Visual Studio/2022/Professional/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/ctest.exe" --test-dir build/release -C Release -E "T:/" -j 8 --output-on-failure
```

`ctest.exe` n'est pas dans le `PATH`. L'exclusion `-E "T:/"` écarte les tests BFEC, dont le
nom contient leur chemin. Chaque lot s'arrête sur une livraison ; le suivant attend le feu
vert de Gabriel.

### Règles

1. **Un test doit pouvoir échouer, et on l'a vu échouer.** Vu rouge *sans recompiler* : on
   relance l'exécutable avec une valeur attendue ou un scénario faussé en argument. C'est
   pourquoi les valeurs attendues vivent dans le CSV. Le vu rouge est noté au journal.
2. **Un bug corrigé arrive avec un test public** : un scénario minimal dans TWD_land et sa
   ligne `basetests.csv`, qui échoue avant la correction. La ligne BFEC, s'il y en a une,
   reste comme contrôle réaliste.
3. **Un bug révélé par un nouveau test ne se corrige pas dans le même lot** : le test est
   inscrit désactivé dans `knownbugs.csv` (lot 1), avec une issue.
4. **Entrées explicites** : modèle, scénario et valeurs attendues arrivent par les arguments
   du CSV. Les valeurs par défaut d'un source ne pointent que vers TWD_land.
5. **Aucune écriture sous `Examples/Models`** : le test copie le projet dans
   `build/release/tests/<test>/` (patron de `testWrapperCorePlanning`).
6. **Un nouveau test privilégie MOSEK.**
   - Il prend `Models::FMTSolverInterface::MOSEK` quand FMT est compilé avec MOSEK
     (`FMTWITHMOSEK`), `CLP` sinon : patron `#ifdef FMTWITHMOSEK` de `doplanning.cpp`,
     paramètre `solver` des DTO du Core compris.
   - Les tests existants gardent leur solveur, CLP ou MOSEK : on ne les convertit pas.
   - La valeur attendue doit tenir avec les deux solveurs, puisqu'un build sans MOSEK
     retombe sur CLP : d'où la règle 7.
7. **Déterminisme** : un seul thread, graine fixe. On vérifie des quantités uniques, comme
   l'objectif, plutôt qu'une solution parmi plusieurs optimales.
8. **Fonctionnalité absente** (OSI, GDAL, ONNX) : le test rend 77 (« Skipped »), pas 0. Les
   anciens tests passent à 77 quand on les touche (lot 1 et suivants). L'absence de MOSEK n'en
   fait pas partie : le test retombe sur CLP (règle 6).
9. **Toute valeur de référence modifiée, ligne retirée ou test supprimé se justifie** dans le
   commit et au journal. `UnitTestFMTAreaParser` a disparu sans trace dans un correctif (§ 2.5).
10. **Criticité d'abord** : l'ordre des lots suit la section 3.

### Patron d'un test

Depuis le lot 1, `Examples/C++/tests/TestTools.h` (namespace `Testing`, inclus par
`#include "TestTools.h"` dans tout exécutable créé par `createexecutable`) fournit :

- `Checker` : `check(condition, description)` et `checkNear(valeur, attendue, tolérance,
  description)` affichent une ligne `ok` ou `FAILED` ; `exitCode()` rend 0 ou 1 ;
- `SKIP_RETURN_CODE` (77) et `skip(raison)` ;
- `copyProject(source, destination)` : remplace la destination par une copie du projet ;
- `runTest(corps)` : rend le code du corps, ou 1 si une exception s'échappe, après l'avoir
  affichée avec ses exceptions imbriquées.

L'en-tête compile sans avertissement avec MSVC (`/W4 /permissive-`) après `windows.h`, dont les
macros (`min`, `max`, `NEAR`, `ERROR`...) arrivent par GDAL : ne pas y introduire ces noms.

- `main(argc, argv)` lit ses entrées ; sans argument, il retombe sur TWD_land.
- Code de retour : 0 réussite, 1 échec, 77 fonctionnalité absente.
- Commentaires et sorties console en anglais ; ce fichier en français.
- Un nouvel exécutable va dans `Examples/C++/` (ou `FMTWrapperCore/tests/` pour le Core) ;
  sa ligne dans `basetests.csv` l'inscrit dans ctest.

### Techniques privilégiées

- **Tests métamorphiques** : deux calculs qui doivent concorder, sans valeur de référence à
  maintenir.
  - Présolve activé ou non : même objectif (existe déjà : `presolvetest`, un seul scénario).
  - Rejeu de la cédule optimale : même objectif.
  - Scénarios lus d'un coup ou un par un : mêmes modèles, comparés par leur écriture texte.
  - Écrire, relire, réécrire : fichiers identiques.
- **Valeurs calculables à la main** sur la racine de TWD_land :
  - superficie totale 1814,76 ha (403,28 + 7 × 201,64) ;
  - les yields d'âge sont interpolés linéairement (`FMTAgeYieldHandler::get`, par
    `getLinearValue`) : `volumetotal` de `peuplement1` vaut 120 à 7 ans.
- **Tests négatifs** : un modèle invalide doit lever le `FMTexc` attendu.

## 2. Inventaire

### 2.1 Des CSV à ctest

- `Examples/C++/CMakeLists.txt` lit `Examples/C++/tests/*.csv`. Colonnes : cible, puis trois
  arguments ; le `|` sépare des valeurs dans une colonne. Le nom ctest est la concaténation
  cible + arguments 1 et 2 (espaces remplacés par `_`).
- `basetests.csv` est suivi par git ; `BFECtests.csv` est local et ignoré par git (invisible
  pour ripgrep : le lire directement).
- Une ligne dont la cible n'existe pas n'est pas inscrite. Depuis le lot 1, la configuration
  l'annonce : `CMake Warning ... <fichier>.csv: no target named "<cible>"`. Avant, elle était
  ignorée sans rien dire.
- Chaque test inscrit par un CSV reçoit (lot 1) :
  - `SKIP_RETURN_CODE 77` ;
  - `FIXTURES_REQUIRED ExamplesModels` : ctest lance `ExamplesModelsSnapshot` avant les tests
    et `ExamplesModelsUnchanged` après. Ces deux tests exécutent
    `cmake/TestsDataSnapshot.cmake`, qui hache les 273 fichiers de `Examples/Models` et
    échoue en listant tout fichier ajouté, retiré ou modifié. L'empreinte est écrite dans
    `build/release/tests/ExamplesModels.snapshot` ;
  - `RESOURCE_LOCK <cible>` si la cible figure dans `FMTTESTSsharedoutputs`
    (`planningtest`, `testActionsAggregations`, `testOAschedulerBFEC`,
    `testreadwriteproject`) : ses lignes écrivent dans les mêmes fichiers et ne tournent
    jamais en même temps ;
  - `DISABLED TRUE` si la ligne vient de `knownbugs.csv` : ctest l'affiche « Disabled » sans
    l'exécuter.
- Répertoire de travail : `build/release/bin/Release`. Entrées :
  `../../../../Examples/Models/...` ; sorties : `../../tests/<test>/`, soit
  `build/release/tests/<test>/`.
- Les arguments sont figés dans `build/release/CTestTestfile.cmake` : **modifier un CSV exige
  de reconfigurer CMake**.
- `UnitTestFMTexcelcache` est inscrit par `Excel/CMakeLists.txt`, hors CSV.

### 2.2 Ce que vérifient les tests base, par étape de la chaîne

« Vérifie » = compare une valeur ; « exécute » = n'échoue que sur une exception.

| Étape | Vérifie une valeur | Exécute sans vérifier |
|---|---|---|
| Lecture d'un scénario | indirect : objectifs de `doplanning` (16 scénarios), `landaggregatestest`, `testlevelopt` ; direct : constantes (`UnitTestFMTParser`, 3 cas), un yield complexe (`testWrapperCoreGetYield`), nombre de masques (`testWrapperCoreGetAllMasks`), âge maximal (`testWrapperCoreGetMaxAge`, borne seulement), gabarits (`templatestest`, au moins un modèle) | tous les autres tests |
| Lecture de plusieurs scénarios d'un coup | indirect : `testlevelopt` (9 scénarios, objectifs) | `testreadwriteproject`, `planningtest`, `replanningtest`, `replanningmodeladaption`, `UnitTestCallbackLogger` |
| Objets du cœur (masques, yields, opérabilité, transitions) | `nochoice` (développements sans choix d'un masque), `testyieldsfromfactor` (monotonie), `testtimeyieldoutputs` (2 outputs de yields temporels) ; le reste indirectement par les objectifs | `testaddtomodel` (vérification de valeur morte) |
| Présolve et postsolve | `presolvetest` (objectif avec ou sans présolve, NOT_MASK seulement) ; `doplanning` (présolve de 10 itérations) | -- |
| Graphe et matrice | indirect (objectifs) | `testoutputshadowcost` (vérifie seulement la résolution après retrait et remise d'une contrainte) |
| Optimisation | `doplanning` (16), `testlevelopt` (9), `landaggregatestest` ; `testSolvers` (chaque solveur résout, sans valeur) | `Simpleplanning`, `createmodel` |
| Résultats et rejeu | `testlevel` (26 outputs), `testoutput` (3 outputs après rejeu d'une cédule), `forcesolution` (6 valeurs et conservation de la superficie), `sumandavgtest` (somme et moyenne), `FMTNsstest` (5 lignes sur 7) | `Simpleplanning` (écrit la cédule), `testareavariabilities`, `testmultisolutions` |
| Écriture et relecture | `testmodelwriter` (5 scénarios, objectif après relecture), `testOutputType` (nombre d'outputs), `testWrapperCoreWriteToProject` (fichiers présents) | `testreadwriteproject`, `nonspatialupdate` |
| Tâches (planification, replanification) | `sumandavgtest`, `testWrapperCorePlanning` | `planningtest`, `replanningtest`, `replanningmodeladaption`, `UnitTestCallbackLogger` |
| Spatial (SES, cartes, rasters) | `Spatialyexplicitsimulation_doplanning` (une vérification valide), `testWrapperCoreRasterization` | `Spatialyexplicitsimulation`, `maptoFMTforest`, `vectorfieldtoraster`, `GetCarbonpredictors` |
| Divers | `testGCBMtransitionparser` (aller-retour), `testgdaldrivers`, `testOPAreaclustering` (une solution existe), `testWrapperCoreAreaVariability`, `testWrapperCoreEnvironment`, `UnitTestFMTexcelcache` | `testyieldmodel`, `testlpyieldmodel`, `testlpyieldmodelopt` |

### 2.3 Exécutables base sans vérification (17)

`Simpleplanning`, `Spatialyexplicitsimulation`, `GetCarbonpredictors`, `maptoFMTforest`,
`testyieldmodel`, `testareavariabilities`, `replanningtest`, `testmultisolutions`,
`testlpyieldmodel`, `testlpyieldmodelopt`, `planningtest`, `createmodel`,
`vectorfieldtoraster`, `nonspatialupdate`, `testreadwriteproject`,
`replanningmodeladaption`, `UnitTestCallbackLogger` (construit sa tâche sans la lancer).

### 2.4 Vérifications mortes

- `Spatialyexplicitsimulation_doplanning.cpp:87-88` : le contrôle des patchs découpe `stats`
  au lieu de `result`, et `spresults.at(1) != "2"` est toujours vrai quand la première
  condition l'est.
- `testaddtomodel.cpp` : le vecteur `totest` n'est jamais rempli ; la boucle qui compare les
  valeurs ne s'exécute pas. Seul `isYld` est vérifié.
- `FMTWrapperCore/tests/testWrapperCoreSA.cpp:150` : l'écart de plus de 5 % ne fait
  qu'afficher un avertissement (hors ctest de toute façon).

### 2.5 Hors ctest, désactivés ou jamais exécutés

- `teststdconstraints` : **désactivé dans `knownbugs.csv`** (lot 1).
  - Sa ligne de `basetests.csv` s'appelait `stdconstraints` depuis sa création (`e97d30d4`,
    2022-05-13) : il n'a jamais tourné.
  - Il ne peut pas passer : le scénario `stdconstraints` optimise un output `TEST2` qui n'a
    jamais été défini, et la lecture lève `FMTexc(34)` (output non défini). Ce sont les
    données du test qui sont fausses, pas FMT.
  - Ce test visait l'expression régulière des contraintes (`rxequations` de
    `FMTOptimizationParser`, passée de `boost::regex` à `std::regex`) sur des lignes comme
    `test2 <= 567 + 1000,45345.322 1..3,6,8`. À réécrire au lot 2 : définir l'output dans le
    scénario, puis vérifier les contraintes lues (bornes par période) plutôt qu'un objectif.
- Ligne `coordinatetest` : retirée au lot 1, sa source avait été supprimée.
- `testWrapperCoreSA` : pas de jeu de données (repli `TODO`).
- `UnitTestFMTmodelcache` (Excel) : compilé, jamais inscrit.
- `UnitTestFMTAreaParser` : **supprimé avec sa ligne de `basetests.csv` par `f06f55fd`**
  (correctif d'adjacence, 2026-07-20). C'était un test unitaire de géométries synthétiques,
  sans données externes : `git show f06f55fd^:Examples/C++/UnitTestFMTAreaParser.cpp`.

### 2.6 Exécutables qui n'ont que des tests BFEC (25)

`FMTsetsolution`, `FMTsetsolutionbylp`, `ParsersTests`, `PointToRasters`,
`Spatial_doplanning`, `TestSimpleplanning20p`, `UnitTestFMToutputCache`, `replanner`,
`sasolve`, `testActionsAggregations`, `testActionsBuilder`, `testActionsBuilderMap`,
`testActionsSplit`, `testOAAdjacency`, `testOAschedulerBFEC`, `testOAschedulertask`,
`testWrapperCoreAggregateAllActions`, `testWrapperCoreBuildAction`,
`testWrapperCoreOperatingArea`, `testWrapperCoreSES`, `testWrapperCoreSplitActions`,
`test_rotations`, `testgetYields`, `testreadOAschedulerparameters`, `testsesevaluation`.

## 3. Lacunes classées par criticité

Critère : une étape que tout modèle traverse passe avant une fonction de niche ; une erreur
**silencieuse** (résultat faux sans exception) passe avant une erreur qui lève.

### Niveau 1 : la chaîne que tout modèle traverse

| # | Étape et fonctions | Lacune |
|---|---|---|
| 1 | **Lecture d'un scénario** : `FMTModelParser::readproject` (`Include/FMTModelParser.h:109`), parseurs de sections, `FMTParser` (constantes, boucles FOR) | Rien ne vérifie la structure lue : thèmes, agrégats, superficie, opérabilités, transitions, yields, lifespan, outputs, contraintes. Une erreur de lecture qui ne change pas l'objectif des scénarios testés passe. |
| 2 | **Lecture de plusieurs scénarios d'un coup** : `referenceRead` (`Source/FMTModelParser.cpp:560-830`) réutilise les sections déjà lues d'un autre scénario ; la réutilisation des outputs ne dépend pas des actions ni des yields (commentaire « should be with action?? », l. 640) | Aucune comparaison avec des lectures séparées. L'interface lit plusieurs scénarios d'un coup (`Controller::addScenarios`), comme les tâches et plusieurs tests. |
| 3 | **Erreurs de lecture** : levée des `FMTexc`, `setErrorsToWarnings` | Aucun test négatif : une erreur de modèle qui n'est plus signalée, ou signalée par le mauvais code, passe. |
| 4 | **Objets du cœur** : `FMTDevelopment::grow`, `operable`, `operate` (`Include/FMTDevelopment.h:209-274`), `FMTMask::isSubsetOf`, `FMTTheme::validate`, `FMTYields::get` (`Include/FMTYields.h:230`), lifespan et `_DEATH` par défaut | Aucune vérification unitaire de l'opérabilité (bornes d'âge, de yield, de période, `_LOCK`), des transitions (proportions, cibles) ni du vieillissement ; seulement des objectifs. |
| 5 | **Présolve et postsolve** : `FMTModel::preSolve` / `postSolve` (`Include/FMTModel.h:279-285`) | Le test métamorphique ne couvre qu'un scénario (NOT_MASK) ; les outputs après postsolve ne sont pas comparés. |
| 6 | **Graphe et matrice** : `FMTSrModel::buildPeriod` (`Include/FMTSrModel.h:261`), `FMTGraph::build` (`Include/FMTGraph.hpp:462`), `FMTLpModel::setObjective` / `setConstraint` | Les statistiques du graphe (`FMTGraphStats` : sommets, arcs, lignes, colonnes) ne sont jamais vérifiées. |
| 7 | **Résultats et rejeu** : `getSolution`, `setSolution` / `boundSolution` (`Include/FMTSrModel.h:173,329`), `doPlanning(false, cédules)` | Le rejeu d'une cédule optimale n'est pas comparé à l'optimisation ; `FMTsetsolution` n'a que des tests BFEC. |
| 8 | **Écriture et relecture** : `FMTModelParser::write` (`Source/FMTModelParser.cpp:467`), `writeToProject` (`Include/FMTModelParser.h:118`), `FMTScheduleParser` | Pas d'idempotence (écrire, relire, réécrire) ni d'aller-retour de cédule ; `testreadwriteproject` n'a aucune vérification. |

L'optimisation elle-même (objectifs) est l'étape la mieux protégée : 26 objectifs vérifiés.

### Gains rapides : des lignes CSV sans code

Chacune demande une reconfiguration de CMake et, sauf pour les tests métamorphiques, une
valeur mesurée une fois puis vue rouge.

- `presolvetest` sur d'autres scénarios (métamorphique, aucune valeur à fixer ; il reste en
  CLP) : par exemple `LP`, `COS`, `Shift`, `TSLA`, `fullcarbon`, `equation`.
- `testmodelwriter` sur d'autres scénarios (métamorphique, en MOSEK).
- `doplanning` sur des scénarios inutilisés (§ 4.3), après avoir vérifié qu'ils sont faits
  pour être optimisés.
- `FMTNsstest` : les lignes `randomYield` et `randomYieldUnit` n'ont pas de valeur attendue ; à
  fixer si le résultat est déterministe (graine 0).

### Niveau 2 : flux courants

- **Tâches** : `planningtest`, `replanningtest`, `replanningmodeladaption`,
  `UnitTestCallbackLogger` ne vérifient rien ; `replanner` n'a que des tests BFEC.
- **SES** : `Spatialyexplicitsimulation` ne vérifie rien ; le contrôle des patchs de
  `Spatialyexplicitsimulation_doplanning` est mort ; `testsesevaluation`, `Spatial_doplanning`
  et `testWrapperCoreSES` n'ont que des tests BFEC.
- **Cartes et rasters** : `maptoFMTforest` et `vectorfieldtoraster` ne vérifient rien ;
  `PointToRasters` n'a que des tests BFEC.
- **NSS** : `FMTNsstest` vérifie 5 lignes sur 7.
- **Core du wrapper** : couvert en base, sauf `AggregateAllActions`, `SplitActions`,
  `BuildAction`, `OperatingArea` et `SES` (BFEC) et `SA` (hors ctest).

### Niveau 3 : fonctions spécialisées

- Recuit simulé : `sasolve` (BFEC), `testWrapperCoreSA` (hors ctest).
- Ordonnanceur d'aires d'opération et adjacence : `testOAschedulertask`,
  `testOAschedulerBFEC`, `testOAAdjacency`, `testreadOAschedulerparameters` (BFEC).
- Transformations d'actions (TBE) : `testActions*` et leurs équivalents du Core (BFEC).
- Modèles de rendement ONNX : `testyieldmodel`, `testlpyieldmodel(opt)` ne vérifient rien ;
  les scénarios `DECISION`, `randomYield`, `TSLA`, `UNIT_COVERAGE` et `COMPLEX_NSS` sont
  vérifiés par `FMTNsstest` ou `doplanning`.
- Regroupement d'UA : `testOPAreaclustering` (une solution existe) ; prédicteurs :
  `GetCarbonpredictors` (rien) ; `test_rotations`, `testgetYields`, `ParsersTests`,
  `TestSimpleplanning20p`, `FMTsetsolutionbylp` (BFEC).
- Géométries : `UnitTestFMTAreaParser` à restaurer si son API existe encore.

## 4. Scénarios bidons (TWD_land)

### 4.1 Le modèle

- 3 thèmes :
  - THEME1 : `unite1-3`, agrégat `UC` ;
  - THEME2 : `peuplement1-3` et `feu`, agrégats `prod` et `pourpl` ;
  - THEME3 : `UTR1-3`, agrégats `UTA1` et `UTA2`.
- 8 strates, 1814,76 ha.
- 5 actions (`aFire`, `arecup`, `COUPEtotale`, `aCaribou`, `plantation`) ; `_DEATH` a une
  transition mais pas d'action (créée par défaut à la lecture).
- Yields d'âge de 0 à 30 ans, 3 yields complexes (`*YC`), 16 outputs, lifespan 80 (0 pour
  `feu`).
- 74 scénarios dans `Scenarios/`.
- `Carte/TWD_land.shp` : 8 polygones, champs `AGE`, `THEME1`-`THEME3`, `SUPERFICIE`, sans
  `STANLOCK`. `Carte/TWD_LAND_forcesolution_modif.*` sert à `forcesolution`.
- `rasters/` : `AGE.tif`, `THEME1-3.tif`, sans `STANLOCK.tif`.
- `TWD_land.db` (SQLite) sert aux scénarios `DB_test*` ; `Templates/` à `templatestest`.

### 4.2 Règles

- **Ne jamais modifier les sections racine** (`TWD_land.*`) : tous les scénarios en héritent,
  donc toutes les valeurs attendues changeraient.
- **Un dossier `Scenarios/<nom>` par besoin**, nommé d'après la fonctionnalité ou l'issue.
- **Des scénarios petits** : la suite base doit rester autour d'une minute.
- Un scénario invalide (test négatif) ne sert qu'au test qui attend son exception.

### 4.3 Catalogue

| Besoin | Faisabilité | Données à créer |
|---|---|---|
| Structure lue (lacune 1) | texte seul | aucune : racine et scénarios existants |
| Lecture groupée (lacune 2) | texte seul | aucune : `levelopt1-9`, `LP`/`LP2`/`LP4`/`LP5`, trio de replanification |
| Tests négatifs (lacune 3) | texte seul | un `Scenarios/INVALID_<erreur>` par erreur visée |
| Opérabilité, transitions, vieillissement (lacune 4) | texte seul | aucune : développements construits dans le test sur la racine |
| Fonctions de yield qu'aucun scénario n'utilise : `_SUBTRACT`, `_YTP`, `_ENDPOINT`, `_DELTA`, `_DISCOUNTFACTOR` | texte seul | un scénario de yields, valeurs calculées à la main |
| `*PARTIAL`, `*STRATA`, `_SEQ`, `_INVLOCK` | texte seul, à confirmer | liste heuristique : vérifier d'abord dans les parseurs que ces mots-clés existent |
| Rejeu `FMTsetsolution` (lacune 7) | texte seul | `LP` a une cédule, mais le test exige un output `OVOLTOTREC` codé en dur : scénario bidon qui définit `OVOLTOTREC`, ou nom d'output en argument (il optimise déjà en MOSEK) |
| SES, rastérisation | carte existante | aucune |
| Recuit simulé | à évaluer | scénario `Spatial` et rasters existants ; `sasolve` lit aussi `STANLOCK.tif`, absent |
| Ordonnanceur d'UA | carte synthétique probable | yield `YOUVERT` utilisé par une action, CSV de paramètres, thème d'UA en argument (codé 14 dans `testOAschedulertask`), plus de polygones |
| Adjacence, patchs | carte synthétique | `FMTLayer` construit en mémoire (constructeur depuis `std::map<FMTCoordinate,T>`), ou écrit par `FMTAreaParser::writeForest` / `writeLayer` : ni Python ni GDAL Python requis |
| Transformations d'actions | à évaluer | structure d'actions agrégées à reproduire |

**Scénarios existants qu'aucun test C++ n'utilise** : `actionrecurse`, `AVERAGEP`, `peak`,
`Predeclared_Primary`, `Predeclared_Primary_ref`, `Spatial_8`, `LPwfire`, `Globalex4`,
`Globalex5`. Les exemples Python et R lisent `Globalex1/2/7`, `Globalnofire` et `Localex1/3-6`.

## 5. Journal des lots

### Lot 0 : analyse et ce fichier (2026-09-16)

**Statut** : livré le 2026-09-16. Documentation seulement, aucun code modifié.

- Exploration de l'infrastructure (CSV, `CMakeLists.txt`, dernier `LastTest.log`), des sources
  des tests base et de TWD_land ; décisions de Gabriel en section 1.
- Constats :
  - deux lignes de `basetests.csv` ignorées (§ 2.5) ;
  - 17 exécutables base sans vérification (§ 2.3), trois vérifications mortes (§ 2.4) ;
  - `sumandavgtest` réécrit `Examples/Models/TWD_land/Scenarios/sumavg/TWD_land._seq` à chaque
    ctest (`FMTPlanningTask` reçoit le fichier primaire) ; contenu identique pour l'instant ;
  - ctest tourne avec `-j 8` et les 3 lignes de `testreadwriteproject` écrivent dans le même
    projet `build/release/tests/testreadwriteproject/test.pri` ;
  - `UnitTestFMTAreaParser` supprimé par `f06f55fd` ;
  - `cmake/BaseInstall.cmake` lance ctest sans vérifier son résultat (consigné, pas proposé).
- Tests BFEC désignés par exécutable seulement ; pas de chemin privé dans ce fichier.

### Lot 1 : filets et en-tête commun (2026-09-16)

**Statut** : livré le 2026-09-16, compilé par Gabriel le 2026-09-17, **validé** le 2026-09-17 :
suite base de 83 tests, 82 verts et `teststdconstraints` désactivé, en 9,3 s avec `-j 8` (voir
« Validation » ci-dessous).

- **`Examples/C++/CMakeLists.txt`** :
  - alerte de configuration pour une ligne sans cible ;
  - `SKIP_RETURN_CODE 77` et `FIXTURES_REQUIRED ExamplesModels` sur chaque test des CSV ;
  - `RESOURCE_LOCK` pour 4 exécutables ;
  - lignes de `knownbugs.csv` inscrites `DISABLED` ;
  - tests `ExamplesModelsSnapshot` et `ExamplesModelsUnchanged` ;
  - `enable_testing()` sort de la boucle des CSV (§ 2.1).
- **Verrous confirmés à la lecture des sources** :
  - `testreadwriteproject` : ses 3 lignes écrivent `tests/testreadwriteproject/test.pri` ;
  - `testOAschedulerBFEC` : ses 2 lignes écrivent `tests/testOAschedulerBFEC/bfecoptsol.yld` ;
  - `testActionsAggregations` : `writeDisturbances` nomme ses rasters par période seulement,
    dans le dossier commun ;
  - `planningtest` : ses 3 lignes (base et BFEC) écrivent dans `tests/planning`, et
    `FMTParser::createOGRDataset` supprime un jeu de données de sortie qui existe déjà.
  - Écartés, sorties nommées par scénario ou dossier en argument : `testmodelwriter`,
    `replanner`, `testWrapperCoreGetAllMasks`, `testOAschedulertask`.
- **`cmake/TestsDataSnapshot.cmake`** : empreinte SHA256 de `Examples/Models`, sans git.
- **`cmake/ConfigFunctions.cmake`** : `Examples/C++/tests` dans les inclusions de
  `createexecutable`.
- **`Examples/C++/tests/TestTools.h`** : § 1, « Patron d'un test ».
- **`sumandavgtest.cpp`** :
  - travaille sur `tests/sumandavgtest/TWD_land`, une copie ;
  - écrit ses sorties dans `tests/sumandavgtest/outputs`, hors du dossier de la copie ;
  - rend 77 sans OSI ;
  - édité octet par octet, cp1252 conservé.
- **CSV** : `coordinatetest` retiré de `basetests.csv` ; `stdconstraints` devient
  `teststdconstraints` dans `knownbugs.csv` (§ 2.5).
- **Écarts au plan** :
  - `teststdconstraints` devait recevoir un objectif attendu, mais son scénario ne se lit pas :
    il est désactivé, et sa réécriture passe au lot 2 ;
  - la reconfiguration préalable par Gabriel (vu rouge de l'alerte) est remplacée par un
    projet CMake jetable, hors dépôt, qui reprend la boucle des CSV avec des cibles factices
    du même nom : une seule reconfiguration suffit.

Vérifications (hors build, 2026-09-16) :

- **Projet CMake jetable** :
  - avec le `basetests.csv` d'avant : exactement deux alertes (`coordinatetest`,
    `stdconstraints`), aucune pour l'en-tête ni pour les lignes BFEC (vu rouge) ;
  - avec les nouveaux CSV : aucune alerte ; 82 tests hors `T:/` (83 attendus dans le vrai
    build, avec `UnitTestFMTexcelcache`) ; propriétés présentes sur tous les tests des CSV ;
    4 groupes de verrous (3, 2, 2 et 3 tests) ; `teststdconstraints` seul désactivé.
- **Fixture** :
  - passage normal vert (273 fichiers, 0,15 s) ;
  - un test factice qui crée un fichier sous `Examples/Models` fait échouer
    `ExamplesModelsUnchanged`, qui nomme le fichier (vu rouge) ;
  - un fichier modifié et un fichier supprimé sont listés, code de sortie 1.
- **`TestTools.h`** : compilé avec `cl` (VS 2022, `/W4 /permissive-`, après `windows.h`) sans
  avertissement. Un auto-test vérifie `Checker`, `skip`, `runTest` (exception imbriquée,
  exception non standard) et `copyProject` (copie récursive, remplacement d'une copie).
- **`teststdconstraints.exe`**, lancé à la main : lève `FMTexc(34)` (output `TEST2` non défini).
- **Encodages** : aucun U+FFFD introduit, octets cp1252 de `sumandavgtest.cpp` conservés, CRLF
  partout.

Validation (après le build de Gabriel, 2026-09-17) :

- **`CTestTestfile.cmake` régénéré** :
  - les deux tests de fixture sont inscrits ;
  - `teststdconstraints` est inscrit `DISABLED` ;
  - plus aucune trace de `coordinatetest` ni de `stdconstraints` ;
  - `SKIP_RETURN_CODE 77` sur les 173 tests des CSV (79 base, 93 BFEC, 1 désactivé) ;
  - verrous 3, 2, 2 et 3.
- **Suite base** (`-E "T:/" -j 8`) : 83 tests ; 82 verts ; `teststdconstraints` « Not Run
  (Disabled) » ; 9,3 s réelles.
- **Fixture** : `ExamplesModelsSnapshot` se termine 3e, `ExamplesModelsUnchanged` en dernier
  (83/83), les deux verts.
- **`sumandavgtest`** vert sur sa copie :
  - la cédule suivie par git, `Examples/Models/TWD_land/Scenarios/sumavg/TWD_land._seq`,
    garde sa date du 2026-09-16 12:10:40 ;
  - celle de la copie est réécrite ;
  - `git status` ne montre rien sous `Examples/Models`.
- **Nettoyage** : `build/release/tests/sumandavgtest/` garde `TWD_land.csv` et `sumavg.csv`, écrits
  par l'ancien emplacement des sorties. Ce sont des restes sans effet.

## 6. Prochain lot

### Lot 2 : lecture

- Structure de la racine et de quelques scénarios (valeurs calculables à la main).
- Scénarios lus d'un coup comparés aux lectures une par une (`referenceRead`).
- Premiers tests négatifs du parseur (`Scenarios/INVALID_<erreur>`).
- `teststdconstraints` réécrit :
  - l'output `TEST2` est défini dans son scénario ;
  - le test vérifie les contraintes lues (bornes par période) ;
  - il retourne dans `basetests.csv`.

### Ensuite (par criticité)
- **Lot 3 : objets du cœur.** Opérabilité, transitions, vieillissement, lifespan sur des
  développements construits à la main.
- **Lot 4 : construction et optimisation.** `presolvetest` étendu (lignes CSV) ; statistiques
  du graphe par période ; rejeu de la cédule optimale.
- **Lot 5 : écriture et relecture.** Idempotence, aller-retour de cédule ; vérifications
  ajoutées à `testreadwriteproject`, `Simpleplanning`, `createmodel`, `nonspatialupdate` ;
  vérification morte de `testaddtomodel`.
- **Lots suivants** : niveau 2, puis niveau 3 avec leurs scénarios bidons.

## 7. Pièges connus

- **Encodage** : `planningtest.cpp`, `testmodelwriter.cpp`, `testreadwriteproject.cpp` et
  `sumandavgtest.cpp` sont en cp1252. Les éditer en Python latin-1 (`newline=''`), jamais avec
  un outil qui suppose l'UTF-8. Les fichiers CMake et `basetests.csv` sont en UTF-8 ; le
  dépôt est en `core.autocrlf=true` (CRLF dans l'arbre de travail).
- **`TWD_land.out`** contient déjà des U+FFFD dans les descriptions d'outputs (accents perdus
  autrefois) : ne pas le « corriger » en passant.
- **Format CSV** : séparateur `;`, pas de commentaire possible, première ligne = en-tête. Une
  colonne vide disparaît de la commande : `argc` change (les lignes `FMTNsstest` sans valeur
  ne vérifient rien).
- **Nom de cible exact** : casse comprise. Une ligne mal nommée n'est pas inscrite ; depuis le
  lot 1, la configuration l'annonce par `no target named` : lire les alertes après chaque
  reconfiguration. Avant, `testWrapperCoreGetYield` n'a pas tourné de 2024 à 2026, et
  `teststdconstraints` jamais.
- **Modifier un CSV exige de reconfigurer CMake.**
- **Un jeu de données de sortie qui existe déjà est supprimé** (`FMTParser::createOGRDataset`,
  CSV, GeoTIFF...) : ne jamais placer la copie d'un projet dans le dossier de sortie d'une
  tâche (voir `sumandavgtest`).
- **Nouvel exécutable dont plusieurs lignes écrivent dans les mêmes fichiers** : l'ajouter à
  `FMTTESTSsharedoutputs` dans `Examples/C++/CMakeLists.txt`.
- **L'empreinte de `Examples/Models` compare le contenu** : un test qui réécrit un fichier à
  l'identique passe inaperçu. La règle 5 reste la vraie protection.
- **`Examples/C++/CMakeLists.txt`** contient déjà un U+FFFD dans son en-tête (« Québec ») :
  préexistant, à ne pas confondre avec une corruption introduite.
- **Dans l'environnement de Claude**, `cmd` ne cherche pas les exécutables dans le dossier
  courant : lancer un test par son chemin (`./test.exe`).
- **Valeurs par défaut des sources** : plusieurs pointent vers des chemins privés ou locaux
  (restes de débogage, par ex. `testoutput`, `testmodelwriter`, `presolvetest`,
  `templatestest`). Une ligne base sans argument dépend de ses défauts.
- **Solveur des tests existants (2026-09-17), laissé tel quel** :
  - 25 exécutables base optimisent en CLP ;
  - en MOSEK : `testmodelwriter`, `planningtest`, `nonspatialupdate`, `createmodel`,
    `testOutputType` ;
  - `doplanning` prend MOSEK quand `FMTWITHMOSEK` est défini, CLP sinon : c'est le patron des
    nouveaux tests (règle 6) ;
  - `templatestest` prend CLP dès qu'il reçoit des arguments, donc toujours dans ctest ;
  - `testSolvers` essaie tous les solveurs disponibles ;
  - `UnitTestCallbackLogger` et les tests Excel sont entièrement sous `FMTWITHMOSEK`.

  Une valeur attendue prise sur un optimum non unique peut changer d'un solveur à l'autre.
- **Course connue** sur certains modèles privés (caches de yields partagés entre threads) :
  les nouveaux tests tournent sur un seul thread. Pour distinguer une intermittence d'une
  régression, relancer l'exécutable sans recompiler.
- **`FMTPlanningTask` qui reçoit le fichier primaire** réécrit
  `Scenarios/<scénario>/<projet>._seq` des scénarios optimisés : travailler sur une copie.
- **`.gitignore`** exclut `*.lp`, `*.mps` et `*html` : pas de fichier de référence sous ces
  extensions.
- **Mon passage de ctest écrase** `build/release/Testing/Temporary/LastTest.log`.
- Plusieurs tests lèvent avec un nom de méthode copié d'un autre test (`"presolvetest"` dans
  `doplanning`) : lire le numéro de ligne plutôt que le nom.

## 8. Mesures

À refaire à la fin de chaque lot.

| Date | Tests base inscrits | Durée cumulée | Exécutables base | Sans vérification | Vérifications mortes | Lignes CSV ignorées | Désactivés | Exécutables BFEC seulement |
|---|---|---|---|---|---|---|---|---|
| 2026-09-16 (lot 0) | 80 (79 lignes + `UnitTestFMTexcelcache`) | 51 s cumulées (passage complet avec BFEC) | 48 | 17 | 3 | 2 | 0 | 25 |
| 2026-09-17 (lot 1) | 83 (79 lignes, 2 fixtures, `teststdconstraints` désactivé, `UnitTestFMTexcelcache`) | 9,3 s réelles ; 65,7 s cumulées | 48 | 17 | 3 | 0 | 1 | 25 |

- Tests base inscrits : `ctest ... -N -E "T:/"` (fixtures et tests désactivés compris).
- Durée : la référence est le temps réel de la suite base seule (`Total Test time (real)`).
  La somme des « Test time » de `LastTest.log` varie avec la charge : avec `-j 8`, les tests
  base lancés ensemble se ralentissent entre eux. Les 65,7 s du lot 1 ne se comparent donc pas
  aux 51 s du lot 0, mesurées dans le passage complet.
- Exécutables base : premières colonnes distinctes de `basetests.csv` qui nomment une cible.
- Lignes CSV ignorées : alertes `no target named` à la configuration.
- Désactivés : lignes de `knownbugs.csv`.
