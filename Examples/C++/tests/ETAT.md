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
  affichée avec ses exceptions imbriquées (`printException`) ;
- `visitNested(exception, visiteur)` : parcourt la chaîne d'exceptions imbriquées sans
  `std::terminate` (lot 2, voir § 7).

Depuis le lot 2, trois tests génériques pilotés par le CSV servent de modèles :

- `testScenarioReading` : une ligne = une requête sur un scénario lu (`THEMES`, `ATTRIBUTES`,
  `AGGREGATE`, `AREA`, `ACTIONS`, `OPERABLE`, `YIELD`, `OUTPUTS`, `CONSTRAINT`... ; liste en
  tête du source) et sa valeur attendue ;
- `testJointScenarioReading` : une ligne = une liste de scénarios lus ensemble ;
- `testReadingErrors` : une ligne = un scénario invalide et le `FMTexc` attendu.

Ajouter une ligne à l'un d'eux ne demande pas de recompiler, seulement de reconfigurer.

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
| Lecture d'un scénario | direct : `testScenarioReading` (lot 2 : thèmes, attributs, agrégats, aire par masque, actions et drapeaux, outputs et contraintes de ROOT ; noms de thèmes, aire héritée et contraintes de `LP3` ; contraintes de `stdconstraints`), constantes (`UnitTestFMTParser`, 3 cas), un yield complexe (`testWrapperCoreGetYield`), nombre de masques (`testWrapperCoreGetAllMasks`), âge maximal (`testWrapperCoreGetMaxAge`, borne seulement), gabarits (`templatestest`, au moins un modèle) ; indirect : objectifs de `doplanning` (16 scénarios), `landaggregatestest`, `testlevelopt` | tous les autres tests |
| Lecture de plusieurs scénarios d'un coup | direct : `testJointScenarioReading` (lot 2 : 7 combinaisons, 32 scénarios) ; indirect : `testlevelopt` (9 scénarios, objectifs) | `testreadwriteproject`, `planningtest`, `replanningtest`, `replanningmodeladaption`, `UnitTestCallbackLogger` |
| Erreurs de lecture | `testReadingErrors` (lot 2 : 7 scénarios invalides, 6 codes `FMTexc`) | -- |
| Objets du cœur (masques, yields, opérabilité, transitions) | `testScenarioReading` (lot 2 : opérabilité aux bornes d'âge, de yield et d'agrégat, `_DEATH` tiré du lifespan, interpolation des yields d'âge, opérateurs de yields complexes), `nochoice` (développements sans choix d'un masque), `testyieldsfromfactor` (monotonie), `testtimeyieldoutputs` (2 outputs de yields temporels) ; transitions et vieillissement : indirectement par les objectifs | `testaddtomodel` (vérification de valeur morte) |
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

