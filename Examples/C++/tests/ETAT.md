# État des tests FMT sans `T:\`

> Fichier de suivi du chantier « plus de tests exécutables sans les données privées ». Il est
> mis à jour **à la fin de chaque lot**. Une session qui démarre à froid doit pouvoir savoir
> d'ici quoi faire, sans relire l'historique.
>
> Le dépôt est public : ce fichier ne cite ni chemin `T:\` ni modèle privé. Les tests BFEC
> y sont désignés par le nom de leur exécutable.
>
> **Ordre de lecture pour démarrer à froid** : le § 0 situe la chaîne de FMT et dit comment on
> ajoute un test ; le § 1 donne les règles et le patron ; le § 6 dit quoi faire ensuite ; le § 7
> se lit avant de toucher au code, il tient les pièges déjà payés. Les § 2 à 5 sont l'inventaire,
> les lacunes, les scénarios et le journal, à consulter au besoin.

## 0. Vue d'ensemble

### Ce que FMT fait d'un modèle, et où les tests se branchent

Un modèle Woodstock est un dossier de sections (`.lan` le paysage, `.are` l'inventaire, `.yld`
les rendements, `.act` les actions, `.trn` les transitions, `.lif` la durée de vie, `.out` les
outputs, `.opt` l'optimisation, `.seq` les cédules), plus des scénarios qui en remplacent une
partie : `Scenarios/<nom>/<projet>._yld` remplace le `.yld` de la racine, et ce qu'un scénario
ne redéfinit pas, il l'hérite. FMT traverse toujours ce modèle dans le même ordre, et chaque
étape a ses tests :

1. **Lecture.** `FMTModelParser::readproject` appelle un parseur par section et rend un
   `FMTModel` par scénario demandé : thèmes, agrégats, développements initiaux, yields,
   actions, transitions, lifespan, outputs, contraintes. Une erreur ici fausse tout le reste
   en silence. → `testScenarioReading`, `testJointScenarioReading`, `testReadingErrors`
   (lot 2), lacunes 1 à 3 du § 3.
2. **Objets du cœur.** Un développement est un masque (`FMTMask`) avec un âge, un verrou et
   une période. Il vieillit (`grow`), devient opérable (`operable`) et se transforme
   (`operate`, qui applique la transition et rend des chemins avec leurs proportions). Les
   rendements répondent à une requête (`FMTYields::get`). → requêtes `OPERABLE`, `OPERATE`,
   `GROW` et `YIELD` de `testScenarioReading` (lot 3), lacune 4.
3. **Graphe et matrice.** `FMTSrModel::buildPeriod` construit, période par période, un graphe
   dont les sommets sont des développements et les arcs des actions ou de la croissance ;
   `FMTLpModel` le traduit en matrice, dont les lignes sont des contraintes et les colonnes des
   variables. → requête `STATS` de `testScenarioPlanning` (lot 4), lacune 6.
4. **Optimisation.** L'objectif et les contraintes du `.opt` sont résolus par MOSEK ou CLP, le
   présolve pouvant réduire le modèle avant. → les 16 objectifs de `doplanning` et compagnie,
   `presolvetest` pour le présolve, lacune 5.
5. **Résultats.** La solution donne une cédule par période (`getSolution`) et des valeurs
   d'outputs (`getOutput`). Une cédule peut être rejouée dans un modèle neuf
   (`doPlanning(false, cédules)`). → requête `REPLAY` de `testScenarioPlanning` (lot 4),
   lacune 7.
6. **Écriture.** `FMTModelParser::write` réécrit un modèle lu, `writeToProject` un projet
   complet, `FMTScheduleParser` les cédules. C'est la seule étape encore sans test de valeur.
   → lacune 8, lot 5 à venir.

Un test base ne fait jamais rien d'autre que ceci : partir de TWD_land, s'arrêter à une de ces
étapes, et comparer ce qu'il y trouve à une valeur écrite dans le CSV.

### Où vivent les choses

| Quoi | Où |
|---|---|
| Sources des exécutables de test | `Examples/C++/*.cpp`, et `FMTWrapperCore/tests/*.cpp` pour le Core |
| Lignes qui inscrivent les tests | `Examples/C++/tests/basetests.csv` (public), `knownbugs.csv` (désactivées), `BFECtests.csv` (local, privé, ignoré par git) |
| En-tête commun des tests | `Examples/C++/tests/TestTools.h` |
| Modèle public et ses scénarios | `Examples/Models/TWD_land`, `Examples/Models/TWD_land/Scenarios/<nom>/` |
| Inscription dans ctest | `Examples/C++/CMakeLists.txt` (boucle sur les CSV) |
| Empreinte du modèle entre deux passages | `cmake/TestsDataSnapshot.cmake` |
| Sorties des tests | `build/release/tests/<test>/` |
| Suivi du chantier | ce fichier |

### Comment on ajoute un test

1. **Choisir la lacune** au § 3 : on descend la liste par criticité, une étape que tout modèle
   traverse avant une fonction de niche.
2. **Calculer la valeur attendue à la main**, depuis les fichiers du modèle, jamais depuis ce
   que FMT affiche : sinon le test grave le comportement actuel, bogue compris. Quand le calcul
   à la main n'a pas de sens (statistiques d'un graphe), on mesure une fois, on le dit au
   journal, et on vérifie au moins que la valeur ne dépend ni du solveur ni du passage.
3. **Réutiliser un test générique** quand il peut répondre (`testScenarioReading`,
   `testScenarioPlanning`, `testReadingErrors`, `testJointScenarioReading`) : il n'y a alors
   qu'une ligne CSV à écrire, sans compilation. Sinon, écrire un exécutable dans
   `Examples/C++/`, sur le patron du § 1.
4. **Ajouter la ligne CSV** (§ 2.1), faire reconfigurer CMake, et compiler si un source a
   changé.
5. **Voir le test rouge** en faussant la valeur attendue ou l'entrée, puis vert (règle 1).
6. **Noter au journal** (§ 5) et refaire les mesures (§ 8).

### Ce chantier et l'issue #350

L'issue #350, « Add comprehensive unit tests for the Core namespace » (jalon FMT2.0), demande
des tests **unitaires** classe par classe : constructeurs, copies, comparaisons, hachage,
sérialisation, allocations, concurrence, le tout sous un cadre à la GoogleTest dans `Tests/Core/`,
avec un inventaire dans `Documentation/Testing/CoreTestCoverage.md` et une couverture publiée.

Ce chantier-ci travaille à l'autre échelle : il protège **la chaîne** que tout modèle traverse
(§ 0), depuis un modèle public, par des lignes CSV. Les deux se complètent plutôt qu'ils ne se
recoupent :

- un test unitaire sur `FMTOperator::precedence` aurait attrapé le défaut 1 du § 2.7 en trois
  lignes, alors qu'il nous a fallu un scénario et une enquête ;
- à l'inverse, le défaut 2 ne se voit d'aucune classe prise isolément : il naît de
  `FMTYieldParser::_cleanUp`, qui reconstruit tout un bloc à la lecture. Seule une lecture de
  modèle le révèle.

Ce que ce chantier apporte déjà à #350 : l'inventaire de ce qui est vérifié (§ 2), le classement
par criticité, qui est la colonne « Priority » de leur tableau (§ 3), l'infrastructure ctest
(alerte de ligne sans cible, empreinte du modèle, code 77, lignes désactivées, § 2.1), et quatre
défauts caractérisés avec leur test désactivé (§ 2.7), ce qu'ils appellent des tests de
caractérisation avant refonte. Les lots 2 à 4 couvrent, au niveau du comportement, la matière de
leurs jalons 2 à 7 : `FMTMask`, `FMTTheme`, `FMTDevelopment`, `FMTSpec`, `FMTData`,
`FMTExpression`, `FMTOperator`, les gestionnaires de rendements, `FMTAction`, `FMTTransition`,
`FMTSchedule`, `FMTOutput`, `FMTConstraint` et `FMTGraphStats`.

Ce que #350 demande et que ce chantier ne fait pas : constructeurs et copies, opérateurs de
comparaison, hachage, aller-retour de sérialisation, absence d'allocation, concurrence,
portabilité Linux et Clang, couverture publiée.

**Ce chantier prend en charge la fermeture de #350** (décision du 2026-09-25). Il garde sa façon
de travailler — l'ordre par criticité, les lots, le vu rouge — et absorbe le volet unitaire que
l'issue demande. La feuille de route complète est au § 6 ; voici ce que #350 exige, où on en est,
et quel lot s'en charge :

| Demande de #350 | État | Lot |
|---|---|---|
| Inventaire des composants et de leur couverture | partiel : fait par étape de la chaîne (§ 2), reste à faire par classe | 6 |
| Cible et infrastructure de test dans CMake et ctest | fait pour les tests de chaîne (§ 2.1) ; reste la cible `FMTCoreTests` et l'étiquette `unit;core` | 6 |
| Cadre de test unitaire choisi et documenté | à trancher | 6 |
| Tests unitaires des classes déjà suspectes (défauts du § 2.7) | non fait ; leur comportement n'est touché qu'indirectement | 7 |
| Tests unitaires du reste de Core (jalons 5 à 7 de #350) | non fait, même remarque | 8 |
| Constructeurs, copies, comparaisons, hachage | non fait | 7 et 8 |
| Aller-retour de sérialisation | non fait | 9 |
| Absence d'allocation, concurrence | non fait | 9 |
| Portabilité Linux et Clang | non fait ; la suite ne tourne que sous Windows, et le dépôt n'a pas d'intégration continue visible | 9 |
| Couverture publiée | non fait ; informative après un lot, consignée dans la documentation aux versions | 9 |
| Tests de caractérisation avant refonte | fait pour quatre défauts (§ 2.7), à étendre | continu |

À chaque lot, noter au journal ce qu'il apporte à #350. Les conditions de fermeture sont au § 6 ;
le jour venu, écrire ici ce qui l'a fermée et ce que ce chantier reprend ou abandonne.

Trois décisions ont été prises le 2026-09-25 :

1. **Le cadre de test.** GoogleTest, **pour la seule cible des tests unitaires**, parce que c'est
   lui qui donne les filtres et le rapport par cas que #350 demande. `TestTools.h` reste maison
   et sans dépendance pour les tests de chaîne, dont la force est d'ajouter un cas sans
   recompiler. Ni la bibliothèque ni les tests de chaîne ne dépendent de GoogleTest.
2. **Le partage des fichiers de suivi.** La couverture et l'inventaire par classe se consignent
   dans la documentation (`Documentation/Testing/`). Ce fichier-ci reste le fichier de travail
   du chantier : règles, ordre des travaux, pièges, journal.
3. **Le ménage viendra après.** Ce fichier est valide tant que le chantier tourne. Le jour où la
   couverture sera bonne, on décidera ce qui mérite de devenir permanent — et où : ce qui relève
   des conventions va dans `Documentation/CodingStandards.md` ou `AGENTS.md`, ce qui relève de
   l'état d'un chantier disparaît avec lui.

La couverture elle-même reste écartée comme critère d'ordre : elle arrive au lot 9 et sert à
lister ce qui reste sans test une fois l'essentiel couvert.

### Quel effort donner à l'agent

Les niveaux sont `low`, `medium`, `high`, `xhigh` et `max` (de « faible » à « ultra » dans
l'interface). Ce qui suit est tiré de ce que les lots 1 à 4 ont réellement demandé.

| Travail | Effort | Pourquoi |
|---|---|---|
| Lancer la suite après un build, rapporter, mettre à jour les mesures | `low` | Commandes connues, aucune décision. |
| Ajouter des lignes CSV à un test générique, avec des valeurs calculables à la main | `medium` | Il faut lire les fichiers du modèle et calculer juste, mais le chemin est balisé. |
| Écrire ou étendre un exécutable de test | `high` | Il faut lire l'API de FMT, choisir ce qui est vérifiable, et se méfier des classes non exportées et des macros de `windows.h` (§ 7). |
| Un lot entier de la section 6 | `high` | C'est le rythme des lots 2 à 4 : quelques jours de travail condensés, avec ses vérifications. |
| Enquêter sur un résultat suspect, démontrer un défaut de FMTlib | `xhigh` ou `max` | Les quatre défauts du § 2.7 ont demandé de lire le code jusqu'à la cause, de monter des scénarios A/B et de remonter l'historique git. À `medium`, l'agent aurait écrit « `_SUM` ne garde que la dernière source », ce qui était faux. |
| Mettre à jour ce fichier, rédiger le texte d'une issue | `medium` | Rédaction, sans exploration. |

Deux repères. **Trop bas est dangereux quand une valeur attendue doit être calculée à la main** :
un agent pressé prend la valeur que FMT affiche, et le test grave alors le bogue au lieu de le
révéler. **Trop haut ne casse rien**, cela coûte seulement du temps et des jetons ; en cas de
doute sur une enquête, monter d'un cran.

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
- **Décisions du 2026-09-25** :
  - **Cadre unitaire** : GoogleTest pour la seule cible des tests unitaires de Core ; les tests
    de chaîne gardent `TestTools.h` (§ 0).
  - **Fermeture de #350** : ce chantier s'en charge, sans changer son ordre par criticité
    (§ 0 et § 6).
  - **Un test ctest par cas** (`gtest_discover_tests`), pour qu'un échec soit identifiable et
    un cas désactivable seul.
  - **Défaut révélé par un test unitaire** : cas préfixé `DISABLED_`, entrée au § 2.7, issue
    documentée et tâche de correction (règle 3).
  - **En-tête de licence** dans `Tests/`, comme la bibliothèque ; les exemples restent
    l'exception historique.
  - **Emplacement** : `Tests/Core/` pour l'unitaire, `Examples/C++/` inchangé pour la chaîne.
  - **Couverture** : mesurée à titre informatif à la fin d'un lot, consignée dans la
    documentation au moment des versions.
  - **Corrections de FMTlib** : jamais dans un lot de tests. Un défaut donne une issue
    documentée, corrigée dans une session à part.

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
   reste comme contrôle réaliste. *Pourquoi* : un test qui vit sur `T:\` ne protège personne
   d'autre que nous ; quiconque compile FMT doit pouvoir constater que le bogue est parti.
3. **Un bug révélé par un nouveau test ne se corrige pas dans le même lot.** Le test est
   inscrit désactivé — ligne de `knownbugs.csv` pour un test de chaîne, cas préfixé
   `DISABLED_` pour un test unitaire — et il donne lieu à **une issue documentée et à une tâche
   de correction**, menée dans une session séparée de celle qui écrit les tests (décision du
   2026-09-25). *Pourquoi* : un lot qui ajoute des tests et corrige FMT en même temps devient
   impossible à relire, et la correction revient à qui connaît le code. La ligne désactivée
   garde la trace du bogue et passe dans `basetests.csv` le jour de la correction — un défaut
   révélé n'est donc jamais un défaut oublié.
4. **Entrées explicites** : modèle, scénario et valeurs attendues arrivent par les arguments
   du CSV. Les valeurs par défaut d'un source ne pointent que vers TWD_land. *Pourquoi* :
   plusieurs tests ont des défauts codés en dur vers des chemins privés ou disparus (§ 7) ; une
   ligne sans argument teste alors autre chose que ce qu'on croit, ou rien du tout.
5. **Aucune écriture sous `Examples/Models`** : le test copie le projet dans
   `build/release/tests/<test>/` (patron de `testWrapperCorePlanning`). *Pourquoi* : deux tests
   qui écrivent au même endroit se marchent dessus, et un modèle modifié en cours de route
   fausse tous les tests suivants. L'empreinte prise avant et après chaque passage (§ 2.1) le
   détecte.
6. **Un nouveau test privilégie MOSEK.**
   - Il prend `Models::FMTSolverInterface::MOSEK` quand FMT est compilé avec MOSEK
     (`FMTWITHMOSEK`), `CLP` sinon : patron `#ifdef FMTWITHMOSEK` de `doplanning.cpp`,
     paramètre `solver` des DTO du Core compris.
   - Les tests existants gardent leur solveur, CLP ou MOSEK : on ne les convertit pas.
   - La valeur attendue doit tenir avec les deux solveurs, puisqu'un build sans MOSEK
     retombe sur CLP : d'où la règle 7.
7. **Déterminisme** : un seul thread, graine fixe. On vérifie des quantités uniques, comme
   l'objectif, plutôt qu'une solution parmi plusieurs optimales. *Pourquoi* : un test qui échoue
   une fois sur dix finit par être ignoré, et ce jour-là il aura raison. Un optimum peut être
   atteint par plusieurs solutions, qui changent avec le solveur ou l'ordre des calculs.
8. **Fonctionnalité absente** (OSI, GDAL, ONNX) : le test rend 77 (« Skipped »), pas 0. Les
   anciens tests passent à 77 quand on les touche (lot 1 et suivants). L'absence de MOSEK n'en
   fait pas partie : le test retombe sur CLP (règle 6).
9. **Toute valeur de référence modifiée, ligne retirée ou test supprimé se justifie** dans le
   commit et au journal. *Pourquoi* : changer une valeur attendue est la façon la plus simple de
   faire taire une régression sans s'en rendre compte. `UnitTestFMTAreaParser` a disparu sans
   trace dans un correctif (§ 2.5), et `stdconstraints` n'a jamais tourné pendant trois ans sans
   que personne ne le voie.
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
  `AGGREGATE`, `AREA`, `ACTIONS`, `OPERABLE`, `OPERATE`, `GROW`, `YIELD`, `OUTPUTS`,
  `CONSTRAINT`... ; liste en tête du source) et sa valeur attendue. Un développement s'y écrit
  `<masque>|<âge>[|<verrou>[|<période>]]`, et une valeur attendue écrite `FMTexc(<code>)` exige
  que la requête lève ce code (lot 3) ;
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

**Un exemple complet.** La ligne

```
testScenarioReading;../../../../Examples/Models/TWD_land/TWD_land.pri|ROOT|AREA|? ? ?;1814.76;
```

inscrit un test dont le nom est la cible suivie des deux premiers arguments, espaces remplacés
par `_`. ctest lance `testScenarioReading.exe` depuis `build/release/bin/Release` avec deux
arguments : `../../../../Examples/Models/TWD_land/TWD_land.pri|ROOT|AREA|? ? ?`, qui dit quoi
lire et quoi demander, puis `1814.76`, la valeur attendue. La quatrième colonne, vide, disparaît
de la commande.

**Pourquoi ce découpage.** Le CSV n'offre que quatre colonnes alors qu'une requête en demande
souvent plus : le `|` sert donc de séparateur à l'intérieur d'une colonne, et chaque test
redécoupe ses arguments lui-même. Les valeurs attendues vivent dans le CSV, et non dans le
code, pour deux raisons : on peut en fausser une sans recompiler, ce qu'exige la règle 1, et
une ligne de plus ne coûte qu'une reconfiguration.

### 2.2 Ce que vérifient les tests base, par étape de la chaîne

« Vérifie » = compare une valeur ; « exécute » = n'échoue que sur une exception.

| Étape | Vérifie une valeur | Exécute sans vérifier |
|---|---|---|
| Lecture d'un scénario | direct : `testScenarioReading` (lot 2 : thèmes, attributs, agrégats, aire par masque, actions et drapeaux, outputs et contraintes de ROOT ; noms de thèmes, aire héritée et contraintes de `LP3` ; contraintes de `stdconstraints`), constantes (`UnitTestFMTParser`, 3 cas), un yield complexe (`testWrapperCoreGetYield`), nombre de masques (`testWrapperCoreGetAllMasks`), âge maximal (`testWrapperCoreGetMaxAge`, borne seulement), gabarits (`templatestest`, au moins un modèle) ; indirect : objectifs de `doplanning` (16 scénarios), `landaggregatestest`, `testlevelopt` | tous les autres tests |
| Lecture de plusieurs scénarios d'un coup | direct : `testJointScenarioReading` (lot 2 : 7 combinaisons, 32 scénarios) ; indirect : `testlevelopt` (9 scénarios, objectifs) | `testreadwriteproject`, `planningtest`, `replanningtest`, `replanningmodeladaption`, `UnitTestCallbackLogger` |
| Erreurs de lecture | `testReadingErrors` (lot 2 : 7 scénarios invalides, 6 codes `FMTexc`) | -- |
| Objets du cœur (masques, yields, opérabilité, transitions) | `testScenarioReading` (lot 2 : opérabilité aux bornes d'âge, de yield et d'agrégat, `_DEATH` tiré du lifespan, interpolation des yields d'âge, opérateurs de yields complexes), `nochoice` (développements sans choix d'un masque), `testyieldsfromfactor` (monotonie), `testtimeyieldoutputs` (2 outputs de yields temporels) ; transitions et vieillissement : indirectement par les objectifs | `testaddtomodel` (vérification de valeur morte) |
| Présolve et postsolve | `presolvetest` (objectif avec ou sans présolve ; NOT_MASK, plus 6 scénarios au lot 4) ; `doplanning` (présolve de 10 itérations) | -- |
| Graphe et matrice | `testScenarioPlanning` `STATS` (lot 4 : sommets, arcs, lignes et colonnes de 3 scénarios) ; indirect (objectifs) | `testoutputshadowcost` (vérifie seulement la résolution après retrait et remise d'une contrainte) |
| Optimisation | `doplanning` (16), `testlevelopt` (9), `landaggregatestest` ; `testSolvers` (chaque solveur résout, sans valeur) | `Simpleplanning`, `createmodel` |
| Résultats et rejeu | `testlevel` (26 outputs), `testoutput` (3 outputs après rejeu d'une cédule), `forcesolution` (6 valeurs et conservation de la superficie), `sumandavgtest` (somme et moyenne), `FMTNsstest` (5 lignes sur 7) ; `testScenarioPlanning` `REPLAY` (lot 4 : rejeu de la cédule optimale, tous les outputs de toutes les périodes, 4 scénarios) | `Simpleplanning` (écrit la cédule), `testareavariabilities`, `testmultisolutions` |
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

Quatre défauts de FMTlib révélés par ces tests, sans correction dans ce chantier (règle 3). Les
lignes de `knownbugs.csv` échouent aujourd'hui : elles passeront dans `basetests.csv` avec la
correction. Issues GitHub : **#346** (priorité de `^`) et **#347** (bloc `*YC` mélangé, qui
couvre aussi la constante d'équation), **#357** (`_DISCOUNTFACTOR`) et **#358** (condition
de source ignorée).
Démonstrations refaites le
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
3. **`_DISCOUNTFACTOR` n'utilise pas le taux lu** (lot 3, issue #357).
   - `FMTTimeYieldHandler::get` (`Source/FMTTimeYieldHandler.cpp:98`) prend `data.at(1)`, le
     nombre d'années par période, comme taux d'actualisation : `rateofreturn` et `pertio`
     pointent tous deux sur `data.at(1)`, et `data.at(0)`, le taux lu par `FMTYieldParser`
     (`4%` donne 0,04), ne sert à rien.
   - `_DISCOUNTFACTOR(4%,5,FULL)` vaut 1/6^5 = 0,000128600823 à la période 1, au lieu de
     1/1,04^5 = 0,821927107.
   - L'exposant de `HALF` est `période x 0,5` là où la convention usuelle est `période - 0,5` :
     à confirmer avant de corriger (les deux donnent la même valeur à la période 1).
   - Lignes désactivées : `timeyields` `DISCOUNTFULL` (périodes 1 et 2) et `DISCOUNTNONE`
     (période 2).
4. **Condition d'une source de transition ignorée en silence** (lot 3, issue #358).
   - La section des transitions n'accepte que `@AGE(min..max)` et `@YLD(yield,min..max)`, avec
     `_MAXAGE` pour l'infini : les formes que FMT écrit lui-même (`Include/FMTBounds.hpp:77`).
     Toute autre écriture est avalée sans message : `_AGE >= 10`, qui est la forme de la
     section des actions, mais aussi `@AGE(12)`, `@AGE(10..)`, `@YLD(yield,100..)`, et même un
     yield inexistant dans ces formes-là.
   - Conséquence : la source s'applique à tous les développements de son masque. Dans le
     scénario `INVALID_sourcecondition`, `arecup`, dont la source demande `_AGE >= 10`, opère
     un développement de 5 ans.
   - `FMTTransitionParser::getSource` (`Source/FMTTransitionParser.cpp:65`) passe la fin de la
     ligne à `FMTParser::_setSpec`, qui la rend inchangée quand rien ne correspond ; personne
     ne vérifie ce reste.
   - Ligne désactivée : `testReadingErrors` sur `INVALID_sourcecondition`, qui attend
     `FMTexc(31)` (`FMTunsupported_transition`, déjà levé par ce parseur pour un `_REPLACE`
     impossible). Le code exact est à confirmer à la correction.

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

**État au lot 4** : les lacunes 1, 2 et 3 ont leurs premiers tests (`testScenarioReading`,
`testJointScenarioReading`, `testReadingErrors`). La lacune 4 est couverte : opérabilité (âge,
yield, verrou, `_LOCKEXEMPT`), `operate` et les transitions (cibles, proportions, `_AGE` et
`_LOCK` de cible, sources conditionnées), vieillissement, et les fonctions de yield sauf
`_ENDPOINT` et `_DELTA`. Le lot 4 entame les lacunes 5, 6 et 7 : présolve sur 7 scénarios,
statistiques de la matrice, rejeu de la cédule optimale. Restent les statistiques période par
période, `setSolution` ligne à ligne, et toute la lacune 8, objet du lot 5.

### Ce que chaque lacune veut dire

Le critère de classement demande deux choses. D'abord, une étape que **tout** modèle traverse
passe avant une fonction de niche : une erreur dans la lecture d'un `.yld` touche tous les
calculs, une erreur dans l'ordonnanceur d'aires d'opération ne touche que ceux qui s'en
servent. Ensuite, une erreur **silencieuse** passe avant une erreur qui lève : une exception
arrête le calcul et se voit, alors qu'un mauvais nombre se retrouve dans un plan
d'aménagement sans que personne ne s'en aperçoive.

1. **Lecture d'un scénario** (étape 1 du § 0). Tout part de là : un attribut mal rangé, un
   yield mal interpolé ou une contrainte mal bornée, et tout le reste est faux sans qu'aucune
   exception ne soit levée. Avant le lot 2, rien ne regardait le modèle lu : les tests
   vérifiaient des objectifs, qui ne bougent pas forcément quand la lecture change.
   `testScenarioReading` interroge maintenant le modèle lu, une ligne CSV par question.
2. **Lecture de plusieurs scénarios d'un coup.** `referenceRead` réutilise les sections déjà
   lues d'un autre scénario pour éviter de tout relire ; l'interface et plusieurs tâches
   lisent ainsi. Si la réutilisation prend une section au mauvais scénario, le modèle obtenu
   n'est pas celui du fichier, sans message. `testJointScenarioReading` lit les mêmes
   scénarios ensemble puis un par un, écrit les deux modèles et compare les fichiers : aucune
   valeur de référence à maintenir.
3. **Erreurs de lecture.** Un modèle invalide doit lever un `FMTexc` précis, que l'interface
   traduit pour l'aménagiste. Si une erreur cesse d'être signalée, ou change de code, un
   modèle faux passe pour bon. `testReadingErrors` lit des scénarios `INVALID_*`, chacun copié
   de la racine avec une seule ligne fautive, et exige le code attendu.
4. **Objets du cœur** (étape 2). L'opérabilité décide de ce qu'on peut couper, la transition
   de ce que ça devient, le vieillissement de l'état suivant. Ces trois fonctions sont
   appelées des milliers de fois par la construction du graphe ; elles n'étaient vérifiées
   qu'à travers un objectif, donc pas du tout en pratique. Le lot 3 les interroge directement,
   sur des développements construits dans le test.
5. **Présolve et postsolve** (étape 4). Le présolve retire du modèle ce qui ne peut pas
   servir, pour accélérer la résolution ; il doit donner **exactement** le même optimum. C'est
   le test métamorphique parfait : deux fois le même scénario, avec et sans présolve, aucune
   valeur à maintenir. Il ne tournait que sur un scénario avant le lot 4, il en couvre sept.
6. **Graphe et matrice** (étape 3). C'est l'étape la plus coûteuse et la moins visible : si un
   arc manque, l'optimum reste plausible mais le modèle n'est plus celui qu'on croit. La
   requête `STATS` fige les quatre nombres qui décrivent la structure (sommets, arcs, lignes,
   colonnes) ; ils ne dépendent ni du solveur ni du passage. Ce qui reste : les mêmes nombres
   période par période, qui diraient *où* la structure a changé.
7. **Résultats et rejeu** (étape 5). Rejouer une cédule est ce que font la replanification et
   les analyses de sensibilité : on impose une solution connue au lieu de laisser le solveur
   choisir. Si le rejeu ne reproduit pas la solution, tout ce qui en découle est faux. La
   requête `REPLAY` compare tous les outputs de toutes les périodes entre l'optimisation et
   son rejeu. Ce qui reste : `setSolution` période par période, plus fin que
   `doPlanning(false, cédules)`.
8. **Écriture et relecture** (étape 6). FMT écrit des modèles que d'autres relisent : sauvegarde
   depuis l'interface, modèle présolvé, projet transformé par une tâche. Deux défauts
   d'écriture sont déjà connus (§ 7) et personne ne les avait vus, faute de test. L'aller-retour
   est métamorphique : écrire, relire, réécrire, et les fichiers doivent être identiques. C'est
   l'objet du lot 5.

### Gains rapides : des lignes CSV sans code

Chacune demande une reconfiguration de CMake et, sauf pour les tests métamorphiques, une
valeur mesurée une fois puis vue rouge.

- `presolvetest` sur d'autres scénarios (métamorphique, aucune valeur à fixer ; il reste en
  CLP) : par exemple `LP`, `COS`, `Shift`, `TSLA`, `fullcarbon`, `equation`.
- `testmodelwriter` sur d'autres scénarios (métamorphique, en MOSEK).
- `doplanning` sur des scénarios inutilisés (§ 4.4), après avoir vérifié qu'ils sont faits
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

### 4.3 Comment monter un scénario

Un scénario est un dossier `Examples/Models/TWD_land/Scenarios/<nom>/` qui ne contient **que
les sections qu'il change**, nommées comme la section de la racine avec un souligné à la place
du point : `TWD_land._yld` remplace `TWD_land.yld`, `TWD_land._trn` remplace `TWD_land.trn`.
Tout ce que le dossier ne contient pas est hérité de la racine.

Un fichier de scénario remplace **toute** la section, pas une ligne : pour ne changer qu'une
ligne, on copie la section de la racine et on modifie cette ligne, en expliquant en tête par un
commentaire (`;`) ce qui est changé et pourquoi. C'est ce que font les sept scénarios
`INVALID_*` et `transitions`.

Le reste de la marche à suivre :

- le nom du dossier est ce que les tests passent en argument ; le garder court, à cause de la
  limite de 260 caractères de Windows (§ 7) ;
- fichiers en ASCII et en CRLF, comme le reste du dépôt ;
- ajouter un scénario ne demande pas de reconfigurer CMake ; ajouter la ligne CSV qui s'en sert,
  oui ;
- l'empreinte de `Examples/Models` est prise au début de chaque passage de ctest : un scénario
  ajouté entre deux passages ne la dérange pas, mais un test qui écrirait sous `Examples/Models`
  la ferait échouer, et c'est voulu (règle 5) ;
- un scénario invalide ne sert qu'au test négatif qui attend son exception ; il ne doit pas
  entrer dans les lectures groupées.

### 4.4 Catalogue

| Besoin | Faisabilité | Données à créer |
|---|---|---|
| Structure lue (lacune 1) | **fait au lot 2** | aucune : racine, `LP3` ; `stdconstraints` a reçu l'output `TEST2` |
| Lecture groupée (lacune 2) | **fait au lot 2** | aucune : 7 combinaisons de scénarios existants |
| Tests négatifs (lacune 3) | **fait au lot 2** | `INVALID_undefinedoutput` (34), `INVALID_undefinedattribute` (19), `INVALID_undefinedaggregate` (19), `INVALID_invalidnumber` (12), `INVALID_undefinedyield` (8), `INVALID_leakingtransition` (38), `INVALID_unclosedforloop` (81) : chacun copie une section de la racine et n'en change qu'une ligne, commentée en tête. `INVALID_sourcecondition` (31) s'y ajoute au lot 3, désactivé tant que le défaut 4 du § 2.7 tient |
| Opérateurs de yields complexes | **fait au lot 2** | deux blocs `*YC` à valeurs calculables à la main : `yieldoperators`, mélangé à des équations numériques (§ 2.7), et `yieldoperatorsalone`, sans équation numérique, où les mêmes opérateurs sont justes |
| Opérabilité, transitions, vieillissement (lacune 4) | **fait au lot 3** | racine pour les 5 actions et le `_DEATH` ; `transitions` pour les proportions 60/40, l'`_AGE` et le `_LOCK` de cible, et deux sources conditionnées par `@AGE` et `@YLD` |
| Fonctions de yield qu'aucun scénario n'utilise | **fait au lot 3** sauf `_ENDPOINT` et `_DELTA` (sémantique à clarifier) | `yieldoperatorsalone` (`_SHIFT`, `_MAX`, `_MIN`, `_YTP`, `_DISTANCE`, `_MAI`, `_CAI`) ; `timeyields` pour un yield de temps et `_DISCOUNTFACTOR` (§ 2.7) |
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
- **Nouveaux scénarios** : `yieldoperators` et les 7 `INVALID_*` (§ 4.4).
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

### Lot 3 : objets du cœur (2026-09-18)

**Statut** : livré le 2026-09-18, compilé par Gabriel le même jour, **validé** : 213 tests base
inscrits, 198 verts, 15 désactivés, 12,9 s. Le worktree a maintenant son `BFECtests.csv` :
303 tests inscrits en tout, dont 91 exclus par `-E "T:/"`.

- **Trois requêtes de plus dans `testScenarioReading`** :
  - `OPERATE|<action>|<développement>` : masque, âge, verrou et proportion de chaque chemin
    rendu par `FMTDevelopment::operate` ;
  - `GROW|<développement>` : le développement vieilli ;
  - un développement s'écrit `<masque>|<âge>[|<verrou>[|<période>]]`, ce qui ajoute le verrou à
    `OPERABLE` et la période à `YIELD` ;
  - une valeur attendue écrite `FMTexc(<code>)` exige que la requête lève ce code.
- **30 lignes base** :
  - 14 sur la racine : transitions des 5 actions et du `_DEATH`, verrou de la source conservé,
    garde-fou `FMTexc(79)`, vieillissement avec et sans verrou, opérabilité verrouillée et
    `_LOCKEXEMPT` ;
  - 7 sur le nouveau scénario `transitions` ;
  - 5 sur `yieldoperatorsalone` (`_SHIFT`, `_MAX`, `_MIN`, `_YTP`, `_DISTANCE`) ;
  - 4 sur le nouveau scénario `timeyields` (yield de temps lu par période, valeur tenue après
    la dernière période).
- **3 lignes désactivées** : `_DISCOUNTFACTOR` (§ 2.7, défaut 3).
- **Valeurs** calculées à la main depuis `TWD_land.trn`, `.act`, `.lif` et la table d'âge de la
  racine.
- **Écarts au plan** : `_ENDPOINT` et `_DELTA` ne sont pas couverts, leur sémantique restant à
  clarifier ; `_DISTANCE` est testé avec des bornes qui évitent son ambiguïté d'âge (§ 7).

Vérifications (hors build, 2026-09-18) :

- **Compilation** du test modifié avec `cl` et les options du `.vcxproj`, lié au `FMTlib.lib`
  du build de `new_test`.
- **Rejeu des CSV** découpés comme `Examples/C++/CMakeLists.txt` : 209 noms de tests, aucun
  doublon ; les 57 lignes rejouées (lots 2 et 3) se comportent comme attendu.
- **Vu rouge** des 30 lignes base : valeur faussée sans recompiler, aucune ne survit.
- **Syntaxe des sources de transition** : une sonde montre que `@AGE(10.._MAXAGE)` et
  `@YLD(volumetotal,100.._MAXAGE)` sont bien lues, alors que `_AGE >= 10`, `@AGE(12)`,
  `@AGE(10..)` et `@YLD(y,100..)` sont ignorées sans message (§ 7).
- **Encodages** : nouveaux fichiers en ASCII et CRLF ; aucun U+FFFD introduit.

Validation (après le build de Gabriel, 2026-09-18) :

- **Suite base** (`-E "T:/" -j 8`) : 212 tests, 198 verts, 14 « Not Run (Disabled) », 11,3 s
  réelles. Les 22 lignes `OPERATE`, `GROW`, `transitions` et `timeyields` coûtent 1,9 s en
  tout. Après l'ajout du complément ci-dessous et une reconfiguration : 213 tests,
  198 verts, 15 désactivés, 12,9 s réelles et 99,7 s cumulées (§ 8).
- **Vu rouge sur le vrai build** : `OPERATE` avec un âge faux, `GROW` avec un verrou faux,
  `OPERABLE` verrouillé inversé et la forme `FMTexc(41)` au lieu de `FMTexc(79)` rendent 1.
- **Les lignes désactivées**, lancées à la main, échouent toutes avec les valeurs du § 2.7,
  dont les trois du `_DISCOUNTFACTOR` (0,000128600823 et 1,65e-08).
- **Fixture** verte et `git status` inchangé sous `Examples/Models`.

Complément (2026-09-18, après validation) : Gabriel tranche que la condition de source ignorée
est un défaut. Le scénario `INVALID_sourcecondition` et sa ligne désactivée de
`testReadingErrors` (`FMTexc(31)`) sont ajoutés ; une reconfiguration suffit, sans compilation.
La ligne échoue aujourd'hui, la lecture ne levant rien, et une requête `OPERATE` montre au
passage qu'un développement de 5 ans passe par une source qui demande 10 ans.

### Lot 4 : construction et optimisation (2026-09-18)

**Statut** : livré le 2026-09-18, compilé par Gabriel le même jour, **validé** : 226 tests base
inscrits, 211 verts, 15 désactivés, 22,0 s réelles et 169,2 s cumulées.

- **Nouvel exécutable `testScenarioPlanning`**, piloté par le CSV comme ceux du lot 2, avec
  deux requêtes :
  - `STATS|<longueur>` : `<sommets>|<arcs>|<lignes>|<colonnes>` de la matrice une fois le
    scénario optimisé (`FMTSrModel::getStats`). Valeur mesurée une fois, identique en MOSEK
    et en CLP et d'un passage à l'autre ;
  - `REPLAY|<longueur>` : optimise, puis rejoue la cédule optimale dans un modèle neuf avec
    `doPlanning(false, cédules)` et compare tous les outputs de toutes les périodes. Rien à
    maintenir : le modèle est sa propre référence. Un deuxième argument nomme le scénario qui
    reçoit la cédule, ce qui sert au vu rouge.
  - Il prend MOSEK si FMT est compilé avec, CLP sinon (règle 6), et rend 77 sans OSI.
- **13 lignes base** : 3 `STATS` (`LP`, `division`, `equation`), 4 `REPLAY` (`LP`,
  `Globalfire`, `NOCHOICE`, `equation`) et 6 lignes `presolvetest` (`LP`, `division`,
  `equation`, `COS`, `Shift`, `TSLA`), qui ne demandent aucun code : le test métamorphique du
  présolve ne passait que sur `NOT_MASK`.
- **Écarts au plan** : les statistiques restent globales, pas encore période par période ;
  les lignes `REPLAY` de `division` et `actionseries` sont écartées faute d'avoir pu les voir
  rouges (leurs outputs ne bougent pas quand la cédule vient d'un autre scénario).

Vérifications (hors build, 2026-09-18) :

- **Compilation** du nouveau test avec `cl`, en MOSEK et en CLP.
- **Rejeu des CSV** découpés comme `Examples/C++/CMakeLists.txt` : 223 noms, aucun doublon ;
  les 14 lignes de `testScenarioPlanning` et `presolvetest` passent, entre 0,13 et 0,24 s.
- **Vu rouge** : valeur faussée pour les 3 lignes `STATS` ; pour les 4 lignes `REPLAY`, la
  cédule rejouée dans un autre scénario (`LP` dans `actionseries`, `Globalfire` dans
  `ACT_DEATH`, `NOCHOICE` dans `Globalfire`, `equation` dans `division`).
- **Mêmes résultats en MOSEK et en CLP** pour les statistiques et les rejeux.

Validation (après le build de Gabriel, 2026-09-18) :

- **Suite base** : 226 tests, 211 verts, 15 désactivés. Les 14 lignes du lot 4 coûtent 3,7 s
  cumulées, 0,47 s au plus ; la suite passe de 12,9 s à 22,0 s réelles, l'ordonnancement de
  `-j 8` amplifiant le coût réel des lignes qui optimisent deux fois.
- **Vu rouge sur le vrai build** : `STATS` avec une colonne faussée, `REPLAY` de `LP` rejoué
  dans `actionseries` et de `NOCHOICE` dans `Globalfire` rendent 1 ; les mêmes lignes non
  faussées rendent 0.
- **Fixture** verte et `git status` inchangé sous `Examples/Models`.

## 6. Prochain lot

### État au 2026-09-25

- Lots 0 à 4 livrés, validés et commités ; rien ne bloque. Les quatre défauts du § 2.7 ont leur
  issue (#346, #347, #357, #358), et la suite base est verte : 226 tests, 211 verts,
  15 désactivés.
- Les lacunes de la chaîne sont listées ci-dessous et au § 3 ; elles ne sont suivies que dans ce
  fichier, pas dans des issues GitHub.
- **Décision du 2026-09-25 : ce chantier va jusqu'à la fermeture de l'issue #350** (« Add
  comprehensive unit tests for the Core namespace », jalon FMT2.0). Il absorbe donc le volet des
  tests unitaires, en gardant son ordre par criticité. Le tableau du § 0 dit quel lot répond à
  quelle demande de l'issue ; la feuille de route ci-dessous va du lot 5 au lot 9, et les
  conditions de fermeture sont à la fin.

### Décisions encore ouvertes

Prises le 2026-09-25 : GoogleTest pour la seule cible unitaire, couverture et inventaire par
classe dans la documentation, ménage du permanent remis à plus tard (§ 0). Restent :

Tout ce qui engageait la forme du travail est tranché (§ 1). Restent :

| Décision | Quand | Remarque |
|---|---|---|
| Intégration continue | lot 9 | sans CI, la portabilité Linux et Clang de #350 ne peut être que manuelle : soit on en ajoute une, soit on écarte la demande au moment de fermer l'issue |
| Suivi des lots dans GitHub | quand le lot 6 démarre | un commentaire par lot sous #350, qui demande des changements focalisés |
| Langue et contenu d'`AGENTS.md` | à la demande | il est en anglais et sa section « Language » ne dit rien des issues ni des demandes de tirage |

### Feuille de route jusqu'à la fermeture de #350

**L'ordre reste celui du risque, pas celui de la couverture.** On finit d'abord la chaîne, parce
qu'une étape entière sans test de valeur est plus urgente qu'une classe déjà exercée
indirectement. On attaque ensuite les classes où des défauts ont **déjà** été trouvés, puis
celles que tout modèle traverse, puis le reste. La mesure de couverture arrive en dernier : elle
sert à trouver les trous qui restent, pas à décider par quoi commencer. C'est aussi ce que dit
#350 : « Coverage should be used to identify missing tests, not as the sole quality objective. »

Chaque lot vaut une demande de tirage, comme #350 le souhaite : des changements focalisés plutôt
qu'un gros lot unique.


#### D'abord : corriger les quatre défauts du § 2.7 — session séparée

Décision du 2026-09-25 : les défauts #346, #347, #357 et #358 sont corrigés **avant la suite du
chantier**, dans une session distincte de celle qui écrit les tests. Ce chantier ne touche pas à
FMTlib.

Chaque issue porte en commentaire la préparation de sa correction : où corriger, ce qu'il faut
décider en même temps, le risque sur les modèles existants et les lignes désactivées qui doivent
passer au vert. Deux points en sont ressortis :

- la correction de #347 répare le chemin des **thèmes indexés**, que seul le scénario `AVERAGEP`
  utilise dans TWD_land et qu'aucun test ne lit : une ligne de test sur ce scénario devrait
  précéder la correction, sinon elle se fait à l'aveugle sur ce cas ;
- la correction de #358 fera **refuser** des modèles qui se lisent aujourd'hui en silence : une
  lecture à blanc des modèles de production s'impose avant de diffuser.

Quand les corrections seront faites, la session de tests reprend ainsi :

1. relancer la suite : les 15 lignes désactivées doivent passer au vert ;
2. les déplacer de `knownbugs.csv` vers `basetests.csv`, en le justifiant au journal (règle 9) ;
3. revérifier les valeurs attendues des tests qui dépendaient des yields touchés (§ 2.7) ;
4. reprendre la feuille de route au lot 5.

#### Lot 5 : écriture et relecture (lacune 8) — effort `high`

- **Pourquoi.** C'est la dernière étape de la chaîne du § 0 sans aucun test de valeur. FMT écrit
  des modèles que d'autres relisent : sauvegarde depuis l'interface, modèle présolvé écrit pour
  inspection, projet transformé par une tâche. Les deux défauts d'écriture du § 7 n'ont été vus
  qu'en écrivant un modèle à la main au lot 2, par hasard.
- **Comment.** L'aller-retour est métamorphique, donc sans valeur de référence à maintenir :
  écrire un modèle lu dans `build/release/tests/`, le relire, le réécrire, et comparer les deux
  écritures fichier par fichier. `testJointScenarioReading` fait déjà cette comparaison ligne à
  ligne, son code est réutilisable tel quel. Même chose pour une cédule : écrire, relire,
  comparer.
- **Où.** Un exécutable de plus dans `Examples/C++/`, sur le patron des tests génériques, plus
  des vérifications ajoutées à `testreadwriteproject`, `Simpleplanning`, `createmodel` et
  `nonspatialupdate`, qui tournent aujourd'hui sans rien vérifier (§ 2.3), et la vérification
  morte de `testaddtomodel` à réparer (§ 2.4).
- **Attention.** Les défauts 1 et 2 du § 2.7 font échouer l'aller-retour dès qu'un scénario
  utilise `_SUM` ou un objectif `_PENALTY` : ce sont des lignes désactivées dans
  `knownbugs.csv`, pas un contournement à écrire dans le test.

#### Lot 6 : socle unitaire de Core (jalon 1 de #350) — effort `high`

- **Pourquoi.** Sans cible ni inventaire, les lots suivants n'ont nulle part où aller. C'est
  aussi ce qui débloque la fermeture de #350 : sa première demande est un socle, pas des tests.
- **Quoi.**
  1. **Ajouter GoogleTest** (décision prise, § 0) : dépendance de la seule cible des tests
     unitaires, déclarée dans `vcpkg.json` — `AGENTS.md` interdit d'y toucher hors d'une tâche
     de dépendances, celle-ci en est une. Un build sans GoogleTest doit continuer de compiler
     FMT et de lancer les tests de chaîne.
  2. **`Tests/Core/CMakeLists.txt` et la cible `FMTCoreTests`**, inscrite avec
     `LABELS "unit;core"` pour que `ctest -L core` fonctionne, et respectant
     `-DWITHOUT_TESTING=ON`.
  3. **`Documentation/Testing/CoreTestCoverage.md`** : l'inventaire par classe que demande #350,
     avec ses colonnes. La colonne « Priority » se remplit depuis le § 3 de ce fichier plutôt que
     d'être réinventée. L'inventaire se génère depuis les en-têtes, pour qu'aucune classe ne
     manque.
  4. **Trois tests témoins**, ceux qui auraient attrapé nos défauts sans enquête :
     `FMTOperator::precedence`, `FMTBounds::in` aux bornes, `FMTSpec::allowWithoutYield`.
  5. **Trancher l'en-tête de licence** de ce nouveau dossier : la bibliothèque en porte un, les
     62 exemples de `Examples/C++/` et les 15 tests du wrapper n'en portent aucun.
- **Où ça s'insère.** `Tests/` est un dossier neuf, à côté de `Examples/C++/`. Les deux suites
  cohabitent dans le même ctest ; l'étiquette `core` permet de ne lancer que l'unitaire.

#### Lot 7 : unitaires des classes déjà suspectes (jalons 2 à 4 de #350) — effort `high`

- **Pourquoi cet ordre plutôt que celui de #350.** #350 ordonne par nature (types de valeur,
  puis expressions, puis rendements). On ordonne par risque constaté : les classes où le § 2.7 a
  déjà trouvé un défaut passent devant.
- **Ordre proposé.**
  1. `FMTOperator`, `FMTExpression`, `FMTFunctionCall` — défaut #346, et c'est le moteur de toute
     équation de rendement ;
  2. `FMTData` et les gestionnaires de rendements (`FMTAgeYieldHandler`,
     `FMTComplexYieldHandler`, `FMTTimeYieldHandler`) — défauts #347 et #357 ;
  3. `FMTSpec` et `FMTBounds` — défaut #358, et c'est ce qui décide de toute opérabilité ;
  4. `FMTMask`, puis `FMTTheme`, puis `FMTDevelopment` — traversés par tout modèle.
- **Quoi pour chacune.** Construction, copie, comparaison, hachage quand il existe, bornes,
  entrées invalides et exceptions attendues, comme le détaille #350.

#### Lot 8 : unitaires du reste de Core (jalons 5 à 7 de #350) — effort `high`

`FMTAction`, `FMTTransition`, `FMTSchedule`, `FMTEvent`, `FMTOutput` et ses nœuds,
`FMTConstraint`, `FMTGraphStats` et les types spatiaux. Ces classes sont déjà exercées par les
tests de chaîne des lots 3 et 4 : le travail consiste à descendre au niveau de l'objet construit
dans le test, là où les copies, les comparaisons et les cas limites deviennent atteignables.

#### Lot 9 : exigences transverses de #350 — effort `xhigh`

- **Sérialisation** : aller-retour sur les types sérialisables, et compatibilité avec des
  archives existantes si on en garde.
- **Allocations** : vérifier qu'aucune allocation n'a lieu après préparation sur les chemins
  chauds que nomme #350 (rendements complexes, comparaison de masques, évaluation d'expression,
  traversée de graphe).
- **Concurrence** : seulement sur ce qui est documenté comme utilisable en parallèle ; ce qui ne
  l'est pas se documente au lieu d'être testé.
- **Portabilité** : le dépôt n'a pas d'intégration continue visible. Il faudra soit en ajouter
  une, soit documenter une exécution Linux et Clang faite à la main, sans quoi cette demande de
  #350 reste ouverte.
- **Couverture** : OpenCppCoverage sous MSVC, gcov et lcov ailleurs. Elle arrive ici, en dernier,
  et sert à lister les fichiers et les méthodes publiques encore sans test. Mesure informative à
  la fin d'un lot ; le rapport n'est consigné dans `Documentation/Testing/` qu'au moment des
  versions (décision du 2026-09-25).

#### Compléments des lacunes 6 et 7 — effort `medium` à `high`

Les statistiques par période tiennent dans une requête de plus, d'où `medium` ; `setSolution` demande de lire l'API du rejeu, d'où `high`.

- **Statistiques période par période.** Aujourd'hui `STATS` fige la matrice entière ; les mêmes
  nombres par période diraient *où* la structure a changé plutôt que seulement *qu'elle* a
  changé. `FMTSrModel::buildPeriod` rend déjà un `FMTGraphStats` par période : il suffirait
  d'une requête `STATS|<longueur>|<période>`.
- **`setSolution` période par période.** `REPLAY` passe par `doPlanning(false, cédules)`, qui
  rejoue tout d'un coup. `setSolution(période, cédule, tolérance)` et `boundSolution` sont ce
  qu'utilisent la replanification et `FMTsetsolution` ; ils méritent leur propre test, avec le
  même patron métamorphique.

#### Lots suivants : niveaux 2 et 3 — effort `high`

Le § 3 les détaille. Le niveau 2 (tâches, SES, cartes, NSS, Core du wrapper) demande surtout des
vérifications dans des tests qui tournent déjà sans rien vérifier. Le niveau 3 (recuit simulé,
ordonnanceur d'aires d'opération, transformations d'actions, modèles ONNX) demande d'abord des
données : le § 4 dit lesquelles sont montables à partir de la carte existante et lesquelles
exigent une carte synthétique.

#### Proposition sans retour : modèles BFEC — effort `xhigh`

Effort `xhigh` : c'est une enquête, pas une tâche balisée. Relever, dans les modèles du ministère, les blocs `*YC` qui mélangent une équation numérique et
un autre opérateur, pour mesurer l'effet réel du défaut 2 du § 2.7. Ce relevé se fait en lecture
seule, avec la sonde du lot 2 ; il dirait si des modèles de production calculent aujourd'hui des
rendements faux.

#### Fermer #350

L'issue peut être fermée quand, ensemble :

- `CoreTestCoverage.md` ne montre plus de composant prioritaire sans test ;
- `ctest -L core` tourne, et la suite de chaîne reste verte ;
- les exigences transverses du lot 9 sont satisfaites ou explicitement écartées dans l'issue, en
  disant pourquoi ;
- les quatre défauts du § 2.7 sont corrigés ou portés par un test unitaire en plus de leur ligne
  désactivée ;
- ce fichier et `CoreTestCoverage.md` disent chacun ce qu'ils couvrent et pointent l'un vers
  l'autre.

Le jour de la fermeture, écrire au § 0 ce qui l'a fermée, et ce que ce chantier reprend ou
abandonne.

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
- **Condition d'une source de transition** : la section des transitions n'accepte que
  `@AGE(min..max)` et `@YLD(yield,min..max)`, avec `_MAXAGE` pour l'infini — les formes que FMT
  écrit lui-même. `_AGE >= 10` (la forme de la section des actions), `@AGE(12)`, `@AGE(10..)`
  et `@YLD(yield,100..)` sont avalées sans un mot : la source s'applique alors à tous les
  développements de son masque, et un yield inexistant n'est même pas signalé. Vérifier une
  condition de source en écrivant le modèle relu, ou par une requête `OPERATE` (lot 3).
- **`_DISTANCE` compte à partir de l'âge 1**, alors que son commentaire dit 0 :
  `_DISTANCE(volumetotal,0,100)` rend 4 et non 5 sur la racine. Choisir des bornes que l'âge 0
  n'atteint pas, ou trancher la sémantique avant d'en faire une valeur attendue.
- **Rejeu d'une cédule** : `doPlanning(false, cédules)` reproduit les outputs calculés à
  partir de la cédule, mais pas les outputs `_LEVEL`, qui sont des variables du modèle : sur
  `ndywlevel`, `SUPRECLEVEL` vaut 4,7e-231 au rejeu contre 362,95 à l'optimisation. Le rejeu
  demande aussi `FORCE_PARTIAL_BUILD` et une `TOLERANCE`, comme dans `FMTsetsolution`.
- **Outputs sans valeur finie** : `DIVIDEZERO` de la racine (`yieldzero / yieldzero`) et
  `OSUPRATIOBASE` de `division` rendent « pas un nombre », et `TEST3` de `equation` rend
  l'infini. Une comparaison numérique doit les traiter à part, sans quoi elle échoue même
  quand les deux côtés sont identiques.
- **`operate` qui ne change rien lève `FMTexc(79)`** (`FMTFork::_getPath`) : c'est le garde-fou
  contre une boucle dans le graphe. Une cible `? ? ?` sans changement d'âge ni de verrou le
  déclenche, par exemple `aCaribou` sur un développement déjà verrouillé au niveau de la cible.
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

À refaire à la fin de chaque lot, après le build de Gabriel. Les nombres se lisent dans la
sortie console du passage de ctest : le total après « tests passed », le temps réel après
« Total Test time », et la somme des temps par test pour la durée cumulée. Ne pas les chercher
dans `LastTest.log`, que le moindre appel à ctest réécrit (§ 7).


| Date | Tests base inscrits | Durée cumulée | Exécutables base | Sans vérification | Vérifications mortes | Lignes CSV ignorées | Désactivés | Exécutables BFEC seulement |
|---|---|---|---|---|---|---|---|---|
| 2026-09-16 (lot 0) | 80 (79 lignes + `UnitTestFMTexcelcache`) | 51 s cumulées (passage complet avec BFEC) | 48 | 17 | 3 | 2 | 0 | 25 |
| 2026-09-17 (lot 1) | 83 (79 lignes, 2 fixtures, `teststdconstraints` désactivé, `UnitTestFMTexcelcache`) | 9,3 s réelles ; 65,7 s cumulées | 48 | 17 | 3 | 0 | 1 | 25 |
| 2026-09-17 (lot 2) | 167 (156 lignes, 8 désactivées, 2 fixtures, `UnitTestFMTexcelcache`) | 13,8 s réelles ; 104,5 s cumulées | 51 | 17 | 3 | 0 | 8 | 25 |
| 2026-09-18 (complément lot 2) | 179 (165 lignes, 11 désactivées, 2 fixtures, `UnitTestFMTexcelcache`) | 13,7 s réelles ; 100,3 s cumulées | 51 | 17 | 3 | 0 | 11 | 25 |
| 2026-09-18 (lot 3) | 213 (195 lignes, 15 désactivées, 2 fixtures, `UnitTestFMTexcelcache`) | 12,9 s réelles ; 99,7 s cumulées | 51 | 17 | 3 | 0 | 15 | 25 |
| 2026-09-18 (lot 4) | 226 (208 lignes, 15 désactivées, 2 fixtures, `UnitTestFMTexcelcache`) | 22,0 s réelles ; 169,2 s cumulées | 52 | 17 | 3 | 0 | 15 | 25 |

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