- `teststdconstraints` : **supprimé au lot 2**, remplacé par 8 lignes de `testScenarioReading`
  sur le scénario `stdconstraints`.
  - Sa ligne de `basetests.csv` s'appelait `stdconstraints` depuis sa création (`e97d30d4`,
    2022-05-13) : il n'a jamais tourné.
  - Il ne pouvait pas passer : le scénario optimisait un output `TEST2` jamais défini, et la
    lecture levait `FMTexc(34)`. Ce sont les données du test qui étaient fausses, pas FMT.
  - Il visait l'expression régulière des contraintes (`m_rxequations` de
    `FMTOptimizationParser`, passée de `boost::regex` à `std::regex`) sur des lignes comme
    `test2 <= 567 + 1000,45345.322 1..3,6,8`.
  - Le lot 2 définit `TEST2` dans `Scenarios/stdconstraints/TWD_land._out` et vérifie les 7
    lignes lues. La virgule est un séparateur de milliers (`FMTParser::_getNum` l'efface) :
    `567 + 1000,45345.322` donne 100045912.322. Une liste de périodes `1..3,6,8` donne une
    contrainte par morceau (1..3, 6, 8).
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

### 2.7 Bogues révélés, inscrits dans `knownbugs.csv`

Deux défauts de FMTlib révélés au lot 2, sans correction dans ce chantier (règle 3). Les lignes
de `knownbugs.csv` échouent aujourd'hui : elles passeront dans `basetests.csv` avec la
correction. Issues GitHub : **#346** (priorité de `^`) et **#347** (bloc `*YC` mélangé, qui
couvre aussi la constante d'équation). Démonstrations refaites le
2026-09-17 avec les exécutables du build de `new_test`, sur des copies de TWD_land hors dépôt.

1. **Priorité de `^` dans `_EQUATION` (issue #346).**
   - `FMTOperator::precedence` (`Source/FMTOperator.cpp:175`) donne à `^` la même priorité
     qu'à `*` et `/`, et `FMTExpression::_getPostFix` dépile les opérateurs de priorité
     égale : l'expression s'évalue de gauche à droite.
   - `2*3^2` vaut 36 au lieu de 18, et `12/2^2` vaut 36 au lieu de 3. `2*(3^2)` et `3^2*2`
     valent 18 : les parenthèses contournent le défaut.
   - Présent depuis l'ajout de `^` (`da6df324`, 2019-08-21).
   - Sur la racine de TWD_land, `YBM_RTM_BOG` = `1.2159259330000001*10^0.87191668899999997`
     vaut 8.82975 (soit (1.2159...×10)^0.8719...) au lieu de 9.05365. `YBM_RTM_MEH` est
     touché de la même façon. Ce sont les deux seules expressions de TWD_land concernées.
   - Lignes : `yieldoperators` `PRECEDENCE`, ROOT `YBM_RTM_BOG`.
2. **Yields complexes d'un bloc `*YC` qui contient une équation avec un nombre ou une
   constante (issue #347).**
   - Dans un tel bloc, tout yield dont l'opérateur n'est pas `_EQUATION` (`_SUM`, `_MULTIPLY`,
     `_DIVIDE`, `_SUBTRACT`, `_RANGE`, `_MAI`...) rend la valeur de sa dernière source, sans
     constante :
     - `_SUM(VOLUMETOTAL,VOLUMETOTAL)` = 120 au lieu de 240 ;
     - `_SUM(FIRST,4)` = 6 au lieu de 10 ;
     - `_RANGE(VOLUMETOTAL,0,50)` = 120 au lieu de 0 ;
     - `_MAI(VOLUMETOTAL)` = 120 au lieu de 17,14.

     Le même yield est juste dans un bloc sans nombre, ou quand l'équation numérique est dans
     un autre bloc `*YC`. Une équation sans nombre, comme `_EQUATION(VOLUMETOTAL+VOLUMETOTAL)`,
     ne déclenche rien.
   - Mécanisme :
     1. `FMTYieldParser::_getEq` garde le texte des nombres et des constantes parmi les
        sources de l'équation. Avant `c59ea269` (2020-11-27, version 0.9.5), ce texte était
        vide.
     2. `FMTComplexYieldHandler::indexes` prend ces textes pour des index de thème : le bloc
        passe par la branche des index de `FMTYieldParser::_cleanUp`, même sans thème indexé.
     3. Cette branche reconstruit chaque yield du bloc en `_EQUATION` avec
        `FMTData::toExpression`, qui ne convient qu'aux équations : `_SUM(A,B)` devient
        `_EQUATION(AB)`, sans opérateur ni constante.
     4. `FMTExpression::_evaluatePostFix` rend le sommet de la pile, soit la dernière source.
   - Même branche : sans thème indexé, une constante d'équation vaut 0, car
     `FMTYieldParser::_getIndexValues` ne résout les constantes que dans sa boucle sur les
     thèmes indexés. `_EQUATION(#DEUX*3)` vaut 0 au lieu de 6 ; `_MULTIPLY(VOLUMETOTAL,#DEUX)`,
     hors équation, est juste.
   - Preuves :
     - le modèle relu puis écrit par `FMTModelParser::write` contient `_EQUATION(AB)` ;
     - l'exécutable officiel `testWrapperCoreGetYield` donne les mêmes valeurs ;
     - dans une simulation `FMTNssModel` (`FMTNsstest`), l'inventaire de
       `_SUM(VOLUMETOTAL,VOLUMETOTAL)` égale celui de `VOLUMETOTAL`.
   - Dans TWD_land, relevé par une sonde : seul `YBM_RTM_F` = `_SUM(YBM_RTM_BOG,YBM_RTM_MEH)`
     est touché (il vaut `YBM_RTM_MEH`), dans la racine et 57 scénarios qui en reprennent le
     bloc. Les `_RANGE`, `_DIVIDE`, `_MAI`... des autres scénarios sont dans des blocs sans
     nombre et restent justes. Quatre scénarios, illisibles sans conversion d'erreurs en
     avertissements, n'ont pas été relevés.
   - Contournements : mettre les équations numériques dans un autre bloc `*YC`, ou écrire
     `_EQUATION(FIRST+SECOND)`.
   - Correction : d'après le code, les équations numériques ne fonctionnent aujourd'hui que
     grâce à cette reconstruction (sans elle, `_toMap` lirait leurs nombres comme des yields
     inconnus, donc 0). Piste : dans `_cleanUp`, ne reconstruire que les `_EQUATION`, et
     résoudre nombres et constantes hors de la boucle des thèmes indexés.
   - Lignes désactivées : `yieldoperators` `SUMOFYIELDS`, `SUMWITHCONSTANT`, `SUMOFAGEYIELDS`,
     `PRODUCT`, `QUOTIENT`, `SUBTRACTION`, `INRANGE`, `GROWTH` et `CONSTANT`.
   - Lignes base qui protègent le comportement juste : les 9 yields de `yieldoperatorsalone`,
     les mêmes opérateurs dans un bloc sans équation numérique.

Dans TWD_land, `YBM_RTM_BOG`, `YBM_RTM_MEH` et `YBM_RTM_F` ne servent qu'à l'output `TOTALCAR`,
qu'aucun objectif ni aucune contrainte n'utilise. À la correction, relancer la suite base pour
repérer les valeurs attendues qui changent, et justifier au journal toute valeur modifiée
(règle 9).

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

**État au lot 2** : les lacunes 1, 2 et 3 ont leurs premiers tests (`testScenarioReading`,
`testJointScenarioReading`, `testReadingErrors`). Il reste pour la lacune 1 les transitions et
le détail des yields par section. La lacune 4 est entamée par l'opérabilité et les yields ;
restent `operate` (transitions) et le vieillissement, au lot 3.

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
- 82 scénarios dans `Scenarios/` : 74 d'origine, plus 8 créés au lot 2 (`yieldoperators` et
  7 `INVALID_*`) ; `stdconstraints` a reçu un `_out`.
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
| Structure lue (lacune 1) | **fait au lot 2** | aucune : racine, `LP3` ; `stdconstraints` a reçu l'output `TEST2` |
| Lecture groupée (lacune 2) | **fait au lot 2** | aucune : 7 combinaisons de scénarios existants |
| Tests négatifs (lacune 3) | **fait au lot 2** | `INVALID_undefinedoutput` (34), `INVALID_undefinedattribute` (19), `INVALID_undefinedaggregate` (19), `INVALID_invalidnumber` (12), `INVALID_undefinedyield` (8), `INVALID_leakingtransition` (38), `INVALID_unclosedforloop` (81) : chacun copie une section de la racine et n'en change qu'une ligne, commentée en tête |
| Opérateurs de yields complexes | **fait au lot 2** | deux blocs `*YC` à valeurs calculables à la main : `yieldoperators`, mélangé à des équations numériques (§ 2.7), et `yieldoperatorsalone`, sans équation numérique, où les mêmes opérateurs sont justes |
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

### Lot 2 : lecture (2026-09-17)

**Statut** : livré le 2026-09-17 dans le worktree `new_test`, compilé par Gabriel le 2026-09-17,
**validé** le 2026-09-17 : suite base de 167 tests, 159 verts et 8 désactivés, en 13,8 s avec
`-j 8` (voir « Validation » ci-dessous). Reste à inscrire le numéro des issues du § 2.7.

- **Trois exécutables** dans `Examples/C++/` :
  - `testScenarioReading` : requêtes sur un scénario lu (§ 1, « Patron d'un test ») ;
  - `testJointScenarioReading` : scénarios lus ensemble ou un par un, comparés par leur
    écriture texte, dans `build/release/tests/testJointScenarioReading/<hachage>` ;
  - `testReadingErrors` : code `FMTexc` attendu, cherché dans toute la chaîne d'exceptions.
- **`basetests.csv`, 77 lignes** :
  - 63 requêtes : 43 sur ROOT, 7 sur `LP3`, 8 sur `stdconstraints`, 5 sur `yieldoperators` ;
  - 7 combinaisons de lecture groupée ;
  - 7 erreurs de lecture.
- **Valeurs attendues** : toutes calculées à la main depuis les fichiers du modèle, puis
  confirmées à l'exécution. Pour les points à interpréter, le code a été lu avant de fixer la
  valeur : interpolation linéaire, âge au-delà du dernier point (dernière valeur,
  `FMTYieldHandler::getLinearValue`), `_LENGTH` (`INT_MAX`), virgule des contraintes.
- **`knownbugs.csv`** : la ligne `teststdconstraints` est remplacée par les 8 lignes des deux
  bogues du § 2.7.
- **`teststdconstraints.cpp` supprimé** (§ 2.5) ; `Scenarios/stdconstraints/TWD_land._out`
  définit `TEST2`.
- **Nouveaux scénarios** : `yieldoperators` et les 7 `INVALID_*` (§ 4.3).
- **Correctif de `TestTools.h` (défaut du lot 1)** : `printException` appelait
  `std::rethrow_if_nested` sur une exception FMT dont le pointeur imbriqué est vide, donc
  `std::terminate`. Toute exception FMT qui s'échappait d'un test finissait en arrêt anormal
  au lieu d'un retour à 1. Le parcours passe par `visitNested`, qui vérifie `nested_ptr()`
  comme `FMTExceptionHandler::throwNested`.
- **Écarts au plan** :
  - `teststdconstraints` n'est pas réécrit mais remplacé par des lignes du test générique ;
  - les bogues de yields (§ 2.7) ont été trouvés en posant les valeurs attendues des yields
    complexes de la racine ; ils ont conduit au scénario `yieldoperators`.

Vérifications (hors build, 2026-09-17) :

- **Compilation** : chaque test compilé dans le scratchpad avec les options du `.vcxproj`
  généré (`cl`, mêmes définitions et bibliothèques) et lié au `FMTlib.lib` du build de `dev`,
  dont les sources de FMTlib sont identiques à celles de `new_test`. Deux erreurs corrigées
  avant livraison : `FMTActionComparator` n'est pas exportée (erreur de lien), et
  `DIFFERENCE` est une macro de `windows.h`.
- **Rejeu des CSV**, découpés comme le fait `Examples/C++/CMakeLists.txt` : les 77 lignes base
  passent, les 8 lignes de `knownbugs.csv` échouent, et les 85 noms ctest sont distincts.
- **Vu rouge**, sans recompiler :
  - `testScenarioReading` avec une valeur attendue fausse ;
  - `testJointScenarioReading` en comparant `LP` avec `LP2` lu seul (6 échecs) ;
  - `testReadingErrors` avec le code attendu 0.
- **`TestTools.h`** : auto-test étendu (exception imbriquée à la manière de FMT, `visitNested`),
  vert avec `cl /W4 /permissive-`.
- **Bogues du § 2.7** recoupés avec l'exécutable officiel `testWrapperCoreGetYield` et avec
  `FMTNsstest` (planification), sur trois builds.
- **Encodages** : fichiers nouveaux en ASCII et CRLF ; aucun U+FFFD introduit.

Validation (après le build de Gabriel, 2026-09-17) :

- **`CTestTestfile.cmake` régénéré** (`ctest --show-only=json-v1`) :
  - 167 tests inscrits, aucun sous `T:/` : `BFECtests.csv` est absent de ce worktree ;
  - les 164 lignes des CSV sont toutes inscrites (156 base, 8 de `knownbugs.csv`) : aucune
    ligne sans cible ;
  - `SKIP_RETURN_CODE 77` et `FIXTURES_REQUIRED` sur ces 164 tests, dont 8 `DISABLED` ;
  - plus aucune trace de `teststdconstraints` ;
  - verrous : `testreadwriteproject` 3 et `planningtest` 1. Les lignes BFEC des autres
    verrous manquent dans ce worktree.
- **Suite base** (`-E "T:/" -j 8`) : 167 tests ; 159 verts ; les 8 lignes de `knownbugs.csv`
  « Not Run (Disabled) » ; 13,8 s réelles.
- **Fixture** :
  - `ExamplesModelsSnapshot` est le premier test exécuté à se terminer (9/167, juste après
    les 8 désactivés) ;
  - `ExamplesModelsUnchanged` se termine en dernier (167/167) ;
  - les deux sont verts, et `git status` est inchangé après le passage.
- **Vu rouge sur le vrai build**, sans recompiler. Chaque commande rend 1 sur la vérification
  elle-même, pas sur une exception :
  - `testScenarioReading` : aire fausse, actions dans le mauvais ordre, `COUPETOTALE`
    opérable à 4 ans ;
  - `testJointScenarioReading` : `LP|LP2` comparés à `LP2|LP` lus seuls (6 échecs, sections
    `.act`, `.opt` et `.pri`) ;
  - `testReadingErrors` : code attendu 0 sur `INVALID_undefinedoutput` ; ROOT, qui ne lève
    rien, avec le code 34.

  Les mêmes commandes avec les bonnes valeurs rendent 0. Sur le code faux, le message liste
  la chaîne (`FMTexc(53)` quatre fois, puis `FMTexc(34)`) et le test rend 1 : le correctif de
  `TestTools.h` tient.
- **Lignes de `knownbugs.csv` lancées à la main** : les 8 échouent encore, avec les valeurs du
  § 2.7 (36 ; 8,82975442 ; 5 ; 6 ; 120 ; 5 ; 5 ; 5).
- **Durées des nouveaux tests** sous `-j 8` :
  - `testScenarioReading` : 5,7 s cumulées pour 63 lignes, 0,17 s au plus ;
  - `testJointScenarioReading` : 13,3 s pour 7 lignes, dont 9,85 s pour la ligne
    `fullcarbon` (4,3 s lancée seule) ;
  - `testReadingErrors` : 23,5 s pour 7 lignes, mais de 0,05 à 0,08 s par ligne lancée
    seule. Ce sont les tests d'optimisation lancés en même temps qui ralentissent tout (§ 8).

### Complément du lot 2 : démonstrations du § 2.7 (2026-09-17)

**Statut** : livré le 2026-09-17 (12 lignes CSV et deux scénarios, aucun code), **validé** le
2026-09-18 après la reconfiguration de Gabriel : 179 tests inscrits, 168 verts, 11 désactivés,
13,7 s réelles. Les 9 lignes de `yieldoperatorsalone` coûtent 0,5 s en tout.

- Demandé par Gabriel, qui doutait de la description du deuxième bogue. La cause a été trouvée
  dans le code et démontrée avec les exécutables du build (§ 2.7) : ce n'est pas « `_SUM` ne
  garde que la dernière source », mais la conversion en `_EQUATION` de tout un bloc `*YC` dès
  qu'une de ses équations contient un nombre ou une constante. Un troisième défaut est apparu
  au passage : sans thème indexé, une constante d'équation vaut 0.
- **Nouveau scénario `yieldoperatorsalone`** : les mêmes opérateurs, seuls dans leur bloc.
  9 lignes base, calculées à la main depuis la table d'âge de la racine (`volumetotal` = 120 à
  7 ans) : `_SUM` (240, et 150 avec une constante), `_MULTIPLY` (240), `_DIVIDE` (60),
  `_SUBTRACT` (100), `_RANGE` (1 dans l'intervalle, 0 hors de l'intervalle), `_MAI` (17,14),
  `_CAI` (10).
- **`yieldoperators`** reçoit trois yields de plus et un `TWD_land._con` : `INRANGE` (`_RANGE`),
  `GROWTH` (`_MAI`) et `CONSTANT` (`_EQUATION(#TWO*3)`), inscrits dans `knownbugs.csv`. Ils
  rendent 120, 120 et 0.
- Vérifications : les 9 lignes base passent et ont été vues rouges avec une valeur fausse ; les
  3 lignes connues échouent avec les valeurs du § 2.7 ; au rejeu des CSV découpés comme
  `Examples/C++/CMakeLists.txt`, 176 noms de tests, aucun doublon. La suite déjà inscrite
  (167 tests) reste verte après la modification du scénario `yieldoperators`.

## 6. Prochain lot

### Reste du lot 2

- **Rien ne bloque.** Les issues #346 et #347 sont ouvertes (§ 2.7) et la suite est verte.
- **Modèles BFEC** : j'ai proposé d'y relever les blocs `*YC` mélangés, pour mesurer l'effet
  réel du § 2.7. Sans retour pour l'instant.

### Lot 3 : objets du cœur

- `operate` et transitions : cibles, proportions, `_LOCK`, âge cible, `_DEATH`. Une requête
  `OPERATE|<action>|<masque>|<âge>` de `testScenarioReading` rendrait les chemins de
  développement (masque, âge, verrou, proportion).
- Vieillissement (`grow`) et verrous.
- Fonctions de yield qu'aucun scénario n'utilise : `_YTP`, `_ENDPOINT`, `_DELTA`,
  `_DISCOUNTFACTOR`, `_RANGE`, `_MAI`, `_CAI`. Les ajouter à `yieldoperators`, en gardant en
  tête le § 2.7 pour celles qui prennent plusieurs sources.

### Ensuite (par criticité)
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
- **Tout appel de ctest réécrit** `build/release/Testing/Temporary/LastTest.log`, même
  `ctest -N` ou `--show-only`, qui le laissent vide. Le copier avant le premier appel, et
  prendre les durées dans la sortie console du passage.
- **Exceptions imbriquées de FMT** : `FMTDefaultExceptionHandler::raise` lance même
  l'exception la plus profonde par `std::throw_with_nested`, hors de tout `catch`. Son
  pointeur imbriqué est donc vide, et `std::rethrow_if_nested` appelle alors `std::terminate`.
  - Parcourir une chaîne avec `Testing::visitNested`, jamais avec `std::rethrow_if_nested`
    directement.
  - `testWrapperCoreAreaVariability`, `testWrapperCorePlanning` et `testWrapperCoreSA`
    l'appellent encore dans leur affichage d'erreur : à corriger quand on les touche.
- **Macros de `windows.h`** (par GDAL) : outre `min`, `max`, `ERROR`, `NEAR`, `FAR`, `IN`,
  `OUT`, `DELETE`, aussi `DIFFERENCE` (lot 2). Écrire `(std::max)(...)` et
  `(std::numeric_limits<int>::max)()`.
- **Classes non exportées** : `Core::FMTActionComparator` (et d'autres comparateurs) n'a pas
  `FMTEXPORT`. Un test qui l'utilise ne se lie pas.
- **Argument vide** : CMake retire une colonne vide de la commande, donc un test ne peut pas
  attendre une valeur vide. Par exemple, les thèmes de la racine n'ont pas de nom.
- **Longueur des chemins** : le dossier de sortie d'un test ne doit pas porter une liste de
  noms. `testJointScenarioReading` le nomme par un hachage, car 9 noms de scénarios
  dépassaient la limite de 260 caractères de Windows. Un fichier de scénario au-delà de cette
  limite (dossier courant et chemin relatif mis bout à bout) est ignoré sans message : FMT lit
  la section de la racine à la place.
- **Yield inconnu** : `FMTYields::get` rend 0 sans erreur pour un nom qu'aucun bloc ne
  contient. Une requête `YIELD` sur un nom mal écrit passe donc si la valeur attendue est 0.
- **`FMTModelParser::write`** attrape ses exceptions sans les relancer : vérifier que les
  fichiers existent.
- **Lecture de `output_type`** : seule, avec le gestionnaire par défaut, elle lève
  `FMTexc(64)`. Ce scénario n'est lu qu'avec des erreurs converties en avertissements.
- **Transition d'une action inconnue** : simple avertissement `FMTignore` (41), pas une
  erreur. Pas de test négatif possible.
- **Écriture (pour le lot 5)** :
  - `_SUM(YBM_RTM_BOG,YBM_RTM_MEH)` est réécrit `_EQUATION(YBM_RTM_BOGYBM_RTM_MEH)` sans
    opérateur, que la relecture remplace par 0 ;
  - l'objectif `_MIN _PENALTY(...)` est réécrit `_MIN y_PENALTY(...)`.
- **Sondes hors build** (compilation dans le scratchpad contre le FMTlib de `dev`) : un
  exécutable placé hors de `bin/Release` charge `C:\Windows\System32\onnxruntime.dll`, trop
  ancienne (« version [16] is not supported »). Copier `onnxruntime.dll` à côté de
  l'exécutable de sonde.
- Plusieurs tests lèvent avec un nom de méthode copié d'un autre test (`"presolvetest"` dans
  `doplanning`) : lire le numéro de ligne plutôt que le nom.

## 8. Mesures

À refaire à la fin de chaque lot.

| Date | Tests base inscrits | Durée cumulée | Exécutables base | Sans vérification | Vérifications mortes | Lignes CSV ignorées | Désactivés | Exécutables BFEC seulement |
|---|---|---|---|---|---|---|---|---|
| 2026-09-16 (lot 0) | 80 (79 lignes + `UnitTestFMTexcelcache`) | 51 s cumulées (passage complet avec BFEC) | 48 | 17 | 3 | 2 | 0 | 25 |
| 2026-09-17 (lot 1) | 83 (79 lignes, 2 fixtures, `teststdconstraints` désactivé, `UnitTestFMTexcelcache`) | 9,3 s réelles ; 65,7 s cumulées | 48 | 17 | 3 | 0 | 1 | 25 |
| 2026-09-17 (lot 2) | 167 (156 lignes, 8 désactivées, 2 fixtures, `UnitTestFMTexcelcache`) | 13,8 s réelles ; 104,5 s cumulées | 51 | 17 | 3 | 0 | 8 | 25 |
| 2026-09-18 (complément lot 2) | 179 (165 lignes, 11 désactivées, 2 fixtures, `UnitTestFMTexcelcache`) | 13,7 s réelles ; 100,3 s cumulées | 51 | 17 | 3 | 0 | 11 | 25 |

- Tests base inscrits : `ctest ... -N -E "T:/"` (fixtures et tests désactivés compris).
- Durée : la référence est le temps réel de la suite base seule (`Total Test time (real)`).
  La somme des « Test time » de `LastTest.log` varie avec la charge : avec `-j 8`, les tests
  base lancés ensemble se ralentissent entre eux. Les 65,7 s du lot 1 ne se comparent donc pas
  aux 51 s du lot 0, mesurées dans le passage complet. Au lot 2, la somme vient de la sortie
  console : `LastTest.log` avait été réécrit (§ 7).
- Exécutables base : premières colonnes distinctes de `basetests.csv` qui nomment une cible.
- Lignes CSV ignorées : alertes `no target named` à la configuration. Sans la sortie de la
  configuration, compter les lignes des CSV absentes de `ctest -N` (lot 2).
- Désactivés : lignes de `knownbugs.csv`.
