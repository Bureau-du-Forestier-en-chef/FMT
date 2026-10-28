# État des tests FMT sans `T:\`

Depuis la phase 6 de la réorganisation (section 9, 2026-10-15), ce fichier est `Tests/ETAT.md` ; il
était `Examples/C++/tests/ETAT.md`, dont `git log --follow Tests/ETAT.md` garde l'historique.
Depuis la phase 7 (2026-10-16), chaque exécutable de test porte le nom de l'action qu'il exerce,
verbe en tête (`ReadScenario`, `SetSolution`) : `Tests/Support/RenamedTests.csv` donne l'ancien
nom de chacun. Les sections vivantes emploient les
nouveaux noms ; les sections datées (journal de la section 5, état de la section 6.1, mesures de la
section 8, phases de la section 9.5) gardent ceux de leur époque.

> Fichier de suivi du chantier « plus de tests exécutables sans les données privées ». Il est
> mis à jour **à la fin de chaque lot**. Une session qui démarre à froid doit pouvoir savoir
> d'ici quoi faire, sans relire l'historique.
>
> Le dépôt est public : ce fichier ne cite ni chemin `T:\` ni modèle privé. Les tests BFEC
> y sont désignés par le nom de leur exécutable.
>
> **Ordre de lecture pour démarrer à froid** : la section 0 situe la chaîne de FMT et dit comment
> on ajoute un test ; la section 1 donne les règles et le patron ; la section 6 dit quoi faire
> ensuite ; la section 7 se lit avant de toucher au code, elle tient les pièges déjà payés. Les
> sections 2 à 5 sont l'inventaire, les lacunes, les scénarios et le journal, à consulter au
> besoin. **La réorganisation en trois niveaux (section 9), passée avant la suite de la feuille de
> route le 2026-10-08, est terminée depuis la phase 9b (2026-10-19)** : la feuille de route de la
> section 6.3 reprend, et la section 9.7 dit ce qui reste ouvert.
>
> Les sections et sous-sections sont numérotées : « section 2.7 » renvoie à la sous-section 2.7
> de ce fichier. Les lots (lot 1, lot 2...) sont les étapes du chantier, sans lien avec ces
> numéros : le lot 2, par exemple, est raconté en section 5.3.

## 0. Vue d'ensemble

### 0.1 Ce que FMT fait d'un modèle, et où les tests se branchent

Un modèle Woodstock est un dossier de sections (`.lan` le paysage, `.are` l'inventaire, `.yld`
les rendements, `.act` les actions, `.trn` les transitions, `.lif` la durée de vie, `.out` les
outputs, `.opt` l'optimisation, `.seq` les cédules), plus des scénarios qui en remplacent une
partie : `Scenarios/<nom>/<projet>._yld` remplace le `.yld` de la racine, et ce qu'un scénario
ne redéfinit pas, il l'hérite. FMT traverse toujours ce modèle dans le même ordre, et chaque
étape a ses tests :

1. **Lecture.** `FMTModelParser::readproject` appelle un parseur par section et rend un
   `FMTModel` par scénario demandé : thèmes, agrégats, développements initiaux, yields,
   actions, transitions, lifespan, outputs, contraintes. Une erreur ici fausse tout le reste
   en silence. → `ReadScenario`, `ReadScenariosTogether`, `ReadInvalidScenario`
   (lot 2), lacunes 1 à 3 de la section 3.1.
2. **Objets du cœur.** Un développement est un masque (`FMTMask`) avec un âge, un verrou et
   une période. Il vieillit (`grow`), devient opérable (`operable`) et se transforme
   (`operate`, qui applique la transition et rend des chemins avec leurs proportions). Les
   rendements répondent à une requête (`FMTYields::get`). → requêtes `OPERABLE`, `OPERATE`,
   `GROW` et `YIELD` de `ReadScenario` (lot 3), lacune 4.
3. **Graphe et matrice.** `FMTSrModel::buildPeriod` construit, période par période, un graphe
   dont les sommets sont des développements et les arcs des actions ou de la croissance ;
   `FMTLpModel` le traduit en matrice, dont les lignes sont des contraintes et les colonnes des
   variables. → requête `STATS` de `PlanScenario` (lot 4), lacune 6.
4. **Optimisation.** L'objectif et les contraintes du `.opt` sont résolus par MOSEK ou CLP, le
   présolve pouvant réduire le modèle avant. → les 16 objectifs de `DoPlanning` et compagnie,
   `Presolve` pour le présolve, lacune 5.
5. **Résultats.** La solution donne une cédule par période (`getSolution`) et des valeurs
   d'outputs (`getOutput`). Une cédule peut être rejouée dans un modèle neuf
   (`doPlanning(false, cédules)`). → requête `REPLAY` de `PlanScenario` (lot 4),
   lacune 7.
6. **Écriture.** `FMTModelParser::write` réécrit un modèle lu, `writeToProject` un projet
   complet, `FMTScheduleParser` les cédules. → allers-retours de `IntegrationParser`, premier
   exécutable du niveau intégration, et `WriteAndReadProject`, `SolveAndWriteSchedules`,
   `UpdateNonSpatialModel` (lot 5), lacune 8.

Un test base ne fait jamais rien d'autre que ceci : partir de TWD_land, s'arrêter à une de ces
étapes, et comparer ce qu'il y trouve à une valeur écrite dans le CSV.

### 0.2 Où vivent les choses

| Quoi | Où |
|---|---|
| Sources des exécutables de test | `Tests/System/Cpp/*.cpp`, `Tests/System/Cpp/FMTWrapperCore/*.cpp` pour le Core et `Tests/System/Cpp/Excel/*.cpp` pour Excel (depuis la phase 6) |
| Tests unitaires (GoogleTest) | `Tests/Unit/Core/<Classe>.cpp`, une suite par classe, dans l'exécutable `UnitCore` (depuis la phase 8) ; comment en ajouter : `AGENTS.md`, « Adding a unit or integration test » |
| Tests d'intégration (GoogleTest) | `Tests/Integration/Parser/<Parseur>.cpp`, une suite par parseur, dans l'exécutable `IntegrationParser`, étiquettes `integration;parser`, avec la fixture `ExamplesModels` (depuis le lot 5) ; outils communs dans `ParserRoundTrip.h` du même dossier |
| Inventaire par classe de `Core` (#350) | `Documentation/Testing/CoreTestCoverage.md`, en anglais, régénéré par `cmake -P Documentation/Testing/CoreTestCoverage.cmake` depuis les en-têtes, les cas GoogleTest et les colonnes écrites à la main de `CoreTestCoverage.csv` (depuis le 2026-10-20) |
| Lignes qui inscrivent les tests | `Tests/System/Cpp/basetests.csv` (public), `knownbugs.csv` (désactivées), `BFECtests.csv` (local, privé, ignoré par git) |
| En-tête commun des tests | `Tests/Support/TestTools.h` |
| Modèle public et ses scénarios | `Examples/Models/TWD_land`, `Examples/Models/TWD_land/Scenarios/<nom>/` |
| Inscription dans ctest | `Tests/Support/TestRegistration.cmake` (fonctions), appelées par `Tests/System/Cpp/CMakeLists.txt` pour ses CSV et le test Excel, et par `Tests/Unit/Core/CMakeLists.txt` pour `UnitCore` (phase 8) ; fixture `ExamplesModels` dans `Tests/CMakeLists.txt` (depuis la phase 4) |
| Empreinte du modèle entre deux passages | `Tests/Support/TestsDataSnapshot.cmake` |
| Sorties des tests | `build/release/tests/<test>/` |
| Suivi du chantier | ce fichier |

### 0.3 Comment on ajoute un test

1. **Choisir la lacune** en section 3 : on descend la liste par criticité, une étape que tout modèle
   traverse avant une fonction de niche.
2. **Calculer la valeur attendue à la main**, depuis les fichiers du modèle, jamais depuis ce
   que FMT affiche : sinon le test grave le comportement actuel, bogue compris. Quand le calcul
   à la main n'a pas de sens (statistiques d'un graphe), on mesure une fois, on le dit au
   journal, et on vérifie au moins que la valeur ne dépend ni du solveur ni du passage.
3. **Réutiliser un test générique** quand il peut répondre (`ReadScenario`,
   `PlanScenario`, `ReadInvalidScenario`, `ReadScenariosTogether`) : il n'y a alors
   qu'une ligne CSV à écrire, sans compilation. Sinon, écrire un exécutable dans
   `Tests/System/Cpp/`, sur le patron de la section 1.5.
4. **Ajouter la ligne CSV** (section 2.1), faire reconfigurer CMake, et compiler si un source a
   changé.
5. **Voir le test rouge** en faussant la valeur attendue ou l'entrée, puis vert (règle 1).
6. **Noter au journal** (section 5) et refaire les mesures (section 8).

### 0.4 Ce chantier et l'issue #350

L'issue #350, « Add comprehensive unit tests for the Core namespace » (jalon FMT2.0), demande
des tests **unitaires** classe par classe : constructeurs, copies, comparaisons, hachage,
sérialisation, allocations, concurrence, le tout sous un cadre à la GoogleTest dans `Tests/Core/`,
avec un inventaire dans `Documentation/Testing/CoreTestCoverage.md` et une couverture publiée.

Ce chantier-ci travaille à l'autre échelle : il protège **la chaîne** que tout modèle traverse
(section 0.1), depuis un modèle public, par des lignes CSV. Les deux se complètent plutôt qu'ils ne
se recoupent :

- un test unitaire sur `FMTOperator::precedence` aurait attrapé le défaut 1 de la section 2.7 en
  trois lignes, alors qu'il nous a fallu un scénario et une enquête ;
- à l'inverse, le défaut 2 ne se voit d'aucune classe prise isolément : il naît de
  `FMTYieldParser::_cleanUp`, qui reconstruit tout un bloc à la lecture. Seule une lecture de
  modèle le révèle.

Ce que ce chantier apporte déjà à #350 : l'inventaire de ce qui est vérifié (section 2), le
classement par criticité, qui est la colonne « Priority » de leur tableau (section 3),
l'infrastructure ctest (alerte de ligne sans cible, empreinte du modèle, code 77, lignes
désactivées, section 2.1), et quatre défauts caractérisés avec leur test désactivé (section 2.7), ce
qu'ils appellent des tests de caractérisation avant refonte. Les lots 2 à 4 couvrent, au niveau du
comportement, la matière de leurs jalons 2 à 7 : `FMTMask`, `FMTTheme`, `FMTDevelopment`, `FMTSpec`,
`FMTData`, `FMTExpression`, `FMTOperator`, les gestionnaires de rendements, `FMTAction`,
`FMTTransition`, `FMTSchedule`, `FMTOutput`, `FMTConstraint` et `FMTGraphStats`.

Ce que #350 demande et que ce chantier ne fait pas : constructeurs et copies, opérateurs de
comparaison, hachage, aller-retour de sérialisation, absence d'allocation, concurrence,
portabilité Linux et Clang, couverture publiée.

**Ce chantier prend en charge la fermeture de #350** (décision du 2026-09-25). Il garde sa façon
de travailler — l'ordre par criticité, les lots, le vu rouge — et absorbe le volet unitaire que
l'issue demande. La feuille de route complète est en section 6.3 ; voici ce que #350 exige, où on en
est, et quel lot s'en charge :

| Demande de #350 | État | Lot |
|---|---|---|
| Inventaire des composants et de leur couverture | fait : par étape de la chaîne (section 2.2), et par classe dans `Documentation/Testing/CoreTestCoverage.md` (2026-10-20, section 6.3), qui se régénère depuis les en-têtes | inventaire du 2026-10-20 |
| Cible et infrastructure de test dans CMake et ctest | fait : tests de chaîne (section 2.1), puis cible `UnitCore` et étiquettes `unit;core` (phase 8, section 9.5) | 6, phase 8 |
| Cadre de test unitaire choisi et documenté | fait : GoogleTest (décision du 2026-09-25), dans `vcpkg.json` et décrit dans `AGENTS.md` depuis la phase 8 | 6, phase 8 |
| Tests unitaires des classes déjà suspectes (défauts de la section 2.7) | non fait ; leur comportement n'est touché qu'indirectement | 7 |
| Tests unitaires du reste de Core (jalons 5 à 7 de #350) | non fait, même remarque | 8 |
| Constructeurs, copies, comparaisons, hachage | non fait | 7 et 8 |
| Aller-retour de sérialisation | non fait | 9 |
| Absence d'allocation, concurrence | non fait | 9 |
| Portabilité Linux et Clang | non fait ; la suite ne tourne que sous Windows, et le dépôt n'a pas d'intégration continue visible | 9 |
| Couverture publiée | non fait ; informative après un lot, consignée dans la documentation aux versions | 9 |
| Tests de caractérisation avant refonte | fait pour quatre défauts (section 2.7), puis cinq défauts d'écriture au lot 5 (section 2.7, défauts 6 à 10, #379 à #383), à étendre | continu |

À chaque lot, noter au journal ce qu'il apporte à #350. Les conditions de fermeture sont en
section 6.3.10 ; le jour venu, écrire ici ce qui l'a fermée et ce que ce chantier reprend ou
abandonne.

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

### 0.5 Quel effort donner à l'agent

Les niveaux sont `low`, `medium`, `high`, `xhigh` et `max` (de « faible » à « ultra » dans
l'interface). Ce qui suit est tiré de ce que les lots 1 à 4 ont réellement demandé.

| Travail | Effort | Pourquoi |
|---|---|---|
| Lancer la suite après un build, rapporter, mettre à jour les mesures | `low` | Commandes connues, aucune décision. |
| Ajouter des lignes CSV à un test générique, avec des valeurs calculables à la main | `medium` | Il faut lire les fichiers du modèle et calculer juste, mais le chemin est balisé. |
| Écrire ou étendre un exécutable de test | `high` | Il faut lire l'API de FMT, choisir ce qui est vérifiable, et se méfier des classes non exportées et des macros de `windows.h` (section 7). |
| Un lot entier de la section 6 | `high` | C'est le rythme des lots 2 à 4 : quelques jours de travail condensés, avec ses vérifications. |
| Enquêter sur un résultat suspect, démontrer un défaut de FMTlib | `xhigh` ou `max` | Les quatre défauts de la section 2.7 ont demandé de lire le code jusqu'à la cause, de monter des scénarios A/B et de remonter l'historique git. À `medium`, l'agent aurait écrit « `_SUM` ne garde que la dernière source », ce qui était faux. |
| Mettre à jour ce fichier, rédiger le texte d'une issue | `medium` | Rédaction, sans exploration. |

Deux repères. **Trop bas est dangereux quand une valeur attendue doit être calculée à la main** :
un agent pressé prend la valeur que FMT affiche, et le test grave alors le bogue au lieu de le
révéler. **Trop haut ne casse rien**, cela coûte seulement du temps et des jetons ; en cas de
doute sur une enquête, monter d'un cran.

## 1. Objectif, règles et patron

### 1.1 Objectif

Protéger d'abord **la chaîne que tout modèle traverse** (lecture, objets du cœur, graphe,
optimisation, résultats, écriture) par des tests inscrits dans `basetests.csv`, qui tournent
sur le modèle public `Examples/Models/TWD_land`, sans `T:\`, en une minute environ.

### 1.2 Décisions de Gabriel (2026-09-16)

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
    de chaîne gardent `TestTools.h` (section 0.4).
  - **Fermeture de #350** : ce chantier s'en charge, sans changer son ordre par criticité
    (sections 0.4 et 6.3).
  - **Un test ctest par cas** (`gtest_discover_tests`), pour qu'un échec soit identifiable et
    un cas désactivable seul.
  - **Défaut révélé par un test unitaire** : cas préfixé `DISABLED_`, entrée en section 2.7, issue
    documentée et tâche de correction (règle 3).
  - **En-tête de licence** dans `Tests/`, comme la bibliothèque ; les exemples restent
    l'exception historique.
  - ~~**Emplacement** : `Tests/Core/` pour l'unitaire, `Examples/C++/` inchangé pour la chaîne.~~
    **Remplacée le 2026-10-08** par les trois dossiers `Tests/Unit`, `Tests/Integration` et
    `Tests/System` (section 9).
  - **Couverture** : mesurée à titre informatif à la fin d'un lot, consignée dans la
    documentation au moment des versions.
  - **Corrections de FMTlib** : jamais dans un lot de tests. Un défaut donne une issue
    documentée, corrigée dans une session à part.
- **Décisions du 2026-10-08**, après les commentaires de gcyr sur #350 (détail en section 9.2) :
  trois niveaux de tests dans `Tests/Unit`, `Tests/Integration` et `Tests/System` ; tout
  l'existant est du niveau système et y déménage ; Arrange / Act / Assert et noms Méthode +
  Condition + Comportement ; GoogleTest aussi pour l'intégration ; `cmake --install` ne lance plus
  que la suite publique ; un résumé à la fin de chaque passage ; une session par phase, orchestrée
  depuis une session qui garde les décisions.

### 1.3 Rôles

- **Gabriel** : compile, reconfigure CMake, lance la suite BFEC, commite.
- **Claude** : écrit les tests et ce fichier, fait les contrôles statiques, puis **lance la
  suite base après le build de Gabriel** :

```
& "C:/Program Files/Microsoft Visual Studio/2022/Professional/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/ctest.exe" --preset all-public
```

`ctest.exe` n'est pas dans le `PATH`. Le preset `all-public` (`CMakePresets.json`) lance tout
sauf les tests BFEC, dont le nom garde `T:/`, huit à la fois, avec la sortie des échecs. Une ligne
privée ne reçoit jamais de nom écrit à la main (section 2.1) : son nom garde donc toujours `T:/`. Un niveau
seul : `--preset system`, `system-private` (la campagne BFEC, par Gabriel) ou `performance` ; sans
preset, par étiquette ancrée, par exemple `--test-dir build/release -C Release -L "^system$"`.
Depuis la phase 2, `RunTests.bat <preset>` (dans `tools\tests\` depuis le 2026-10-14, avec
`RunPrivateTests.bat`) lance un preset et finit par le résumé, et
l'installation ne lance plus la suite BFEC : Gabriel la lance par `RunTests.bat system-private`
(les deux décrits dans `AGENTS.md`, section Testing).
Chaque lot s'arrête sur une livraison ; le suivant attend le feu vert de Gabriel.

### 1.4 Règles

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
   plusieurs tests ont des défauts codés en dur vers des chemins privés ou disparus (section 7) ;
   une ligne sans argument teste alors autre chose que ce qu'on croit, ou rien du tout.
5. **Aucune écriture sous `Examples/Models`** : le test copie le projet dans
   `build/release/tests/<test>/` (patron de `WrapperCoreRunPlanning`). *Pourquoi* : deux tests
   qui écrivent au même endroit se marchent dessus, et un modèle modifié en cours de route
   fausse tous les tests suivants. L'empreinte prise avant et après chaque passage (section 2.1) le
   détecte.
6. **Un nouveau test privilégie MOSEK.**
   - Il prend `Models::FMTSolverInterface::MOSEK` quand FMT est compilé avec MOSEK
     (`FMTWITHMOSEK`), `CLP` sinon : patron `#ifdef FMTWITHMOSEK` de `DoPlanning.cpp`,
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
   trace dans un correctif (section 2.5), et `stdconstraints` n'a jamais tourné pendant trois ans
   sans que personne ne le voie.
10. **Criticité d'abord** : l'ordre des lots suit la section 3.

### 1.5 Patron d'un test

Depuis le lot 1, `TestTools.h` (dans `Tests/Support/` depuis la phase 6 ; namespace `Testing`, inclus par
`#include "TestTools.h"` dans tout exécutable créé par `createexecutable`) fournit :

- `Checker` : `check(condition, description)` et `checkNear(valeur, attendue, tolérance,
  description)` affichent une ligne `ok` ou `FAILED` ; `exitCode()` rend 0 ou 1 ;
- `SKIP_RETURN_CODE` (77) et `skip(raison)` ;
- `copyProject(source, destination)` : remplace la destination par une copie du projet ;
- `runTest(corps)` : rend le code du corps, ou 1 si une exception s'échappe, après l'avoir
  affichée avec ses exceptions imbriquées (`printException`) ;
- `visitNested(exception, visiteur)` : parcourt la chaîne d'exceptions imbriquées sans
  `std::terminate` (lot 2, voir section 7).

Depuis le lot 2, trois tests génériques pilotés par le CSV servent de modèles :

- `ReadScenario` : une ligne = une requête sur un scénario lu (`THEMES`, `ATTRIBUTES`,
  `AGGREGATE`, `AREA`, `ACTIONS`, `OPERABLE`, `OPERATE`, `GROW`, `YIELD`, `OUTPUTS`,
  `CONSTRAINT`... ; liste en tête du source) et sa valeur attendue. Un développement s'y écrit
  `<masque>|<âge>[|<verrou>[|<période>]]`, et une valeur attendue écrite `FMTexc(<code>)` exige
  que la requête lève ce code (lot 3) ;
- `ReadScenariosTogether` : une ligne = une liste de scénarios lus ensemble ;
- `ReadInvalidScenario` : une ligne = un scénario invalide et le `FMTexc` attendu.

Ajouter une ligne à l'un d'eux ne demande pas de recompiler, seulement de reconfigurer.

L'en-tête compile sans avertissement avec MSVC (`/W4 /permissive-`) après `windows.h`, dont les
macros (`min`, `max`, `NEAR`, `ERROR`...) arrivent par GDAL : ne pas y introduire ces noms.

- `main(argc, argv)` lit ses entrées ; sans argument, il retombe sur TWD_land.
- Code de retour : 0 réussite, 1 échec, 77 fonctionnalité absente.
- Commentaires et sorties console en anglais ; ce fichier en français.
- Un nouvel exécutable va dans `Tests/System/Cpp/` (ou `Tests/System/Cpp/FMTWrapperCore/` pour le Core) ;
  sa ligne dans `basetests.csv` l'inscrit dans ctest.

### 1.6 Techniques privilégiées

- **Tests métamorphiques** : deux calculs qui doivent concorder, sans valeur de référence à
  maintenir.
  - Présolve activé ou non : même objectif (existe déjà : `Presolve`, un seul scénario).
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

- `Tests/System/Cpp/CMakeLists.txt` passe `Tests/System/Cpp/*.csv` à `registercsvtests`
  (`Tests/Support/TestRegistration.cmake`, depuis la phase 4). Colonnes : cible, trois
  arguments, puis, depuis la phase 9a, `given` et `then`, facultatives, qui ne servent qu'au nom ;
  le `|` sépare des valeurs dans une colonne. Le nom ctest a deux formes (section 9.3) :
  - `System.<cible>.Given<given>When<cible>Then<then>`, écrit à la main, quand la ligne a ses deux
    colonnes, en PascalCase. La configuration s'arrête, en nommant le fichier et la ligne, sur une
    colonne seule, sur un nom hors de `^[A-Z][A-Za-z0-9]*$` et sur un nom déjà donné ;
  - sinon, la cible suivie des arguments 1 et 2 réduits (espaces remplacés par `_`). Les lignes de
    `BFECtests.csv` gardent toujours cette forme, et donc leur `T:/` : leurs colonnes `given` et
    `then` sont ignorées, avec un avertissement. Depuis la phase 9b, toutes les lignes publiques
    sont nommées à la main, et une ligne de `basetests.csv` ou de `knownbugs.csv` sans ces colonnes
    donne un avertissement qui nomme le fichier et la ligne.
- `basetests.csv` est suivi par git ; `BFECtests.csv` est local et ignoré par git (invisible
  pour ripgrep : le lire directement). Une copie restée dans `Examples/C++/tests/`, son dossier
  avant la phase 6, est lue à sa place, avec un avertissement qui dit où la déplacer.
- Une ligne dont la cible n'existe pas n'est pas inscrite. Depuis le lot 1, la configuration
  l'annonce : `CMake Warning ... <fichier>.csv: no target named "<cible>"`. Avant, elle était
  ignorée sans rien dire.
- Chaque test inscrit par un CSV reçoit (lot 1) :
  - `SKIP_RETURN_CODE 77` ;
  - `FIXTURES_REQUIRED ExamplesModels` : ctest lance `ExamplesModelsSnapshot` avant les tests
    et `ExamplesModelsUnchanged` après. Ces deux tests exécutent
    `Tests/Support/TestsDataSnapshot.cmake`, qui hache les 273 fichiers de `Examples/Models` et
    échoue en listant tout fichier ajouté, retiré ou modifié. L'empreinte est écrite dans
    `build/release/tests/ExamplesModels.snapshot` ;
  - `RESOURCE_LOCK <cible>` si la cible figure dans `FMTTESTSsharedoutputs`
    (`RunPlanningTask`, `AggregateActions`, `ScheduleOperatingAreas`,
    `WriteAndReadProject`) : ses lignes écrivent dans les mêmes fichiers et ne tournent
    jamais en même temps ;
  - `DISABLED TRUE` si la ligne vient de `knownbugs.csv` : ctest l'affiche « Disabled » sans
    l'exécuter ;
  - depuis la phase 1, les étiquettes `system` et `cpp`, et depuis la phase 6 celle de son
    composant, `wrappercore` ou `excel`, quand sa cible vient de `Tests/System/Cpp/FMTWrapperCore/`
    ou de `Tests/System/Cpp/Excel/` (propriété de cible `FMTTESTScomponent`, lue par
    `registersystemtest`).
- Répertoire de travail : `build/release/bin/Release`. Entrées :
  `../../../../Examples/Models/...` ; sorties : `../../tests/<test>/`, soit
  `build/release/tests/<test>/`.
- Les arguments sont figés dans `build/release/CTestTestfile.cmake` : **modifier un CSV exige
  de reconfigurer CMake**.
- `UseExcelCache` est inscrit par `Tests/System/Cpp/CMakeLists.txt`, hors CSV, avec `registersystemtest`,
  la fonction qui inscrit chaque ligne.

**Un exemple complet.** La ligne

```
ReadScenario;../../../../Examples/Models/TWD_land/TWD_land.pri|ROOT|AREA|? ? ?;1814.76;;TheRootInventory;SumsTheAreaOfAllDevelopments
```

inscrit le test
`System.ReadScenario.GivenTheRootInventoryWhenReadScenarioThenSumsTheAreaOfAllDevelopments`.
ctest lance `ReadScenario.exe` depuis `build/release/bin/Release` avec deux arguments :
`../../../../Examples/Models/TWD_land/TWD_land.pri|ROOT|AREA|? ? ?`, qui dit quoi lire et quoi
demander, puis `1814.76`, la valeur attendue. La quatrième colonne, vide, disparaît de la
commande, et les deux dernières ne servent qu'au nom. Sans elles, le test s'appellerait
`System.ReadScenario.TWD_land|ROOT|AREA|?_?_?|1814.76`, la cible suivie des deux premiers
arguments réduits.

**Pourquoi ce découpage.** Le CSV n'offre à la commande que quatre colonnes, alors qu'une requête en demande
souvent plus : le `|` sert donc de séparateur à l'intérieur d'une colonne, et chaque test
redécoupe ses arguments lui-même. Les valeurs attendues vivent dans le CSV, et non dans le
code, pour deux raisons : on peut en fausser une sans recompiler, ce qu'exige la règle 1, et
une ligne de plus ne coûte qu'une reconfiguration.

### 2.2 Ce que vérifient les tests base, par étape de la chaîne

« Vérifie » = compare une valeur ; « exécute » = n'échoue que sur une exception.

| Étape | Vérifie une valeur | Exécute sans vérifier |
|---|---|---|
| Lecture d'un scénario | direct : `ReadScenario` (lot 2 : thèmes, attributs, agrégats, aire par masque, actions et drapeaux, outputs et contraintes de ROOT ; noms de thèmes, aire héritée et contraintes de `LP3` ; contraintes de `stdconstraints`), constantes (`ConvertStringsToConstants`, 3 cas), un yield complexe (`WrapperCoreGetYield`), nombre de masques (`WrapperCoreGetAllMasks`), âge maximal (`WrapperCoreGetMaxAge`, borne seulement), gabarits (`ReadTemplates`, au moins un modèle) ; indirect : objectifs de `DoPlanning` (16 scénarios), `PlanWithLandAggregates`, `OptimizeWithLevels` | tous les autres tests |
| Lecture de plusieurs scénarios d'un coup | direct : `ReadScenariosTogether` (lot 2 : 7 combinaisons, 32 scénarios) ; indirect : `OptimizeWithLevels` (9 scénarios, objectifs), `WriteAndReadProject` (lot 5 : 4 scénarios relus d'un projet écrit, objectifs) | `RunPlanningTask`, `ReplanConcurrently`, `ReplanWithModelAdaption`, `LogThroughCallback` |
| Erreurs de lecture | `ReadInvalidScenario` (lot 2 : 7 scénarios invalides, 6 codes `FMTexc`) | -- |
| Objets du cœur (masques, yields, opérabilité, transitions) | `ReadScenario` (lot 2 : opérabilité aux bornes d'âge, de yield et d'agrégat, `_DEATH` tiré du lifespan, interpolation des yields d'âge, opérateurs de yields complexes), `GetNoChoice` (développements sans choix d'un masque), `GetYieldsFromFactor` (monotonie), `ComputeTimeYieldOutputs` (2 outputs de yields temporels), `AddYieldHandlers` (lot 5 : yield ajouté par type de peuplement, à chaque âge de base et interpolé) ; transitions et vieillissement : indirectement par les objectifs | -- |
| Présolve et postsolve | `Presolve` (objectif avec ou sans présolve ; NOT_MASK, plus 6 scénarios au lot 4) ; `DoPlanning` (présolve de 10 itérations) | -- |
| Graphe et matrice | `PlanScenario` `STATS` (lot 4 : sommets, arcs, lignes et colonnes de 3 scénarios) ; indirect (objectifs) | `ResolveAfterConstraintChange` (vérifie seulement la résolution après retrait et remise d'une contrainte) |
| Optimisation | `DoPlanning` (16), `OptimizeWithLevels` (9), `PlanWithLandAggregates` ; `SolveWithEachSolver` (chaque solveur résout, sans valeur) ; `CreateModel` (lot 5 : modèle construit en code optimal, même optimum relu) | -- |
| Résultats et rejeu | `ComputeLevelOutputs` (26 outputs), `ReplayScheduleOutputs` (3 outputs après rejeu d'une cédule), `ForceSolution` (6 valeurs et conservation de la superficie), `SumAndAverageOutputs` (somme et moyenne), `SimulateNonSpatially` (5 lignes sur 7) ; `PlanScenario` `REPLAY` (lot 4 : rejeu de la cédule optimale, tous les outputs de toutes les périodes, 4 scénarios) ; `SolveAndWriteSchedules` (lot 5 : cédule optimale écrite et relue, superficie par action et développement), `UpdateNonSpatialModel` (lot 5 : superficie conservée à la période 3) | `GetModelFromProportions`, `GetVariabilities` |
| Écriture et relecture | `WriteModel` (5 scénarios, objectif après relecture), `WriteAndReadOutputTypes` (nombre d'outputs), `WrapperCoreWriteToProject` (fichiers présents) ; lot 5 : `IntegrationParser` (allers-retours de `write`, `writeToProject` et `FMTScheduleParser`, section 5.7), `WriteAndReadProject` (même optimum relu par scénario), `UpdateNonSpatialModel` (superficie et contraintes décalées relues), `CreateModel` | -- |
| Tâches (planification, replanification) | `SumAndAverageOutputs`, `WrapperCoreRunPlanning` | `RunPlanningTask`, `ReplanConcurrently`, `ReplanWithModelAdaption`, `LogThroughCallback` |
| Spatial (SES, cartes, rasters) | `DoSpatiallyExplicitPlanning` (une vérification valide), `WrapperCoreRasterize`, `GetGCBMTransitions` (issue #369 : attribut final majoritaire de chaque clé GCBM) | `SimulateSpatially`, `MapToForest`, `ConvertVectorFieldToRaster`, `GetCarbonPredictors` |
| Divers | `WriteAndReadGCBMTransitions` (aller-retour), `ListGDALDrivers`, `ClusterOperatingAreas` (une solution existe), `WrapperCoreRunAreaVariability`, `WrapperCoreDescribeEnvironment`, `UseExcelCache` | `SimulateWithYieldModel`, `PlanWithLpYieldModel`, `OptimizeWithLpYieldModel` |

### 2.3 Exécutables base sans vérification (13)

`SimulateSpatially`, `GetCarbonPredictors`, `MapToForest`, `SimulateWithYieldModel`,
`GetModelFromProportions`, `ReplanConcurrently`, `GetVariabilities`, `PlanWithLpYieldModel`,
`OptimizeWithLpYieldModel`, `RunPlanningTask`, `ConvertVectorFieldToRaster`,
`ReplanWithModelAdaption`, `LogThroughCallback` (construit sa tâche sans la lancer).

Le lot 5 (2026-10-19, section 5.7) en a sorti `SolveAndWriteSchedules`, `CreateModel`,
`UpdateNonSpatialModel` et `WriteAndReadProject`, qui vérifient maintenant ce qu'ils écrivent.

**Lignes qui ne vérifient que l'absence d'erreur (phase 9b, 2026-10-19), pour les lots suivants.**
Leur nom le dit, sans promettre de résultat (section 9.2). 16 lignes de `basetests.csv` depuis le
lot 5, qui en a sorti 6 (les lignes `…ThenEndsWithoutError` de `SolveAndWriteSchedules`,
`CreateModel` et `UpdateNonSpatialModel`, les 3 lignes `…ThenRereadsTheWrittenProjectWithoutError`
de `WriteAndReadProject`) :

- `…ThenEndsWithoutError`, 15 lignes : la ligne de chacun des 12 exécutables ci-dessus autres que
  `ReplanConcurrently`, les deux lignes `SimulateNonSpatially` sans valeur attendue (scénarios
  `randomYield` et `randomYieldUnit`, section 7), et la ligne `loggerlifetime` de
  `SolveWithEachSolver`, qui ne voit qu'un plantage (section 9.5, phase 0) ;
- `…ThenWritesAReadableScheduleForTheFirstReplicate`, la ligne de `ReplanConcurrently` : il relit la
  cédule de son premier réplicat, et `FMTScheduleParser::read` lève (`FMTexc` de chemin invalide)
  quand elle manque. L'inventaire du lot 0 ne l'avait pas vu ; rien d'autre n'est vérifié.

Vérification faible, relevée au même moment : la ligne `DoPlanning` du scénario `mai` attend un
objectif 0, et le présolve vide le modèle (aucune variable dans `LastTest.log` du 2026-10-19) ; elle
vérifie surtout que la lecture et la planification d'un `_MAI` ne lèvent pas.

### 2.4 Vérifications mortes

- `DoSpatiallyExplicitPlanning.cpp:94-95` : le contrôle des patchs découpe `stats`
  au lieu de `result`, et `spresults.at(1) != "2"` est toujours vrai quand la première
  condition l'est.
- ~~`AddYieldHandlers.cpp` : le vecteur `totest` n'est jamais rempli ; la boucle qui compare les
  valeurs ne s'exécute pas. Seul `isYld` est vérifié.~~ **Réparée au lot 5** (section 5.7) : la
  comparaison parcourt chaque type de peuplement, et une valeur calculée à la main vient du CSV.
- `Tests/System/Cpp/FMTWrapperCore/WrapperCoreRunSimulatedAnnealing.cpp:157` : l'écart de plus de 5 % ne fait
  qu'afficher un avertissement (hors ctest de toute façon).

### 2.5 Hors ctest, désactivés ou jamais exécutés

- `teststdconstraints` : **supprimé au lot 2**, remplacé par 8 lignes de `ReadScenario`
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
- `WrapperCoreRunSimulatedAnnealing` : pas de jeu de données (repli `TODO`).
- `ConvertThemeSelectionToMask` (Excel) : compilé, jamais inscrit.
- `UnitTestFMTAreaParser` : **supprimé avec sa ligne de `basetests.csv` par `f06f55fd`**
  (correctif d'adjacence, 2026-07-20). C'était un test unitaire de géométries synthétiques,
  sans données externes : `git show f06f55fd^:Examples/C++/UnitTestFMTAreaParser.cpp`.

### 2.6 Exécutables qui n'ont que des tests BFEC (25)

`SetSolution`, `SetSolutionByLp`, `ReadDatabaseSections`, `ConvertPointsToRasters`,
`DoSpatialPlanning`, `SolveTwentyPeriods`, `GetOutputCacheValues`, `ReplanOnDemand`,
`SolveWithSimulatedAnnealing`, `AggregateActions`, `BuildAction`, `BuildActionOnMap`,
`SplitActions`, `GetOperatingAreaNeighbors`, `ScheduleOperatingAreas`, `RunOpAreaSchedulerTask`,
`WrapperCoreAggregateAllActions`, `WrapperCoreBuildAction`,
`WrapperCoreScheduleOperatingAreas`, `WrapperCoreRunSes`, `WrapperCoreSplitActions`,
`GetRotations`, `GetYieldsFromGraph`, `ReadOaSchedulerParameters`, `EvaluateSesConstraints`.

### 2.7 Bogues révélés, inscrits dans `knownbugs.csv`

Quatre défauts de FMTlib révélés par ces tests, sans correction dans ce chantier (règle 3). Les
lignes de `knownbugs.csv` échouent aujourd'hui : elles passeront dans `basetests.csv` avec la
correction. Issues GitHub : **#346** (priorité de `^`) et **#347** (bloc `*YC` mélangé, qui
couvre aussi la constante d'équation), **#357** (`_DISCOUNTFACTOR`) et **#358** (condition
de source ignorée). **#357 est corrigé depuis le 2026-10-15** (item 3) : ses lignes sont passées
dans `basetests.csv`, et `knownbugs.csv` ne garde que les 12 lignes de #346, #347 et #358.

**Précision de Gabriel (2026-10-08) : seul #357 est un vrai défaut.** Les comportements de #346,
#347 et #358 sont connus : ce sont des syntaxes ou des cas particuliers « documentés » par la
suite BFEC, qui les teste sur des modèles réels. Leurs lignes de `knownbugs.csv` attendent donc
un comportement que FMT n'a pas choisi d'avoir ; elles restent désactivées en attendant une revue
commune des tests BFEC, qui dira ce qu'on ramène dans la suite publique et sous quelle forme.
Depuis les phases 8 et 9a, #346 a deux tests qui se contredisent : le cas unitaire activé
`Unit.FMTOperator.GivenPowerAndMultiplyWhenPrecedenceThenTheyAreEqual` fixe le comportement
actuel, et les deux lignes désactivées de `knownbugs.csv` (`…ThenAppliesThePowerFirst`) attendent
la priorité mathématique. La revue commune gardera l'un des deux.
Le lot 7a (section 5.9) ajoute quatre cas unitaires de `FMTExpression` sur #346 : trois fixent le
comportement actuel (`2*3^2` = 36, `12/2^2` = 36, et `2^3^2` = 64, le même mécanisme), le quatrième le
contournement (`2*(3^2)` = 18). Pour #347, deux cas, présentés à Gabriel et retenus le 2026-10-21, fixent
deux étapes du mécanisme décrit ci-dessous : `Unit.FMTComplexYieldHandler.GivenAnEquationWhoseNumberIsKeptAsTextWhenIndexesThenTakesTheNumberForAThemeIndex`
(étape 2) et `Unit.FMTData.GivenASumWhenToExpressionThenJoinsItsSourcesWithoutAnOperator` (étapes 3 et 4).
Pour #358, un cas d'intégration, présenté à Gabriel et retenu le 2026-10-21 (lot 7b, section 5.10), fixe le
comportement actuel : `Integration.FMTTransitionParser.GivenASourceConditionInTheActionFormWhenReadThenAppliesTheSourceToEveryAge`
lit le scénario `INVALID_sourcecondition` et donne la fourche de `arecup` (`_AGE >= 10`) à un développement de 5 ans.
Il contredit la ligne désactivée `ReadInvalidScenario` de `knownbugs.csv`, qui attend le refus (`FMTexc(31)`) : la
revue commune gardera l'un des deux, comme pour #346.
Le lot 8a (section 5.15) fixe aussi, au niveau unitaire, ce que `FMTTransition::getFork` fait d'une fourche sans condition :
il la rend à tout âge (`Unit.FMTTransition.GivenASourceWithoutConditionWhenGetForkThenGivesTheForkAtEveryAge`) ; rien n'est à
corriger dans `FMTTransition`.
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
3. **`_DISCOUNTFACTOR` n'utilisait pas le taux lu** (lot 3, issue #357). **Corrigé le
   2026-10-15** par le commit `15427a6d`, dans une session à part.
   - Cause : `FMTYieldParser` range le taux en `data.at(0)` (`4%` donne 0,04) et le nombre
     d'années par période en `data.at(1)`, mais `FMTTimeYieldHandler::get`
     (`Source/FMTTimeYieldHandler.cpp:98`) lisait `data.at(1)` pour `rateofreturn` comme pour
     `pertio` : le taux lu ne servait à rien. Présent depuis l'ajout de `_DISCOUNTFACTOR`
     (`d5ef0e2e`, 2019-11-15).
   - Vu rouge sur le build de `new_test` du 2026-10-14, avant la correction :
     `_DISCOUNTFACTOR(4%,5,FULL)` valait 1/6^5 = 0,000128600823 à la période 1 et
     1/6^10 = 1,65381716879e-8 à la période 2, `DISCOUNTNONE` 0,000128600823 à la période 2.
   - Correction : `rateofreturn` lit `data.at(0)`.
   - **Convention `HALF` retenue par Gabriel (2026-10-15) : exposant `période - 0,5`**, au lieu
     de `période x 0,5`. Rien dans la documentation de FMT ne la fixait. Woodstock (messages 373
     et 374 de `Wk.msg`) décrit le troisième paramètre comme l'actualisation de la *première*
     période, `NONE` par défaut : l'exposant vaut donc `(période - 1)` plus 0, 0,5 ou 1. Avec
     `période x 0,5`, `HALF` égalait `NONE` à la période 2 et actualisait moins que lui ensuite
     (période 3 : 1/1,04^7,5 = 0,745 contre 1/1,04^10 = 0,676).
   - Calcul des valeurs attendues : facteur = 1 / (1 + taux)^(années x exposant), avec l'exposant
     `FULL` = p, `NONE` = p - 1, `HALF` = p - 0,5. Scénario `timeyields`, 4 % et 5 ans par
     période :

     | Yield | Période | Exposant | Calcul | Valeur attendue |
     |---|---|---|---|---|
     | `DISCOUNTFULL` | 1 | 1 | 1/1,04^5 | 0,821927106759 |
     | `DISCOUNTFULL` | 2 | 2 | 1/1,04^10 | 0,675564168826 |
     | `DISCOUNTNONE` | 1 | 0 | 1/1,04^0 | 1 |
     | `DISCOUNTNONE` | 2 | 1 | 1/1,04^5 | 0,821927106759 |
     | `DISCOUNTHALF` | 1 | 0,5 | 1/1,04^2,5 | 0,906601956075 |
     | `DISCOUNTHALF` | 2 | 1,5 | 1/1,04^7,5 | 0,745160722739 |
     | `DISCOUNTHALF` | 3 | 2,5 | 1/1,04^12,5 | 0,612467796912 |

   - Lignes : les trois lignes `DISCOUNTFULL` (périodes 1 et 2) et `DISCOUNTNONE` (période 2) sont
     passées de `knownbugs.csv` à `basetests.csv`, valeurs inchangées ; `DISCOUNTNONE` (période 1)
     et `DISCOUNTHALF` (périodes 1 à 3) s'y ajoutent, avec le yield `DISCOUNTHALF` du scénario
     `timeyields`. Vu rouge des nouvelles lignes sur le même build : `DISCOUNTHALF` donnait
     0,0113402302907, 0,000128600823 et 1,4583629489e-6 ; `DISCOUNTNONE` (période 1) vaut 1 avec
     ou sans le défaut, vu rouge avec une attente faussée à 0,99.
   - Les deux conventions de `HALF` coïncident à la période 1 : seules les périodes 2 et 3
     distinguent la convention retenue.
   - Vu vert après le build de Gabriel (2026-10-15, `15427a6d`) : les 7 lignes `DISCOUNT*`
     passent, lancées une à une puis dans `RunTests.bat all-public` (section 9.5).
5. **Taux décimal de `_DISCOUNTFACTOR` tronqué** (révélé le 2026-10-15 en vérifiant #357).
   **Corrigé le 2026-10-15** par le commit `62f9e288`, dans la session de #357, à la demande
   de Gabriel : pas de ligne `knownbugs.csv`, les lignes vont directement dans `basetests.csv`.
   - Cause : `FMTYieldParser::m_rxdiscount` lisait le taux par `[\d]*`, et le reste du premier
     argument ne servait qu'à chercher un `%` : la partie décimale était perdue, et une constante
     donnait un nombre vide. Même regex depuis `d5ef0e2e` (2019-11-15).
   - Woodstock accepte un réel sans `%` (messages 102 et 369 de `Wk.msg`), et
     `FMTData::toString` réécrit le taux sous cette forme (`0.04`) : un modèle écrit puis relu
     perdait son taux.
   - Correction : le groupe du taux prend tout jusqu'à la virgule ou au `%` (`[^,%]*`), que
     `_getNum` lit comme un réel ou une constante. Les taux entiers, comme `4%`, donnent les
     mêmes groupes qu'avant.
   - Valeurs attendues, période 1, 5 ans par période, exposant 1 (`FULL`) :

     | Yield | Écriture | Calcul | Valeur attendue |
     |---|---|---|---|
     | `DISCOUNTDECIMAL` (`timeyields`) | `0.04` | 1/1,04^5 | 0,821927106759 |
     | `DISCOUNTDECIMALPERCENT` (`timeyields`) | `4.5%` | 1/1,045^5 | 0,802451046501 |
     | `DISCOUNTCONSTANT` (`discountconstant`) | `#RATE%`, `RATE 4.5` | 1/1,045^5 | 0,802451046501 |

   - Le cas de la constante a son propre scénario, `discountconstant` (ses `_yld` et `_con`) :
     avant la correction, il fait échouer la lecture de tout le scénario (`FMTexc(12)`, nombre
     invalide), ce qui aurait entraîné les autres lignes de `timeyields`.
   - Vu rouge sur le build `15427a6d` : `DISCOUNTDECIMAL` donnait 1 (taux lu 0),
     `DISCOUNTDECIMALPERCENT` 0,821927106759 (taux lu 4 %), et `DISCOUNTCONSTANT` levait
     `FMTexc(12)`.
   - Vu vert après le build de Gabriel (2026-10-15, `62f9e288`) : les 3 lignes passent, lancées
     une à une puis dans `RunTests.bat all-public` (section 9.5).

**Défauts d'écriture révélés par le lot 5 (2026-10-19).** Cinq défauts de `FMTModelParser::write`,
révélés par les allers-retours de `IntegrationParser` (section 5.7) et démontrés avec les exécutables
du build (A/B, exécutable officiel, origine git). Chacun a un cas `DISABLED_` dans
`Tests/Integration/Parser/FMTModelParser.cpp`, pas de ligne `knownbugs.csv`, et son issue, #379 à
#383, ouvertes par Gabriel le 2026-10-20 (règle 3), dont les numéros sont aussi dans les commentaires
des cas. Aucun n'est corrigé dans ce chantier.

6. **Objectif `_PENALTY` sans signe réécrit `y_PENALTY` (issue #379)** : `FMTConstraint::operator std::string`
   reprend le dernier caractère du nom `Penalty<opérateur>_<variable>`, `y` quand l'opérateur est
   vide. Relu, l'opérateur vaut `Y`, traité comme `+` : même optimum, objectif relu différent. Depuis
   `2e3dc129` (2019-06-07). Cas `GivenAPenaltyObjectiveWithoutSignWhenWriteThenRereadsTheSameObjective`.
   **Corrigé** par `83e2309e` (« fix(penalty) », sur `master`, entré dans `new_test` par la fusion `eea7fea2`
   du 2026-10-23) : un opérateur vide n'écrit plus de signe. La fusion a réactivé le cas d'intégration ; le cas
   unitaire `GivenAMinimizationOfPenaltiesWithoutSignWhenConvertedToStringThenWritesThemWithoutSign` (lot 7b) l'est
   depuis le 2026-10-23, à valider au prochain build.
7. **`_DISCOUNTFACTOR` réécrit en série temporelle de ses paramètres (issue #380)** (`DISCOUNTFULL 1 0.04 5`),
   silencieux : relu, `DISCOUNTFULL` vaut 0,04 à la période 1 et 5 à la période 2. Et l'écriture
   lève (« invalid vector subscript ») quand le bloc `*YT` n'a pas de base, comme dans
   `discountconstant`. `FMTTimeYieldHandler::operator std::string` ignore l'opérateur du `FMTData` ;
   série depuis `d5ef0e2e` (2019-11-15), plantage depuis `949f301f` (2022-12-08). Cas
   `GivenDiscountFactorsWhenWriteThenRereadsTheirValues` et
   `GivenADiscountRateFromAConstantWhenWriteThenWritesTheModel`.
8. **Sorties cachées d'un output écrites comme des outputs (issue #381)** : `FMTOutput::_toWsFormat` (sources
   d'inventaire et d'action mêlées, `~RATIO0IN`, `~RATIO1OUT` dans `TSLA`) et `_operatorFormat`
   (opérateurs enchaînés, `~<nom>_<n>` dans `division`, `level`...) ; relues comme des outputs, elles
   sont répétées ou redécoupées à chaque réécriture. Même optimum sur `TSLA`. Depuis `4261a23e`
   (2024-07-30) et `3a589899` (2024-09-10). Cas
   `GivenARatioOfAnInventoryOutputToAnActionOutputWhenWriteThenRereadsTheSameOutputs`.
9. **`_RANDOM(<output>)` réécrite sans `_RANDOM` (issue #382)** : le type `FMTrandomaction`, marqué « need to be
   deleted » dans `Include/FMTConstraint.h`, est écrit comme une contrainte ordinaire et relu comme
   tel. Depuis `95a9185e` (2021-01-22). Cas
   `GivenRandomActionConstraintsWhenWriteThenRereadsTheSameModelAndRewritesTheSameFiles`.
10. **`FMTYields::operator==` compare les adresses des gestionnaires (issue #383)** (`std::unique_ptr` dans la
    `FMTList`) : deux copies d'un modèle sont inégales (`FMTModel::operator==`), et `writeToProject`
    écrit le `._yld` de chaque scénario, même identique à la racine (`WrapperCoreWriteToProject` le
    montre sur ses deux copies). Depuis `2cbb45a6` (2021-10-29). Cas
    `GivenAScenarioThatOnlyChangesItsObjectiveWhenWriteToProjectThenWritesItsOptimizeAndScheduleSectionsOnly`.

Le comportement connu de #347 bloque aussi l'aller-retour de la racine et des scénarios qui gardent
son `.yld` : `YBM_RTM_F`, écrit `_EQUATION(YBM_RTM_BOGYBM_RTM_MEH)`, se relit 0 (cas désactivé
`GivenTheRootModelWhenWriteThenRereadsTheSameModelAndRewritesTheSameFiles`, avec le défaut 6, #379). #346 ne
change rien à l'aller-retour : l'expression est réécrite telle qu'elle a été lue.

Dans TWD_land, `YBM_RTM_BOG`, `YBM_RTM_MEH` et `YBM_RTM_F` ne servent qu'à l'output `TOTALCAR`,
qu'aucun objectif ni aucune contrainte n'utilise. À la correction, relancer la suite base pour
repérer les valeurs attendues qui changent, et justifier au journal toute valeur modifiée
(règle 9).

**Défauts révélés par le lot 7a (2026-10-21).** Trois défauts, démontrés avec l'exécutable officiel
`ReadScenario` du build de `new_test` (FMTlib de `f4252745`), sur une copie de TWD_land hors du dépôt
(scratchpad), en ajoutant une ligne ou un bloc au `._yld` du scénario `timeyields` (A/B, une seule chose
change). Les deux premiers ont un cas `DISABLED_` dans `UnitCore`. Le troisième est dans `FMTYieldParser`,
hors des classes du lot : son cas `DISABLED_` est dans `IntegrationParser` depuis le lot 7c (section 5.11). Leurs
issues, #386 (défaut 11), #388 (défaut 12) et
#387 (défaut 13), ont été ouvertes par Gabriel le 2026-10-21 (règle 3) ; leurs numéros sont aussi dans
les commentaires des cas désactivés, dans `CoreTestCoverage.csv` et dans le changelog. Aucun n'est
corrigé dans ce chantier.

11. **Un moins qui suit un opérateur est lu comme une soustraction** (`FMTExpression`, issue #386).
    - `_getPostFix` (`Source/FMTExpression.cpp:65`) traite tout `-` comme l'opérateur binaire : dans
      `2*-3`, le `-`, moins prioritaire que `*`, donne le postfixe `2 * 3 -`. `_evaluatePostFix`
      (ligne 230) donne alors au `*` un opérande gauche absent, 0 : `0*2`, puis `0-3`, soit -3 au lieu
      de -6. Le moins en tête marche par la même règle (`-5+2` vaut -3).
    - Sur la racine (`UNITE1 PEUPLEMENT1 UTR1`, 7 ans, `VOLUMETOTAL` = 120), dans un bloc `*YC` ajouté :
      `_EQUATION(-1*VOLUMETOTAL)` vaut -120, `_EQUATION(VOLUMETOTAL*-1)` vaut -1, et
      `_EQUATION(VOLUMETOTAL*(-1))` vaut 0 : les parenthèses n'y font rien. Dans un autre bloc, sans
      aucun nombre, donc hors du mécanisme de #347 : `_EQUATION(VOLUMETOTAL*VOLUMETOTAL)` vaut 14400,
      `_EQUATION(VOLUMETOTAL*-VOLUMETOTAL)` vaut -120 au lieu de -14400.
    - Contournement : `(0-1)` (`_EQUATION(VOLUMETOTAL*(0-1))` vaut -120), ou le moins en tête.
    - Origine : la conversion en postfixe est la même depuis `2e3dc129` (2019-06-07) ; le 0 de
      l'opérande absent vient de `bb73b9f2` (2019-08-23), qui a rendu possible le moins en tête.
    - Aucun modèle de `Examples/Models` n'écrit de moins après un opérateur. Non vérifié dans les
      outputs, qui passent aussi par `FMTExpression::shuntingYard` (`FMTOutput::shuntingYard`).
    - Cas : `Unit.FMTExpression.GivenAMinusAfterAMultiplicationWhenShuntingYardThenNegatesTheValueThatFollows`.
12. **Les yields d'un bloc `*YT` lisent tous la période de départ du premier** (`FMTTimeYieldHandler`,
    issue #388).
    - `FMTYieldParser` pousse une période de départ par ligne d'un bloc `*YT`, et
      `FMTTimeYieldHandler::pushData` les range dans l'ordre des noms (`cmpYieldString` : le plus court
      d'abord). `get` (`Source/FMTTimeYieldHandler.cpp:116`) lit `getBases().at(0)` pour tout yield du
      bloc, alors que `operator std::string` (ligne 29) écrit la période de chaque yield à sa place.
    - A : `timeyields` plus la ligne `LATE 1 5 6` ; `PERIODVALUE` vaut 10 à la période 1 et 30 à la
      période 3. B : `LATE 3 5 6` ; `PERIODVALUE` vaut 0 et 10. `LATE`, premier dans l'ordre des noms,
      reste juste (0 à la période 1, 5 à la période 3).
    - Origine : `9fd69f73` (2025-02-05), qui a fait lire une période de départ à `get` ; avant, `get`
      n'en lisait aucune (valeur à l'indice de la période).
    - Dans TWD_land, chaque bloc `*YT` n'a qu'une période de départ (0, ou 1 dans `timeyields`, dont les
      `_DISCOUNTFACTOR` n'en poussent pas) : aucun test public n'est touché. Modèles BFEC : non vérifié.
    - Cas : `Unit.FMTTimeYieldHandler.GivenTwoTimeYieldsStartingAtDifferentPeriodsWhenGetThenEachStartsAtItsOwnPeriod`,
      construit comme le parseur, à côté du cas vert `GivenTwoTimeYieldsStartingAtTheSamePeriod…`, qui ne
      diffère que par la période de départ de `LATE`.
13. **En colonnes (`_CP`), chaque ligne d'un bloc `*YT` retarde d'une période les valeurs des lignes
    suivantes** (`FMTYieldParser`, issue #387, relevé en lisant le parseur pour le défaut 12).
    - Pour chaque ligne après la première, `FMTYieldParser::read` (`Source/FMTYieldParser.cpp:637`)
      remplit les périodes de `getLastBase()` à celle de la ligne : la première de la boucle est la
      dernière déjà poussée, dont la valeur est donc poussée deux fois.
    - Bloc `_CP CPA CPB`, lignes `1 10 100`, `3 30 300`, `5 50 500` : `CPA` vaut 10, 10, 10, 30, 30, 30,
      50 aux périodes 1 à 7, au lieu de 10, 10, 30, 30, 50, 50, 50 que la boucle cherche à donner (à la
      période d'une ligne, sa valeur ; entre deux lignes, la précédente). `CPB` vaut 100 à la période 3.
    - Origine : la boucle date de `2e3dc129` (2019-06-07). D'après le code, avant `9fd69f73`, `get`
      lisait l'indice de la période, ce qui compensait le doublon de la deuxième ligne quand la première
      est à la période 1, mais pas ceux des lignes suivantes.
    - Aucun modèle de `Examples/Models` n'écrit de `_CP`, hors du scénario `timeyieldcolumns` du lot 7c.
    - Cas (lot 7c, proposé à Gabriel et retenu le 2026-10-22) :
      `Integration.FMTYieldParser.GivenTimeYieldsWrittenInColumnsWhenReadThenEachLineGivesItsValuesFromItsPeriod`,
      désactivé, sur le nouveau scénario `timeyieldcolumns` : le `.yld` de la racine suivi du bloc ci-dessus. Il attend
      `CPA` à 10, 10, 30, 30, 50, 50, 50 et `CPB` à 100, 100, 300, 300, 500, 500, 500 aux périodes 1 à 7, et lit 10, 10,
      10, 30, 30, 30, 50 et 100, 100, 100, 300, 300, 300, 500. `ReadScenario` (`YIELD`) donne les mêmes valeurs.

**Défauts révélés par le lot 7b (2026-10-21).** Quatre défauts de FMTlib, démontrés comme ceux du lot 7a : cause
dans le code, A/B où une seule chose change, exécutable officiel du build de `new_test` (FMTlib de `f4252745`),
origine git. Les deux premiers sont les observations du lot 7a (section 5.9). Chacun a un cas `DISABLED_` dans
`UnitCore`. Leurs issues, #389 (défaut 14), #390 (défaut 15), #391 (défaut 16) et #392 (défaut 17), ont été
ouvertes par Gabriel le 2026-10-22 (règle 3) ; leurs numéros sont aussi dans les commentaires des cas désactivés,
dans `CoreTestCoverage.csv` et dans le changelog. Aucun n'est corrigé dans ce chantier.

14. **`_MAX` part de 0** (`FMTComplexYieldHandler`, issue #389).
    - `_getMax` (`Source/FMTComplexYieldHandler.cpp:677`) part de `maxValue = 0`, alors que `_getMin` part du plus
      grand double et lève quand il ne trouve aucune source : le maximum de yields tous négatifs vaut 0.
    - A/B avec `ReadScenario` sur une copie de TWD_land hors du dépôt (scratchpad), un bloc `*YC` sans nombre ajouté
      au `._yld` de `yieldoperatorsalone`, `UNITE1 PEUPLEMENT1 UTR1` à 7 ans (`VOLUMETOTAL` = 120) :
      `NEGATIVE _MULTIPLY(VOLUMETOTAL,-1)` vaut -120 et `MORENEGATIVE _MULTIPLY(VOLUMETOTAL,-2)` -240 ;
      `_MAX(NEGATIVE,MORENEGATIVE)` vaut 0 au lieu de -120, alors que `_MIN(NEGATIVE,MORENEGATIVE)` vaut -240 et
      `_MAX(NEGATIVE,VOLUMETOTAL)` 120.
    - Origine : `20cbff3a` (2023-12-14, « fix #191 »), qui ajoute `_MAX` et `_MIN`.
    - Aucun modèle de `Examples/Models` ne prend le `_MAX` de yields négatifs. Modèles BFEC : non vérifié.
    - Cas : `Unit.FMTComplexYieldHandler.GivenAMaximumOfNegativeYieldsWhenGetThenReturnsTheLargest`, à côté du cas
      vert `GivenAMinimumOfNegativeYieldsWhenGetThenReturnsTheSmallest`.
15. **`FMTYields::getFromFactor` multiplie le taux et les années d'un `_DISCOUNTFACTOR`** (`FMTTimeYieldHandler`,
    issue #390).
    - `FMTTimeYieldHandler::getFromFactor` (`Source/FMTTimeYieldHandler.cpp:170`) multiplie tout le `FMTData` du yield
      (`FMTData::operator*`, `Source/FMTData.cpp:394`), et les données d'un `_DISCOUNTFACTOR` sont son taux et ses
      années par période (item 3 ci-dessus) : un facteur 2 donne 1/1,08^10 au lieu de 2/1,04^5.
    - A/B avec `FMT.pyd` du build (`getfromfactor`, module importé de `build/release/bin/Release`), scénario
      `timeyields` lu dans le dépôt, `UNITE1 PEUPLEMENT1 UTR1` à 7 ans, période 1 : un facteur 2 fait passer
      `VOLUMETOTAL` de 120 à 240 et `PERIODVALUE` de 10 à 20, mais `DISCOUNTFULL` de 0,821927106759 à 0,463193488085
      (1/1,08^10) au lieu de 1,643854213519.
    - Origine : `2cbb45a6` (2021-10-29), qui donne `getfromfactor` aux yields de temps ; avant, seuls les yields
      d'âge étaient multipliés.
    - Exposé en Python et en R (`getfromfactor`). `GetYieldsFromFactor` ne multiplie que `VOLUMETOTAL` : aucun test
      public n'est touché.
    - Cas : `Unit.FMTYields.GivenAFactorOfADiscountFactorWhenGetFromFactorThenMultipliesItsValue`, à côté du cas
      vert `GivenADiscountFactorWhenGetThenReturnsTheFactorOfThePeriod`.
16. **`FMTYields::getFromFactor` ne multiplie aucun yield quand on n'en nomme aucun** (`FMTYields`, issue #391,
    relevé en écrivant le cas du défaut 15).
    - `FMTYields::getFromFactor` (`Source/FMTYields.cpp:343`) ne passe un gestionnaire à son `getFromFactor` que s'il
      contient un des yields nommés : sans nom, aucun. Sa documentation (`Include/FMTYields.h:218`, « all if
      empty ») et les gestionnaires d'âge et de temps, qui multiplient tout sur une liste vide, disent tous.
    - Démonstration : même script `FMT.pyd`, `getfromfactor(2.0, [])` laisse `VOLUMETOTAL`, `PERIODVALUE` et
      `DISCOUNTFULL` à 120, 10 et 0,821927106759, alors que `getfromfactor(2.0, ["VOLUMETOTAL"])` donne 240.
    - Origine : `087e2278` (2021-12-09, « fix for getfromfactor »), qui ajoute ce filtre ; avant, chaque
      gestionnaire recevait la liste, et une liste vide les multipliait tous.
    - Cas : `Unit.FMTYields.GivenAFactorWithoutYieldNamesWhenGetFromFactorThenMultipliesEveryYield`.
17. **`_ENDPOINT` provoque une violation d'accès dès que sa source sort de ses bornes** (`FMTComplexYieldHandler`,
    issue #392, relevé en couvrant `_ENDPOINT`, que le lot 7a avait laissé).
    - `_getEndPoint` (`Source/FMTComplexYieldHandler.cpp:573` et `583`) appelle `_getChangesFrom` sur le gestionnaire
      complexe, qui lit `m_bases[peakstep]` (`Source/FMTYieldHandler.cpp:318`) : un gestionnaire `*YC` n'a aucun âge
      de base, et `operator[]` lit hors d'un vecteur vide.
    - A/B avec `ReadScenario` sur la même copie : `_ENDPOINT(VOLUMETOTAL,VOLUMETOTAL,100,200)` vaut 0 à 7 ans
      (`VOLUMETOTAL` dans ses bornes) et lève `FMTexc(85)`, erreur Win32 3221225477 (0xC0000005, violation d'accès), à
      20 ans ; `_ENDPOINT(VOLUMETOTAL,VOLUMETOTAL,150,250)` lève à 7 ans et vaut 0 à 20 ans. Une sonde unitaire hors
      du dépôt donne la même chaîne : quatre `FMTexc(53)`, puis `FMTexc(85)`.
    - Origine : `bb73b9f2` (2019-08-23), qui introduit ce calcul ; la séparation des gestionnaires (`2cbb45a6`,
      2021-10-29) l'a gardé.
    - Aucun modèle de `Examples/Models` n'a de `_ENDPOINT`. Sa valeur hors de ses bornes reste à trancher (section
      5.5) : le cas demande seulement que l'appel ne lève rien.
    - Cas : `Unit.FMTComplexYieldHandler.GivenAnEndPointWhoseSourceIsBelowItsLowerBoundWhenGetThenRaisesNothing`, à
      côté du cas vert `GivenAnEndPointWhoseSourceIsWithinItsBoundsWhenGetThenReturnsZero`.

**Défaut révélé par le lot 7c (2026-10-22).** L'observation de `FMTParser::bounds` du lot 7b (section 5.10), démontrée
comme les défauts des lots 7a et 7b : cause dans le code, A/B où une seule chose change, exécutable officiel
`ReadScenario` du build de `new_test` (FMTlib de `f4252745`), origine git. Il a deux cas `DISABLED_` dans
`IntegrationParser`. Son issue, #393, a été ouverte par Gabriel le 2026-10-22 (règle 3) ; son numéro est aussi dans les
commentaires des cas, dans `CoreTestCoverage.csv` et dans le changelog. Il n'est pas corrigé dans ce chantier.

18. **Une condition d'opérabilité écrite avec `>` ou `<` est lue sans sa borne** (`FMTParser::bounds`, issue #393).
    - `FMTParser::getBaseOperators` (`Source/FMTParser.cpp:2195`) liste `=`, `<=`, `>=`, `<` et `>`, que
      `FMTActionParser::_getBounds` reconnaît donc tous les cinq ; mais `FMTParser::bounds` (`Source/FMTParser.cpp:405`)
      ne traite que les trois premiers. Pour `<` et `>`, il rend des bornes du plus petit au plus grand entier, ou réel,
      sans message : la condition disparaît. Seul le parseur des actions l'appelle, pour `_AGE`, `_CP` et les yields
      d'une condition d'opérabilité.
    - A/B avec `ReadScenario` (`OPERABLE`) sur le nouveau scénario `strictbounds`, `UNITE1 PEUPLEMENT1 UTR1` à la
      période 1 : `_AGE > 10` est opérable à 0, 5, 10, 11 et 20 ans, `_AGE >= 11` à 11 et 20 ans seulement ;
      `_AGE < 5` à 0, 4, 5 et 20 ans, `_AGE <= 4` à 0 et 4 ans seulement. Sur une copie du scénario dans le
      scratchpad, les deux autres formes : `volumetotal > 100` est opérable à 0, 5 et 6 ans (`volumetotal >= 100`, la
      condition de `COUPEtotale`, à 5 et 6 ans seulement), et `_CP > 2` aux périodes 1 à 3 (`_CP >= 3` à la période 3
      seulement). La lecture n'émet aucun avertissement sur ces conditions.
    - `FMTModelParser::write` écrit ces actions sans condition (`? PROD ?`) : le modèle écrit puis relu n'en garde
      aucune trace (sonde du lot 7c).
    - Woodstock refuse ces deux opérateurs : les messages 142 et 143 de `Wk.msg`, fatals, demandent `>=` et `<=`. Un
      modèle valide pour Woodstock n'en contient donc pas. La correction choisira entre les lire comme des bornes
      strictes (`_AGE > 10` comme `_AGE >= 11` ; pour un réel, il faut une borne ouverte) et les refuser comme
      Woodstock. Les deux cas désactivés attendent la première ; si la seconde est retenue, ils deviendront des tests
      de l'exception.
    - Origine : `2e3dc129` (2019-06-07, le premier commit), où `FMTparser::bounds` avait déjà la même liste et les
      mêmes trois branches.
    - Aucun des 42 fichiers d'actions d'`Examples/Models` n'écrit `<` ni `>`, hors de `strictbounds`. Modèles BFEC,
      relevé en lecture seule le 2026-10-22 : aucune condition d'opérabilité en `<` ni `>` dans les 113 fichiers
      d'actions des 62 modèles de la suite privée ; leurs 16 `>` isolés séparent des actions, dans une `*ACTIONSERIES`
      (15) ou un `*AGGREGATE` (1). Le défaut ne change donc aucun résultat de ces modèles.
    - Cas : `Integration.FMTActionParser.GivenAMinimumAgeWrittenWithGreaterThanWhenReadThenTheActionIsOperableAboveThatAgeOnly`
      et `Integration.FMTActionParser.GivenAMaximumAgeWrittenWithLessThanWhenReadThenTheActionIsOperableBelowThatAgeOnly`,
      à côté des cas verts `GivenAMinimumAgeWrittenWithGreaterOrEqual…` et `GivenAMaximumAgeWrittenWithLessOrEqual…`
      du même scénario.
    - Cas unitaire du lot 8a (section 5.15), vert :
      `Unit.FMTAction.GivenAMinimumAgeWrittenWithGreaterThanAsTheParserReadsItWhenUpdateThenTheActionAcceptsEveryAge` construit
      l'action avec les bornes que `FMTParser::bounds` rend pour `>` : elle accepte tout âge et s'écrit sans condition. La
      correction est dans le parseur : ce cas ne l'exige pas.

**Défauts révélés par les lacunes 6 et 7 (2026-10-26, section 5.14).** Démontrés comme les précédents : cause dans le
code, A/B où une seule chose change, origine git. Aucun exécutable officiel ne les appelle : ni FMT, ni le wrapper, ni
Python, ni R (`unboundSolution` et `isPeriodBounded` ne sont pas exposés) ; la démonstration passe donc par les cas
d'`IntegrationModels`, compilés hors du dépôt contre le `FMTlib.dll` du build de `new_test` du 2026-10-26. Chacun a
son cas `DISABLED_`. Issues ouvertes par Gabriel le 2026-10-26 (règle 3) : #399 et #400 ; leurs numéros sont aussi
dans les commentaires des cas et dans le changelog (`FMTSrModel` n'est pas une classe de Core : rien dans
`CoreTestCoverage.csv`). Ils ne sont pas corrigés dans ce chantier.

19. **`unboundSolution` ne rend pas les bornes d'avant `boundSolution`** (`FMTSrModel`, issue #399).
    - `FMTSrModel::unboundSolution` (`Source/FMTSrModel.cpp:130` et `131`) donne aux variables de la période les bornes
      `-std::numeric_limits<double>::max()` et `max()`, et non celles qu'elles avaient : 0 et l'infini (`updateMatrix`,
      lignes 1160 et 1161). Une superficie peut alors être négative, et la matrice n'a plus d'optimum.
    - A/B sur `LP` (5 périodes, MOSEK puis CLP), modèle construit (`build`) et résolu (65 733,264791), puis
      `boundSolution` sur les périodes 1 à 5 (optimum inchangé) : A, les bornes d'avant remises à la main par
      `setColSetBounds`, `resolve` rend l'optimum 65 733,264791 ; B, `unboundSolution` sur les périodes 1 à 5, 666 des
      675 colonnes ont une borne inférieure de -1e30 au lieu de 0 (MOSEK ramène `max()` à 1e30), `resolve` et
      `initialSolve` échouent, et le modèle n'est plus optimal.
    - Origine : `8594bf5a` (2019-07-22, « New unbound function for FMTlpmodel »), avec `-COIN_DBL_MAX` ; `0ed917f3`
      (2023-12-08) n'a fait que passer à `std::numeric_limits`.
    - Portée : aucun appel dans FMT ni dans les tests ; ni Python ni R ne l'exposent. Seul un programme C++ qui
      replanifie à la main l'appelle.
    - Cas : `Integration.FMTSrModel.GivenBoundSolutionsWhenUnboundSolutionThenResolvesToTheModelBefore` (bornes
      inférieures d'avant, `resolve` optimal, même optimum), à côté du cas vert
      `GivenTheOptimalSchedulesSetInTheWholeMatrixWhenBoundSolutionThenResolvesToTheOptimum`.
20. **`isPeriodBounded` rend vrai pour toute période du graphe** (`FMTSrModel`, issue #400).
    - `FMTSrModel::isPeriodBounded` (`Source/FMTSrModel.cpp:92` à `96`) n'incrémente `totaledges` que dans la branche
      où la variable est bornée (`upper == lower`) : `boundededges == totaledges` est toujours vrai, et la fonction rend
      vrai dès que le graphe a la période. Sa documentation (`Include/FMTSrModel.h:148`) dit vrai si les superficies de
      la période sont bornées.
    - A/B sur le même modèle résolu de `LP` : A, après `boundSolution(1)`, `isPeriodBounded(1)` rend vrai ; B, sans
      `boundSolution`, il rend aussi vrai, pour les périodes 0 à 6, alors qu'aucune variable des périodes 1 à 5 n'est
      fixée. Après `unboundSolution`, il rend encore vrai.
    - Origine : `80c556d3` (2019-11-01, « Added period delete… »), qui l'introduit pour `erasePeriod`.
    - Portée : son seul appel, `FMTLpModel::erasePeriod`, lève `FMTfunctionfailed` dès son entrée (« This function is
      need to be rewrite… ») ; ni Python ni R ne l'exposent. Aucun résultat n'en dépend aujourd'hui.
    - Cas : `Integration.FMTSrModel.GivenAnOptimizedModelWhenIsPeriodBoundedThenReturnsFalse`, à côté du cas vert
      `GivenBoundSolutionsWhenIsPeriodBoundedThenReturnsTrue`.

**Défaut révélé par le lot 8a (2026-10-27).** L'observation de `FMTTransitionMask::setMask` de la section 5.15, démontrée à la
demande de Gabriel le même jour : cause dans le code, A/B où une seule chose change, origine git. Aucun exécutable officiel ne passe
par ce chemin (portée ci-dessous) : la démonstration passe donc, comme pour les défauts 19 et 20, par une sonde compilée hors du
dépôt contre le `FMTlib.dll` du build de `new_test` (FMTlib du 2026-10-26), sur une copie de TWD_land dans le scratchpad. Il a un
cas `DISABLED_` dans `UnitCore`. Son issue, #402, a été ouverte par Gabriel le 2026-10-27 (règle 3) ; son numéro est aussi dans le
commentaire du cas désactivé, dans `CoreTestCoverage.csv` et dans le changelog. Il n'est pas corrigé dans ce chantier. Les autres
écarts du lot (l'ordre d'`FMTActualDevelopment`, `reduceLockToDeath`, `FMTFork::getMax`, `FMTList::push_back` d'une liste) restent
des observations (section 5.15).

21. **`FMTTransitionMask::setMask` ne refait pas la sélection de la cible** (`FMTTransitionMask`, issue #402).
    - `setMask` (`Source/FMTTransitionMask.cpp:142`) change le masque sans appeler `build` : la sélection (`selection`,
      `flippedselection`) que `trans` combine avec le masque du développement reste celle du masque de construction.
      `FMTModel::pushTheme` (`Source/FMTModel.cpp:481`), qu'appelle `FMTModel::buildAction`, passe ainsi les cibles des transitions
      au masque du nouveau thème : sur un développement des quatre thèmes, `trans` combine une sélection de 10 bits avec un masque
      de 12, perd le nouveau thème, et le masque obtenu n'a aucun attribut pour lui (`FMTundefined_attribute`, 19).
    - A/B unitaire (cas ci-dessous) : la cible `? FEU ? ?` construite avec les quatre thèmes donne `UNITE1 FEU UTR1 VPOST` pour
      `UNITE1 PEUPLEMENT1 UTR1 VPOST` ; la cible `? FEU ?` des trois thèmes passée par `setMask` lève `FMTexc(53)` puis
      `FMTexc(19)`.
    - A/B sur un modèle : `buildAction("ABUILT", "VOLUMETOTAL")` sur la racine, `FMTLpModel` en CLP, `doPlanning(false)` sur 2
      périodes. A, le modèle bâti, en mémoire, sans presolve (`PRESOLVE_ITERATIONS` à 0) : `FMTexc(19)` en opérant `AFIRE` sur
      `UNITE1 PEUPLEMENT1 UTR1 PRE`. B, le même modèle écrit par `writeToProject` puis relu, sans presolve : 126 sommets, 173 arcs,
      69 lignes et 156 colonnes. Le modèle bâti avec le presolve (10 passes) donne les mêmes nombres, car
      `FMTTransitionMask::preSolveRef` refait la sélection, et la racine seule aussi. Opérer `COUPETOTALE` sur
      `UNITE1 PEUPLEMENT1 UTR1 PRE` à 20 ans : `FMTexc(19)` en mémoire, `UNITE1 PEUPLEMENT1 UTR1 PRE` à 0 an une fois relu.
    - Origine : `setmask` est ajouté, sans appelant, par `adc38e0d` (2022-07-28, « Fix #82 ») ; son premier appelant est
      `pushtheme`, ajouté par `e86e9a86` (2024-07-17, « Action builder for TBE done »).
    - Portée : seul un modèle de `buildAction` gardé en mémoire et construit sans presolve est touché. `BuildAction` et
      `WrapperCoreBuildAction`, tests privés, écrivent le modèle bâti, qu'on relit ; `BuildActionOnMap` n'en lit que les thèmes et
      les superficies ; `buildSchedule` n'opère pas les transitions ; ni Python ni R n'exposent `buildAction`. Le wrapper garde le
      modèle bâti dans `ModelCache`, mais ses planifications passent par `doPlanning`, qui présolve par défaut (il ne touche pas à
      `PRESOLVE_ITERATIONS`). La simulation non spatiale d'une replanification (`FMTNssModel`) faite sur un tel modèle : non vérifiée.
    - Cas : `Unit.FMTTransitionMask.GivenAMaskOfANewThemeWhenSetMaskThenTransAppliesTheNewMask`, à côté du cas vert
      `GivenATargetBuiltWithANewThemeWhenTransThenKeepsTheAttributeOfTheNewTheme`.
    - Hors de ce défaut, relevé par la même sonde sur 5 périodes : le modèle bâti, relu ou présolvé, échoue à la période 3 en
      opérant `ARECUP` sur `UNITE2 FEU UTR1 POST` (aucun `VOLUMETOTAL` pour ce développement, `FMTexc(42)` ; après le presolve, une
      violation d'accès, `FMTexc(85)`). Non analysé : `VOLUMETOTAL` n'est peut-être pas un yield que `buildAction` peut prendre
      pour cible sur TWD_land.

**Défauts révélés par le lot 8b (2026-10-27).** Deux défauts de FMTlib, démontrés comme les précédents : cause dans le code,
A/B où une seule chose change, exécutable officiel (`FMT.pyd` du build de `new_test`, FMTlib du 2026-10-26, lancé par des scripts
du scratchpad qui lisent le dépôt sans l'écrire), origine git. Chacun a un cas `DISABLED_` dans `UnitCore`. Leurs issues sont à
ouvrir par Gabriel (règle 3), textes rédigés à la livraison du lot (section 5.16). Ils ne sont pas corrigés dans ce chantier.

22. **Le cache des yields complexes sert à un modèle la valeur calculée pour un autre** (`FMTYieldsCache`, issue à ouvrir).
    - `FMTYieldsCache` garde ses valeurs dans une seule table statique (`Source/FMTYieldsCache.cpp:23`), partagée par tous les
      gestionnaires complexes du processus, que rien ne vide hors d'un manque de mémoire (`_clearIfTooBig`). Sa clé
      (`FMTYieldDevelopment`) ne retient que l'âge, la période, le masque résumé du développement et le nom du yield : ni le modèle,
      ni ses rendements. `FMTComplexYieldHandler::get` (`Source/FMTComplexYieldHandler.cpp:769`) cherche le yield dans le cache
      avant de le calculer, et y met un yield dont le calcul a pris plus de 0,05 ms (lignes 870 à 872). Deux modèles de mêmes
      thèmes et de mêmes masques de rendements donnent le même masque résumé : le second reçoit le yield complexe de même nom du
      premier, quelle que soit sa définition.
    - A/B avec `FMT.pyd`, sur une copie des fichiers de la racine de TWD_land dans le scratchpad, avec deux scénarios qui ajoutent
      au `.yld` de la racine un bloc `*YC ? ? ?` sans nombre (hors du mécanisme de #347) : `SLOWYIELD
      _EQUATION(VOLUMETOTAL+...+VOLUMETOTAL)`, de 300 termes dans `SLOWA` et de 600 dans `SLOWB`, et `FASTYIELD`, 2 et 3 fois
      `VOLUMETOTAL`. Premier développement de la superficie, `UNITE1 PEUPLEMENT1 UTR1` à 12 ans (`VOLUMETOTAL` = 170). Lu seul dans
      son processus, `SLOWB` donne `SLOWYIELD` = 102 000 (600 x 170, calculé en 0,65 ms) ; lu après `SLOWA` dans le même processus,
      51 000 (300 x 170, la valeur de `SLOWA`), servi en 0,009 ms. Dans l'ordre inverse, `SLOWA` reçoit 102 000. `FASTYIELD`,
      calculé en moins de 0,05 ms, n'est pas mis en cache et reste juste : 340 et 510.
    - Cas unitaire : un premier `FMTYields` calcule `_MULTIPLY(VOLUME,2)` (400 à 20 ans), mis en cache comme FMT le fait après un
      calcul lent ; un second, de mêmes masques, où le même yield vaut `_MULTIPLY(VOLUME,3)`, rend 400 au lieu de 600.
    - Origine : `b65a8f1b` (2024-08-09, « Fix #238 »), qui ajoute la table statique, alors clée par le développement entier et le
      nom du yield ; `bd104949` (2024-08-16, « less memory ») passe au masque résumé et ajoute le seuil de 0,05 ms.
    - Portée : tout processus qui calcule les yields complexes de deux modèles de mêmes thèmes où un yield complexe de même nom
      diffère, quand son calcul dépasse 0,05 ms (l'effet dépend donc de la machine) : des scénarios qui redéfinissent un yield
      complexe, lus dans le même processus (Python, R, l'interface et son `ModelCache`, une tâche qui planifie plusieurs
      scénarios), ou les yields que rend `getFromFactor`, dont les yields complexes calculés avant le facteur seraient servis
      après. Non vérifié : `FMTPlanningTask`, les modèles BFEC et les tests publics qui lisent plusieurs scénarios.
    - La correction aura aussi à dire quand le cache se vide : la ligne de dette d'`Architecture.md` (section 5.16).
    - Cas : `Unit.FMTYieldsCache.GivenAComplexYieldCachedForOneSectionOfYieldsWhenAnotherSectionDefinesItOtherwiseThenComputesItFromItsOwnDefinition`,
      à côté du cas vert `GivenAComplexYieldCachedWithItsValueWhenYieldsGetItAgainThenServesTheSameValue`.
23. **`FMTConstantParser::write` écrit les valeurs d'une constante sans séparateur** (`FMTConstants`, issue à ouvrir).
    - `FMTConstants::operator std::string` (`Source/FMTConstants.cpp:104`) ajoute chaque valeur par `std::to_string`, sans espace
      entre elles, et `FMTConstantParser::write` écrit cette chaîne comme section des constantes.
    - A/B avec `FMT.pyd` : le `._con` du scénario `LEVEL1` (`CorCour 2`, `Creguc 100 200 300 400 500 600 700`), lu par
      `FMTconstantparser.read`, écrit dans le scratchpad par `write` : `CORCOUR 2.000000` et
      `CREGUC 100.000000200.000000300.000000400.000000500.000000600.000000700.000000`. Relu, puis réécrit : `CORCOUR 2.000000` et
      `CREGUC 100.000000`. La constante d'une valeur fait l'aller-retour ; celle de sept valeurs n'en garde qu'une.
    - Origine : `2e3dc129` (2019-06-07, le premier commit).
    - Portée : rien dans FMT n'appelle `FMTConstantParser::write`, et `FMTModelParser::write` n'écrit pas de constantes (un modèle
      n'en garde pas). Python et R l'exposent (`FMTconstantparser.write`) ; leurs tests `testParser` vérifient seulement que la
      classe existe.
    - Cas : `Unit.FMTConstants.GivenAConstantOfSeveralValuesWhenConvertedToStringThenWritesEachValueAsAWord`, à côté du cas vert
      `GivenAConstantOfOneValueWhenConvertedToStringThenWritesALineOfItsNameAndItsValue`.

## 3. Lacunes classées par criticité

Critère : une étape que tout modèle traverse passe avant une fonction de niche ; une erreur
**silencieuse** (résultat faux sans exception) passe avant une erreur qui lève.

### 3.1 Niveau 1 : la chaîne que tout modèle traverse

| # | Étape et fonctions | Lacune |
|---|---|---|
| 1 | **Lecture d'un scénario** : `FMTModelParser::readproject` (`Include/FMTModelParser.h:109`), parseurs de sections, `FMTParser` (constantes, boucles FOR) | Rien ne vérifie la structure lue : thèmes, agrégats, superficie, opérabilités, transitions, yields, lifespan, outputs, contraintes. Une erreur de lecture qui ne change pas l'objectif des scénarios testés passe. |
| 2 | **Lecture de plusieurs scénarios d'un coup** : `referenceRead` (`Source/FMTModelParser.cpp:560-830`) réutilise les sections déjà lues d'un autre scénario ; la réutilisation des outputs ne dépend pas des actions ni des yields (commentaire « should be with action?? », l. 640) | Aucune comparaison avec des lectures séparées. L'interface lit plusieurs scénarios d'un coup (`Controller::addScenarios`), comme les tâches et plusieurs tests. |
| 3 | **Erreurs de lecture** : levée des `FMTexc`, `setErrorsToWarnings` | Aucun test négatif : une erreur de modèle qui n'est plus signalée, ou signalée par le mauvais code, passe. |
| 4 | **Objets du cœur** : `FMTDevelopment::grow`, `operable`, `operate` (`Include/FMTDevelopment.h:209-274`), `FMTMask::isSubsetOf`, `FMTTheme::validate`, `FMTYields::get` (`Include/FMTYields.h:230`), lifespan et `_DEATH` par défaut | Aucune vérification unitaire de l'opérabilité (bornes d'âge, de yield, de période, `_LOCK`), des transitions (proportions, cibles) ni du vieillissement ; seulement des objectifs. |
| 5 | **Présolve et postsolve** : `FMTModel::preSolve` / `postSolve` (`Include/FMTModel.h:279-285`) | Le test métamorphique ne couvre qu'un scénario (NOT_MASK) ; les outputs après postsolve ne sont pas comparés. |
| 6 | **Graphe et matrice** : `FMTSrModel::buildPeriod` (`Include/FMTSrModel.h:261`), `FMTGraph::build` (`Include/FMTGraph.hpp:462`), `FMTLpModel::setObjective` / `setConstraint` | ~~Les statistiques du graphe (`FMTGraphStats` : sommets, arcs, lignes, colonnes) ne sont jamais vérifiées.~~ Couverte : `STATS` (lot 4) pour la matrice entière, `IntegrationModels` période par période (section 5.14). |
| 7 | **Résultats et rejeu** : `getSolution`, `setSolution` / `boundSolution` (`Include/FMTSrModel.h:173,329`), `doPlanning(false, cédules)` | ~~Le rejeu d'une cédule optimale n'est pas comparé à l'optimisation ; `SetSolution` n'a que des tests BFEC.~~ Couverte : `REPLAY` (lot 4), puis `setSolution`, `setSolutionByLp` et `boundSolution` période par période dans `IntegrationModels` (section 5.14) ; `unboundSolution` et `isPeriodBounded` ont un cas désactivé (défauts 19 et 20, #399 et #400). |
| 8 | **Écriture et relecture** : `FMTModelParser::write` (`Source/FMTModelParser.cpp:467`), `writeToProject` (`Include/FMTModelParser.h:118`), `FMTScheduleParser` | ~~Pas d'idempotence (écrire, relire, réécrire) ni d'aller-retour de cédule ; `WriteAndReadProject` n'a aucune vérification.~~ Couverte au lot 5 (voir ci-dessous) ; restent les sections que cinq défauts empêchent de relire à l'identique (section 2.7, défauts 6 à 10). |

L'optimisation elle-même (objectifs) est l'étape la mieux protégée : 26 objectifs vérifiés.

**État au lot 4** : les lacunes 1, 2 et 3 ont leurs premiers tests (`ReadScenario`,
`ReadScenariosTogether`, `ReadInvalidScenario`). La lacune 4 est couverte : opérabilité (âge,
yield, verrou, `_LOCKEXEMPT`), `operate` et les transitions (cibles, proportions, `_AGE` et
`_LOCK` de cible, sources conditionnées), vieillissement, et les fonctions de yield sauf
`_ENDPOINT` et `_DELTA`. Le lot 4 entame les lacunes 5, 6 et 7 : présolve sur 7 scénarios,
statistiques de la matrice, rejeu de la cédule optimale. Restent les statistiques période par
période, `setSolution` ligne à ligne, et toute la lacune 8, objet du lot 5.

**État au lot 5** (2026-10-19) : la lacune 8 a ses tests de valeur. Au niveau intégration,
`IntegrationParser` écrit, relit et réécrit des scénarios de TWD_land, et compare le modèle relu
(sections, valeurs des yields) et les deux écritures, fichier par fichier : 6 scénarios entiers et 3
sections (transitions, contraintes, outputs) passent, `writeToProject` est vérifié sur un projet neuf
et sur un scénario ajouté, et une cédule l'est par écriture simple et par ajout. 7 cas sont
désactivés, sur #347 et sur les défauts 6 à 10 de la section 2.7 (#379 à #383). Au niveau système,
`WriteAndReadProject`, `SolveAndWriteSchedules`, `UpdateNonSpatialModel` et `CreateModel` vérifient ce
qu'ils écrivent. Détail en section 5.7.

**État au 2026-10-26** (section 5.14) : les lacunes 6 et 7 sont couvertes au niveau intégration (`IntegrationModels`,
suite `FMTSrModel`, décision de Gabriel du 2026-10-26). `buildPeriod` est vérifié période par période sur `LP`,
`division`, `equation` et `harvestfromperiod3`, dont la récolte n'entre dans la matrice qu'à la période 3, et la
construction période par période donne le graphe de `doPlanning`. La cédule optimale de `LP`, reposée période par
période par `setSolution` ou `setSolutionByLp`, redonne les outputs de l'optimisation, et `boundSolution` son optimum ;
une cédule de trop de superficie ou d'une action non opérable est refusée. Deux défauts sont révélés : `unboundSolution`
(19) et `isPeriodBounded` (20), section 2.7. L'aller-retour d'écriture des contraintes spatiales a son cas dans
`IntegrationParser` (#397).

### 3.2 Ce que chaque lacune veut dire

Le critère de classement demande deux choses. D'abord, une étape que **tout** modèle traverse
passe avant une fonction de niche : une erreur dans la lecture d'un `.yld` touche tous les
calculs, une erreur dans l'ordonnanceur d'aires d'opération ne touche que ceux qui s'en
servent. Ensuite, une erreur **silencieuse** passe avant une erreur qui lève : une exception
arrête le calcul et se voit, alors qu'un mauvais nombre se retrouve dans un plan
d'aménagement sans que personne ne s'en aperçoive.

1. **Lecture d'un scénario** (étape 1 de la section 0.1). Tout part de là : un attribut mal rangé,
   un yield mal interpolé ou une contrainte mal bornée, et tout le reste est faux sans qu'aucune
   exception ne soit levée. Avant le lot 2, rien ne regardait le modèle lu : les tests
   vérifiaient des objectifs, qui ne bougent pas forcément quand la lecture change.
   `ReadScenario` interroge maintenant le modèle lu, une ligne CSV par question.
2. **Lecture de plusieurs scénarios d'un coup.** `referenceRead` réutilise les sections déjà
   lues d'un autre scénario pour éviter de tout relire ; l'interface et plusieurs tâches
   lisent ainsi. Si la réutilisation prend une section au mauvais scénario, le modèle obtenu
   n'est pas celui du fichier, sans message. `ReadScenariosTogether` lit les mêmes
   scénarios ensemble puis un par un, écrit les deux modèles et compare les fichiers : aucune
   valeur de référence à maintenir.
3. **Erreurs de lecture.** Un modèle invalide doit lever un `FMTexc` précis, que l'interface
   traduit pour l'aménagiste. Si une erreur cesse d'être signalée, ou change de code, un
   modèle faux passe pour bon. `ReadInvalidScenario` lit des scénarios `INVALID_*`, chacun copié
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
   d'écriture étaient connus avant le lot 5 (section 7) et personne ne les avait vus, faute de test.
   L'aller-retour est métamorphique : écrire, relire, réécrire, et les fichiers doivent être
   identiques. Le lot 5 a montré que cela ne suffit pas : un `_DISCOUNTFACTOR` perdu à la première
   écriture se relit et se réécrit à l'identique. Le modèle relu est donc aussi comparé au modèle
   écrit, section par section et par les valeurs de ses yields.

### 3.3 Gains rapides : des lignes CSV sans code

Chacune demande une reconfiguration de CMake et, sauf pour les tests métamorphiques, une
valeur mesurée une fois puis vue rouge.

- `Presolve` sur d'autres scénarios (métamorphique, aucune valeur à fixer ; il reste en
  CLP) : par exemple `LP`, `COS`, `Shift`, `TSLA`, `fullcarbon`, `equation`.
- `WriteModel` sur d'autres scénarios (métamorphique, en MOSEK).
- `DoPlanning` sur des scénarios inutilisés (section 4.4), après avoir vérifié qu'ils sont faits
  pour être optimisés.
- `SimulateNonSpatially` : les lignes `randomYield` et `randomYieldUnit` n'ont pas de valeur attendue ; à
  fixer si le résultat est déterministe (graine 0).

### 3.4 Niveau 2 : flux courants

- **Tâches** : `RunPlanningTask`, `ReplanConcurrently`, `ReplanWithModelAdaption`,
  `LogThroughCallback` ne vérifient rien ; `ReplanOnDemand` n'a que des tests BFEC.
- **SES** : `SimulateSpatially` ne vérifie rien ; le contrôle des patchs de
  `DoSpatiallyExplicitPlanning` est mort ; `EvaluateSesConstraints`, `DoSpatialPlanning`
  et `WrapperCoreRunSes` n'ont que des tests BFEC.
- **Cartes et rasters** : `MapToForest` et `ConvertVectorFieldToRaster` ne vérifient rien ;
  `ConvertPointsToRasters` n'a que des tests BFEC.
- **NSS** : `SimulateNonSpatially` vérifie 5 lignes sur 7.
- **Core du wrapper** : couvert en base, sauf `AggregateAllActions`, `SplitActions`,
  `BuildAction`, `OperatingArea` et `SES` (BFEC) et `SA` (hors ctest).

### 3.5 Niveau 3 : fonctions spécialisées

- Recuit simulé : `SolveWithSimulatedAnnealing` (BFEC), `WrapperCoreRunSimulatedAnnealing` (hors ctest).
- Ordonnanceur d'aires d'opération et adjacence : `RunOpAreaSchedulerTask`,
  `ScheduleOperatingAreas`, `GetOperatingAreaNeighbors`, `ReadOaSchedulerParameters` (BFEC).
- Transformations d'actions (TBE) : `AggregateActions`, `BuildAction`, `BuildActionOnMap`, `SplitActions` et leurs équivalents du Core (BFEC).
- Modèles de rendement ONNX : `SimulateWithYieldModel`, `PlanWithLpYieldModel` et `OptimizeWithLpYieldModel` ne vérifient rien ;
  les scénarios `DECISION`, `randomYield`, `TSLA`, `UNIT_COVERAGE` et `COMPLEX_NSS` sont
  vérifiés par `SimulateNonSpatially` ou `DoPlanning`.
- Regroupement d'UA : `ClusterOperatingAreas` (une solution existe) ; prédicteurs :
  `GetCarbonPredictors` (rien) ; `GetRotations`, `GetYieldsFromGraph`, `ReadDatabaseSections`,
  `SolveTwentyPeriods`, `SetSolutionByLp` (BFEC).
- Géométries : `UnitTestFMTAreaParser` à restaurer si son API existe encore. Depuis #394 et #396, `getSchemeNeighbors`
  a quatre cas d'intégration sur des cartes synthétiques, avec ou sans Z (sections 5.12 et 5.13) ; `getClusters` n'a que `ClusterOperatingAreas`.

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
  `STANLOCK`. `Carte/TWD_LAND_forcesolution_modif.*` sert à `ForceSolution`.
- `rasters/` : `AGE.tif`, `THEME1-3.tif`, sans `STANLOCK.tif`.
- `TWD_land.db` (SQLite) sert aux scénarios `DB_test*` ; `Templates/` à `ReadTemplates`.

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
  limite de 260 caractères de Windows (section 7) ;
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
| Tests négatifs (lacune 3) | **fait au lot 2** | `INVALID_undefinedoutput` (34), `INVALID_undefinedattribute` (19), `INVALID_undefinedaggregate` (19), `INVALID_invalidnumber` (12), `INVALID_undefinedyield` (8), `INVALID_leakingtransition` (38), `INVALID_unclosedforloop` (81) : chacun copie une section de la racine et n'en change qu'une ligne, commentée en tête. `INVALID_sourcecondition` (31) s'y ajoute au lot 3, désactivé tant que le défaut 4 de la section 2.7 tient |
| Opérateurs de yields complexes | **fait au lot 2** | deux blocs `*YC` à valeurs calculables à la main : `yieldoperators`, mélangé à des équations numériques (section 2.7), et `yieldoperatorsalone`, sans équation numérique, où les mêmes opérateurs sont justes |
| Opérabilité, transitions, vieillissement (lacune 4) | **fait au lot 3** | racine pour les 5 actions et le `_DEATH` ; `transitions` pour les proportions 60/40, l'`_AGE` et le `_LOCK` de cible, et deux sources conditionnées par `@AGE` et `@YLD` |
| Fonctions de yield qu'aucun scénario n'utilise | **fait au lot 3** sauf `_ENDPOINT` et `_DELTA` (sémantique à clarifier) | `yieldoperatorsalone` (`_SHIFT`, `_MAX`, `_MIN`, `_YTP`, `_DISTANCE`, `_MAI`, `_CAI`) ; `timeyields` pour un yield de temps et `_DISCOUNTFACTOR` (section 2.7) |
| `*PARTIAL`, `*STRATA`, `_SEQ`, `_INVLOCK` | texte seul, à confirmer | liste heuristique : vérifier d'abord dans les parseurs que ces mots-clés existent |
| Rejeu `SetSolution` (lacune 7) | **fait** (section 5.14), sans `SetSolution` | `IntegrationModels` repose la cédule optimale de `LP`, tirée de l'optimisation : aucun output codé en dur ni scénario nouveau. `SetSolution` garde ses lignes BFEC |
| Structure qui change d'une période à l'autre (lacune 6) | **fait** (section 5.14) | `harvestfromperiod3` : les actions de la racine, `COUPEtotale` opérable à partir de la période 3 (`_CP >= 3`) |
| Contraintes `_SIZE`, `_ADJACENCY` et `_GROUP` écrites et relues (#397) | **fait** (section 5.14) | `spatialconstraints` : les contraintes de `Spatial` et une `_GROUP(coupetotale,_TH1,2)`, qu'aucun autre scénario n'a |
| SES, rastérisation | carte existante | aucune |
| Recuit simulé | à évaluer | scénario `Spatial` et rasters existants ; `SolveWithSimulatedAnnealing` lit aussi `STANLOCK.tif`, absent |
| Ordonnanceur d'UA | carte synthétique probable | yield `YOUVERT` utilisé par une action, CSV de paramètres, thème d'UA en argument (codé 14 dans `RunOpAreaSchedulerTask`), plus de polygones |
| Adjacence, patchs | carte synthétique | `FMTLayer` construit en mémoire (constructeur depuis `std::map<FMTCoordinate,T>`), ou écrit par `FMTAreaParser::writeForest` / `writeLayer` : ni Python ni GDAL Python requis |
| Transformations d'actions | à évaluer | structure d'actions agrégées à reproduire |
| Opérateurs stricts d'une condition d'action, bloc `*YT` en colonnes | **fait au lot 7c** | `strictbounds` (`_AGE` avec `>`, `>=`, `<` et `<=`, défaut 18 de la section 2.7) et `timeyieldcolumns` (`_CP`, défaut 13) : chacun copie une section de la racine et y ajoute ses lignes, commentées en tête |
| Transitions GCBM d'un SES (issue #369) | **fait** | `GCBMtransitions` : la racine, sauf la plantation de `unite2` qui passe en `UTR1` ; une clé GCBM sur THEME3 finit alors avec deux attributs, dont le majoritaire n'est pas le premier en ordre alphabétique. `GetGCBMTransitions` le lit avec la cédule de `Spatial` |

**Scénarios existants qu'aucun test C++ n'utilise** : `actionrecurse`, `AVERAGEP`, `peak`,
`Predeclared_Primary`, `Predeclared_Primary_ref`, `Spatial_8`, `LPwfire`, `Globalex4`,
`Globalex5`. Les exemples Python et R lisent `Globalex1/2/7`, `Globalnofire` et `Localex1/3-6`.

## 5. Journal des lots

### 5.1 Lot 0 : analyse et ce fichier (2026-09-16)

**Statut** : livré le 2026-09-16. Documentation seulement, aucun code modifié.

- Exploration de l'infrastructure (CSV, `CMakeLists.txt`, dernier `LastTest.log`), des sources
  des tests base et de TWD_land ; décisions de Gabriel en section 1.
- Constats :
  - deux lignes de `basetests.csv` ignorées (section 2.5) ;
  - 17 exécutables base sans vérification (section 2.3), trois vérifications mortes (section 2.4) ;
  - `sumandavgtest` réécrit `Examples/Models/TWD_land/Scenarios/sumavg/TWD_land._seq` à chaque
    ctest (`FMTPlanningTask` reçoit le fichier primaire) ; contenu identique pour l'instant ;
  - ctest tourne avec `-j 8` et les 3 lignes de `testreadwriteproject` écrivent dans le même
    projet `build/release/tests/testreadwriteproject/test.pri` ;
  - `UnitTestFMTAreaParser` supprimé par `f06f55fd` ;
  - `cmake/BaseInstall.cmake` lance ctest sans vérifier son résultat (consigné, pas proposé).
- Tests BFEC désignés par exécutable seulement ; pas de chemin privé dans ce fichier.

### 5.2 Lot 1 : filets et en-tête commun (2026-09-16)

**Statut** : livré le 2026-09-16, compilé par Gabriel le 2026-09-17, **validé** le 2026-09-17 :
suite base de 83 tests, 82 verts et `teststdconstraints` désactivé, en 9,3 s avec `-j 8` (voir
« Validation » ci-dessous).

- **`Examples/C++/CMakeLists.txt`** :
  - alerte de configuration pour une ligne sans cible ;
  - `SKIP_RETURN_CODE 77` et `FIXTURES_REQUIRED ExamplesModels` sur chaque test des CSV ;
  - `RESOURCE_LOCK` pour 4 exécutables ;
  - lignes de `knownbugs.csv` inscrites `DISABLED` ;
  - tests `ExamplesModelsSnapshot` et `ExamplesModelsUnchanged` ;
  - `enable_testing()` sort de la boucle des CSV (section 2.1).
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
- **`Examples/C++/tests/TestTools.h`** : section 1.5, « Patron d'un test ».
- **`sumandavgtest.cpp`** :
  - travaille sur `tests/sumandavgtest/TWD_land`, une copie ;
  - écrit ses sorties dans `tests/sumandavgtest/outputs`, hors du dossier de la copie ;
  - rend 77 sans OSI ;
  - édité octet par octet, cp1252 conservé.
- **CSV** : `coordinatetest` retiré de `basetests.csv` ; `stdconstraints` devient
  `teststdconstraints` dans `knownbugs.csv` (section 2.5).
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

### 5.3 Lot 2 : lecture (2026-09-17)

**Statut** : livré le 2026-09-17 dans le worktree `new_test`, compilé par Gabriel le 2026-09-17,
**validé** le 2026-09-17 : suite base de 167 tests, 159 verts et 8 désactivés, en 13,8 s avec
`-j 8` (voir « Validation » ci-dessous). Reste à inscrire le numéro des issues de la section 2.7.

- **Trois exécutables** dans `Examples/C++/` :
  - `testScenarioReading` : requêtes sur un scénario lu (section 1.5, « Patron d'un test ») ;
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
  bogues de la section 2.7.
- **`teststdconstraints.cpp` supprimé** (section 2.5) ; `Scenarios/stdconstraints/TWD_land._out`
  définit `TEST2`.
- **Nouveaux scénarios** : `yieldoperators` et les 7 `INVALID_*` (section 4.4).
- **Correctif de `TestTools.h` (défaut du lot 1)** : `printException` appelait
  `std::rethrow_if_nested` sur une exception FMT dont le pointeur imbriqué est vide, donc
  `std::terminate`. Toute exception FMT qui s'échappait d'un test finissait en arrêt anormal
  au lieu d'un retour à 1. Le parcours passe par `visitNested`, qui vérifie `nested_ptr()`
  comme `FMTExceptionHandler::throwNested`.
- **Écarts au plan** :
  - `teststdconstraints` n'est pas réécrit mais remplacé par des lignes du test générique ;
  - les bogues de yields (section 2.7) ont été trouvés en posant les valeurs attendues des yields
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
- **Bogues de la section 2.7** recoupés avec l'exécutable officiel `testWrapperCoreGetYield` et avec
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
- **Lignes de `knownbugs.csv` lancées à la main** : les 8 échouent encore, avec les valeurs de la
  section 2.7 (36 ; 8,82975442 ; 5 ; 6 ; 120 ; 5 ; 5 ; 5).
- **Durées des nouveaux tests** sous `-j 8` :
  - `testScenarioReading` : 5,7 s cumulées pour 63 lignes, 0,17 s au plus ;
  - `testJointScenarioReading` : 13,3 s pour 7 lignes, dont 9,85 s pour la ligne
    `fullcarbon` (4,3 s lancée seule) ;
  - `testReadingErrors` : 23,5 s pour 7 lignes, mais de 0,05 à 0,08 s par ligne lancée
    seule. Ce sont les tests d'optimisation lancés en même temps qui ralentissent tout (section 8).

### 5.4 Complément du lot 2 : démonstrations de la section 2.7 (2026-09-17)

**Statut** : livré le 2026-09-17 (12 lignes CSV et deux scénarios, aucun code), **validé** le
2026-09-18 après la reconfiguration de Gabriel : 179 tests inscrits, 168 verts, 11 désactivés,
13,7 s réelles. Les 9 lignes de `yieldoperatorsalone` coûtent 0,5 s en tout.

- Demandé par Gabriel, qui doutait de la description du deuxième bogue. La cause a été trouvée
  dans le code et démontrée avec les exécutables du build (section 2.7) : ce n'est pas « `_SUM` ne
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
  3 lignes connues échouent avec les valeurs de la section 2.7 ; au rejeu des CSV découpés comme
  `Examples/C++/CMakeLists.txt`, 176 noms de tests, aucun doublon. La suite déjà inscrite
  (167 tests) reste verte après la modification du scénario `yieldoperators`.

### 5.5 Lot 3 : objets du cœur (2026-09-18)

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
- **3 lignes désactivées** : `_DISCOUNTFACTOR` (section 2.7, défaut 3).
- **Valeurs** calculées à la main depuis `TWD_land.trn`, `.act`, `.lif` et la table d'âge de la
  racine.
- **Écarts au plan** : `_ENDPOINT` et `_DELTA` ne sont pas couverts, leur sémantique restant à
  clarifier ; `_DISTANCE` est testé avec des bornes qui évitent son ambiguïté d'âge (section 7).

Vérifications (hors build, 2026-09-18) :

- **Compilation** du test modifié avec `cl` et les options du `.vcxproj`, lié au `FMTlib.lib`
  du build de `new_test`.
- **Rejeu des CSV** découpés comme `Examples/C++/CMakeLists.txt` : 209 noms de tests, aucun
  doublon ; les 57 lignes rejouées (lots 2 et 3) se comportent comme attendu.
- **Vu rouge** des 30 lignes base : valeur faussée sans recompiler, aucune ne survit.
- **Syntaxe des sources de transition** : une sonde montre que `@AGE(10.._MAXAGE)` et
  `@YLD(volumetotal,100.._MAXAGE)` sont bien lues, alors que `_AGE >= 10`, `@AGE(12)`,
  `@AGE(10..)` et `@YLD(y,100..)` sont ignorées sans message (section 7).
- **Encodages** : nouveaux fichiers en ASCII et CRLF ; aucun U+FFFD introduit.

Validation (après le build de Gabriel, 2026-09-18) :

- **Suite base** (`-E "T:/" -j 8`) : 212 tests, 198 verts, 14 « Not Run (Disabled) », 11,3 s
  réelles. Les 22 lignes `OPERATE`, `GROW`, `transitions` et `timeyields` coûtent 1,9 s en
  tout. Après l'ajout du complément ci-dessous et une reconfiguration : 213 tests,
  198 verts, 15 désactivés, 12,9 s réelles et 99,7 s cumulées (section 8).
- **Vu rouge sur le vrai build** : `OPERATE` avec un âge faux, `GROW` avec un verrou faux,
  `OPERABLE` verrouillé inversé et la forme `FMTexc(41)` au lieu de `FMTexc(79)` rendent 1.
- **Les lignes désactivées**, lancées à la main, échouent toutes avec les valeurs de la section 2.7,
  dont les trois du `_DISCOUNTFACTOR` (0,000128600823 et 1,65e-08).
- **Fixture** verte et `git status` inchangé sous `Examples/Models`.

Complément (2026-09-18, après validation) : Gabriel tranche que la condition de source ignorée
est un défaut. Le scénario `INVALID_sourcecondition` et sa ligne désactivée de
`testReadingErrors` (`FMTexc(31)`) sont ajoutés ; une reconfiguration suffit, sans compilation.
La ligne échoue aujourd'hui, la lecture ne levant rien, et une requête `OPERATE` montre au
passage qu'un développement de 5 ans passe par une source qui demande 10 ans.

### 5.6 Lot 4 : construction et optimisation (2026-09-18)

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

### 5.7 Lot 5 : écriture et relecture (2026-10-19)

**Décision de Gabriel (2026-10-19)** : l'aller-retour d'un modèle et celui d'une cédule sont des tests
d'intégration (un composant, les parseurs, avec sa vraie dépendance, les formats de fichiers), et non
un nouvel exécutable système. Ils forment le premier composant de `Tests/Integration` : l'exécutable
GoogleTest `IntegrationParser`, étiquettes `integration;parser` (section 9.2).

**Sonde A/B sur les exceptions de `FMTModelParser::write`** (exécutable `WriteModel.exe` du build,
lancé d'un dossier de travail du scratchpad, scénario `LEVEL1`) :
- A, dossier de sortie présent : écriture des 10 fichiers, relecture, même optimum, code 0 ;
- B, dossier de sortie absent : `FMTexc(13)` sur `LEVEL1.pri`, puis la relecture n'a jamais lieu et
  le processus finit en `0xC0000409` (exception sortie de `main`) ;
- C, `LEVEL1.pri` présent en lecture seule : même fin que B, et aucun autre fichier n'est écrit.

`write` relance donc son exception hors du build R, comme la phase 3 l'avait lu dans le code
(`printExceptions`, puis `throwNested`) : la note de la section 7 était fausse et a été corrigée.
Pas de ligne de dette ni d'issue.

**Ajouts.**
- `Tests/Support/TestRegistration.cmake` : `registergoogletests(<cible> <niveau> <composant>
  [EXAMPLES_MODELS] <sources>)`. L'option donne aux cas la fixture `ExamplesModels`. Vérifié dans un
  projet CMake jetable (cmake et ninja de VS 2022, GoogleTest de vcpkg) : les cas `Integration.*` ont
  `FIXTURES_REQUIRED=ExamplesModels` et les étiquettes `integration;parser`, un cas `DISABLED_` est
  inscrit désactivé, les cas unitaires restent sans fixture.
- `Tests/Integration/CMakeLists.txt` inclut `Parser/CMakeLists.txt`, qui crée `IntegrationParser` à
  partir des `.cpp` du dossier. `ParserRoundTrip.h` : dossier de sortie par cas
  (`build/release/tests/IntegrationParser/<cas>/`), lecture d'un scénario, aller-retour
  (`writeRereadAndRewrite`), comparaison des fichiers ligne à ligne (reprise de
  `ReadScenariosTogether`), des sections par leur `operator==`, et des yields par leurs valeurs.
- Les yields se comparent par valeurs, car `FMTYields::operator==` compare des adresses (#383, défaut 10 de
  la section 2.7) : tout yield du modèle lu, hors noms en `_` et `~`, pour chaque développement de
  l'inventaire, aux âges 0 à 30 et aux périodes 1 à 3, à 1e-9 près en relatif ; un yield qui lève doit
  lever des deux côtés. Une requête par appel : une `FMTYieldRequest` garde le premier `FMTYields` qui
  l'a servie (section 7, ligne de dette de `FMTYieldRequest` complétée dans `Architecture.md`).

**Cas de `FMTModelParser`** (`Tests/Integration/Parser/FMTModelParser.cpp`, 18 cas, 11 verts) :

| Cas | Scénario | Vérifie | État |
|---|---|---|---|
| `GivenTheRootModelWhenWrite…ThenRereadsTheSameModelAndRewritesTheSameFiles` | ROOT | modèle et fichiers | désactivé : #347 (#379 corrigé par `83e2309e`) |
| `GivenYieldsOnNegatedMasks…` | `NOT_MASK` | modèle et fichiers | vert |
| `GivenEquationsInYieldsOutputsAndConstraints…` | `equation` | modèle et fichiers | vert |
| `GivenItsOwnLandscapeActionsAndTimeYields…` | `COS` | modèle et fichiers | vert |
| `GivenAMeanAnnualIncrementYield…` | `mai` | modèle et fichiers | vert |
| `GivenAShiftedYield…` | `Shift` | modèle et fichiers | vert |
| `GivenADistanceYieldAndARatioOfInventoryOutputs…` | `DISTANCE` | modèle et fichiers | vert |
| `GivenRandomActionConstraints…` | `COMPLEX_NSS` | modèle et fichiers | désactivé : #382 |
| `GivenSplitTargetsAgesLocksAndSourceConditions…ThenRereadsTheSameTransitions` | `transitions` | transitions et `.trn` | vert |
| `GivenThousandsSeparatorsAndListsOfPeriods…ThenRereadsTheSameConstraints` | `stdconstraints` | contraintes et `.opt` | vert |
| `GivenAPenaltyObjectiveWithoutSign…ThenRereadsTheSameObjective` | ROOT | contraintes et `.opt` | actif depuis la correction de #379 (`83e2309e`) |
| `GivenOutputsBuiltByForLoops…ThenRereadsTheSameOutputs` | `ForLoops` | outputs et `.out` | vert |
| `GivenARatioOfAnInventoryOutputToAnActionOutput…ThenRereadsTheSameOutputs` | `TSLA` | outputs et `.out` | désactivé : #381 |
| `GivenDiscountFactors…ThenRereadsTheirValues` | `timeyields` | valeurs des `DISCOUNT*` | désactivé : #380 |
| `GivenADiscountRateFromAConstant…ThenWritesTheModel` | `discountconstant` | `write` ne lève pas | désactivé : #380 |
| `GivenNoPrimaryFileWhenWriteToProjectThenWritesTheSameFilesAsWrite` | `NOT_MASK` | fichiers de `writeToProject` = ceux de `write` | vert |
| `GivenAProjectWhenWriteToProjectThenRereadsTheScenarioAsTheModelWritten` | `NOT_MASK`, puis `equation` en scénario | modèle relu du projet et fichiers | vert |
| `GivenAScenarioThatOnlyChangesItsObjectiveWhenWriteToProject…OnlyItsOptimizeAndScheduleSections` | `NOT_MASK` modifié | sections écrites du scénario | désactivé : #383 |

Les scénarios qui gardent le `.yld` de la racine (`transitions`, `stdconstraints`, `ForLoops`) ne
sont comparés que sur leur section, sans quoi #347 les ferait tous échouer : ce n'est pas un
contournement, chaque cas vérifie la section que son scénario définit, et la racine a son cas désactivé.

**Cas de `FMTScheduleParser`** (`Tests/Integration/Parser/FMTScheduleParser.cpp`, 2 cas verts), sur la
cédule de `LP`, sans solveur :
`GivenTheSchedulesOfAScenarioWhenWriteThenRereadsAndRewritesTheSameSchedules` (relecture égale par
`FMTSchedule::operator==`, réécriture identique) et
`GivenAScheduleFileWhenWriteWithAppendThenRereadsTheSchedulesOfBothWrites` (période 1 écrite, les
suivantes ajoutées).

**Exécutables système** (sources en Arrange / Act / Assert, solveurs inchangés) :
- `WriteAndReadProject` (CLP) : vide son dossier de sortie au départ, ce qu'il ne faisait pas (le
  projet d'un passage précédent devenait la racine des scénarios suivants, sans que rien ne le voie) ;
  chaque scénario relu du projet, résolu de nouveau, doit atteindre l'optimum du modèle écrit.
  `argv[3]` facultatif donne l'ordre de relecture ; un autre ordre doit échouer. Then
  `RereadsEachScenarioWithItsOptimum` (3 lignes) ;
- `SolveAndWriteSchedules` (CLP) : le modèle doit être résolu, et la cédule écrite, relue par
  `FMTScheduleParser::read`, doit redonner la superficie de chaque développement de chaque action à
  chaque période, à 1e-5 près (écriture à 6 décimales). Entrées explicites (`TWD_land.pri`, `LP`),
  `argv[3]` facultatif le scénario dont la cédule est attendue. Then `RereadsTheOptimalSchedules` ;
- `UpdateNonSpatialModel` (MOSEK) : le modèle déplacé à la période 3 garde la superficie totale de
  l'inventaire, 1814,76 ha calculés à la main (403,28 + 7 × 201,64, dans le CSV), et le modèle écrit
  se relit avec le même nombre de développements, cette superficie et les contraintes décalées. Then
  `KeepsItsTotalAreaAndRereadsItsShiftedConstraints` ;
- `CreateModel` (MOSEK) : le modèle construit en code est optimal, et le modèle écrit puis relu
  atteint le même optimum (175810,134619). Ses sections ne sont pas comparées : un modèle construit en
  code n'a pas les noms d'attributs qu'une lecture remplit, et FMT y ajoute un `_DEATH` par défaut
  (thèmes, transitions et contraintes relus diffèrent, sans effet sur l'optimum ; non analysé plus
  loin). Then `IsOptimalAndRereadsWithTheSameOptimum` ;
- `AddYieldHandlers` : la vérification morte de la section 2.4 compare maintenant, pour chaque type de
  peuplement, `VOLUMETOTALNEXTAGE` à chaque âge de base au `VOLUMETOTAL` cinq ans plus vieux ; une
  valeur interpolée vient du CSV : `UNITE1 PEUPLEMENT1 UTR1` à 2 ans vaut 120, le `volumetotal` de
  `peuplement1` à 7 ans (100 + 2/5 × 50, `TWD_land.yld`). Then `GivesEachStandTheVolumeFiveYearsOlder`.

Les 7 lignes de `basetests.csv` changent de Then ; 3 reçoivent leurs entrées. Les 7 noms d'aujourd'hui
de `Tests/Support/RenamedTestCases.csv` suivent. Projet CMake jetable sur des copies des CSV, cibles
factices : 240 lignes publiques inscrites, aucun avertissement, les 7 noms attendus, les arguments
transmis tels qu'au paragraphe ci-dessus.

**Vu rouge** (règle 1), sur le build de `new_test` (FMTlib du 2026-10-16), rien de commité :
- `IntegrationParser`, copie hors dépôt compilée par `cl` contre `FMTlib.lib` (section 7) : modèle
  relu remplacé par un modèle vide, seconde écriture prise d'un autre scénario (`LEVEL1`), références
  de `writeToProject` prises d'un autre scénario, cédules attendues décalées d'une période et cédule
  écrite comparée au `_seq` d'origine : les 13 cas verts échouent, et chaque vérification échoue au
  moins une fois (les 7 sections, les valeurs des yields, les fichiers `.act` `.are` `.lan` `.opt`
  `.out` `.pri` `.trn` `.yld`, les trois cas de section, les deux de `writeToProject` et les deux de
  cédule). Une seconde copie vide les fichiers écrits et cherche un préfixe de yield inconnu : les deux
  garde-fous échouent. Les 7 cas désactivés échouent, chacun pour sa raison, avec
  `--gtest_also_run_disabled_tests` ;
- sans recompiler, par un argument faussé : `AddYieldHandlers` attendu à 121 ; `UpdateNonSpatialModel`
  à 1814 (les deux superficies) ; `WriteAndReadProject` relu dans l'ordre `LP2|LP|LP4|LP5` (LP et LP2
  ont des optimums différents, 34660,742098 et 4240) ; `SolveAndWriteSchedules` comparé à la cédule de
  `LP2`, puis de `ose_infaisable` (non résolu) ;
- par une copie faussée compilée : la comparaison par type de peuplement et les garde-fous de
  `AddYieldHandlers`, les deux « optimal » et l'optimum de `CreateModel`, la résolution, le nombre de
  développements et les contraintes de `UpdateNonSpatialModel`, le compte et la résolution de
  `WriteAndReadProject`.

**Vu vert avant le build de Gabriel** : les mêmes copies, non faussées, compilées hors dépôt et
lancées depuis `build/release/bin/Release` : 13 cas d'intégration verts en moins d'une seconde, et les 7 lignes
système vertes avec les arguments de `basetests.csv`. Les sources du dépôt (cp1252, CRLF) ont été
recompilées telles quelles, sans avertissement à `/W4` dans les nouvelles sources.

**Ce que le lot apporte à #350** : le premier composant du niveau intégration, et cinq défauts
caractérisés par des cas désactivés, des tests de caractérisation avant correction.

**Observations laissées ouvertes** : la transition `_DEATH` ajoutée par défaut par FMT n'est pas égale
(`FMTTransition::operator==`) à sa forme écrite puis relue, à texte identique (`level`, `CreateModel`) ;
`WrapperCoreWriteToProject` ne vide pas son dossier de sortie, si bien que ses passages suivants
écrivent le modèle comme scénario d'un projet resté du premier (fichiers racine du 2026-10-16).

**Validation après le build de Gabriel** (2026-10-20, commit `90ef4ad2` de Gabriel, FMTlib inchangé
depuis le 2026-10-16) :
- passage de l'installation (`INSTALL_TEST_SUITE=public`) : 300 tests, 0 échec, 281 exécutés et 19
  désactivés, 21 s. `InstallTests.xml`, `InstallTests.json`, `LastTest.log` et
  `LastTestsDisabled.log` copiés dans le scratchpad de la session avant tout appel de ctest ;
- `ctest --preset <p> -N` : `unit` 7, `integration` 22 (les 20 cas et les 2 fixtures, que la
  fixture `ExamplesModels` amène), `system` 250, `system-private` 94, `performance` 25, `all-public`
  300 ; 19 désactivés dans `all-public`, dont les 7 cas `Integration.`, et aucun nom en `T:/` ;
- `RunTests.bat all-public` : « 0 tests failed out of 281 », 13,8 s réelles ; tableau unit 7 verts,
  **integration 13 verts et 7 désactivés sur 20**, system 236 verts et 12 désactivés sur 248,
  performance 23, other 2 ; Total 281 verts et 19 désactivés sur 300. Les comptes ne changent que des
  20 cas ajoutés, dont les 7 désactivés, justifiés en section 2.7 ; les 7 lignes système renommées
  passent, et la fixture `ExamplesModelsUnchanged` est verte ;
- issues ouvertes par Gabriel le 2026-10-20 : #379 (`_PENALTY`), #380 (`_DISCOUNTFACTOR`), #381
  (sorties cachées), #382 (`_RANDOM`), #383 (`FMTYields::operator==`). Leurs numéros sont dans les
  commentaires des cas désactivés et en section 2.7 (commentaires seuls : la recompilation hors dépôt
  des deux sources ne donne ni erreur ni avertissement) ;
- non relue : la console de la configuration de Gabriel. L'absence de ligne ignorée se déduit des
  250 tests `system` : 240 lignes CSV publiques, `UseExcelCache`, les 7 tests Python et les 2
  fixtures, comme avant le lot.

### 5.8 Session à part : export des classes de Core (2026-10-20)

**Décision de Gabriel (2026-10-20)** : les classes de Core qu'un test lié à la DLL de FMTlib ne peut
pas atteindre sont exportées dans une session à part, avant le lot 7, plutôt que contournées. C'est une
modification de FMTlib, hors d'un lot de tests (section 1.2) : des déclarations seulement, aucun
comportement.

**Démonstration A, avant l'export.** Une sonde hors du dépôt (`ExportProbe.cpp`, scratchpad de la
session), compilée par `cl` avec les options de `UnitCore` (son `CL.command.1.tlog`, sans GoogleTest)
et liée au `FMTlib.lib` de `build/release`, lit la racine de TWD_land, puis construit et appelle
chacune des 23 classes. Le lien échoue sur 64 symboles dans 21 classes : 57 LNK2019, et 7 LNK2001
venus de la table virtuelle de `FMTYieldModelDecisionTree`, construite par la sonde. `dumpbin
/exports` ne trouve aucun membre de ces classes dans `FMTlib.dll` (35 pour `FMTYields`, témoin).

| Classe | Symboles non résolus |
|---|---|
| `FMTActionComparator` | 4 : constructeur, `getAllAggregates`, `getAllAggregatesSet`, `operator()` |
| `FMTActualDevelopmentComparator`, `FMTMaskComparator`, `FMTOutputSourceComparator`, `FMTThemeComparator`, `FMTTransitionComparator` | 2 chacune : constructeur, `operator()` |
| `FMTOutputNodeValueComparator`, `FMTOutputNodeOriginComparator`, `FMTOutputNodeHasher` | 1 chacune : `operator()` |
| `FMTYieldRequest` | 7 : deux constructeurs, `getDevelopment`, `getDatas`, `getResumeMask`, `getVertexGraphInfo`, `getFirstSeen` |
| `FMTExpression` | 6 : deux constructeurs, `getVariables`, `shuntingYard`, `getInFix`, `operator std::string` |
| `FMTFunctionCall` | 3 : constructeur, `call`, `valid` |
| `FMTYieldsCache` | 5 : `inCache`, `get`, `tryToGet`, `set`, `reserve` |
| `FMTYieldDevelopment` | 4 : constructeur, `operator==`, `hash`, `getPeriod` |
| `FMTYieldModel` | 7 : constructeur, destructeur, `getModelName`, `getModelYields`, et par la table virtuelle `setModel`, `operator std::string`, `clearRandomYieldsCache` |
| `FMTYieldModelDecisionTree` | 6 : `getModelType`, `getPeriodicValues`, et par sa table virtuelle `clone`, `predict`, `preSolve`, `postSolve` |
| `FMTYieldModelNep`, `FMTYieldModelPools` | 1 chacune : `getModelType` |
| `FMTYieldModelRandom` | 2 : constructeur, `getModelType` |
| `FMTYieldModelTsla` | 3 : constructeur, `clone`, `getModelType` |
| `FMTYieldModelUnitCoverage` | 2 : constructeur, `clone` |

- `cmpYieldString` se lie : elle est entièrement définie dans son en-tête.
- `FMTYieldModelNn` : abstraite, et ses dérivées demandent un fichier ONNX. La sonde prend l'adresse de
  `predict`, virtuelle, que MSVC atteint par un thunk local, sans demander de symbole. Ses membres
  définis dans `Source/` ne sont pas plus exportés que les autres (`dumpbin`).

**Export.** `FMTEXPORT` devant le nom de 18 classes, comme les classes exportées de Core
(`class FMTEXPORT FMTAction`), et `#include "FMTutility.h"`, où la macro est définie, dans les trois
en-têtes qui ne le voyaient pas : `FMTExpression.h`, `FMTFunctionCall.h`, `FMTYieldDevelopment.h`.
Aucune signature, aucun corps, aucun ordre de déclaration ne change. Édition octet par octet en
Python ; vérifié après coup : chaque en-tête, ses ajouts retirés, est identique au blob de `HEAD`, en
CRLF seulement, l'octet 0xE9 de « Québec » intact, sans U+FFFD.

**Laissées, et pourquoi.**
- `cmpYieldString` : entièrement définie dans son en-tête (ci-dessus).
- `FMTYieldModelNn`, `FMTYieldModelNep`, `FMTYieldModelPools` et `FMTYieldModelDecisionTree` :
  exportées, elles cassent la compilation de FMTlib. Une classe exportée voit les membres définis dans
  son en-tête compilés dans toute source qui l'inclut, dont ses constructeurs `= default`. Or
  `FMTYieldModelNn()` détruit, sur le chemin d'une exception, un `std::unique_ptr<Ort::Session>`, et
  `FMTYieldModelDecisionTree()` un `std::vector<FMTConstraint>`, deux types que leur en-tête ne fait
  que déclarer. Rejeu : C2027 et C2338 dans `FMTYieldModelNep.cpp` et `FMTYieldModelPools.cpp`, C2036
  dans `FMTYieldParser.cpp`. `FMTYieldModelNep` ou `FMTYieldModelPools` exportée seule échoue de même
  (C2027 et C2338, dans sa source et dans `FMTYieldParser.cpp`) : son constructeur `= default` appelle
  celui de `FMTYieldModelNn`. Les exporter demande les types complets dans ces trois sources (des
  `#include`), ou ces constructeurs et destructeurs définis dans `Source/` : plus que `FMTEXPORT`.
  **Décision de Gabriel (2026-10-20)** : on les laisse ainsi pour l'instant. Nouvelle ligne de dette
  dans `Architecture.md`.
- Hors du Core, `Models::FMTModelComparator` et `Heuristics::FMTOperatingAreaSchemeComparator` restent
  sans export : la ligne de dette des comparateurs se réduit à eux.

**Hors MSVC.** `FMTEXPORT` (`Include/FMTutility.h`) ne change pas. Avec GCC, il vaut
`__attribute__((dllexport))` pour FMTlib, que le build MSYS2 lie déjà avec `-Wl,--export-all-symbols` :
tous ses symboles sont déjà exportés. Il est vide dans la cible R `FMT`, qui compile les sources sans
`FMTSHAREDLIB` ni `FMTLIBIMPORT`. Non compilé ici, faute de GCC sur le poste : le build R reste à
Gabriel.

**Avertissements.** Rejeu de toutes les unités du build avant et après (`CL.command.1.tlog` de chaque
cible, objets et `.pch` envoyés dans le scratchpad) : 368 unités de 90 cibles, dont les 222 de FMTlib,
`FMT`, `FMTWrapperCore`, `UnitCore`, `IntegrationParser`, les bancs, les tests système et les deux
cibles C++/CLI, leur en-tête précompilé d'abord. Aucune différence d'erreur ni d'avertissement, unité
par unité : avant comme après, un C4244 dans `FMTYieldModelNn.cpp` (`xmemory`) et onze C4273 dans
`FMTExcelWrapper`. C4251 et C4275 sont de niveau 2 (vérifié sur un fichier minimal) : le build, en
`/W1`, n'en montre aucun, pour aucune classe exportée. En `/W2`, les nouvelles classes donnent des
C4251 du même genre que les classes déjà exportées (`FMTAction`, `FMTTheme`, le comparateur exporté
`FMTOutputComparator`), et aucun C4275 : les bases de `FMTYieldRequest` et des modèles de rendement
sont exportées. Rien n'est désactivé.

**DLL d'essai et sonde B, avant le build de Gabriel.** Les 222 objets de FMTlib rejoués, liés par la
commande de son `link.command.1.tlog`, sorties dans le scratchpad : `FMTlib.dll` se lie et exporte
149 symboles de plus, aucun de moins (4590, puis 4739), tous membres ou tables virtuelles des 18
classes. La sonde, sans les trois modèles laissés, se lie à cette DLL et passe ses 32 vérifications
sur TWD_land (`ALL CHECKS PASSED`, code 0). Avec eux, il reste 8 symboles non résolus, tous de
`FMTYieldModelDecisionTree`, `FMTYieldModelNep` et `FMTYieldModelPools`. `FMT.pyd` (`main.cpp`
rejoué) se lie aussi à cette DLL ; il importe désormais de `FMTlib.dll` les quatre membres par défaut
de `FMTYieldRequest`, qu'expose `PYexportCore.h` et qu'il compilait jusqu'ici en ligne : un `FMT.pyd`
de ce build demande le `FMTlib.dll` du même build, ce que la roue livre déjà ensemble.

**Incident.** Le premier rejeu n'envoyait pas `/Fp` dans le scratchpad : l'unité `/Yc` des deux cibles
C++/CLI a réécrit `build/release/FMTWrapper.dir/Release/cmake_pch.pch` et
`build/release/FMTExcelWrapper.dir/Release/cmake_pch.pch` (mêmes entrées, même taille). Rien d'autre
n'a été écrit dans `build/release` (`find -newer`). L'effet annoncé ici, une simple recompilation,
était faux : un PCH managé (`/clr`) garde le chemin de l'objet de son unité `/Yc`, ici celui du rejeu,
supprimé depuis, et MSBuild, qui voyait ces `.pch` plus récents que leurs entrées, ne les a pas
régénérés. Le build de Gabriel a donc échoué sur les 11 unités `/Yu` de `FMTWrapper`, en C1093
(`ImportFile`, fichier introuvable), comme le rejeu quand il supprimait cet objet. Les deux `.pch`
contenaient bien le chemin du scratchpad. Ils ont été supprimés le 2026-10-20 : MSBuild les suit comme
sorties de `cmake_pch.cxx` (`CL.write.1.tlog`) et les recrée au build suivant.

**Validation après le build de Gabriel** (2026-10-20, commit `f4252745` de Gabriel, 22 fichiers) :
- premier build en échec sur `FMTWrapper` (C1093, incident ci-dessus) ; après la suppression des deux
  `.pch`, le second passe. MSBuild a recréé les deux `.pch` (15 h 19 et 15 h 20), sans chemin du
  scratchpad, et recompilé `FMTWrapper` et `FMTExcelWrapper` ;
- `FMTlib.dll` du build : 4739 exports, la même liste que la DLL d'essai (149 de plus qu'avant,
  aucun de moins) ;
- sonde B liée au `FMTlib.lib` du build, lancée depuis `bin/Release` : 32 vérifications vertes, code
  0 ;
- installation (`INSTALL_TEST_SUITE=public`, 15 h 23) : 300 tests, 0 échec, 281 exécutés et 19
  désactivés, 19 s, dont les 7 tests Python exécutés. La roue `FMT-1.3.0-cp310-cp310-win_amd64.whl`,
  construite à 15 h 23 et installée dans le Python de vcpkg, s'importe en mode isolé depuis
  `site-packages`, et sa `FMTlib.dll` est celle de `bin/Release` (même SHA-256). `InstallTests.xml`,
  `InstallTests.json`, `LastTest.log` et `LastTestsDisabled.log` copiés dans le scratchpad avant tout
  appel de ctest ;
- `RunTests.bat all-public` : « 0 tests failed out of 281 », 16,3 s réelles ; unit 7 verts,
  integration 13 verts et 7 désactivés sur 20, system 236 verts et 12 désactivés sur 248,
  performance 23, other 2 ; Total 281 verts et 19 désactivés sur 300, comme au lot 5 ;
- avertissements : la console du build n'a pas été relue. Le rejeu de toutes les unités avec les
  commandes de ce build (ses `CL.command.1.tlog`, `/Fo` `/Fd` `/Fp` dans le scratchpad, `.pch` de
  `build/release` inchangés après coup) ne diffère du rejeu d'avant le changement dans aucune des 367
  unités : toujours un C4244 et onze C4273, rien de nouveau. Les avertissements de l'édition de liens
  ne sont pas couverts ;
- restent à Gabriel : la suite BFEC (`RunPrivateTests.bat`) et le build R.

### 5.9 Lot 7a : expressions et gestionnaires de rendements (2026-10-21)

**Statut** : livré le 2026-10-21, commité par Gabriel le même jour (`0c35c546`, changelog `51d03e83`),
**validé** le 2026-10-21 (fin de cette section). Étapes 1 et 2 de la section 6.3.4, découpage de Gabriel
du même jour (section 6.3.4).

**Ajouts** (`Tests/Unit/Core`, cp1252, CRLF, en-tête de licence) : une source par classe, une suite au
nom de la classe, cas Given…When…Then en Arrange / Act / Assert ; 141 cas nouveaux, dont 4 désactivés.

| Source | Cas | Désactivés | Ce qu'ils couvrent |
|---|---|---|---|
| `FMTOperator.cpp` | 18 nouveaux (20) | 0 | construction par symbole et retour au symbole, symbole inconnu, opérateur par défaut, `call` (arithmétique, virgule, division par zéro : infini), `reverse`, `isFactor`, `isDivide`, `associativity`, copie, égalité, hachage, priorités de `-` et de `,` |
| `FMTFunctionCall.cpp` | 10 | 0 | `EXP` et `LN` arrondis à 8 décimales, `EXP(-20)` arrondi à 0, `LN` de 0 et d'un négatif, noms en minuscules et inconnus, appel par défaut, copie |
| `FMTExpression.cpp` | 22 | 1 | évaluation (priorités, parenthèses, gauche à droite, #346, moins en tête, variables, fonctions), variable sans valeur (`std::invalid_argument`), `getInFix`, chaîne, copie, `getVariables`, `simplify` (facteurs et membre de droite d'une équation linéaire, expression rendue) ; désactivé : défaut 11 (#386) |
| `FMTData.cpp` | 22 | 0 | constructeurs, `constant`, `nullData`, `getSources` (somme, équation), `getValues`, chaînes (`_SUM`, `_DISCOUNTFACTOR`, `_EQUATION`), `toExpression` (dont #347), égalité, copie, produit par un facteur, cache (`set`, `get`, `clearCache`, copie sans cache) |
| `FMTAgeYieldHandler.cpp` | 20 | 0 | `get` aux âges de base, entre eux, avant le premier et après le dernier, yield absent, `getYieldLinearValue` sans hors-bornes (`FMToutofrangeyield`), `setYieldValues` (tailles ou âges différents : `FMTrangeerror`), noms, copie, clone, `getFromFactor`, pic (`getPeakfrom`, `getPeak`), type, chaîne `*Y` |
| `FMTTimeYieldHandler.cpp` | 16 | 3 | valeurs avant, pendant et après les périodes, yield absent, deux yields de même départ, `FULL`, `NONE` et `HALF` sur trois périodes, `setYieldValues` (`FMTrangeerror`), copie, clone, `getFromFactor`, chaîne `*YT` ; désactivés : défaut 12 (#388), #380 (deux cas) |
| `FMTComplexYieldHandler.cpp` | 33 | 0 | `_SUM`, `_MULTIPLY`, `_SUBTRACT`, `_DIVIDE` (par zéro, arrondi à 8 décimales), `_RANGE`, `_MAI` (avec années, après le dernier âge : `FMToutofrangeyield`), `_CAI`, `_YTP`, `_EQUATION` (yields, nombre, fonction), `_MAX`, `_MIN`, `_SHIFT`, `_DELTA`, `_DISTANCE`, source inconnue, yield complexe d'un autre, récursion entre deux gestionnaires (`FMTinvalid_yield`), `indexes` (index de thème, #347), type, copie, clone, chaîne `*YC` |

- `Tests/Unit/Core/ExceptionChain.h` : `Testing::getRaisedTypes` rend les codes des exceptions FMT d'un
  appel, de l'extérieure à la plus profonde (par `Testing::visitNested`), et `Testing::hasType` y
  cherche un code. FMT enveloppe l'erreur dans un `FMTfunctionfailed` à chaque méthode traversée : le
  code attendu est celui d'une exception imbriquée.
- Niveau unitaire : aucun modèle sur disque, objets construits en code, données construites comme
  `FMTYieldParser` les construit (commentaire en tête de `FMTData.cpp`). `FMTComplexYieldHandler` ne
  s'évalue qu'à travers `FMTYields::get` : ses cas mettent dans un `FMTYields` construit en code un
  gestionnaire d'âge (`VOLUME`, `STEMS`, `ZERO`), un de temps (`PERIODVALUE`), puis le gestionnaire
  complexe. Nouvelle ligne de dette dans `Architecture.md` : une sonde hors dépôt montre que, sur une
  requête faite d'un développement seul, `get` lit un `FMTYields` nul (violation d'accès 0xC0000005,
  levée en `FMTexc(85)`). Aucune classe du lot ne demande un modèle ou un état global : aucun cas au
  niveau intégration.
- Valeurs calculées à la main (consigne de Gabriel pour `FMTTimeYieldHandler`),
  `_DISCOUNTFACTOR(4%,5,<type>)` : facteur 1 / 1,04^(5 × exposant), exposant `FULL` = p, `NONE` = p - 1,
  `HALF` = p - 0,5 (section 2.7, item 3), tolérance 1e-12 :

  | Type | Période | Exposant | Calcul | Valeur attendue |
  |---|---|---|---|---|
  | `FULL` | 1 | 1 | 1/1,04^5 | 0,821927106759 |
  | `FULL` | 2 | 2 | 1/1,04^10 | 0,675564168826 |
  | `FULL` | 3 | 3 | 1/1,04^15 | 0,555264502713 |
  | `NONE` | 1 | 0 | 1/1,04^0 | 1 |
  | `NONE` | 2 | 1 | 1/1,04^5 | 0,821927106759 |
  | `NONE` | 3 | 2 | 1/1,04^10 | 0,675564168826 |
  | `HALF` | 1 | 0,5 | 1/1,04^2,5 | 0,906601956075 |
  | `HALF` | 2 | 1,5 | 1/1,04^7,5 | 0,745160722739 |
  | `HALF` | 3 | 2,5 | 1/1,04^12,5 | 0,612467796912 |

  Les autres valeurs : celles de `FMTFunctionCall` et des yields complexes sont les valeurs exactes
  arrondies à 8 décimales, comme `call` et `FMTComplexYieldHandler::get` les arrondissent (e = 2,71828183,
  ln 10 = 2,30258509, ln 200 = 5,29831737, 200/3 = 66,66666667) ; le reste est de l'arithmétique sur des
  valeurs choisies (`VOLUME` vaut 100, 200 et 240 aux âges 10, 20 et 30, donc 150 à 15 ans, 220 à 25 ans
  et 50 à 5 ans, interpolé depuis 0 à l'âge 0).
- **#380** : deux cas désactivés montrent qu'il passe par `FMTTimeYieldHandler::operator std::string` :
  le gestionnaire écrit `DISCOUNTFULL 1 0.04 5`, et lève sans période de départ. `FMTData` écrit pourtant
  `_DISCOUNTFACTOR(0.040000000000000001,5,FULL)` (cas vert), que le parseur relit au même taux depuis
  #374 : la correction de #380 peut s'en servir.
- **#346 et #347** : section 2.7. **Défauts révélés** : 11 et 12, avec leur cas désactivé, et 13, sans
  cas (section 2.7).

**Observations, sans défaut annoncé** (rien de ce que FMT exécute n'en dépend, ou ce n'est pas
démontré) :
- l'opérateur et l'appel de fonction par défaut portent la clé 0, d'aucune énumération : `valid()` les
  accepte (cas verts qui le fixent) ;
- `FMTOperator::associativity` rend `RIGHT` pour `-` et `/`, et `LEFT` pour `^` ;
  `FMTExpression::getVariables` compte les parenthèses parmi les variables. Aucune méthode de FMT ne les
  appelle (cas verts commentés) ;
- `FMTExpression::simplify` traiterait une parenthèse comme une variable, mais le parseur de la section
  optimisation ne lui en passe pas : il ne coupe que sur `+ - * /` ;
- `FMTData::getSummaryDevelopment` calcule une période qu'il n'utilise pas : le cache des yields
  complexes garde toujours la période, sans effet sur les valeurs. `FMTData::cacheValue` sur un data sans
  opérateur lit un cache jamais alloué ; seuls les yields complexes l'appellent ;
- d'après le code, `FMTData::operator*` multiplie aussi le taux et les années d'un `_DISCOUNTFACTOR`
  (`getFromFactor` d'un tel yield, quand on le nomme), et `_MAX` part de 0 (le maximum de yields tous
  négatifs vaudrait 0) : non démontré ;
- `FMTData.h` ne se compile pas sans `FMTDevelopment.h` (section 7).

**Non couvert, et pourquoi** : `getEndPoint` et `_ENDPOINT`, `getAge` (gestionnaires d'âge et complexe,
qui lisent un `FMTSpec`), `complexYldToAgeYld`, les surcharges (`_OVERRIDE` : `setTabou`,
`compareSources`) : leurs cas demandent `FMTSpec` et `FMTYields`, et vont avec le lot 7b ou le lot 8.
`getAllYieldsData` : son seul appelant, `FMTYields::getAllYields`, n'est appelé nulle part.

**Vu rouge** (règle 1), rien de commité :
- une copie faussée des sept sources, hors du dépôt, où un en-tête inclus après `<gtest/gtest.h>`
  retourne chaque assertion (`EXPECT_EQ` en `EXPECT_NE`, `EXPECT_TRUE` en `EXPECT_FALSE`,
  `EXPECT_DOUBLE_EQ` et `EXPECT_NEAR` contre l'attendu plus 1, `EXPECT_THROW` en `EXPECT_NO_THROW`,
  `EXPECT_GT` en `EXPECT_LE`, et de même pour les autres ; le script qui écrit la copie refuse toute
  autre macro), compilée par `cl` contre le `FMTlib.lib` de `build/release` : les 139 cas actifs des
  sept suites échouent (les 137 nouveaux et les 2 de `FMTOperator` d'avant), aucun ne passe, aucun par
  une exception inattendue ;
- les 4 cas désactivés, lancés avec `GTEST_ALSO_RUN_DISABLED_TESTS=1`, échouent chacun pour sa raison :
  `2*-3` vaut -3 ; `PERIODVALUE` vaut 0 et 10 ; l'écriture ne contient pas `_DISCOUNTFACTOR(` ; sans
  période de départ, l'écriture lève ;
- les deux cas de #347, ajoutés après l'accord de Gabriel, sont dans ce passage.

**Vu vert avant le build de Gabriel** : les sources du dépôt, telles quelles, compilées par `cl` avec
les options de `UnitCore` (son `CL.command.1.tlog`), `/Fo` `/Fd` `/Fp` dans le scratchpad, liées au
`FMTlib.lib` de `build/release` (FMTlib de `f4252745`, sources de FMTlib inchangées depuis), lancées
depuis `bin/Release` : 144 cas verts (les 7 d'avant et 137 nouveaux), 4 désactivés, en 1 ms ; mêmes
résultats dans un ordre mélangé (`GTEST_SHUFFLE`, graine 4242, 3 répétitions). En `/W4`, aucun
avertissement ne vient des sources de tests : tous viennent des en-têtes de FMTlib. Rien n'a été écrit
dans `build/release` ni sous `Examples/Models`.

**Documents** : `CoreTestCoverage.csv` (statut `Lot 7a (step <n>), done` des sept classes, défauts 11 et
12, ligne de dette, tests manquants), `CoreTestCoverage.md` régénéré (9 classes avec des tests
unitaires, 41 de priorité 1 sans test propre au lieu de 47 ; deux générations identiques), ligne de
dette d'`Architecture.md`, changelog (section non publiée, anglais et français).

**Validation après le build de Gabriel** (2026-10-21, commits `0c35c546` et `51d03e83`, build
reconfiguré ; FMTlib inchangé depuis `f4252745`) :
- l'arbre de travail est identique à `HEAD`, et les deux commits contiennent les 14 fichiers du lot
  (`git show --stat`) ; `UnitCore.exe` du build date de 10 h 47 ;
- installation (`INSTALL_TEST_SUITE=public`, 10 h 48) : 441 tests, 0 échec, 418 exécutés et 23
  désactivés, 17 s ; unit 144 verts et 4 désactivés ; aucun test en `T:/`. `InstallTests.xml`,
  `InstallTests.json`, `LastTest.log` et `LastTestsDisabled.log` copiés dans le scratchpad de la session
  avant tout appel de ctest ;
- `RunTests.bat unit` : « 0 tests failed out of 144 », 1,9 s réelles ; unit 144 verts et 4 désactivés
  sur 148, les quatre cas désactivés ci-dessus ;
- `RunTests.bat all-public` : « 0 tests failed out of 418 », 15,5 s réelles ; unit 144 verts et 4
  désactivés sur 148, integration 13 verts et 7 désactivés sur 20, system 236 verts et 12 désactivés
  sur 248, performance 23, other 2 ; Total 418 verts et 23 désactivés sur 441. Par rapport au lot 5
  (section 5.7), seul le niveau unit change, des 141 cas ajoutés : 137 verts et 4 désactivés, justifiés
  en section 2.7 (#380, défauts 11 et 12). La fixture `ExamplesModelsUnchanged` est verte, et les 7
  tests Python passent ;
- non relues : les consoles de la configuration et du build. Les avertissements ne sont couverts que par
  la passe `/W4` hors dépôt d'avant le build (aucun dans les sources de tests) ;
- issues des défauts 11 à 13 ouvertes par Gabriel le 2026-10-21 : #386, #388 et #387. Leurs numéros
  sont reportés en section 2.7, dans les commentaires des deux cas désactivés, dans
  `CoreTestCoverage.csv` et dans le changelog (commentaires seuls : les deux sources, recompilées hors
  dépôt, ne donnent ni erreur ni avertissement). Le lot n'ajoute que des tests unitaires : la suite BFEC
  n'est pas relancée.

### 5.10 Lot 7b : bornes, spécifications, contraintes, sorties et yields (2026-10-21)

**Statut** : livré le 2026-10-21, commité par Gabriel le 2026-10-22 (`355b2edb`, changelog `1bd707f6`), **validé** le
2026-10-22 (fin de cette section). Étapes 3 et 4 de la section 6.3.4, et ce que le
lot 7a a laissé faute de `FMTSpec` et de `FMTYields` (section 5.9, « Non couvert »).

**Ajouts** (cp1252, CRLF, en-tête de licence) : une source par classe, une suite au nom de la classe, cas
Given…When…Then en Arrange / Act / Assert ; 154 cas nouveaux, dont 7 désactivés.

| Source | Cas | Désactivés | Ce qu'ils couvrent |
|---|---|---|---|
| `FMTBounds.cpp` | 11 nouveaux (14) | 0 | `out` (dont des bornes inutilisées, prises pour 0 à 0), `empty`, `add` (bornes ouvertes de l'action, remplacement), égalité, ordre, `setOrBound`, réels par `FMTYldBounds`, borne basse au-dessus de la haute |
| `FMTAgeBounds.cpp` | 11 | 0 | copie, source et cible, chaînes de l'action (deux conditions, ouvertes, égalité), de la transition (`@AGE(10.._MAXAGE)`, `@AGE(10)`, `_AGE 5`) et des sorties |
| `FMTYldBounds.cpp` | 7 | 0 | copie, deux yields, construction depuis des bornes réelles, chaînes de l'action et de la transition |
| `FMTLockBounds.cpp` | 5 | 0 | copie, égalité, `_LOCK 2`, `_INVLOCK`, `in` sur le verrou |
| `FMTPerBounds.cpp` | 6 | 0 | copie, égalité, `_CP`, périodes d'une contrainte (chaîne vide), `in` |
| `FMTSpec.cpp` | 17 nouveaux (19) | 0 | sans borne, périodes, verrou (sa borne haute n'est pas lue), yields (`allow`, `allowYields`), fusion d'un yield, `empty*`, accesseurs, `add`, copie, égalité et ordre, hachage, chaîne, `isSubsetOf` |
| `FMTConstraint.cpp` | 36 | 2 | bornes, types (`isObjective`, `sense`, `extraVariables`, `acrossPeriod`, `isSpatial`, `getGroup`, `isRandomAction`), `setLength`, variations, buts et pénalités, chaînes (`>=`, `=`, `<=`, `_MAX`, `_MIN` et pénalités, `_EVEN`, `_NDY`), égalité, copie, hachage, `evaluate` ; désactivés : #379, #382 |
| `FMTOutput.cpp` | 21 | 0 | accesseurs, `empty`, copie, égalité, hachage, `+=`, `-=`, `*=`, `/=`, constantes, `shuntingYard`, `isLinear`, sources d'action et d'inventaire, niveau constant, chaînes |
| `FMTYields.cpp` | 22 | 3 | `get` (âge, temps, complexe, yield inconnu, masque d'un autre attribut : `FMTmissingyield`, premier gestionnaire du développement), `isYld`, `isNullYld`, `getAllYieldNames`, `gotYieldType`, copie, égalité avec soi, `getFromFactor`, `getStacked`, `clear`, `swap`, `getAge` ; désactivés : #383, défauts 15 (#390) et 16 (#391) |
| `FMTAgeYieldHandler.cpp` | 5 nouveaux (25) | 0 | `getAge` (âge de base le plus proche, premier âge de base), `getEndPoint` |
| `FMTComplexYieldHandler.cpp` | 12 nouveaux (45) | 2 | `_MIN` de yields négatifs, `_ENDPOINT` dans ses bornes, `getAge`, `complexYldToAgeYld`, surcharges (`compareSources`, `setTabou`, `getTabous`, `_OVERRIDE` écrit, valeurs avec et sans tabou) ; désactivés : défauts 14 (#389) et 17 (#392) |
| `Tests/Integration/Parser/FMTTransitionParser.cpp` | 1 | 0 | #358, comportement connu (section 2.7) |

- Niveau unitaire : aucun modèle sur disque, objets construits en code comme les parseurs les construisent (bornes
  ouvertes au plus petit ou au plus grand de leur type, `RHS`, `Variation`, `GOAL_`, `Penalty<opérateur>_<but>`,
  surcharges données comme `FMTYieldParser::_setOveridedYlds`). Aucune classe du lot ne demande un modèle ou un
  état global : aucune ligne de dette. Seul #358 a un cas d'intégration : son mécanisme est dans
  `FMTParser::_setSpec` (namespace `Parser`), et un cas de `FMTSpec` resterait vert après la correction.
- **#358** : cas présenté à Gabriel, qui a retenu le niveau intégration le 2026-10-21 (section 2.7). Il lit le
  scénario `INVALID_sourcecondition` et vérifie que `arecup` donne à un développement de 5 ans une fourche sans
  borne d'âge. Contrôle hors du dépôt : dans le scénario `transitions`, où la même action a `@AGE(10.._MAXAGE)`, la
  fourche d'un développement de 15 ans garde ses bornes ; dans `INVALID_sourcecondition`, aucune n'en a, à 5 comme
  à 15 ans.
- **#379, #382 et #383** : un cas désactivé de plus chacun, plus près de la cause que le cas d'intégration du lot 5 :
  la chaîne de `_MIN _PENALTY(CARIBOU,RECOLTE)` vaut `_MIN y_PENALTY(CARIBOU,RECOLTE) 1.._LENGTH` ; celle d'une
  contrainte `_RANDOM(OVOL) >= 1` ne contient pas `_RANDOM(` ; une copie de `FMTYields` n'égale pas l'original.
  **#381** : pas de cas unitaire. Le défaut tient à la relecture des sorties cachées que `_toWsFormat` et
  `_operatorFormat` écrivent, et Woodstock demande peut-être ce découpage : un cas de `FMTOutput` seul supposerait
  la correction du côté de l'écriture.
- **Observations du lot 7a** (section 5.9) : toutes deux démontrées, défauts 14 (`_MAX`) et 15 (`getFromFactor` d'un
  `_DISCOUNTFACTOR`), à travers `FMTYields`. Deux autres défauts sont apparus en écrivant les cas : 16 et 17
  (section 2.7).
- **Piège de ce lot** : une `FMTYieldRequest` garde l'adresse de son développement (section 7). Un premier brouillon
  passait un temporaire : deux cas échouaient sans raison apparente, sur de la mémoire libérée.

**Observations, sans défaut annoncé** (lues dans le code ; rien de ce que FMT exécute n'en dépend, ou ce n'est pas
démontré) :
- `FMTSpec::isSubsetOf` compare les bornes basses des yields à l'envers des périodes et des âges : `VOLUME 100..200`
  n'est pas inclus dans `VOLUME 50..300`, et `VOLUME 10..200` l'est (cas vert commenté). Son seul appelant,
  `FMTOutputSource::isSubsetOf`, sert le cache des nœuds d'outputs (`Include/FMTOutputNodeCache.h`), qui n'utilise
  un nœud trouvé que si sa spécification égale celle qu'il cherche ; `_getParentNode` reçoit d'ailleurs
  `m_foundSubset` par valeur, si bien que le sous-ensemble trouvé n'est jamais repris.
- `FMTBounds::out` ne regarde pas si les bornes servent : des bornes inutilisées valent 0 à 0 (cas vert commenté).
  FMT ne l'appelle que sur des bornes de yields, toujours utilisées.
- `FMTSpec::allowWithoutYield` ne lit que la borne basse du verrou (cas vert commenté) ; `FMTSpec::add` ignore le
  verrou de la spécification ajoutée, et rien dans FMT n'appelle `FMTSpec::add`.
- `FMTConstraint::operator std::string` cherche `_ALL` après le découpage sur `_`, ce qui ne peut arriver :
  `_PENALTY(_ALL)` est écrit `_PENALTY(ALL)`, que FMT relit à l'identique (`Penalty+_ALL`). Woodstock : non vérifié.
- `FMTConstraint::evaluate` d'un `_EVEN` avec variation vaut toujours 0 (`_getVariability` compare chaque valeur au
  maximum plus la variation, qu'aucune ne dépasse). Seules les heuristiques spatiales et `FMTYieldModelDecisionTree`
  l'appellent : non démontré avec un exécutable.
- `FMTConstraint::hash(false)` hache deux fois la période basse ; aucun appelant ne passe `false`.
- `FMTOutput::operator=` ne copie pas le groupe (`m_group`), que le constructeur de copie copie, et
  `FMTOutput::serialize` ne l'écrit pas. Il sert à écrire `*GROUP` : non démontré qu'un chemin de FMT y perde un
  groupe.
- `FMTOutputSource::operator==` ignore le yield et les valeurs : deux outputs de même nom et de même description qui
  ne diffèrent que par le yield de leurs sources sont égaux (classe du lot 8).
- `FMTAgeYieldHandler::getAge` ne compare jamais le premier âge de base, et le rend quand aucun autre n'est sous
  l'âge du développement (cas vert commenté). `getEndPoint` lit un itérateur non initialisé quand la valeur égale la
  borne ; `_ENDPOINT` ne l'appelle jamais ainsi.
- `FMTParser::bounds` ne traite pas `<` ni `>`, pourtant dans `getBaseOperators` : `_AGE > 10` dans une action donnerait
  des bornes sans limite (parseur, lu seulement).

**Non couvert, et pourquoi** : `FMTConstraint` : les mots-clés rangés en bornes de yields (`_SETFROM`, `_REIGNORE`,
`_REPLICATE`), `preSolve` et la conversion en yields, qui demandent actions, transitions et thèmes ; `FMTOutput` :
`boundTo`, `getNodes`, `preSolve`, les décompositions et les formats de #381 ; `FMTYields` : `generateDefaultYields`
et les yields de modèle, `preSolve`, `getAllYields` (aucun appelant). Le test du défaut 13 (#387, lecture de `_CP`)
reste à décider (section 6.3.4).

**Démonstrations** (règle de la section 2.7) : défauts 14 et 17 avec l'exécutable officiel `ReadScenario` du build,
sur une copie de TWD_land dans le scratchpad (bloc `*YC` ajouté au `._yld` de `yieldoperatorsalone`) ; défauts 15
et 16 avec `FMT.pyd` du build, par un script du scratchpad qui lit `timeyields` dans le dépôt. Rien n'a été écrit
dans `build/release` ni sous `Examples/Models` (`find -newer`, `git status`).

**Vu rouge** (règle 1), rien de commité :
- une copie faussée des 11 sources unitaires touchées et de la source d'intégration, hors du dépôt, avec l'en-tête du
  lot 7a qui retourne chaque assertion (`ASSERT_NE` ajouté), compilée par `cl` contre le `FMTlib.lib` de
  `build/release` : les 204 cas actifs des 11 suites échouent (les 146 nouveaux et les 58 qu'elles avaient), aucun ne
  passe, aucun par une exception inattendue ; le cas de #358 échoue aussi ;
- les 11 cas désactivés de `UnitCore`, lancés avec `GTEST_ALSO_RUN_DISABLED_TESTS=1`, échouent chacun pour sa raison :
  pour les 7 nouveaux, `y_PENALTY`, pas de `_RANDOM(`, une copie de `FMTYields` inégale, 0,463193488085 au lieu de
  1,643854213518, `VOLUME` resté à 200, `_MAX` à 0 au lieu de -200, et la chaîne `FMTexc(53)` puis `FMTexc(85)` de
  `_ENDPOINT`.

**Vu vert avant le build de Gabriel** : les sources du dépôt, compilées par `cl` avec les options de `UnitCore`
(`/Fo` `/Fd` `/Fp` dans le scratchpad), liées au `FMTlib.lib` de `build/release` (FMTlib de `f4252745`, inchangé
depuis), lancées depuis `bin/Release` : `UnitCore`, 290 cas verts (144 d'avant et 146 nouveaux), 11 désactivés ;
mêmes résultats dans un ordre mélangé (`GTEST_SHUFFLE`, graine 4242, 3 répétitions). `IntegrationParser`, compilé
avec ses trois sources, se lie ; seul le nouveau cas a été lancé (`GTEST_FILTER`), vert, les autres écrivant dans
`build/release/tests`. En `/W4`, aucun avertissement ne vient des sources de tests : les 380 viennent des en-têtes
de FMTlib.

**Documents** : `CoreTestCoverage.csv` (statut `Lot 7b (step <n>), done` de neuf classes, défauts 14 à 17, #358 sur
`FMTSpec` et `FMTTransition`, tests manquants), `CoreTestCoverage.md` régénéré (16 classes avec des tests unitaires,
34 de priorité 1 sans test propre au lieu de 41 ; deux générations identiques), changelog (section non publiée,
anglais et français).

**Validation après le build de Gabriel** (2026-10-22, commits `355b2edb` et `1bd707f6` ; FMTlib inchangé depuis
`f4252745`) :
- avant le report des numéros d'issues, l'arbre de travail était identique à `HEAD`, et les deux commits contiennent
  les 17 fichiers du lot (`git show --stat`) ; `UnitCore.exe` du build date de 8 h 08, `IntegrationParser.exe` de
  8 h 07 ;
- installation (`INSTALL_TEST_SUITE=public`, 8 h 09) : 595 tests, 0 échec, 565 exécutés et 30 désactivés, 19 s ;
  unit 290 verts et 11 désactivés, integration 14 verts et 7 désactivés, system 236 verts et 12 désactivés,
  performance 23, other 2 ; aucun test en `T:/`, et les 7 tests Python passent. La roue
  `FMT-1.3.0-cp310-cp310-win_amd64.whl` date de 8 h 09. `InstallTests.xml`, `InstallTests.json`, `LastTest.log` et
  `LastTestsDisabled.log` copiés dans le scratchpad de la session avant tout appel de ctest ;
- `RunTests.bat unit` : « 0 tests failed out of 290 », 2,9 s réelles ; unit 290 verts et 11 désactivés sur 301, les
  4 cas désactivés du lot 7a et les 7 de ce lot ;
- `RunTests.bat all-public` : « 0 tests failed out of 565 », 16,5 s réelles ; unit 290 verts et 11 désactivés sur
  301, integration 14 verts et 7 désactivés sur 21, system 236 verts et 12 désactivés sur 248, performance 23,
  other 2 ; Total 565 verts et 30 désactivés sur 595. Par rapport au lot 7a (section 5.9), seuls les niveaux unit
  et integration changent, des 154 cas ajoutés : 147 verts et 7 désactivés (#379, #382, #383, #389 à #392),
  justifiés en section 2.7. Le cas de #358 et la fixture `ExamplesModelsUnchanged` sont verts, et l'arbre de travail
  est resté identique à `HEAD` après le passage ;
- non relues : les consoles de la configuration et du build. Les avertissements ne sont couverts que par la passe
  `/W4` hors dépôt d'avant le build (aucun dans les sources de tests) ;
- issues des défauts 14 à 17 ouvertes par Gabriel le 2026-10-22 : #389, #390, #391 et #392. Leurs numéros sont
  reportés en section 2.7, dans les commentaires des quatre cas désactivés, dans `CoreTestCoverage.csv` (document
  régénéré, deux générations identiques) et dans le changelog. Commentaires seuls : les deux sources, recompilées
  hors dépôt en `/W4`, ne donnent ni erreur ni avertissement. Le lot n'ajoute que des tests unitaires et un cas
  d'intégration en lecture : la suite BFEC n'est pas relancée.

### 5.11 Lot 7c : masques, thèmes et développements (2026-10-22)

**Statut** : livré le 2026-10-22, commité par Gabriel le même jour (`5956123b`, changelog `1754fe30`), **validé** le
2026-10-22 (fin de cette section). Étape 5 de la section 6.3.4, qui termine le lot 7, avec l'observation de
`FMTParser::bounds` du lot 7b (section 5.10) et le cas du défaut 13 (#387).

**Ajouts** (sources en cp1252, CRLF, en-tête de licence ; scénarios en ASCII, CRLF, comme leurs voisins) : une source
par classe, une suite au nom de la classe, cas Given…When…Then en Arrange / Act / Assert ; 124 cas nouveaux, dont 3
désactivés.

| Source | Cas | Désactivés | Ce qu'ils couvrent |
|---|---|---|---|
| `FMTMask.cpp` | 43 | 0 | construction depuis une chaîne et les thèmes (attributs, `?`, agrégats, négations, une valeur par thème, chaîne trop courte, attribut inconnu : `FMTundefined_attribute`, thèmes seuls, défaut), copie, déplacement, égalité par les bits, ordre, hachage (dont un ensemble haché), `isSubsetOf` (développement, masque générique, blocs non pleins), intersection, union, valeurs par thème (`get`, `getAttribute`, `getSubsetCount`, agrégat d'un seul attribut, négation, bits qu'aucun agrégat ne nomme : `FMTundefined_attribute`), `decompose`, `set`, thèmes statiques et sélectionnés, `isNotThemesSubset`, `removeAggregates`, `refine`, `resume`, `setExclusiveBits`, `append`, `swap`, `clear`, presolve (`preSolve`, `canPreSolve`, `getPostSolveMask` et `postSolve`) |
| `FMTTheme.cpp` | 34 | 0 | construction comme `FMTLandscapeParser` (noms vides, agrégats, index du thème et premier bit), sans agrégat et sans index, thème vide, `isAttribute` (dont un agrégat), `isAggregate`, `getAttributes` (`?`, attribut, agrégats, négations, agrégat d'agrégat et sa source, valeur inconnue : `FMTundefined_attribute`), `inAggregate`, `isValid`, `validate` (masque complet, plus court, plus long, tabulations, attribut inconnu : `FMTundefined_attribute`), noms et chaîne de la section, thèmes indexés (`useIndex`, `getIndex`, `isIndex`, index absent, attribut inconnu : `FMTundefined_attribute`, chaîne `_INDEX`), copie, égalité, presolve (thème gardé avec ses agrégats, thème réduit à un attribut, filtre vide : `FMTinvalid_maskrange`), `updateFromMask` |
| `FMTDevelopment.cpp` | 42 | 0 | construction, copie, clone, déplacement, égalité, ordre, hachage (dont un ensemble haché), chaîne, accesseurs, `clearLock`, `grow` (âge, période, verrou), `getYieldRequest`, `operable` (âge minimal, intervalle d'âges, yield, masque, verrou, `_LOCKEXEMPT`, période), `anyOperable`, `worthTestingOperability` et ses deux variantes, `getNonOperabilityInfo`, `is`, `operate` à travers des transitions construites en code (âge remis à zéro, attribut cible, âge gardé, `_LOCK` et `_AGE` de cible, deux cibles, deux sources du même masque, aucune source : `FMTinvalid_transition_case`, cible qui ne change rien : `FMTsourcetotarget_transition`, deux cibles identiques : `FMTsame_transitiontargets`), `getInventoryCoef`, `getHarvestCoef` (action entière et partielle) |
| `Tests/Integration/Parser/FMTActionParser.cpp` | 4 | 2 | défaut 18 (section 2.7) : `_AGE > 10` et `_AGE < 5`, désactivés, à côté de `_AGE >= 11` et `_AGE <= 4`, verts, scénario `strictbounds` |
| `Tests/Integration/Parser/FMTYieldParser.cpp` | 1 | 1 | défaut 13 (#387) : un bloc `*YT` en colonnes, scénario `timeyieldcolumns` |

- Deux scénarios nouveaux dans `Examples/Models/TWD_land/Scenarios`, qui le disent en tête par un commentaire :
  `strictbounds` (le `.act` et le `.trn` de la racine, suivis de quatre actions de masque `? prod ?` et de leurs
  transitions) et `timeyieldcolumns` (le `.yld` de la racine, suivi du bloc `_CP` de la section 2.7, défaut 13). Aucun
  scénario existant n'a changé.
- Niveau unitaire : aucun modèle sur disque. Les thèmes sont ceux de TWD_land, construits comme `FMTLandscapeParser`
  (constructeur à huit arguments, un nom vide par attribut) ; les actions et les transitions de TWD_land, comme
  `FMTActionParser`, `FMTTransitionParser` et `FMTModel` (une spécification ou une fourche par masque, puis `update`) ;
  `VOLUMETOTAL` est celui de `peuplement1`. Le presolve suit `FMTModel::preSolve` (filtre de l'union des masques des
  développements, puis thèmes présolvés), et le postsolve `FMTModel::getPostSolveFilter`. Aucune classe du lot ne
  demande un modèle ou un état global, et aucun cas ne contourne le code : aucune ligne de dette.
- `FMTTheme` n'a ni `hash` ni `operator!=` : aucun cas de hachage pour lui. `FMTMaskComparator` et `FMTThemeComparator`
  restent au lot 8, comme `FMTActualDevelopment` et `FMTFuturDevelopment`, que les cas ne lisent qu'à travers
  `FMTDevelopment` (`grow` rend un `FMTFuturDevelopment`).
- Valeurs : bits et chaînes lus dans le code des parseurs et des classes (les bits de `UNITE1` à `UNITE3` sont 0 à 2,
  ceux de `PEUPLEMENT1` à `FEU` 3 à 6, ceux de `UTR1` à `UTR3` 7 à 9) ; `VOLUMETOTAL` interpolé (80 à 4 ans, 120 à 7
  ans, 130 à 8 ans) ; index du scénario `AVERAGEP` ; `std::to_string` écrit six décimales (`16.360000`).

**Étape 2, `FMTParser::bounds` sur `<` et `>`** : démontré, défaut 18 (section 2.7), deux cas désactivés et une issue
(#393, ouverte par Gabriel le 2026-10-22). A/B avec l'exécutable officiel `ReadScenario` (`OPERABLE`), d'abord sur une copie de TWD_land dans le
scratchpad, dont le scénario `strictbounds` avait en plus `volumetotal > 100`, `_CP > 2` et `_CP >= 3`, puis sur le
scénario du dépôt ; origine dans `2e3dc129` ; refus de Woodstock dans les messages 142 et 143 de `Wk.msg`.

**Étape 3, défaut 13 (#387)** : cas désactivé dans `IntegrationParser` (section 2.7, item 13), sur `timeyieldcolumns`.

**Observations, sans défaut annoncé** (vérifiées par une sonde hors du dépôt, compilée contre le `FMTlib.lib` du build ;
rien de ce que FMT exécute à partir d'un modèle n'en dépend, ou ce n'est pas démontré) :
- un masque construit depuis une chaîne plus courte que les thèmes est complété par le premier attribut de chaque thème
  manquant (`UNITE1 PEUPLEMENT1` donne `UNITE1 PEUPLEMENT1 UTR1`, cas vert commenté), alors que `FMTTheme::validate`,
  que les parseurs appellent avant de construire un masque, le complète par `?`. Ce constructeur est exposé en Python
  (`FMTmask(str, themes)`) ;
- `FMTTheme::isAttribute` accepte un agrégat (cas vert commenté) : `isValid` s'en sert pour accepter les agrégats d'un
  masque. `FMTTransitionMask::build` s'en sert aussi, si bien qu'une cible écrite avec un agrégat garderait les bits de
  l'agrégat dans le masque du développement (lu dans le code ; classe du lot 8) ;
- `FMTTheme::updateFromMask` crée l'agrégat `~FMT1A_1` pour `PEUPLEMENT1` et `FEU`, puis rend `!POURPL`, que
  `_bitsToStr` trouve avant lui : l'agrégat créé ne sert pas. Les cas verts prennent des attributs qu'aucune négation
  ne nomme ;
- `FMTMask::setIntersect` garde le nom d'avant (`UC PROD ?` pour les valeurs `UC POURPL UTR3`) ; ses deux appelants,
  dans `FMTModel` et `FMTLineGraph`, ne lisent que les bits ;
- l'âge, le verrou et la période d'un développement sont des `uint8_t` : `setAge(256)` donne 0, `setLock(300)` 44, et
  `grow` d'un développement de 255 ans à la période 255 donne l'âge 0 à la période 0, sans erreur. TWD_land ne dépasse
  pas 80 ans (durée de vie). Modèles BFEC : non vérifié ;
- une action construite en code sans `update` a des bornes d'âge et de période de 0 à 0 : `operable` la refuse à tout
  développement d'âge ou de période non nul (section 7).

**Non couvert, et pourquoi** : `getYieldRequest`, `operable` et `is` avec un sommet de graphe
(`Graph::FMTGraphVertexToYield`, hors du Core) ; `FMTMask::binarizedAppend` et `filter`, qui servent aux clés de
`FMTLineGraph` et de `FMTOutputNode` et à `FMTMaskFilter` (lot 8) ; les data frames du build R de `FMTTheme`
(`FMTWITHR`) ; la sérialisation (lot 9).

**Vu rouge** (règle 1), rien de commité :
- une copie faussée des cinq sources, hors du dépôt, avec l'en-tête des lots 7a et 7b qui retourne chaque assertion (le
  script qui écrit la copie refuse toute autre macro ; `ADD_FAILURE`, qui signale une action absente du modèle, n'est
  pas retourné), compilée par `cl` contre le `FMTlib.lib` de `build/release` : les 119 cas unitaires et les 2 cas
  d'intégration actifs échouent, aucun ne passe, aucun par une exception inattendue ;
- les 3 cas désactivés, lancés avec `GTEST_ALSO_RUN_DISABLED_TESTS=1`, échouent chacun pour sa raison : `_AGE > 10` et
  `_AGE < 5` sont opérables à 0, 4, 5, 10, 11 et 20 ans ; `CPA` vaut 10, 10, 10, 30, 30, 30, 50 et `CPB` 100, 100, 100,
  300, 300, 300, 500.

**Vu vert avant le build de Gabriel** : les sources du dépôt, compilées par `cl` avec les options de `UnitCore` et
d'`IntegrationParser` (leurs `CL.command.1.tlog`, identiques), `/Fo` `/Fd` `/Fp` dans le scratchpad, liées au
`FMTlib.lib` de `build/release` (FMTlib de `f4252745`, inchangé depuis) :
- `UnitCore`, ses 19 sources, lancé depuis `bin/Release` : 409 cas verts (290 d'avant et 119 nouveaux), 11
  désactivés ; mêmes résultats dans un ordre mélangé (`GTEST_SHUFFLE`, graines 4242 à 4244, 3 répétitions) ;
- `IntegrationParser`, ses 5 sources, lancé depuis une racine du scratchpad (`runroot\build\release\bin\Release`, à
  côté d'une copie de `Examples/Models/TWD_land` identique au dépôt), pour que ses écritures restent hors de
  `build/release` : 16 cas verts (14 d'avant et 2 nouveaux), 10 désactivés ; mêmes résultats dans un ordre mélangé
  (graines 4242 à 4244). L'exécutable officiel, lancé de la même racine, donne 14 verts et 7 désactivés, et les mêmes
  six messages « Calling pure virtual function » du cas `COS`, préexistants ;
- en `/W4`, aucun avertissement ne vient des cinq sources nouvelles : les 330 viennent des en-têtes de FMTlib ;
- rien n'a été écrit dans `build/release`, ni sous `Examples/Models` hors des deux scénarios (`find -newer`,
  `git status`).

**Documents** : `CoreTestCoverage.csv` (statut `Lot 7c (step 5), done` des trois classes, défaut 18 sur `FMTAction`,
#387 sur `FMTTimeYieldHandler`, tests manquants), `CoreTestCoverage.md` régénéré (19 classes avec des tests unitaires,
31 de priorité 1 sans test propre au lieu de 34 ; deux générations identiques), sections 2.7, 4.4, 6.1, 6.3.4, 7 et 8,
changelog (section non publiée, anglais et français).

**Validation après le build de Gabriel** (2026-10-22, commits `5956123b` et `1754fe30`, build reconfiguré ; FMTlib
inchangé depuis `f4252745`) :
- l'arbre de travail est identique à `HEAD`, et les deux commits contiennent les 13 fichiers du lot (`git show --stat`) ;
  `IntegrationParser.exe` du build date de 9 h 34, `UnitCore.exe` de 9 h 35 ;
- installation (`INSTALL_TEST_SUITE=public`, 9 h 37) : 719 tests, 0 échec, 686 exécutés et 33 désactivés, 21 s ; unit
  409 verts et 11 désactivés, integration 16 verts et 10 désactivés, system 236 verts et 12 désactivés, performance 23,
  other 2 ; aucun test en `T:/` ; les 7 tests Python et la fixture `ExamplesModelsUnchanged` passent. La roue
  `FMT-1.3.0-cp310-cp310-win_amd64.whl` date de 9 h 36. `InstallTests.xml`, `InstallTests.json`, `LastTest.log` et
  `LastTestsDisabled.log` copiés dans le scratchpad de la session avant tout appel de ctest ;
- `RunTests.bat unit` : « 0 tests failed out of 409 », 5,7 s réelles ; unit 409 verts et 11 désactivés sur 420, les 11
  cas désactivés des lots 7a et 7b (ce lot n'en ajoute aucun au niveau unit) ;
- `RunTests.bat integration` : « 0 tests failed out of 18 », 2,4 s réelles ; integration 16 verts et 10 désactivés sur
  26, other 2 (les deux fixtures) : les 3 cas désactivés de ce lot (défaut 18, deux cas ; #387) et les 7 du lot 5 ;
- `RunTests.bat all-public` : « 0 tests failed out of 686 », 20,1 s réelles ; unit 409 verts et 11 désactivés sur 420,
  integration 16 verts et 10 désactivés sur 26, system 236 verts et 12 désactivés sur 248, performance 23, other 2 ;
  Total 686 verts et 33 désactivés sur 719. Par rapport au lot 7b (section 5.10), seuls les niveaux unit et
  integration changent, des 124 cas ajoutés : 121 verts et 3 désactivés (défaut 18 et #387), justifiés en section
  2.7. La fixture `ExamplesModelsUnchanged` est verte malgré les deux nouveaux scénarios, et l'arbre de travail est
  resté identique à `HEAD` après le passage ;
- non relues : les consoles de la configuration et du build. Les avertissements ne sont couverts que par la passe
  `/W4` hors dépôt d'avant le build (aucun dans les sources de tests) ;
- l'issue du défaut 18, #393, a été ouverte par Gabriel le 2026-10-22 ; son numéro est reporté dans la section 2.7,
  dans les commentaires des deux cas désactivés, dans `CoreTestCoverage.csv` et dans le changelog (2026-10-26). Le lot n'ajoute
  que des tests et deux scénarios qu'eux seuls lisent : la suite BFEC n'est pas relancée.

### 5.12 Session à part : correction de #394 (2026-10-23)

**Défaut.** `FMTAreaParser::_GetGeometryArea` n'avait pas de `break` à la fin du cas des multipolygones et des
collections : l'exécution continuait dans `default`, qui remettait l'aire à 0. `_getNeighborsFromPolygons` écartait
donc tout voisin dont l'intersection avec la zone tampon forme plusieurs morceaux. Démonstration et origine dans
l'issue #394 (A/B sur le test privé `GetOperatingAreaNeighbors` : 192 voisins, 240 avec le seul `break`).

**Correction** (`Source/FMTAreaParser.cpp`) : le `break`, et dans `_getClustersFromPolygons`, l'intersection mesurée
n'est plus convertie par `dynamic_cast<OGRPolygon*>`, qui rend `nullptr` pour un multipolygone : son aire valait 0
et elle n'était jamais détruite. Démontré par une sonde sur le `FMTlib.dll` de `966b668c` : l'intersection d'un
carré et d'un U est un multipolygone d'aire 400 pour GDAL, `nullptr` après la conversion.

**Test** : `Tests/Integration/Parser/FMTAreaParser.cpp`, deux cas de `getSchemeNeighbors` dans `IntegrationParser`,
sur une carte GeoJSON que chaque cas écrit dans son dossier de sortie (champs `THEME1`, `AGE` et `SUPERFICIE`, un
thème construit en code, une zone tampon de 10) :

| Cas | Carte | Avant la correction | Après |
|---|---|---|---|
| `GivenTwoUnitsSharingASide…ThenTheyAreNeighbors` | deux carrés côte à côte (témoin) | vert | vert |
| `GivenTwoUnitsTouchingInTwoSeparatePlaces…ThenTheyAreNeighbors` | un U renversé posé sur un carré | rouge, aucun voisin | vert |

Vus avant le build : le test compilé par `cl` hors du dépôt contre le `FMTlib.lib` de `build/release` (`/W4
/permissive-` : aucun avertissement dans ses sources), lancé depuis `bin/Release` ; pour « après »,
`_GetGeometryArea` est redirigée, dans le processus seulement, vers la fonction corrigée. Sans GDAL ou OSI, les deux
cas finissent en `GTEST_SKIP`.

**Pourquoi pas TWD_land** : sur sa carte, toutes les intersections sont des polygones en un morceau. Une sonde A/B de
`getSchemeNeighbors` (thèmes 1, 2 et 3 et leurs combinaisons, zones tampons de 1, 10 et 100) et de l'appel de
`ClusterOperatingAreas` donne les mêmes voisins et les mêmes groupes, sans aucun multipolygone : `ClusterOperatingAreas`
ne change pas.

**Suite BFEC** : l'attendu de `GetOperatingAreaNeighbors` passe de 192 à 240 dans le `BFECtests.csv` de
ce worktree, et son nom ctest change avec lui (empreinte de ses arguments) ; les autres copies du CSV sont à mettre à
jour de la même façon. `ScheduleOperatingAreas` et `RunOpAreaSchedulerTask` ne comparent pas à une valeur fixe
(l'heuristique contre le rejeu de sa solution), mais leurs voisins changent : ils sont à relancer.

**Démontré, hors de ce correctif** : une géométrie avec Z (`POLYGON Z`, de type 2,5D) vaut aussi 0 dans
`_GetGeometryArea` (sonde : GDAL 100, FMT 0), car le `switch` ne retire pas le drapeau Z (`wkbFlatten`). Avant
`2dc1ad55`, le `dynamic_cast<OGRSurface*>` la mesurait. Sur des shapefiles de deux carrés côte à côte, avec le
correctif de #394, `getSchemeNeighbors` trouve 2 voisins en `Polygon` et en `PolygonM`, 0 en `PolygonZ` et en
`PolygonZM` ; 2 partout avec `wkbFlatten`. Issue #396, ouverte par Gabriel le 2026-10-23, corrigée en section
5.13.

**Validation après le build de Gabriel** (2026-10-23, sources de `966b668c` et le correctif, non commité) :

- passage de l'installation (`INSTALL_TEST_SUITE=public`) : 722 tests, 0 échec, 31 désactivés ; les deux cas de
  `FMTAreaParser` verts ;
- suite privée, lancée par Gabriel : 94 tests réussis, 0 échec, dont `GetOperatingAreaNeighbors` avec l'attendu 240,
  `ScheduleOperatingAreas` et `RunOpAreaSchedulerTask` ;
- `RunTests.bat all-public`, relancé ensuite : « 0 tests failed out of 691 », 21,7 s réelles ; unit 410 verts et 10
  désactivés sur 420, integration 19 verts et 9 désactivés sur 28, system 237 verts et 12 désactivés sur 249,
  performance 23, other 2 ; Total 691 verts et 31 désactivés sur 722. La fixture `ExamplesModelsUnchanged` est verte,
  et l'arbre de travail est resté le même après le passage.

Commité par Gabriel en `44aee1fe`.

### 5.13 Session à part : correction de #396 (2026-10-26)

**Défaut.** `_GetGeometryArea` comparait le type de GDAL tel quel : un polygone avec Z est un `wkbPolygon25D`, qui
tombait dans `default` et valait 0. Sur une carte en Z, `getSchemeNeighbors` ne trouvait aucun voisin. Démonstration
en section 5.12 et dans l'issue.

**Correction** (`Source/FMTAreaParser.cpp`) : `wkbFlatten` retire les drapeaux Z et M avant le `switch`. La
documentation de `_GetGeometryArea` (`Include/FMTAreaParser.h`) le dit.

**Test** : deux cas de plus dans `Tests/Integration/Parser/FMTAreaParser.cpp`, sur les cartes des deux cas de #394,
avec un Z de 5 à chaque sommet. Les deux cartes sont devenues des fonctions partagées (`getSquaresSideBySide`,
`getArchOnASquare`), et `writeMap` prend un `p_withZ`.

| Cas | Intersections | Build de `44aee1fe` | Avec `wkbFlatten` |
|---|---|---|---|
| `GivenTwoUnitsSharingASideWithZCoordinates…ThenTheyAreNeighbors` | polygones avec Z | rouge, aucun voisin | vert |
| `GivenTwoUnitsWithZCoordinatesTouchingInTwoSeparatePlaces…ThenTheyAreNeighbors` | multipolygones avec Z | rouge, aucun voisin | vert |

Vus avant le build comme en section 5.12 : compilés par `cl` hors du dépôt contre le `FMTlib.lib` de `build/release`
(`/W4 /permissive-` : aucun avertissement dans les sources du test), lancés depuis `bin/Release`, la fonction
redirigée dans le processus pour la colonne de droite. Les deux cas de #394 restent verts des deux côtés.

**Portée** : les 22 cartes des modèles de test BFEC sont en `Polygon` 2D ; la suite privée ne devrait pas changer.

**À valider après le build** : `RunTests.bat integration` (quatre cas de `FMTAreaParser`, verts), `all-public`.

**Validé par le passage de Gabriel du 2026-10-26** (section 6.1) : installation, 724 tests, 0 échec, les quatre cas de
`FMTAreaParser` verts ; suite privée, 94 réussis. Commité par Gabriel en `025d3493`.

### 5.14 Lacunes 6 et 7 : statistiques et `setSolution` période par période, aller-retour spatial (2026-10-26)

**Décision de Gabriel (2026-10-26)** : niveau intégration, un nouvel exécutable GoogleTest `IntegrationModels`
(`Tests/Integration/Models`, étiquettes `integration;models`, fixture `ExamplesModels`), suite `FMTSrModel`, la classe
de `buildPeriod`, `setSolution`, `setSolutionByLp`, `boundSolution`, `unboundSolution` et `isPeriodBounded`. Les
requêtes `STATS` et `REPLAY` de `PlanScenario` restent telles quelles.

**Ajouts.**
- `Tests/Integration/Models/CMakeLists.txt` et `FMTSrModel.cpp` (15 cas, dont 2 désactivés), inclus par
  `Tests/Integration/CMakeLists.txt`. MOSEK si FMT est compilé avec, CLP sinon (règle 6) ; sans OSI, `GTEST_SKIP`.
- Un cas dans `Tests/Integration/Parser/FMTModelParser.cpp`, sur le scénario nouveau `spatialconstraints`.
- Deux scénarios de TWD_land, chacun d'un seul fichier commenté en tête : `harvestfromperiod3` (`_act` de la racine,
  `COUPEtotale` opérable à partir de la période 3) et `spatialconstraints` (`_opt` de `Spatial` et une ligne
  `_GROUP(coupetotale,_TH1,2) >= 1 1.._LENGTH`). Aucun scénario existant n'est modifié.

**Cas de `FMTSrModel`** (5 périodes ; les modèles des statistiques sont présolvés d'abord, comme `doPlanning` le fait) :

| Cas | Scénario | Vérifie | État |
|---|---|---|---|
| `GivenAMaxMinHarvestWhenBuildPeriodThenEachPeriodAddsTheKnownGraphAndMatrix` | `LP` | `sommets\|arcs\|lignes\|colonnes` après chaque `buildPeriod` | vert |
| `GivenAConstraintOnARatioOfOutputsWhenBuildPeriod…` | `division` | idem | vert |
| `GivenAnObjectiveOnAnEquationYieldWhenBuildPeriod…` | `equation` | idem | vert |
| `GivenAHarvestOperableFromThePeriodThreeWhenBuildPeriodThenTheHarvestEntersTheMatrixAtThePeriodThree` | `harvestfromperiod3` | idem | vert |
| `GivenAMaxMinHarvestWhenBuildPeriodOneByOneThenBuildsTheGraphOfDoPlanning` | `LP` | sommets, arcs et lignes de transfert de la dernière période = ceux de `doPlanning(false)` | vert |
| `GivenTheOptimalSchedulesWhenSetSolutionPeriodByPeriodThenGivesTheOptimizedOutputs` | `LP` | graphe partiel, `setSolution` vrai à chaque période, tous les outputs de toutes les périodes | vert |
| `GivenTheOptimalSchedulesSetInTheWholeMatrixWhenBoundSolutionThenResolvesToTheOptimum` | `LP` | matrice entière, `setSolution` puis `boundSolution` à chaque période : vrai, optimal, même optimum | vert |
| `GivenTheOptimalSchedulesWhenSetSolutionByLpThenGivesTheOutputsOfSetSolution` | `LP` | `setSolutionByLp` vrai à chaque période, mêmes outputs que `setSolution` | vert |
| `GivenMoreAreaThanItsDevelopmentWhenSetSolutionWithoutThrowingThenReturnsFalse` | `LP` | 503,28 ha coupés sur le peuplement de 403,28 ha, `SETSOLUTION_THROW` à faux : rend faux | vert |
| `GivenMoreAreaThanItsDevelopmentWhenSetSolutionThenRaisesAnInvalidNumber` | `LP` | idem, par défaut : `FMTinvalid_number` | vert |
| `GivenAnActionNotOperableOnItsDevelopmentWhenSetSolutionWithoutThrowingThenReturnsFalse` | `LP` | `aCaribou` sur `UNITE3 PEUPLEMENT3 UTR3` de 8 ans (hors de `UC`) : rend faux | vert |
| `GivenAnActionNotOperableOnItsDevelopmentWhenSetSolutionThenRaisesAnInvalidNumber` | `LP` | idem, par défaut : `FMTinvalid_number` | vert |
| `GivenBoundSolutionsWhenUnboundSolutionThenResolvesToTheModelBefore` | `LP` | bornes inférieures d'avant, `resolve` optimal, même optimum | désactivé : défaut 19 (#399) |
| `GivenBoundSolutionsWhenIsPeriodBoundedThenReturnsTrue` | `LP` | vrai après `boundSolution(1)` | vert |
| `GivenAnOptimizedModelWhenIsPeriodBoundedThenReturnsFalse` | `LP` | faux sans `boundSolution` | désactivé : défaut 20 (#400) |

Le cas d'`IntegrationParser`, `GivenSizeAdjacencyAndGroupConstraintsWhenWriteThenRereadsTheSameConstraints`, écrit,
relit et réécrit `spatialconstraints` : types des contraintes spatiales relues (deux `_SIZE`, une `_ADJACENCY`, une
`_GROUP`), contraintes égales, `.opt` identiques. Il est vert sur `new_test`, qui n'a pas `ff9850f1` ; son commentaire
dit qu'il protège la prochaine fusion de `master` (#397).

**Ce que la cédule optimale ne garantit pas** : `setSolution` laisse de côté une superficie d'action `_LOCKEXEMPT` sous sa
tolérance (en CLP, une plantation de 0,007 ha à la période 3) ; les outputs se comparent donc à la tolérance de
`PlanScenario` (1 % de la valeur, ou 0,01 sous 1). `boundSolution` résout à chaque période : les périodes pas encore
bornées peuvent prendre une autre solution optimale (en MOSEK, 5,7 ha de plantation passent de la période 4 à la
période 5) ; seul l'optimum, unique, est comparé (règle 7).

**Valeurs attendues des statistiques**, lues sur le build puis justifiées :
- **Période 1, comptée à la main** sur les matrices écrites par `writeLp` (noms de lignes et de colonnes). Matrice de
  type II : un sommet qui n'a qu'un arc entrant et sa croissance n'a pas de ligne de transfert, et sa croissance prend la
  colonne de l'arc entrant. Racine (`division`, `equation`, `LP` sans présolve) : 62 sommets = 8 de la période 0, 8
  début de période 1, 8 brûlés, 6 d'âge 0 avec ligne (récupération et mort), 6 d'âge 0 sans ligne (coupe seule), 6
  verrouillés par `aCaribou` et 20 débuts de période 2 (la croissance des verrouillés rejoint celle de leur source) ; 69
  arcs = 8 + 29 (feu, coupe, caribou et croissance des 8 débuts de période, sauf la coupe à 7 ans de `UNITE3` et le
  caribou hors de `UC`) + 14 (récupération et mort des brûlés, sans récupération pour `UNITE3`) + 6 + 6 + 6 ; 22 lignes
  (8 + 8 + 6) ; 57 colonnes (69 moins les 12 croissances sans ligne). `LP` présolvé : `Burnedarea = 0` retire le feu, la
  récupération et la mort des brûlés ; 44 sommets, 42 arcs, 7 lignes (le début de période de `UNITE3` à 7 ans, sans
  coupe, n'en a plus), 28 colonnes. `harvestfromperiod3` : **prédit avant la mesure** à 50|56|22|50 (la racine sans
  ses 7 coupes, ni les 6 âges 0 qu'elles seules créent, ni leurs 6 débuts de période 2), mesuré identique.
- **Période 5** : les sommets et les arcs sont ceux des lignes `STATS` du lot 4 (`LP` 296|395, `division` et `equation`
  426|730), mesurés indépendamment ; leurs lignes ajoutent aux lignes de transfert (156 et 294) celles des contraintes
  (10 et 5).
- **Périodes 2 à 4** : lues sur le build, non comptées à la main. Elles croissent d'une période à l'autre, et le cas
  `…BuildsTheGraphOfDoPlanning` vérifie que la construction période par période donne le graphe de `doPlanning`.
- `division` et `equation` donnent les mêmes nombres : ils ont les actions, les transitions et l'inventaire de la
  racine, et rien ne fixe une action à 0. Le présolve ne les change pas.
- `harvestfromperiod3` contre `division` : 12 sommets de moins à la période 1 ; la période 2 ajoute 46 sommets, moins
  que la période 1 (50), puis la récolte fait ajouter 70, 88 et 106.

**Vérifications avant le build** (FMTlib du build de `new_test` du 2026-10-26, sources compilées par `cl` hors du dépôt,
`/W4 /permissive-`, aucun avertissement dans les nouvelles sources, lancées depuis `build/release/bin/Release`) :
- MOSEK et CLP : 13 cas actifs verts ; les 2 cas désactivés échouent pour leur raison (avec
  `GTEST_ALSO_RUN_DISABLED_TESTS`). Le cas spatial est vert. La copie en CLP a fait voir la plantation de 0,007 ha
  ci-dessus, d'où la tolérance du cas de `setSolution`.
- Configuration rejouée dans le scratchpad (options de `CMakeFMTVS2022vcpkg_GL.bat`, paquets vcpkg de `build/release`) :
  la cible `IntegrationModels` est créée, ctest y découvre les 15 cas, dont 2 désactivés, étiquettes `integration;models`,
  fixture `ExamplesModels` ; un seul avertissement, préexistant (CMP0167 de Boost).

**Vus rouges** (règle 1), copies faussées hors du dépôt, rien de commité :
- entrée faussée par cas : les 13 cas actifs échouent (scénarios des statistiques échangés, `doPlanning` sur `division`,
  cédules réduites de moitié par `getNewSchedule(0.5)` pour `setSolution`, `boundSolution` et `setSolutionByLp`, 103,28
  ha au lieu de 503,28, `aCaribou` sur un peuplement de `UC`, `isPeriodBounded` d'une période hors du graphe) ;
- assertions inversées (un en-tête inclus après `<gtest/gtest.h>`, assertions fatales rendues non fatales) : chaque
  assertion des 13 cas actifs échoue au moins une fois ; celles des 2 cas désactivés échouent sans inversion ;
- cas spatial : le `.opt` réécrit comme l'écrit `ff9850f1` (`_SIZE(_SIZE(COUPETOTALE,4))`,
  `_ADJACENCY(_ADJACENCY(COUPETOTALE,1))`, `_GROUP(COUPETOTALE,_TH1,2))`) fait lever `FMTexc(12)` à la relecture, et le
  cas échoue ; ses trois assertions échouent une fois inversées.
- Le dossier de sortie du cas spatial créé sous `build/release/tests/IntegrationParser` a été effacé ; ctest n'a pas été
  lancé.

**Observation laissée ouverte** : l'écriture d'une contrainte `= 0` donne `BURNEDAREA = -0.000000`, qui se relit à
l'identique.

**Validation après le build de Gabriel** (2026-10-27, commit `50b67582` de Gabriel, build reconfiguré ; FMTlib.dll
du 2026-10-26 à 17 h 45) :
- l'arbre de travail est identique à `HEAD`, et le commit contient les 13 fichiers du lot (`git show --stat`) ;
  `IntegrationModels.exe` du build date de 17 h 50, `IntegrationParser.exe` de 17 h 51 ;
- installation (`INSTALL_TEST_SUITE=public`, 2026-10-26 à 17 h 56) : 740 tests, 0 échec, 707 exécutés et 33
  désactivés, 25 s ; unit 410 verts et 10 désactivés, integration 35 verts et 11 désactivés, system 237 verts et 12
  désactivés, performance 23, other 2. Comparée au passage du point 0 (724 tests, section 6.1), elle n'ajoute que les
  16 cas du lot (14 verts, les 2 désactivés des défauts 19 et 20) ; aucun cas n'est retiré ni ne change d'état ;
- suite privée de Gabriel (2026-10-27, 8 h 41 à 8 h 57) : 94 réussis, 0 échec ;
- `InstallTests.xml`, `InstallTests.json`, `LastTest.log` et `LastTestsDisabled.log` copiés dans le scratchpad de la
  session avant tout appel de ctest ;
- `RunTests.bat all-public` (2026-10-27, 9 h 02) : 740 tests, 0 échec, 707 verts et 33 désactivés, 19 s réelles ;
  integration 35 verts et 11 désactivés sur 46, les autres niveaux comme à l'installation. Les comptes sont ceux
  attendus avant le build.

### 5.15 Lot 8a : actions, transitions, développements, cédules et listes (2026-10-27)

**Statut** : livré le 2026-10-27, commité par Gabriel le même jour (`2ca9a023`), **validé** le 2026-10-27 sur ses 180 premiers
cas (fin de cette section) ; les 2 cas du défaut 21 (#402) sont à valider au prochain build.
Premier tiers du lot 8 (section 6.3.5) : les 15 classes de priorité 1 dont le statut était `Lot 8` et qui construisent et font
évoluer le modèle. Point de départ : le passage validé du 2026-10-27 (`50b67582`, section 5.14), arbre de travail propre.

**Ajouts** (`Tests/Unit/Core`, cp1252, CRLF, en-tête de licence) : une source par classe, une suite au nom de la classe, cas
Given…When…Then en Arrange / Act / Assert ; 182 cas nouveaux, dont 1 désactivé (défaut 21, ajouté avec un cas vert après la
validation, à la demande de Gabriel). `Tests/Unit/Core/CMakeLists.txt` prend les sources par un glob : inchangé.

| Source | Cas | Ce qu'ils couvrent |
|---|---|---|
| `FMTAction.cpp` | 31 | construction (nom, verrou, remise à zéro de l'âge ; nom seul : `_LOCKEXEMPT`, sans remise à zéro), bornes d'âge et de période d'`update` (plusieurs spécifications, spécification sans âge, action vide : bornes inversées), #393, égalité, ordre, hachage, chaînes (`*ACTION`, `*OPERABLE`, `_LOCKEXEMPT`, `*PARTIAL`), `partial`, `useYield`, agrégats (`isPartOf`, `setAggregates`), `operator+=` comme `FMTModel::aggregateActions` (drapeaux de la première action, partiels perdus avec un avertissement, `_LOCKEXEMPT` transmis, action mise à jour : `FMTinvalid_action`, puis `unShrink`), `notUse`, `split`, presolve, GCBM (`~GCBM:<id>:<nom>` ; absent : `FMTfunctionfailed`), séries (`setSeries`, `isInSeries`, `isAllowedInSerie`, `getSerie`, `getLargestSerieSize`) |
| `FMTActionComparator.cpp` | 5 | nom, agrégat avec et sans vérification, `find_if`, `getAllAggregates` (qui regarde toujours les agrégats), `getAllAggregatesSet` |
| `FMTTransition.cpp` | 22 | construction, nom, copie, égalité (nom et fourches), ordre, chaînes (`*CASE`, `*SOURCE` et sa condition, `*TARGET`), `getFork` (première source qui contient le développement, condition d'âge, de yield, #358, aucune source), `isLeaking`, `single`, `ageAfter` (moyenne entière), `mainTarget`, `attributeTargets`, `getStaticThemes`, `canProduce`, `operator+=` (et `FMTinvalid_action` après `update`), presolve |
| `FMTTransitionComparator.cpp` | 2 | nom, `find_if` |
| `FMTTransitionMask.cpp` | 19, dont 1 désactivé | construction, proportion (`setProportion` limite à 100, le constructeur non), `trans` (un attribut, `?`, tous les thèmes, agrégat), `disturb` (remise à zéro, âge, verrou, période et proportion gardés, `_AGE`, `_LOCK`, yield cible), égalité, chaînes, `get`, construction depuis une autre cible, presolve, cible d'un nouveau thème (construite ; par `setMask`, désactivé : défaut 21) |
| `FMTFork.cpp` | 15 | `add`, `clear`, `sumProp`, `getPaths` (une et deux cibles ; cibles identiques : `FMTsame_transitiontargets` et, en avertissement, proportions additionnées ; cible qui ne change rien : `FMTsourcetotarget_transition` et, en avertissement, un niveau de verrou de plus ; remise à zéro à l'âge 0), `single`, `getMax`, égalité, chaîne, presolve |
| `FMTActualDevelopment.cpp` | 20 | constructions, copie, clone, déplacement, égalité (aire comprise), ordre, chaînes (`*A`, `_lock`), `setArea`, `getArea` par la classe de base, `reduceLockToDeath` (sans verrou, verrou qui finit à la durée de vie, après : `FMTdeathwithlock` ; en avertissement : verrou réduit, ou gardé un an après la durée de vie ; sans durée de vie), presolve |
| `FMTActualDevelopmentComparator.cpp` | 3 | mêmes valeurs, aire ignorée, `find_if` dans la section des superficies |
| `FMTFuturDevelopment.cpp` | 7 | construction, défaut, aire nulle (aussi par la classe de base, et depuis un développement réel), copie, clone, déplacement, égalité |
| `FMTDevelopmentPath.cpp` | 5 | défaut, construction, `setProportion`, développement modifié par référence, copie |
| `FMTSchedule.cpp` | 24 | construction, `addEvent` (aires additionnées, niveau de verrou du développement déverrouillé, cédule avec verrou), `operated`, `+=` et `+`, construction depuis des cédules et depuis des aires par verrou, égalité (totaux par développement), chaînes (avec et sans verrou), `clean`, `sort`, `getNewSchedule`, `setPeriod`, `getOperabilities`, action absente (`actionArea`, `find` ; `at` : `std::out_of_range`), action de même nom, `operator[]`, `isFuturConstraints`, presolve, postsolve |
| `FMTLifespans.cpp` | 8 | ordre, chaîne, `findSets`, copie, égalité, `swap`, presolve (avec et sans durée de vie) |
| `FMTList.cpp` | 15 | à travers `FMTLifespans` (`FMTList<int>`) et `FMTAction` (`FMTList<FMTSpec>`) : liste vide, `push_front`, `insert`, `erase`, `pop_back`, `findSets` avant et après `update` (et deux fois), `filterMask`, `unShrink`, copie, `+=` (et `FMTinvalid_action` sur une liste réduite), égalité (ordre compris), `clear`, `getUnion`, `push_back` d'une liste |
| `FMTMaskComparator.cpp` | 3 | bits égaux, nom ignoré, `find_if` |
| `FMTThemeComparator.cpp` | 3 | thème égal, thème présolvé (avec et sans l'option), `find_if` |

- Niveau unitaire : aucun modèle sur disque. Thèmes de TWD_land comme au lot 7c (`FMTTheme.cpp`) ; actions et transitions de
  TWD_land construites comme `FMTActionParser`, `FMTTransitionParser` et `FMTModel` (une spécification ou une fourche par masque,
  puis `update`) ; agrégation comme `FMTModel::aggregateActions` (une action du nom de l'agrégat, puis `+=` d'actions défaites par
  `unShrink`) ; durées de vie de TWD_land (`? feu ? 0`, `? ? ? 80`) ; presolve de `FMTMask.cpp` et postsolve de
  `FMTModel::getPostSolveFilter`. `FMTList` est un modèle de classe : il est testé à travers ses deux instanciations exportées les
  plus simples. Aucun cas ne contourne le code : aucune ligne de dette.
- Erreurs converties en avertissements (`FMTsame_transitiontargets`, `FMTsourcetotarget_transition`, `FMTdeathwithlock`, que
  `FMTExceptionHandler::getErrorsToIgnore` liste) : le gestionnaire d'exceptions d'`FMTObject` est statique, partagé par tous les
  objets du processus ; une garde (`ErrorsAsWarnings`, dans `FMTFork.cpp` et `FMTActualDevelopment.cpp`) lui rend sa liste vide à
  la fin du cas (section 7).
- Valeurs calculées à la main avant la première exécution, depuis la documentation ou le code : bornes de `_setBounds` (la plus
  petite et la plus grande des spécifications ; 0 à l'infini pour une spécification sans âge ; sans spécification, la borne basse
  au-dessus de la haute), chaînes (`std::to_string` à six décimales, `toString` des bornes), `ageAfter` (10 + 21) / 2 = 15 en
  entier, `reduceLockToDeath` 10 - (75 + 10 - 80) + 1 = 6, `VOLUMETOTAL` 150 à 10 ans pour la cible de yield (l'âge de base le plus
  proche sous l'âge du développement). Un seul attendu s'est révélé faux à la première exécution, corrigé après relecture du code
  et non recopié : `FMTempty_action` est un avertissement (`FMTEmptyAction` dérive de `FMTWarning`), si bien que
  `getGCBMActionId` sans agrégat GCBM lève `FMTfunctionfailed` en lisant la définition vide.

**#358 et #393, à l'endroit où ils agissent** (cas verts, sans défaut nouveau, section 2.7) :
- #393 : `FMTAction.GivenAMinimumAgeWrittenWithGreaterThanAsTheParserReadsItWhenUpdateThenTheActionAcceptsEveryAge` construit
  l'action avec les bornes que `FMTParser::bounds` rend pour `_AGE > 10` (du plus petit au plus grand entier) : `update` lui donne
  ces bornes, elle accepte l'âge 5 et s'écrit sans condition (`? PROD ?  `), comme la sonde du lot 7c l'avait vu à l'écriture. La
  correction est dans le parseur, que les deux cas désactivés d'`IntegrationParser` attendent ; aucun cas de `FMTAction` ne l'exige.
- #358 : `FMTTransition.GivenASourceWithoutConditionWhenGetForkThenGivesTheForkAtEveryAge` : une fourche sans condition, ce que
  `FMTParser::_setSpec` laisse d'une condition écrite comme dans la section des actions, est rendue par `getFork` à 5 comme à 15
  ans ; à côté, `GivenASourceAgeConditionWhenGetForkThenGivesNoForkOutsideItsAges` (`@AGE(10.._MAXAGE)`) n'en rend aucune à 5 ans.

**Défaut nouveau** : le défaut 21 (section 2.7, #402), démontré après la validation, à la demande de Gabriel : l'observation de
`setMask` ci-dessous.

**Observations, sans défaut annoncé** (rien de ce que FMT exécute n'en dépend, ou ce n'est pas démontré par un exécutable officiel) :
- **`FMTTransitionMask::setMask` ne refait pas la sélection** (`build`), devenu le défaut 21 (section 2.7) : `trans` garde celle
  du masque de construction.
  `FMTModel::pushTheme`, qu'appelle `FMTModel::buildAction`, passe ainsi les cibles des transitions au masque du nouveau thème.
  Sonde A/B hors du dépôt (FMTlib du build du 2026-10-26), thèmes de TWD_land plus un thème `VPRE`, `VPOST` ajouté comme le fait
  `pushTheme` : la cible `? FEU ? ?` construite avec les quatre thèmes donne `UNITE1 FEU UTR1 VPOST` pour `UNITE1 PEUPLEMENT1 UTR1
  VPOST` ; la cible `? FEU ?` des trois thèmes passée par `setMask` lève `FMTexc(53)` puis `FMTexc(19)` (`FMTundefined_attribute`).
  Non démontré sur un exécutable officiel : le presolve refait la sélection (`FMTTransitionMask::preSolveRef` appelle `build`), et
  `BuildAction`, test privé, écrit le modèle bâti et le relit avant de s'en servir (son `doPlanning` est en commentaire). À
  démontrer, si Gabriel le souhaite, sur un modèle bâti par `buildAction` puis construit sans presolve ni relecture : fait le
  2026-10-27 (section 2.7, défaut 21).
- **`FMTActualDevelopment::operator<` n'est pas un ordre faible strict** : il demande un développement et une aire plus petits,
  si bien que deux développements différents peuvent n'être ni avant ni après l'autre (cas vert commenté). `FMTSrModel::getArea`
  les trie pourtant par `std::sort` (depuis `22f2e94e`, 2021-04-16 ; l'opérateur date de `b6acf940`, 2019-07-22). Effet sur un
  résultat : non démontré.
- **`FMTActualDevelopment::reduceLockToDeath`** lève dès que l'âge plus le verrou dépasse la durée de vie, mais le verrou qu'il
  donne en avertissement vaut les années restantes plus un : à 71 ans avec un verrou de 10 et une durée de vie de 80 ans, l'erreur
  est levée, et l'avertissement « réduit » le verrou à 10 (cas vert commenté ; trouvé par l'entrée faussée du vu rouge, ci-dessous).
  À l'âge de la durée de vie, le verrou devient 0, au-delà 1 (variable `fix`) : rien ne dit ce que fait Woodstock, d'où un test
  manquant.
- **`FMTFork::getMax`** ne met jamais à jour la plus grande proportion vue : il rend le développement de la dernière cible de
  proportion positive (cas vert commenté). Aucun appelant dans FMT ; ni Python ni R ne l'exposent.
- **`FMTList::push_back` d'une liste** remplace les éléments au lieu de les ajouter (cas vert commenté) ; son seul appelant,
  `FMTModel::push_back`, jette le résultat (il finit par `*this = rhs`, les affectations sont en commentaire). `clear` garde le
  filtre : une liste réduite puis vidée ne s'additionne plus (cas vert). `erase`, `insert`, `push_front` et `pop_back` ne vident pas
  le cache de `findSets` (lu dans le code, non vérifié).
- **`FMTSchedule::clean` et `sort`** déplacent les aires dans le vecteur des niveaux de verrou : la position n'est plus le niveau
  (cas verts commentés). `FMTSrModel::setSolution` répartit les aires d'une action `_LOCKEXEMPT` sur les développements verrouillés
  du graphe, quelle que soit leur position : sans effet démontré. Une cédule sans verrou s'écrit sans le niveau de chaque ligne.
- **Documentation des en-têtes contredite par le code** (cas verts commentés) : `FMTAction::operator<` (« la longueur puis le nom » :
  le nom seul), `FMTAction::operator==` et `FMTTransition::operator==` (« le nom » : aussi les spécifications, les drapeaux et les
  partiels, ou les fourches), `FMTAction::operator+=` (« une exception si les remises à zéro diffèrent » : un avertissement, et les
  partiels perdus), `FMTTransition::isLeaking` (« différente de 1 » : de 100), `FMTList::push_back` d'une liste (« à la fin »).
- `FMTActionComparator::getAllAggregates` regarde les agrégats même quand le comparateur ne les vérifie pas, et un `FMTAction`
  construit avec son seul nom est `_LOCKEXEMPT`, ce dont part `FMTModel::aggregateActions` (cas verts).

**Non couvert, et pourquoi** : le presolve avec compression des masques (`_compressMasks`, des actions, des transitions et des
durées de vie) ; `FMTAction::getSeriesNames` (aucun appelant) ;
`FMTList::findSetsWithFiltered`, `reserve`, `apiPushBack` ; le verrou que `reduceLockToDeath` donne à l'âge de la durée de vie et
au-delà ; la sérialisation (lot 9).

**Vus rouges** (règle 1), rien de commité : deux copies faussées des 15 sources, hors du dépôt, compilées par `cl` contre le
`FMTlib.lib` de `build/release` :
- **entrée faussée** : une substitution par cas, dans son Arrange ou son Act, jamais dans son Assert (une table du script qui écrit
  la copie ; il refuse une substitution introuvable, ambiguë ou placée dans l'Assert, et un cas sans substitution) : les 180 cas
  échouent, aucun par une exception. Un premier passage a laissé un cas vert, `…GivenALockThatEndsAtTheLifespan…`, dont l'entrée
  faussée (71 ans au lieu de 70) donnait le même verrou : c'est l'observation de `reduceLockToDeath` ci-dessus. Son entrée faussée
  est passée à 72 ans, et un cas commenté fixe le comportement à 71 ans ;
- **assertions inversées** (un en-tête inclus après `<gtest/gtest.h>`, assertions fatales rendues non fatales ; le script refuse
  toute autre macro que les dix employées) : les 180 cas échouent, aucun par une exception, et les 490 assertions échouent chacune
  une fois (490 messages ; GoogleTest situe les 11 assertions écrites sur deux lignes à leur dernière ligne) ;
- aucun cas désactivé dans ce lot ; les 10 cas désactivés d'avant, lancés avec `GTEST_ALSO_RUN_DISABLED_TESTS=1`, échouent toujours.

**Vu vert avant le build de Gabriel** : les 34 sources du dépôt, compilées par `cl` avec les options de `UnitCore` (son
`CL.command.1.tlog`), `/Fo` et `/Fd` dans le scratchpad, liées au `FMTlib.lib` de `build/release` (FMTlib du 2026-10-26 à 17 h 45,
sources de FMTlib inchangées depuis `025d3493`), lancées depuis le scratchpad avec `bin/Release` dans le `PATH` et
`onnxruntime.dll` copiée à côté (section 7) : 590 cas verts (410 d'avant et 180 nouveaux), 10 désactivés ; mêmes résultats dans un
ordre mélangé (`GTEST_SHUFFLE`, graines 4242 à 4244, 3 répétitions). En `/W4 /permissive-`, aucun avertissement ne vient des 15
sources nouvelles : les 635 viennent des en-têtes de FMTlib. ctest n'a pas été lancé ; rien n'a été écrit dans `build/release` ni
sous `Examples/Models`.

**Documents** : `CoreTestCoverage.csv` (statut `Lot 8a, done` des 15 classes, #358 et #393, remarques des observations, tests
manquants), `CoreTestCoverage.md` régénéré (34 classes avec des tests unitaires, 16 de priorité 1 sans test propre au lieu de 31 :
celles du lot 8b ; deux générations identiques), sections 2.7, 6.1, 6.3.5 et 7, changelogs (section non publiée, anglais et
français).

**Validation après le build de Gabriel** (2026-10-27, sur les 180 premiers cas, avant ceux du défaut 21 ; sources du lot non commitées ; FMTlib inchangé, `FMTlib.dll` du 2026-10-26
à 17 h 45) :
- `UnitCore.exe` du build date de 10 h 08, après la dernière modification des sources du lot (9 h 51) ;
- installation (`INSTALL_TEST_SUITE=public`, 10 h 09) : 920 tests, 0 échec, 887 exécutés et 33 désactivés, 30 s ; unit 590 verts
  et 10 désactivés, integration 35 verts et 11 désactivés, system 237 verts et 12 désactivés, performance 23, other 2 ; aucun test
  en `T:/`. `InstallTests.xml`, `InstallTests.json`, `LastTest.log` et `LastTestsDisabled.log` copiés dans le scratchpad de la
  session avant tout appel de ctest ;
- `RunTests.bat all-public` : « 0 tests failed out of 887 », 27,3 s réelles ; unit 590 verts et 10 désactivés sur 600,
  integration 35 verts et 11 désactivés sur 46, system 237 verts et 12 désactivés sur 249, performance 23, other 2 ; Total 887
  verts et 33 désactivés sur 920. Par rapport au passage du 2026-10-27 (section 5.14), seul le niveau unit change, des 180 cas
  ajoutés, tous verts : les comptes attendus avant le build. La fixture `ExamplesModelsUnchanged` et les tests Python sont verts,
  et l'arbre de travail est resté le même après le passage ;
- non relues : les consoles de la configuration et du build. Les avertissements ne sont couverts que par la passe `/W4` hors dépôt
  d'avant le build. Le lot n'ajoute que des tests unitaires : la suite BFEC n'est pas relancée.

**Défaut 21, après la validation** (2026-10-27, à la demande de Gabriel) : deux cas de plus dans `FMTTransitionMask.cpp`, le cas
vert d'une cible construite avec un nouveau thème et le cas désactivé de `setMask` (section 2.7). Vérifiés hors du dépôt comme le
reste du lot : `UnitCore`, 591 verts et 11 désactivés ; mêmes résultats dans un ordre mélangé (graines 4242 à 4244, 3 répétitions) ;
le cas désactivé, lancé avec `GTEST_ALSO_RUN_DISABLED_TESTS=1`, échoue pour sa raison (`trans` lève, le masque n'est pas
`UNITE1 FEU UTR1 VPOST`) ; entrée faussée, 181 cas actifs sur 181 en échec ; assertions inversées, 181 en échec et chacune des 491
assertions actives une fois ; en `/W4 /permissive-`, aucun avertissement de la source.

**Validation des cas du défaut 21 après le build de Gabriel** (2026-10-27, commits `2ca9a023` et `3e6be3e6`, arbre de travail
propre ; FMTlib inchangé) : `UnitCore.exe` du build date de 11 h 02, après la dernière modification de `FMTTransitionMask.cpp`
(10 h 42) ; installation (`INSTALL_TEST_SUITE=public`, 11 h 03) : 922 tests, 0 échec, 888 exécutés et 34 désactivés, 25 s ; unit
591 verts et 11 désactivés, integration 35 verts et 11 désactivés, system 237 verts et 12 désactivés, performance 23, other 2 ;
aucun test en `T:/`. Le cas vert de la cible au nouveau thème passe, celui de `setMask` est désactivé : les comptes attendus.
Relu dans `InstallTests.xml`, `LastTest.log` et `LastTestsDisabled.log`, copiés dans le scratchpad de la session ; ctest n'a
pas été relancé.

### 5.16 Lot 8b : rendements, outputs et classes de service (2026-10-27)

**Statut** : livré le 2026-10-27, **à compiler et à valider par Gabriel** ; rien n'est commité. Deuxième tiers du lot 8 (section
6.3.5) : les 16 classes de priorité 1 dont le statut était `Lot 8`. Point de départ : le passage validé du 2026-10-27 à 11 h 03
(`2ca9a023`, `3e6be3e6`, section 5.15), arbre de travail propre. Une première session s'est arrêtée à la lecture, sans rien
écrire ; celle-ci a fait le lot.

**Ajouts** (`Tests/Unit/Core`, cp1252, CRLF, en-tête de licence) : une source par classe, une suite au nom de la classe, cas
Given...When...Then en Arrange / Act / Assert ; 143 cas nouveaux, dont 2 désactivés (défauts 22 et 23).
`Tests/Unit/Core/CMakeLists.txt` prend les sources par un glob : inchangé.

| Source | Cas | Ce qu'ils couvrent |
|---|---|---|
| `FMTYieldRequest.cpp` | 11 | construction (adresse du développement, ni sommet ni gestionnaire), masque résumé (le masque du développement par le filtre des rendements ; deux développements qui ne diffèrent que par des thèmes qu'aucun masque ne nomme), gestionnaires qui sélectionnent le développement dans leur ordre, `getFirstSeen` (premier gestionnaire du yield, fin des rendements), âge du développement relu à chaque appel, gestionnaires gardés quand le masque change (ligne de dette), requête construite depuis une autre (même masque ; autre masque : `FMTfunctionfailed`), copie, développement qu'aucun gestionnaire ne sélectionne |
| `FMTYieldDevelopment.cpp` | 6 | période, égalité et hachage, une valeur qui diffère (âge, période, masque, masque plus long, yield de même longueur, yield plus long), âge et période sur 8 bits (256 vaut 0, 300 vaut 44, -1 vaut 255), ensemble haché |
| `FMTYieldsCache.cpp` | 10, dont 1 désactivé | `set`, puis `tryToGet`, `get` et `inCache` ; clé absente (`get` vaut 0) ; clé de l'âge et de la période, pas du verrou ; requête non préparée (clé sans masque) ; masque résumé partagé ; `set` qui garde la première valeur ; deux caches qui partagent leurs valeurs ; yield complexe servi par le cache, même valeur ; valeur du cache servie au lieu de la définition ; désactivé : défaut 22 |
| `FMTYieldHandler.cpp` | 8 | méthodes des dérivées sur la base (`FMTfunctionfailed`), masque, âges de base (`pushBase` rend faux), `getLinearValue` sur des valeurs données, `operator==` par la base (#383), `compare`, indice de surcharge, tabous et index de la base et d'un gestionnaire d'âge, presolve |
| `cmpYieldString.cpp` | 5 | la longueur d'abord, puis les caractères depuis le dernier, nom égal, tri, ordre des yields d'un gestionnaire (`getDataElements`) |
| `FMTOutputSource.cpp` | 29 | constante (valeur de la période, la première avant, la dernière après), yield de temps (sans valeur, 1), source par défaut (`std::out_of_range`), sources d'action et d'inventaire (période suivante), niveaux, `use` (masque et spécification), `getCoef` (yield d'inventaire, superficie, constante de la période, yield de temps, constante fois yield de temps, yield que récolte une action), `targets` et `targetsSet` (agrégats), `isNull`, `resetValues`, égalité sans le yield ni les valeurs, hachage, ordre cohérent avec l'égalité, chaînes, `trimDouble`, `isSubsetOf` (sans et avec les actions), `isSameButDifferentAction`, presolve, `getConstantValue` (deux cas), accesseurs |
| `FMTOutputSourceComparator.cpp` | 3 | première source variable, première constante (ni un niveau, ni un yield de temps), aucune |
| `FMTOutputComparator.cpp` | 3 | nom, autre casse et nom absent, premier de deux outputs de même nom |
| `FMTOutputNode.cpp` | 25 | constructions (dont le nœud général de l'ordonnanceur), `isNull` (facteur ou constante nuls ; nœud par défaut : `std::out_of_range`), une ou plusieurs périodes, période passée, `setPeriod`, `setToGraph` (sans période, inventaire de la période suivante, une période, période précédente et période 0, plusieurs périodes jusqu'à la dernière du graphe, somme, moyenne, moyenne de trois périodes, moyenne à la période 0), égalité, ordre et hachage par la source, yield de la source, chaîne, masque de hachage, `isSubsetOf`, `allowCashDeduction`, `getOutputId`, `fillUpEquation`, copie |
| `FMTOutputNodeHasher.cpp` | 3 | même hachage pour les mêmes valeurs, hachages qui diffèrent, ensemble non ordonné avec le comparateur de valeurs |
| `FMTOutputNodeValueComparator.cpp` | 2 | mêmes valeurs (et le nœud avec lui-même), une valeur qui diffère (constante, facteur, yield, moyenne) |
| `FMTOutputNodeOriginComparator.cpp` | 2 | tri par output d'origine, même origine, nœud sans origine (-1) |
| `FMTMaskFilter.cpp` | 12 | filtre par défaut, filtre des masques d'une liste (bits communs forcés, nom du développement gardé), propriété de `findSets`, masques qui ont tous tous les bits, sélection, sélection et tampon, `getSelectedThemes`, `canPreSolve`, presolve du filtre, `swap`, copie et hachage, hachage d'un filtre dont le tampon est la sélection |
| `FMTLookup.cpp` | 7 | par `FMTLookup<descripteur de sommet, FMTDevelopment>`, comme `FMTGraph::getDevsSet` et `containsDevelopment` : recherche dans un ensemble haché, développement absent, égalité et ordre des développements, hachage, adresse gardée, recherche sans développement ; spécialisation `FMTLookup<std::vector<size_t>, std::string>` |
| `FMTConstants.cpp` | 8, dont 1 désactivé | `isConstant` et `length` (avec le `#` seulement), `set` qui remplace, copie, aucune constante, chaîne d'une constante d'une valeur et de deux constantes ; désactivé : défaut 23 |
| `FMTObject.cpp` | 9 | gestionnaire et journal partagés, erreur levée sans être journalisée, erreur convertie en avertissement (journalisée « Ignoring », sans exception), liste vidée, nombre d'avertissements avant le silence, exceptions sans imbrication, journal passé à tous les objets, mémoire disponible, emplacement de FMTlib |

- Niveau unitaire : aucun modèle sur disque. Thèmes de TWD_land comme au lot 8a ; rendements construits comme `FMTYieldParser` les
  construit (gestionnaires d'âge, de temps et complexe de `FMTYields.cpp`) ; sources d'output comme `FMTOutputParser` ; presolve de
  `FMTMask.cpp`. `FMTLookup` est un modèle de classe : il est testé par son instanciation dans le graphe, `FMTLookup<std::size_t,
  FMTDevelopment>` (le descripteur de sommet du `boost::adjacency_list<vecS, vecS, bidirectionalS>` de `FMTGraph`), et par la
  spécialisation de hachage que déclare `FMTLookup.h`, qu'aucun code n'emploie. `FMTYieldHandler` n'est pas abstraite : ses cas
  portent sur la base elle-même et sur ce qu'un gestionnaire d'âge en hérite, sans refaire ceux des lots 7a et 7b.
- **Deux lignes de dette** dans `Architecture.md` :
  - la table statique de `FMTYieldsCache`, qu'un test ne peut ni vider ni garder pour lui : chaque cas prend des noms de yields
    qu'aucun autre cas d'`UnitCore` n'emploie, et aucun n'attend l'absence d'une clé qu'un cas a posée, si bien que les cas gardent
    leurs résultats dans tout ordre et répétés ;
  - `FMTConstants::get` et `getAll`, membres modèles instanciés dans `Source/FMTConstants.cpp`, que la DLL n'exporte pas (LNK2019
    au premier lien hors du dépôt, `dumpbin /exports` le confirme) : les cas lisent les valeurs par `operator std::string`, et la
    lecture et la conversion des constantes restent couvertes par les parseurs et `ConvertStringsToConstants`.
- Gestionnaire et journal d'`FMTObject` : statiques. Les cas qui les changent passent par une garde (`KeptLog`, dans
  `FMTObject.cpp`) qui installe un gestionnaire par défaut neuf, dont les comptes d'avertissements partent de zéro, et un journal
  qui garde ce que FMT écrit (dérivé de `FMTLogger`, `_cout` redéfini), puis rend un gestionnaire et un journal par défaut à la fin
  du cas (section 7).
- Valeurs calculées à la main avant la première exécution, depuis la documentation ou le code : masques résumés (les bits que tous
  les masques de la liste ont, plus ceux du développement hors de ces bits : `1111111001` pour `UNITE1 PEUPLEMENT1 UTR1` avec les
  masques `UNITE1 ? ?`, `? ? ?` et `UNITE2 ? ?`), valeurs d'âge (2 000 à 20 ans), `VOLUMETOTAL` 120 à 7 ans (de 100 à 5 ans à 150
  à 10 ans), `_MULTIPLY(VOLUME,2)` 400 et `_MULTIPLY(VOLUME,3)` 600, périodes de `setToGraph`, constantes de moyenne 2 / 4 = 0,5 et
  3 / 3 = 1, valeurs d'inventaire 10 x 120 = 1 200 et (10 + 5) x 120 = 1 800, chaînes (`std::to_string` à six décimales,
  `trimDouble`), messages du gestionnaire (`FMTexc(42)`, « Ignoring: », « has reached the maximum number of times it can be
  raised. »), avertissements journalisés 3 fois sur 6 pour un maximum de 2 (`_updateWarningCount`). Deux attendus se sont révélés
  faux à la première exécution, corrigés après relecture du code et non recopiés : la chaîne d'une copie de `FMTConstants` (l'ordre
  d'une `boost::unordered_map` change d'une copie à l'autre : les cas comparent les lignes triées) ; le filtre de masques qui ont
  tous tous les bits, qui devait garder tous les bits du développement (observation ci-dessous). `FMTAgeYieldHandler::getYieldNames`
  trie ses noms par ordre alphabétique : le cas de `cmpYieldString` lit l'ordre par `FMTComplexYieldHandler::getDataElements`
  (relevé dans le code avant sa première exécution).

**#383, à l'endroit où il agit** (cas vert, sans défaut nouveau) :
`FMTYieldHandler.GivenTwoAgeHandlersThatDifferByTheirValuesOnlyWhenComparedThroughTheBaseClassThenTheyAreEqual`.
`FMTAgeYieldHandler` déclare son propre `operator==(const FMTAgeYieldHandler&)` au lieu de redéfinir celui de la base, qui ne
compare que le masque et les âges de base : comparés par la base, deux gestionnaires d'âge de valeurs différentes sont égaux. Une
correction de #383 qui comparerait les gestionnaires de `FMTYields` par `FMTYieldHandler::operator==` les prendrait pour égaux.
#346, #347, #381 et #389 à #392 n'agissent dans aucune classe du lot.

**Défauts nouveaux** : 22 et 23 (section 2.7), démontrés avec `FMT.pyd`.

**Observations, sans défaut annoncé** (rien de ce que FMT exécute n'en dépend, ou ce n'est pas démontré) :
- **`FMTMaskFilter(masks)`, quand tous les masques ont tous les bits** : la sélection devait les garder tous, mais
  `boost::dynamic_bitset<uint8_t>(taille, true)` (`Source/FMTMaskFilter.cpp:83`) lit `true` comme l'entier 1 : la sélection est le
  seul bit 0, et le masque résumé ne garde que `UNITE1` (cas vert commenté). `FMTList::findSets` reste juste, puisque chaque masque
  d'une telle liste sélectionne chaque développement, et les rendements de deux développements dont la clé se confond sont les
  mêmes. Depuis `ba3ccf51` (2021-08-30). Effet sur un résultat : aucun trouvé.
- **`FMTOutputSource::getConstantValue`** lit `(use && sans action) || opérable à l'action` (`Source/FMTOutputSource.cpp:347`) : une
  source d'action compte tout développement opérable à l'action, hors de son masque (cas vert commenté, 1 800 au lieu de 1 200).
  Ni elle ni `canBeDeducedToConstant` n'ont d'appelant dans FMT.
- `FMTOutputSource::operator==` ignore le yield et les valeurs (section 5.10), que le hachage lit : deux constantes égales de valeurs
  différentes ont des hachages différents (cas vert commenté). FMT ne hache les sources que par `FMTOutputNode::hashForValue`, qui
  compare aussi les valeurs.
- `FMTOutputSource::trimDouble` retire aussi les zéros d'un entier écrit sans point (`100` devient `1`) ; FMT ne lui passe que des
  nombres écrits par `std::to_string`.
- Une source sans valeur (`FMTOutputSource()`) et le facteur d'un nœud par défaut : `getValue` et `isNull` lèvent `std::out_of_range`.
- `FMTOutputNode::setToGraph` : à la période 0, une moyenne sans période ne vise aucune période, et sa constante devient infinie
  (1 / 0) ; le nœud n'apporte aucun développement, la valeur reste 0.
- `FMTOutputNodeValueComparator` est une égalité, pas l'ordre que dit sa documentation ; ni lui ni `FMTOutputNodeHasher` n'ont
  d'utilisateur dans FMT.
- `FMTMaskFilter::hash` combine la sélection et le tampon par un ou exclusif : 0 quand ils sont égaux, comme dans
  `FMTModel::getPreSolveFilter` ; rien dans FMT ne hache un filtre.
- `FMTLookup` : une recherche sans développement (pointeur nul) précède toute recherche, elle-même comprise, si bien que l'ordre
  n'est pas strict sur elle ; FMT construit toujours ses recherches depuis un développement.
- `FMTYieldsCache::set` garde une valeur déjà présente (`insert` d'une `concurrent_flat_map`) : une valeur du cache ne se corrige pas.
- `FMTYieldHandler::pushBase` rend toujours faux, alors que sa documentation dit vrai quand l'âge est poussé.
- `FMTObject::setMaxWarningsBeforeSilenced(n)` : un avertissement est journalisé n + 1 fois, puis une ligne dit qu'il a atteint son
  maximum (cas vert commenté) ; la documentation parle du nombre d'avertissements levés avant le silence.
- `FMTConstants` : `length` et `getAll` demandent le `#` du nom, `get` non (lu dans le code ; `get` et `getAll` ne se lient pas) ; la
  chaîne écrit les constantes dans l'ordre d'une table non ordonnée.
- `FMTLookup.h` ne se compile pas seul : il emploie `std::string` et `std::vector` sans les inclure (section 7).

**Non couvert, et pourquoi** : les requêtes de rendement d'un sommet du graphe (`FMTGraphVertexToYield`) et
`FMTYieldsCache::reserve`, qui en demande une ; `_clearIfTooBig` (10 000 valeurs et moins de 10 Go libres) ; la requête par défaut
(pointeurs non initialisés) et `getFirstSeen` avant une première utilisation ; `FMTYieldHandler::postSolve` d'un masque à agrégat ;
`FMTConstants::get` et `getAll` (non exportés) ; `FMTObject::redirectLogToFile` (écrit un fichier), les autres gestionnaires et
journaux, `setTerminateStack`, `setAbortStack` et `checkSignals` ; `FMTOutput::getNodes` (les nœuds d'un output d'outputs) ; la
concurrence (lot 9).

**Vus rouges** (règle 1), rien de commité : deux copies faussées des 16 sources, hors du dépôt, compilées par `cl` contre le
`FMTlib.lib` de `build/release` :
- **entrée faussée** : une substitution par cas actif, dans son Arrange ou son Act, jamais dans son Assert (une table du script qui
  écrit la copie ; il refuse une substitution introuvable, ambiguë ou placée dans l'Assert, et un cas actif sans substitution ; une
  inclusion ajoutée hors des cas, dans la copie de `FMTYieldHandler.cpp`) : les 141 cas actifs échouent, aucun par une exception ;
- **assertions inversées** (l'en-tête du lot 8a, inclus après `<gtest/gtest.h>`, avec `EXPECT_GT` en plus des dix macros) : les 141
  cas échouent, aucun par une exception, et les 426 assertions actives échouent chacune une fois (GoogleTest situe l'assertion
  écrite sur trois lignes à sa dernière ligne) ;
- les 13 cas désactivés, lancés avec `GTEST_ALSO_RUN_DISABLED_TESTS=1`, échouent tous ; les 2 nouveaux pour leur raison :
  `CACHEOTHERSECTION` vaut 400 au lieu de 600, et la ligne de `CREGUC` n'a que 2 mots au lieu de 4.

**Vu vert avant le build de Gabriel** : les 50 sources du dépôt, compilées par `cl` avec les options de `UnitCore` (son
`CL.command.1.tlog`), `/Fo` et `/Fd` dans le scratchpad, liées au `FMTlib.lib` de `build/release` (FMTlib du 2026-10-26 à 17 h 45,
sources de FMTlib inchangées depuis `025d3493`), lancées depuis le scratchpad avec `bin/Release` dans le `PATH` et
`onnxruntime.dll` copiée à côté (section 7) : 732 cas verts (591 d'avant et 141 nouveaux), 13 désactivés ; mêmes résultats dans un
ordre mélangé (`GTEST_SHUFFLE`, graines 4242 à 4244, 3 répétitions). En `/W4 /permissive-`, aucun avertissement ne vient des 16
sources nouvelles : les 609 viennent des en-têtes de FMTlib. ctest n'a pas été lancé ; rien n'a été écrit dans `build/release` ni
sous `Examples/Models` (les démonstrations lisent le dépôt et écrivent dans le scratchpad).

**Documents** : `CoreTestCoverage.csv` (statut `Lot 8b, done` des 16 classes, défauts 22 et 23, #383, remarques des observations,
tests manquants), `CoreTestCoverage.md` régénéré (50 classes avec des tests unitaires, aucune de priorité 1 sans test propre ; deux
générations identiques), deux lignes de dette d'`Architecture.md`, sections 2.7, 6.1, 6.3.5 et 7, changelogs (section non publiée,
anglais et français, issues à numéroter quand Gabriel les aura ouvertes).

**Validation après le build de Gabriel** (2026-10-27, commit `12d46d92`, arbre de travail propre ; FMTlib inchangé, `FMTlib.dll`
du 2026-10-26 à 17 h 45) :
- `UnitCore.exe` du build date de 15 h 50, après la dernière modification des sources du lot (14 h 36) ;
- installation (`INSTALL_TEST_SUITE=public`, 15 h 52) : 1 065 tests, 0 échec, 1 029 exécutés et 36 désactivés, 32 s ; unit 732
  verts et 13 désactivés, integration 35 verts et 11 désactivés, system 237 verts et 12 désactivés, performance 23, other 2 ;
  aucun test en `T:/`. `InstallTests.xml`, `InstallTests.json`, `LastTest.log` et `LastTestsDisabled.log` copiés dans le
  scratchpad de la session avant tout appel de ctest ;
- `RunTests.bat all-public` : « 0 tests failed out of 1029 », 28,3 s réelles ; unit 732 verts et 13 désactivés sur 745,
  integration 35 verts et 11 désactivés sur 46, system 237 verts et 12 désactivés sur 249, performance 23, other 2 ; Total 1 029
  verts et 36 désactivés sur 1 065. Par rapport au passage du 2026-10-27 à 11 h 03 (section 5.15), seul le niveau unit change,
  des 143 cas ajoutés : 141 verts, et les 2 cas des défauts 22 et 23 désactivés. Ce sont les comptes attendus avant le build. La
  fixture `ExamplesModelsUnchanged` et les tests Python sont verts, et l'arbre de travail est resté le même après le passage ;
- non relues : les consoles de la configuration et du build. Les avertissements ne sont couverts que par la passe `/W4` hors dépôt
  d'avant le build. Le lot n'ajoute que des tests unitaires : la suite BFEC n'est pas relancée.

## 6. Prochain lot

### 6.1 État au 2026-10-19

- **Lot 8b livré et validé le 2026-10-27** (section 5.16), commité par Gabriel (`12d46d92`) : 143 cas unitaires des rendements, des
  outputs et des classes de service (16 classes), dont 2 désactivés, les défauts 22 et 23 (section 2.7), démontrés avec `FMT.pyd`,
  issues à ouvrir ; #383 fixé à l'endroit où il agit ; deux lignes de dette d'`Architecture.md`. Plus aucune classe de priorité 1
  sans test propre dans `CoreTestCoverage.md`. Installation et `all-public` : 1 065 tests, 0 échec, 1 029 verts et 36
  désactivés. Le lot 8c suit (section 6.3.5).
- **Lot 8a livré et validé le 2026-10-27** (section 5.15), commité par Gabriel (`2ca9a023`) : 180 cas unitaires des actions, des
  transitions, des développements, des cédules et des listes (15 classes) ; #358 et #393 fixés à l'endroit où ils agissent.
  Installation et `all-public` : 920 tests, 0 échec, 887 verts et 33 désactivés. Puis, à la demande de Gabriel, l'observation de
  `FMTTransitionMask::setMask` démontrée : défaut 21 (section 2.7), aucun exécutable officiel ne passant par ce chemin ; deux cas de
  plus, dont 1 désactivé, validés par l'installation de 11 h 03 (922 tests, 0 échec, 888 verts, 34 désactivés) ; issue #402, ouverte par Gabriel. Les lots 8b et 8c
  suivent (section 6.3.5).
- **Lacunes 6 et 7 commitées (`50b67582`) et validées le 2026-10-27** (section 5.14) : `IntegrationModels` (15 cas
  de `FMTSrModel`, dont 2 désactivés, défauts 19 et 20 de la section 2.7, #399 et #400), un cas d'aller-retour des
  contraintes spatiales dans `IntegrationParser` (#397), deux scénarios nouveaux. Installation et `all-public` : 740
  tests, 0 échec, 33 désactivés ; suite privée : 94 réussis.
- **Fusion de `master` du 2026-10-23 (`eea7fea2`)**, après la v1.4.0 : #379 corrigé (`83e2309e`, défaut 6 de la
  section 2.7), `MAX_RET` des paramètres de l'ordonnanceur (`9f2c58ca`), le test de `WriteModel` (`9b9ade0b`), et le
  test `GetGCBMTransitions` (#369). Elle retirait aussi, par erreur (« Merge conflict in rebase fix », `45ac2df5`,
  confirmé par Gabriel), le correctif du journal de `FMTLpSolver` (`975320eb`, section 9.5, phase 0) et le mode
  `loggerlifetime` de `SolveWithEachSolver.cpp`, alors que sa ligne restait dans `basetests.csv`. Les deux sont
  rétablis le 2026-10-23, à l'identique. `9b9ade0b` codait aussi dans `WriteModel.cpp` des chemins de la machine
  de gcyr (`D:/FMT/...`), dont le dossier de sortie de chaque ligne : ils redeviennent relatifs le même jour. Le changelog, dont la fusion avait rangé sous la v1.4.0 des entrées que
  cette version ne contient pas, est remis en ordre le même jour. **Compilé et testé par Gabriel le 2026-10-26**, avec
  les correctifs de #394 (`44aee1fe`) et de #396 (`025d3493`) : `FMTlib.dll` de 9 h 19, `IntegrationParser.exe` de
  9 h 21. Trace copiée dans le scratchpad de la session des lacunes 6 et 7 avant tout appel de ctest
  (`InstallTests.xml`, `InstallTests.json`, `LastTestsDisabled.log`, `LastTest.log`) :
  - installation (`INSTALL_TEST_SUITE=public`, 9 h 24) : 724 tests, **0 échec**, 693 exécutés et 31 désactivés, 24 s ;
    unit 410 et 10 désactivés, integration 21 et 9 désactivés, system 237 et 12 désactivés, performance 23, les deux
    fixtures. Par rapport au lot 7c (719 tests, 686 et 33), les écarts s'expliquent tous : les quatre cas de
    `FMTAreaParser` (#394, #396), `GetGCBMTransitions` (#369, de la fusion), et deux cas de #379 réactivés (le cas
    unitaire, `31f696a2`, et `GivenAPenaltyObjectiveWithoutSign…`). Le cas unitaire de #382
    (`GivenARandomActionConstraintWhenConvertedToStringThenWritesRandom`) reste désactivé : `new_test` n'a pas
    `ff9850f1` ;
  - suite privée (`LastTest.log`, 9 h 29 à 9 h 50) : 94 tests réussis, 0 échec (92 en `T:/` et les deux fixtures),
    dont `GetOperatingAreaNeighbors`, `ScheduleOperatingAreas` et `RunOpAreaSchedulerTask`.
  
  Issues ouvertes par Gabriel : #393 (défaut 18, 2026-10-22) et #397 (2026-10-26), l'écriture des contraintes
  `_SIZE`, `_ADJACENCY` et `_GROUP` par `ff9850f1`, le correctif de #382, qui n'est que sur `master` : le cas
  d'aller-retour spatial des lacunes 6 et 7 la protège avant la prochaine fusion.
- **Lot 7c livré, commité (`5956123b`, `1754fe30`) et validé le 2026-10-22** (section 5.11) : 119 cas unitaires des
  masques, des thèmes et des développements, et 5 cas d'intégration, dont 3 désactivés (le défaut 18, nouveau, et
  #387). Suite publique : 719 tests, 686 verts et 33 désactivés. L'issue du défaut 18 est #393. **Le lot 7 est
  terminé** ; le lot 8 suit (section 6.3.5).
- **Lot 7b livré, commité (`355b2edb`, `1bd707f6`) et validé le 2026-10-22** (section 5.10) : 153 cas unitaires des
  bornes, des spécifications, des contraintes, des sorties et des yields, dont 7 désactivés (#379, #382, #383, et les
  défauts 14 à 17, #389 à #392), et un cas d'intégration pour #358. Suite publique : 595 tests, 565 verts et 30
  désactivés. Le lot 7c suit (section 6.3.4).
- **Lot 7a livré, commité (`0c35c546`, `51d03e83`) et validé le 2026-10-21** (section 5.9) : 141 cas unitaires des
  expressions et des gestionnaires de rendements, dont 4 désactivés (#380, défauts 11 et 12, #386 et #388), et le
  défaut 13 (#387) sans cas. Les lots 7b et 7c suivent (section 6.3.4).
- **La réorganisation en trois niveaux est terminée** (section 9, ce qui reste ouvert en 9.7) ;
  elle part vers `master` par une demande de tirage. Le lot 6 y est devenu la phase 8. La feuille
  de route reprend au lot 5.
- **Lot 5 livré, commité (`90ef4ad2`) et validé le 2026-10-20** (section 5.7) : premier niveau
  intégration (`IntegrationParser`, 20 cas dont 7 désactivés), vérifications de `WriteAndReadProject`,
  `SolveAndWriteSchedules`, `CreateModel`, `UpdateNonSpatialModel` et `AddYieldHandlers`, cinq défauts
  d'écriture, issues #379 à #383 (section 2.7, défauts 6 à 10). Suite publique : 300 tests, 281 verts
  et 19 désactivés.
- **Export des classes de Core (session à part, 2026-10-20, section 5.8)** : 18 des 23 classes sans
  `FMTEXPORT` de l'inventaire sont exportées (commit `f4252745`), validé après le build de Gabriel
  (sonde B, suite publique et installation inchangées ; BFEC et build R à Gabriel) ; quatre modèles
  de rendement ne peuvent pas l'être tels quels (ligne de dette d'`Architecture.md`). Le lot 7 peut
  commencer.
- **Commits de tests sur `new_test` (Gabriel, 2026-10-20)** : ajouter des commits qui ajoutent des
  tests et renforcent la protection de FMT n'est pas un problème, même avant la fusion de la demande
  de tirage de la réorganisation.
- Lots 0 à 4 et phase 8 livrés, validés et commités ; rien ne bloque. Suite publique avant le lot 5 :
  280 tests, 268 verts, dont 7 unitaires, et 12 désactivés. #357 est corrigé (`15427a6d`), avec #374 que sa
  vérification a révélé (`62f9e288`) ; #346, #347 et #358 sont des comportements connus, dont les
  12 lignes de `knownbugs.csv` attendent la revue commune des tests BFEC (section 2.7).
- Les lacunes de la chaîne sont listées ci-dessous et en section 3.1 ; elles ne sont suivies que
  dans ce fichier, pas dans des issues GitHub.
- **Décision du 2026-09-25 : ce chantier va jusqu'à la fermeture de l'issue #350** (« Add
  comprehensive unit tests for the Core namespace », jalon FMT2.0). Il absorbe donc le volet des
  tests unitaires, en gardant son ordre par criticité. Le tableau de la section 0.4 dit quel lot
  répond à quelle demande de l'issue ; la feuille de route ci-dessous va du lot 5 au lot 9, et les
  conditions de fermeture sont en section 6.3.10.

### 6.2 Décisions encore ouvertes

Prises le 2026-09-25 : GoogleTest pour la seule cible unitaire, couverture et inventaire par
classe dans la documentation, ménage du permanent remis à plus tard (section 0.4). Tout ce qui
engageait la forme du travail est tranché (section 1.2). Restent :

| Décision | Quand | Remarque |
|---|---|---|
| Intégration continue | lot 9 | sans CI, la portabilité Linux et Clang de #350 ne peut être que manuelle : soit on en ajoute une, soit on écarte la demande au moment de fermer l'issue |
| Suivi des lots dans GitHub | quand le lot 6 démarre | un commentaire par lot sous #350, qui demande des changements focalisés |

### 6.3 Feuille de route jusqu'à la fermeture de #350

**L'ordre reste celui du risque, pas celui de la couverture.** On finit d'abord la chaîne, parce
qu'une étape entière sans test de valeur est plus urgente qu'une classe déjà exercée
indirectement. On attaque ensuite les classes où des défauts ont **déjà** été trouvés, puis
celles que tout modèle traverse, puis le reste. La mesure de couverture arrive en dernier : elle
sert à trouver les trous qui restent, pas à décider par quoi commencer. C'est aussi ce que dit
#350 : « Coverage should be used to identify missing tests, not as the sole quality objective. »

Chaque lot vaut une demande de tirage, comme #350 le souhaite : des changements focalisés plutôt
qu'un gros lot unique.

**Inventaire par classe reporté (2026-10-16).** `Documentation/Testing/CoreTestCoverage.md`, prévu au
lot 6 (section 6.3.3, point 3), n'est pas fait par la phase 8, qui reprend ce lot : il passe au lot
qui suit la réorganisation, après la phase 9.

**Inventaire par classe fait (2026-10-20).** `Documentation/Testing/CoreTestCoverage.md` liste les
61 classes et structures de `Core` (templates et classes imbriquées publiques comprises), avec leur
priorité, leurs tests unitaires, d'intégration et système, leurs défauts et les colonnes de #350. Il
est en anglais, comme le reste de `Documentation/`, et se régénère sans build :

```bash
cmake -P Documentation/Testing/CoreTestCoverage.cmake
```

- Le script lit les en-têtes d'`Include/` en octets (certains sont en cp1252), compte les
  `TEST(<Classe>, …)` de `Tests/Unit` et `Tests/Integration`, et garde les colonnes écrites à la main
  dans `CoreTestCoverage.csv`, à côté du document ; le texte du document est dans
  `CoreTestCoverage.md.in`. Une classe apparue reçoit une ligne vide dans le CSV, une classe disparue
  y reste marquée `REMOVED` : le script les affiche et le document les liste.
- Priorité : celle de la section 3 (1 : la chaîne de la section 3.1 ou un défaut de la section 2.7 ;
  2 : le niveau 2 ; 3 : le niveau 3), la règle est écrite en tête du document. Résultat : 50 classes de
  priorité 1, 1 de priorité 2 (`FMTGCBMTransition`), 10 de priorité 3 (les modèles de rendement et
  `FMTSerie`).
- Tests système : seulement une ligne ou un exécutable de la suite publique qui vérifie une valeur de la
  classe (sections 2.2 et 3.1) ; sinon, la colonne dit *None with a check*.
- État au 2026-10-20 : 3 classes ont des tests unitaires (`FMTBounds`, `FMTOperator`, `FMTSpec`), aucune
  n'a de suite d'intégration à son nom, et 47 classes de priorité 1 n'ont aucun test propre. Les
  classes imbriquées publiques de `Core` : aucune aujourd'hui.
- Vérification : le script a été comparé à un prototype jetable en Python (mêmes 61 classes), à une
  dizaine d'en-têtes relus à la main (`FMTBounds.hpp`, `FMTList.hpp`, `FMTOutputNode.h`,
  `FMTYieldHandler.h`, `FMTutility.h`, `FMTMask.h`, `FMTTheme.h`, `FMTLookup.h`, `FMTObject.h`,
  `FMTDevelopment.h`, `FMTAction.h`) et à un en-tête factice (classes imbriquées publiques, privées et
  protégées, `struct` dans une fonction, spécialisation, `"/*"` dans une chaîne, macro sur plusieurs
  lignes, classe retirée). Deux générations de suite donnent le même document.
- Piège : `"+-/*"` dans `FMTGraph.hpp` ouvre un faux commentaire si l'on retire les commentaires avant
  les chaînes ; le script les retire en une passe.


#### 6.3.1 D'abord : corriger les quatre défauts de la section 2.7 — session séparée

**Clos (2026-10-19).** Précision de Gabriel du 2026-10-08 (section 2.7) : seul #357 était un vrai
défaut. Il est corrigé (`15427a6d`), avec #374 que sa vérification a révélé (`62f9e288`), et ses
lignes sont dans `basetests.csv`. #346, #347 et #358 sont des comportements connus : leurs lignes
restent désactivées jusqu'à la revue commune des tests BFEC (section 9.7). Le plan ci-dessous est
gardé pour mémoire.

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
3. revérifier les valeurs attendues des tests qui dépendaient des yields touchés (section 2.7) ;
4. reprendre la feuille de route au lot 5.

#### 6.3.2 Lot 5 : écriture et relecture (lacune 8) — effort `high`

**Fait le 2026-10-19 (section 5.7)**, avec ces écarts au plan ci-dessous : les allers-retours sont au
niveau intégration (`IntegrationParser`, décision de Gabriel), pas dans un exécutable de
`Tests/System/Cpp/` ; ils comparent aussi le modèle relu au modèle écrit, car un défaut de la première
écriture se relit et se réécrit à l'identique ; et ce qui désactive des cas est #347 (le `_SUM` de
`YBM_RTM_F`) et cinq défauts nouveaux, dont l'objectif `_PENALTY` (section 2.7, défauts 6 à 10, #379 à #383), et
non #346, que le plan nommait aussi. Les cas désactivés sont des `DISABLED_`, pas des lignes de
`knownbugs.csv`. Validé après le build de Gabriel le 2026-10-20 (section 5.7) ; issues #379 à #383.

- **Pourquoi.** C'est la dernière étape de la chaîne de la section 0.1 sans aucun test de valeur.
  FMT écrit des modèles que d'autres relisent : sauvegarde depuis l'interface, modèle présolvé écrit
  pour inspection, projet transformé par une tâche. Les deux défauts d'écriture de la section 7
  n'ont été vus qu'en écrivant un modèle à la main au lot 2, par hasard.
- **Comment.** L'aller-retour est métamorphique, donc sans valeur de référence à maintenir :
  écrire un modèle lu dans `build/release/tests/`, le relire, le réécrire, et comparer les deux
  écritures fichier par fichier. `ReadScenariosTogether` fait déjà cette comparaison ligne à
  ligne, son code est réutilisable tel quel. Même chose pour une cédule : écrire, relire,
  comparer.
- **Où.** Un exécutable de plus dans `Tests/System/Cpp/`, sur le patron des tests génériques, plus
  des vérifications ajoutées à `WriteAndReadProject`, `SolveAndWriteSchedules`, `CreateModel` et
  `UpdateNonSpatialModel`, qui tournent aujourd'hui sans rien vérifier (section 2.3), et la vérification
  morte de `AddYieldHandlers` à réparer (section 2.4).
- **Attention.** Les défauts 1 et 2 de la section 2.7 font échouer l'aller-retour dès qu'un scénario
  utilise `_SUM` ou un objectif `_PENALTY` : ce sont des lignes désactivées dans
  `knownbugs.csv`, pas un contournement à écrire dans le test.

#### 6.3.3 Lot 6 : socle unitaire de Core (jalon 1 de #350) — effort `high`

**Fait par la phase 8 de la réorganisation (2026-10-16, section 9.5)**, avec ces écarts au plan
ci-dessous : la cible s'appelle `UnitCore` et vit dans `Tests/Unit/Core` (sections 9.2 et 9.3),
l'en-tête de licence est tranché depuis la phase 7, et l'inventaire `CoreTestCoverage.md` (point 3)
est reporté au lot qui suit la réorganisation (section 6.3). Le témoin `FMTOperator::precedence` fixe
le comportement de `^` que Gabriel a classé comme connu (#346, section 2.7), à côté de la priorité de
`*` sur `+`.

- **Pourquoi.** Sans cible ni inventaire, les lots suivants n'ont nulle part où aller. C'est
  aussi ce qui débloque la fermeture de #350 : sa première demande est un socle, pas des tests.
- **Quoi.**
  1. **Ajouter GoogleTest** (décision prise, section 0.4) : dépendance de la seule cible des tests
     unitaires, déclarée dans `vcpkg.json` — `AGENTS.md` interdit d'y toucher hors d'une tâche
     de dépendances, celle-ci en est une. Un build sans GoogleTest doit continuer de compiler
     FMT et de lancer les tests de chaîne.
  2. **`Tests/Core/CMakeLists.txt` et la cible `FMTCoreTests`**, inscrite avec
     `LABELS "unit;core"` pour que `ctest -L core` fonctionne, et respectant
     `-DWITHOUT_TESTS=ON`.
  3. **`Documentation/Testing/CoreTestCoverage.md`** (fait le 2026-10-20, section 6.3) : l'inventaire par classe que demande #350,
     avec ses colonnes. La colonne « Priority » se remplit depuis la section 3 de ce fichier plutôt
     que d'être réinventée. L'inventaire se génère depuis les en-têtes, pour qu'aucune classe ne
     manque.
  4. **Trois tests témoins**, ceux qui auraient attrapé nos défauts sans enquête :
     `FMTOperator::precedence`, `FMTBounds::in` aux bornes, `FMTSpec::allowWithoutYield`.
  5. **Trancher l'en-tête de licence** de ce nouveau dossier : la bibliothèque en porte un, les
     62 exemples de `Examples/C++/` et les 15 tests du wrapper n'en portent aucun.
- **Où ça s'insère.** `Tests/` est un dossier neuf, à côté de `Examples/C++/`. Les deux suites
  cohabitent dans le même ctest ; l'étiquette `core` permet de ne lancer que l'unitaire.

#### 6.3.4 Lot 7 : unitaires des classes déjà suspectes (jalons 2 à 4 de #350) — effort `high`

- **Pourquoi cet ordre plutôt que celui de #350.** #350 ordonne par nature (types de valeur,
  puis expressions, puis rendements). On ordonne par risque constaté : les classes où la section 2.7
  a déjà trouvé un défaut passent devant.
- **Ordre proposé.**
  1. `FMTOperator`, `FMTExpression`, `FMTFunctionCall` — défaut #346, et c'est le moteur de toute
     équation de rendement ;
  2. `FMTData` et les gestionnaires de rendements (`FMTAgeYieldHandler`,
     `FMTComplexYieldHandler`, `FMTTimeYieldHandler`) — défauts #347 et #357 ;
  3. `FMTSpec` et `FMTBounds` — défaut #358, et c'est ce qui décide de toute opérabilité ; avec
     `FMTBounds`, ses classes dérivées `FMTAgeBounds`, `FMTYldBounds`, `FMTLockBounds` et
     `FMTPerBounds` ;
  4. `FMTConstraint`, `FMTOutput` et `FMTYields` — défauts #379 et #382, #381, #383 ;
  5. `FMTMask`, puis `FMTTheme`, puis `FMTDevelopment` — traversés par tout modèle.
- **Ce que l'inventaire a changé (2026-10-20).** Deux ajouts, sans toucher à l'ordre par risque :
  - l'étape 4 est nouvelle. Le critère de ce lot est la section 2.7 : une classe où un défaut a déjà
    été trouvé. Les défauts 6 à 10 (#379 à #383) y sont entrés au lot 5, après l'écriture de cette
    liste, et désignent ces trois classes ; `FMTConstraint` et `FMTOutput` étaient au lot 8, `FMTYields`
    dans aucun lot. Elles passent devant l'étape 5, comme la section 6.3 met les classes à défaut
    devant celles que tout modèle traverse ;
  - à l'étape 3, les quatre classes dérivées de `FMTBounds` sont nommées : `FMTBounds<int>` n'est
    atteignable que par elles (ligne de dette d'`Architecture.md`), et leurs constructeurs sont ce que
    lit le parseur.
  Les statuts `Lot 7, step <n>` de `CoreTestCoverage.md` suivent ces étapes.
- **Vérifié avant l'étape 1 (2026-10-20, session à part, section 5.8).** `FMTExpression` et
  `FMTFunctionCall` étaient déclarées sans `FMTEXPORT` : une sonde liée au `FMTlib.lib` du build ne se
  liait pas (LNK2019 sur leurs constructeurs, `shuntingYard`, `getVariables`, `getInFix`, `call`,
  `valid`). Elles sont exportées depuis cette session : l'étape 1 les construit directement, sans
  passer par `FMTYields::get`, et aucune ligne de dette ne s'ajoute.
- **Quoi pour chacune.** Construction, copie, comparaison, hachage quand il existe, bornes,
  entrées invalides et exceptions attendues, comme le détaille #350.
- **Découpage en trois sessions (Gabriel, 2026-10-21).**
  - **7a** : étapes 1 et 2. **Fait et validé le 2026-10-21** (section 5.9, `0c35c546`) :
    141 cas, dont 4 désactivés ; défauts 11 à 13 (section 2.7) ; non couverts : `getEndPoint`,
    `_ENDPOINT`, `getAge`, `complexYldToAgeYld` et les surcharges, qui demandent `FMTSpec` et `FMTYields`.
  - **7b** : étapes 3 et 4, les bornes (`FMTSpec`, `FMTBounds` et ses quatre classes dérivées),
    `FMTConstraint`, `FMTOutput` et `FMTYields`. **Fait et validé le 2026-10-22** (section 5.10, `355b2edb`) :
    154 cas, dont 7 désactivés, avec ce que 7a avait laissé ; défauts 14 à 17 (#389 à #392, section 2.7). Le test
    du défaut 13 (#387, lecture de `_CP`) est fait au lot 7c.
  - **7c** : étape 5, `FMTMask`, `FMTTheme` et `FMTDevelopment`. **Fait et validé le 2026-10-22** (section 5.11,
    `5956123b`) : 124 cas, dont 3 désactivés ; défaut 18 (section 2.7) et le cas de #387.
    `FMTActualDevelopment` et `FMTFuturDevelopment` restent au lot 8.
- **Le lot 7 est terminé** (2026-10-22) : ses cinq étapes ont leurs tests unitaires, et les défauts qu'elles ont révélés
  ont leur cas désactivé.

#### 6.3.5 Lot 8 : unitaires du reste de Core (jalons 5 à 7 de #350) — effort `high`

`FMTAction`, `FMTTransition`, `FMTSchedule`, `FMTEvent`, `FMTOutput` et ses nœuds,
`FMTConstraint`, `FMTGraphStats` et les types spatiaux. Ces classes sont déjà exercées par les
tests de chaîne des lots 3 et 4 : le travail consiste à descendre au niveau de l'objet construit
dans le test, là où les copies, les comparaisons et les cas limites deviennent atteignables.

**Liste revue avec l'inventaire (2026-10-20).** Le lot 8 prend les classes de `CoreTestCoverage.md`
dont le statut est `Lot 8`, dans l'ordre de leur priorité : les classes de priorité 1 que le lot 7 ne
prend pas, puis la priorité 2, puis la 3. Ce que cela change :

- `FMTConstraint` et `FMTOutput` passent au lot 7 (section 6.3.4, étape 4) ;
- s'ajoutent les classes de priorité 1 qu'aucun lot ne nommait : `FMTActualDevelopment`,
  `FMTFuturDevelopment`, `FMTDevelopmentPath`, `FMTFork`, `FMTTransitionMask`, `FMTLifespans`,
  `FMTList`, `FMTConstants`, `FMTObject`, `FMTYieldHandler`, `FMTYieldsCache`, `FMTYieldDevelopment`,
  `FMTMaskFilter`, `FMTLookup`, `FMTOutputSource`, `FMTOutputNode`, et les aides `cmpYieldString`,
  `FMTOutputComparator` et `FMTOutputNodeHasher` ;
- puis `FMTGCBMTransition` (priorité 2), puis les modèles de rendement, `FMTModelYieldHandler` et
  `FMTSerie` (priorité 3) ;
- `FMTYieldRequest` et les huit comparateurs du Core n'étaient pas exportés : un test lié à la DLL ne
  pouvait pas les appeler. Ils le sont depuis la session à part du 2026-10-20 (section 5.8), avec
  `FMTOutputNodeHasher`, `FMTYieldsCache`, `FMTYieldDevelopment`, `FMTYieldModel`,
  `FMTYieldModelRandom`, `FMTYieldModelTsla`, `FMTYieldModelUnitCoverage` et les deux classes de la
  section 6.3.4 : 18 des 23 classes sans `FMTEXPORT` de l'inventaire. Leur statut est redevenu
  `Lot 8`. Restent sans `FMTEXPORT` :
  - `cmpYieldString`, entièrement définie dans son en-tête : un test l'appelle sans export ;
  - `FMTYieldModelNn`, `FMTYieldModelNep`, `FMTYieldModelPools` et `FMTYieldModelDecisionTree`, qu'on
    ne peut pas exporter telles quelles (ligne de dette d'`Architecture.md`, section 5.8). Un test ne
    peut ni les construire ni appeler leurs membres : statut `Lot 8, after priority 2 and the debt
    row`. Leur export demande plus que `FMTEXPORT` ; Gabriel les a laissées ainsi le 2026-10-20 (section
    5.8) : une modification de FMTlib à faire dans une autre session avant leur tour ;
- `FMTEvent`, `FMTGraphStats` et les types spatiaux ne sont pas dans l'espace de noms `Core`
  (`Spatial`, `Graph`) : l'inventaire ne les liste pas. Ils restent au lot 8 parce que le jalon 7 de
  #350 les nomme.

**Découpage en trois sessions (2026-10-27).**
- **8a** : les 15 classes de priorité 1 qui construisent et font évoluer le modèle : `FMTAction`, `FMTActionComparator`,
  `FMTTransition`, `FMTTransitionComparator`, `FMTTransitionMask`, `FMTFork`, `FMTLifespans`, `FMTActualDevelopment`,
  `FMTActualDevelopmentComparator`, `FMTFuturDevelopment`, `FMTDevelopmentPath`, `FMTSchedule`, `FMTList`, `FMTMaskComparator` et
  `FMTThemeComparator`. **Fait le 2026-10-27** (section 5.15) : 182 cas, dont 1 désactivé, et le défaut 21 (section 2.7).
- **8b** : les 16 autres classes de priorité 1 : les rendements (`FMTYieldHandler`, `FMTYieldRequest`, `FMTYieldsCache`,
  `FMTYieldDevelopment`, `cmpYieldString`), les outputs (`FMTOutputSource`, `FMTOutputSourceComparator`, `FMTOutputNode`,
  `FMTOutputNodeHasher`, `FMTOutputNodeOriginComparator`, `FMTOutputNodeValueComparator`, `FMTOutputComparator`), `FMTMaskFilter`,
  `FMTLookup`, `FMTConstants` et `FMTObject`. **Fait le 2026-10-27** (section 5.16) : 143 cas, dont 2 désactivés, et les défauts
  22 et 23 (section 2.7) ; la section *Priority 1 classes without a test of their own* de `CoreTestCoverage.md` est vide.
- **8c** : le reste du lot 8 : la priorité 2 (`FMTGCBMTransition`), la priorité 3 (les modèles de rendement,
  `FMTModelYieldHandler`, `FMTSerie`), et `FMTEvent`, `FMTGraphStats` et les types spatiaux, hors de `Core`, que le jalon 7 de
  #350 nomme. À venir.

#### 6.3.6 Lot 9 : exigences transverses de #350 — effort `xhigh`

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

#### 6.3.7 Compléments des lacunes 6 et 7 — effort `medium` à `high`

**Fait le 2026-10-26 (section 5.14)**, avec ces écarts au plan ci-dessous : au niveau intégration (`IntegrationModels`,
décision de Gabriel) et non par une requête `STATS|<longueur>|<période>` de `PlanScenario` ; `setSolutionByLp` et
les cédules faussées en plus ; un aller-retour des contraintes spatiales dans `IntegrationParser` (#397). Défauts 19
et 20 révélés (section 2.7).

Les statistiques par période tiennent dans une requête de plus, d'où `medium` ; `setSolution` demande de lire l'API du rejeu, d'où `high`.

- **Statistiques période par période.** Aujourd'hui `STATS` fige la matrice entière ; les mêmes
  nombres par période diraient *où* la structure a changé plutôt que seulement *qu'elle* a
  changé. `FMTSrModel::buildPeriod` rend déjà un `FMTGraphStats` par période : il suffirait
  d'une requête `STATS|<longueur>|<période>`.
- **`setSolution` période par période.** `REPLAY` passe par `doPlanning(false, cédules)`, qui
  rejoue tout d'un coup. `setSolution(période, cédule, tolérance)` et `boundSolution` sont ce
  qu'utilisent la replanification et `SetSolution` ; ils méritent leur propre test, avec le
  même patron métamorphique.

#### 6.3.8 Lots suivants : niveaux 2 et 3 — effort `high`

Les sections 3.4 et 3.5 les détaillent. Le niveau 2 (tâches, SES, cartes, NSS, Core du wrapper)
demande surtout des vérifications dans des tests qui tournent déjà sans rien vérifier. Le niveau 3
(recuit simulé, ordonnanceur d'aires d'opération, transformations d'actions, modèles ONNX) demande
d'abord des données : la section 4 dit lesquelles sont montables à partir de la carte existante et
lesquelles exigent une carte synthétique.

#### 6.3.9 Proposition sans retour : modèles BFEC — effort `xhigh`

Effort `xhigh` : c'est une enquête, pas une tâche balisée. Relever, dans les modèles du ministère, les blocs `*YC` qui mélangent une équation numérique et
un autre opérateur, pour mesurer l'effet réel du défaut 2 de la section 2.7. Ce relevé se fait en
lecture seule, avec la sonde du lot 2 ; il dirait si des modèles de production calculent aujourd'hui
des rendements faux.

#### 6.3.10 Fermer #350

L'issue peut être fermée quand, ensemble :

- `CoreTestCoverage.md` ne montre plus de composant prioritaire sans test : sa section *Priority 1
  classes without a test of their own* est vide (47 classes le 2026-10-20) ;
- `ctest -L core` tourne, et la suite de chaîne reste verte ;
- les exigences transverses du lot 9 sont satisfaites ou explicitement écartées dans l'issue, en
  disant pourquoi ;
- #357 est corrigé (`15427a6d`) ; pour #346, #347 et #358, comportements connus, la revue commune
  des tests BFEC a décidé du sort de leurs lignes de `knownbugs.csv` et de leurs tests (section
  9.7) ;
- ce fichier et `CoreTestCoverage.md` disent chacun ce qu'ils couvrent et pointent l'un vers
  l'autre (fait le 2026-10-20 : sections 0.2 et 6.3 d'ici, introduction de `CoreTestCoverage.md`).

Le jour de la fermeture, écrire en section 0.4 ce qui l'a fermée, et ce que ce chantier reprend ou
abandonne.

## 7. Pièges connus

- **Encodage** : depuis la phase 7, toutes les sources C++ de `Tests/System/Cpp` portent
  l'en-tête de licence, donc le « é » de « Québec ». 68 sont en cp1252 : les 62 qui étaient en
  ASCII, `RunPlanningTask.cpp`, `WriteModel.cpp`, `WriteAndReadProject.cpp`,
  `ReplanConcurrently.cpp`, `SumAndAverageOutputs.cpp` et `QueryVersionFeature.cpp`. 13 sont en UTF-8 :
  `RunOpAreaSchedulerTask.cpp`, `ScheduleOperatingAreas.cpp`, `ReplanOnDemand.cpp`,
  `WrapperCoreRunSimulatedAnnealing.cpp` et `WrapperCoreRunSes.cpp`, puis 8 avec BOM
  (`GetModelFromProportions.cpp`, `WriteAndReadGCBMTransitions.cpp`, `GetYieldsFromGraph.cpp`,
  `GetVariabilities.cpp`, `ReplayScheduleOutputs.cpp`, `ResolveAfterConstraintChange.cpp`,
  `WriteAndReadOutputTypes.cpp`, `SetSolution.cpp`). `Tests/Support/TestTools.h` est en cp1252.
  Vérifier l'encodage avant d'éditer, et éditer un fichier cp1252 en Python latin-1
  (`newline=''`), jamais avec un outil qui suppose l'UTF-8. Les fichiers CMake, Python et R de
  `Tests/` et `basetests.csv` sont en UTF-8 ; le dépôt est en `core.autocrlf=true` (CRLF dans
  l'arbre de travail).
- **`TWD_land.out`** contient déjà des U+FFFD dans les descriptions d'outputs (accents perdus
  autrefois) : ne pas le « corriger » en passant.
- **Format CSV** : séparateur `;`, pas de commentaire possible, première ligne = en-tête. Une
  colonne vide disparaît de la commande : `argc` change (les lignes `SimulateNonSpatially` sans valeur
  ne vérifient rien).
- **Nom de cible exact** : casse comprise. Une ligne mal nommée n'est pas inscrite ; depuis le
  lot 1, la configuration l'annonce par `no target named` : lire les alertes après chaque
  reconfiguration. Avant, `WrapperCoreGetYield` n'a pas tourné de 2024 à 2026, et
  `teststdconstraints` jamais.
- **`BFECtests.csv` est local à chaque worktree, et ses copies divergent.** Au 2026-09-25,
  celles de `new_test` et de `FMT` dataient du 2026-07-27 et gardaient deux lignes mortes :
  `UnitTestFMTFormLogger`, devenu `UnitTestCallbackLogger`, et `testWrapperCoreGetYield.cpp`,
  au nom de cible faux. Le chantier #349 les a corrigées dans `new_test` seulement. La copie de
  `dev` (2026-09-16) a en plus la ligne `testWrapperCoreOperatingArea` de la migration. Après
  une copie, lire les alertes `no target named` de la configuration. Depuis la phase 6, il se lit
  dans `Tests/System/Cpp/` ; une copie restée dans `Examples/C++/tests/` est lue à sa place, avec
  un avertissement, jusqu'à ce qu'on la déplace. Depuis la phase 7, une copie aux anciens noms
  d'exécutables est lue par la table `Tests/Support/RenamedTests.csv`, avec un avertissement qui
  compte ses lignes, jusqu'à ce que `tools/testMigration/Update_PrivateTests.bat` la renomme : le
  script la déplace au besoin, garde une sauvegarde `BFECtests.csv.<date>-<heure>.bak` ignorée par
  git, et ne change rien à un second passage. Les dossiers de sortie passés en argument,
  `../../tests/<exécutable>`, suivent le renommage, comme dans `basetests.csv` : le dossier d'un
  ancien nom n'est plus créé par la configuration, et `MapToForest` plante sans le sien.
- **Modifier un CSV exige de reconfigurer CMake.**
- **Un jeu de données de sortie qui existe déjà est supprimé** (`FMTParser::createOGRDataset`,
  CSV, GeoTIFF...) : ne jamais placer la copie d'un projet dans le dossier de sortie d'une
  tâche (voir `SumAndAverageOutputs`).
- **Nouvel exécutable dont plusieurs lignes écrivent dans les mêmes fichiers** : l'ajouter à
  `FMTTESTSsharedoutputs` dans `Tests/System/Cpp/CMakeLists.txt`.
- **L'empreinte de `Examples/Models` compare le contenu** : un test qui réécrit un fichier à
  l'identique passe inaperçu. La règle 5 reste la vraie protection.
- **`Tests/System/Cpp/CMakeLists.txt`** (l'ancien `Examples/C++/CMakeLists.txt`) contient déjà un
  U+FFFD dans son en-tête (« Québec »), comme `FMTWrapperCore/CMakeLists.txt` et `Excel/CMakeLists.txt` :
  préexistant, à ne pas confondre avec une corruption introduite.
- **Dans l'environnement de Claude**, `cmd` ne cherche pas les exécutables dans le dossier
  courant : lancer un test par son chemin (`./test.exe`).
- **Valeurs par défaut des sources** : plusieurs pointent vers des chemins privés ou locaux
  (restes de débogage, par ex. `ReplayScheduleOutputs`, `WriteModel`, `Presolve`,
  `ReadTemplates`). Une ligne base sans argument dépend de ses défauts.
- **Solveur des tests existants (2026-09-17), laissé tel quel** :
  - 25 exécutables base optimisent en CLP ;
  - en MOSEK : `WriteModel`, `RunPlanningTask`, `UpdateNonSpatialModel`, `CreateModel`,
    `WriteAndReadOutputTypes` ;
  - `DoPlanning` prend MOSEK quand `FMTWITHMOSEK` est défini, CLP sinon : c'est le patron des
    nouveaux tests (règle 6) ;
  - `ReadTemplates` prend CLP dès qu'il reçoit des arguments, donc toujours dans ctest ;
  - `SolveWithEachSolver` essaie tous les solveurs disponibles ;
  - `LogThroughCallback` et les tests Excel sont entièrement sous `FMTWITHMOSEK`.

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
  - `WrapperCoreRunAreaVariability`, `WrapperCoreRunPlanning` et `WrapperCoreRunSimulatedAnnealing`
    l'appellent encore dans leur affichage d'erreur : à corriger quand on les touche.
- **Macros de `windows.h`** (par GDAL) : outre `min`, `max`, `ERROR`, `NEAR`, `FAR`, `IN`,
  `OUT`, `DELETE`, aussi `DIFFERENCE` (lot 2) et `small` (`#define small char`, de `rpcndr.h`, lot 7c). Écrire
  `(std::max)(...)` et `(std::numeric_limits<int>::max)()`.
- **Classes non exportées** : une classe sans `FMTEXPORT` dont les membres sont définis dans
  `Source/` ne se lie pas dans un test (LNK2019). Depuis le 2026-10-20 (section 5.8), les comparateurs
  du Core, `FMTYieldRequest`, `FMTExpression` et `FMTFunctionCall` sont exportés. Restent sans export :
  `Models::FMTModelComparator`, `Heuristics::FMTOperatingAreaSchemeComparator`, `FMTYieldModelNn` et
  ses dérivées `FMTYieldModelNep` et `FMTYieldModelPools`, et `FMTYieldModelDecisionTree` (lignes de
  dette d'`Architecture.md`). `ReadScenario` cherche encore les actions et les transitions par leur
  nom, avec un commentaire qui dit leurs comparateurs non exportés : à reprendre dans un lot de tests.
- **`FMTBounds<int>` à côté d'une classe de bornes exportée** (phase 8) : un exécutable dont une
  source utilise `Core::FMTBounds<int>` directement et une autre `FMTAgeBounds`, `FMTPerBounds` ou
  `FMTLockBounds` (par `FMTSpec`, par exemple) ne se lie pas : LNK2005 sur `~FMTBounds<int>`. MSVC
  importe `FMTBounds<int>` du DLL dans la source qui voit une classe dérivée exportée (`__imp_` dans
  son objet) et le définit dans l'autre ; même chose pour `FMTBounds<double>` et `FMTYldBounds`.
  Depuis `6bd900dc` (2026-09-15, #202), `FMTBounds.hpp` ne contient plus que le gabarit ; avant, les
  classes dérivées étaient dans le même en-tête. Les tests passent par `FMTAgeBounds` ; ligne de dette
  dans `Documentation/Architecture.md`.
- **Tests GoogleTest** (phase 8) :
  - ctest lance l'exécutable avec `--gtest_list_tests` (même `ctest -N` ou `--show-only`) quand il
    est plus récent que sa liste : il lui faut ses DLL, dans `bin/Release`. Configuré mais pas
    construit, `UnitCore` donne un seul test, `UnitCore_NOT_BUILT`, sans étiquette et en échec ;
  - les étiquettes passent à `gtest_discover_tests` sous la forme `unit\;core` : non échappée, la
    liste ne garde que `unit` (CMake 3.31, vu dans un projet jetable) ;
  - le vu rouge d'un test unitaire demande de recompiler : une copie faussée hors du dépôt, compilée
    par `cl` contre le `FMTlib.lib` du build ;
  - même chose pour un test d'intégration (lot 5) : le vu rouge passe par une copie faussée hors du
    dépôt, compilée par `cl` contre le `FMTlib.lib` du build et lancée depuis `bin/Release`, avec ce
    dossier dans le `PATH` et `onnxruntime.dll` à côté de l'exécutable ;
  - le résumé ne montre pas la raison d'un `GTEST_SKIP` (il cherche `SKIPPED: `) : les premiers
    tests d'intégration (lot 5) n'en ont pas, à reprendre avec le premier qui en aura besoin ;
  - les DLL de GoogleTest (`gtest`, `gtest_main`, `gmock`, `gmock_main`) partent dans
    `bin/Release` et dans la roue Python, comme toutes les DLL de vcpkg : laissé tel quel (décision
    de Gabriel, 2026-10-16), à reprendre avec la liste des DLL de l'installation.
  - lancé par un `.bat`, un exécutable GoogleTest perd ses options `--gtest_x=y` : cmd coupe les
    arguments sur `=`. Passer par les variables `GTEST_FILTER`, `GTEST_ALSO_RUN_DISABLED_TESTS`,
    `GTEST_BRIEF`, `GTEST_SHUFFLE` (lot 7a) ;
  - vu rouge d'une suite entière (lot 7a) : une copie faussée hors du dépôt, où un en-tête inclus après
    `<gtest/gtest.h>` redéfinit chaque assertion en son contraire ; tous les cas actifs doivent échouer
    (section 5.9) ;
  - `IntegrationParser` lu et écrit par des chemins relatifs à `bin/Release` (`../../../../Examples/Models`,
    `../../tests/IntegrationParser`) : pour le lancer sans écrire dans `build/release` (lot 7c), lui donner pour dossier
    courant `<racine>\build\release\bin\Release`, à côté d'une copie de `Examples/Models/TWD_land` dans
    `<racine>\Examples\Models`. Depuis le scratchpad, les chemins de ses cas dépassent 260 caractères
    (`create_directories` échoue) : prendre le nom court 8.3 de la racine (`for %I in (...) do @echo %~sI`).
- **Argument vide** : CMake retire une colonne vide de la commande, donc un test ne peut pas
  attendre une valeur vide. Par exemple, les thèmes de la racine n'ont pas de nom.
- **Longueur des chemins** : le dossier de sortie d'un test ne doit pas porter une liste de
  noms. `ReadScenariosTogether` le nomme par un hachage, car 9 noms de scénarios
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
  demande aussi `FORCE_PARTIAL_BUILD` et une `TOLERANCE`, comme dans `SetSolution`.
- **Outputs sans valeur finie** : `DIVIDEZERO` de la racine (`yieldzero / yieldzero`) et
  `OSUPRATIOBASE` de `division` rendent « pas un nombre », et `TEST3` de `equation` rend
  l'infini. Une comparaison numérique doit les traiter à part, sans quoi elle échoue même
  quand les deux côtés sont identiques.
- **`operate` qui ne change rien lève `FMTexc(79)`** (`FMTFork::_getPath`) : c'est le garde-fou
  contre une boucle dans le graphe. Une cible `? ? ?` sans changement d'âge ni de verrou le
  déclenche, par exemple `aCaribou` sur un développement déjà verrouillé au niveau de la cible.
- **`FMTModelParser::write` relance ses exceptions** hors du build R (sonde A/B du lot 5, section
  5.7 ; la note d'avant, « attrape ses exceptions sans les relancer », était fausse) : un dossier
  absent ou un fichier en lecture seule fait lever `FMTexc(13)` dès le `.pri`, et rien d'autre n'est
  écrit. Sortie de `main`, l'exception finit le processus en `0xC0000409` ; un test passe par
  `Testing::runTest` ou GoogleTest, qui l'attrapent.
- **Lecture de `output_type`** : seule, avec le gestionnaire par défaut, elle lève
  `FMTexc(64)`. Ce scénario n'est lu qu'avec des erreurs converties en avertissements.
- **Transition d'une action inconnue** : simple avertissement `FMTignore` (41), pas une
  erreur. Pas de test négatif possible.
- **Écriture** (relevé au lot 2, testé au lot 5) :
  - `_SUM(YBM_RTM_BOG,YBM_RTM_MEH)` est réécrit `_EQUATION(YBM_RTM_BOGYBM_RTM_MEH)` sans
    opérateur, que la relecture remplace par 0 (#347) ;
  - l'objectif `_MIN _PENALTY(...)` est réécrit `_MIN y_PENALTY(...)` (#379, section 2.7, défaut 6) ;
  - quatre autres défauts d'écriture, révélés au lot 5 : section 2.7, défauts 7 à 10 (#380 à #383) ;
  - écrire, relire et réécrire ne suffit pas : un défaut de la première écriture peut se relire et se
    réécrire à l'identique (`_DISCOUNTFACTOR`, #380). Comparer aussi le modèle relu au modèle
    écrit (`Tests/Integration/Parser/ParserRoundTrip.h`).
- **Un yield complexe ne s'évalue qu'à travers `FMTYields::get`** (lot 7a) : sa requête doit avoir été
  préparée par `FMTYields` ; faite d'un développement seul, `FMTComplexYieldHandler::get` lit un
  `FMTYields` nul (ligne de dette d'`Architecture.md`). Les gestionnaires d'âge et de temps lisent
  seulement l'âge ou la période de la requête, et se testent seuls.
- **`FMTData.h` ne se compile pas seul** (lot 7a) : son `std::map<FMTDevelopment, double>` demande le
  type complet (C2079 dans `<utility>`). Inclure `FMTDevelopment.h` avec lui.
- **`FMTData` écrit ses nombres avec 17 chiffres** (`boost::lexical_cast`) : 0,04 devient
  `0.040000000000000001`, que le parseur relit 0,04. Une chaîne attendue doit le prévoir.
- **`FMTYields` ne se compare pas** : `operator==` compare les adresses des gestionnaires (section
  2.7, défaut 10, #383), si bien que deux copies sont inégales. Comparer des valeurs de yields.
- **`FMTYieldRequest` garde le premier `FMTYields` qui l'a servie** (`_updateData`), avec des
  itérateurs vers lui. Réutilisée avec un autre objet de yields, elle lit le premier ; réutilisée
  après la destruction de la copie que rend `FMTModel::getYields()`, elle lit de la mémoire libérée
  (noms de yields illisibles et exceptions en série dans une sonde du lot 5). Construire une requête
  par appel : `yields.get(development.getYieldRequest(), nom)`, `yields` gardé en vie.
- **Une `FMTYieldRequest` garde l'adresse de son développement** (lot 7b) : construite sur un temporaire
  (`FMTYieldRequest request(getDevelopment(...))`), elle lit de la mémoire libérée dès l'instruction suivante. Nommer
  le développement et le garder en vie aussi longtemps que la requête.
- **Une action ou une transition construite dans le code passe par `update`** (lot 7c), comme dans `FMTModel` : c'est
  `update` qui réduit les masques et fixe les bornes d'âge et de période que `FMTDevelopment::operable` lit d'abord.
  Sans lui, ces bornes valent 0 à 0, et l'action n'est opérable pour aucun développement d'âge ou de période non nul.
- **Un modèle construit en code ne se compare pas section par section à sa relecture** : la lecture
  remplit des noms d'attributs que le code laisse vides, et FMT ajoute un `_DEATH` par défaut, dont la
  transition n'est pas égale à sa forme écrite et relue (`CreateModel`, `level`). Comparer l'optimum.
- **Dossier de sortie d'un test qui écrit un projet** : `writeToProject` écrit un modèle comme
  scénario quand le `.pri` existe déjà. Un test qui ne vide pas son dossier relit le projet d'un
  passage précédent : `WriteAndReadProject` le fait depuis le lot 5, `WrapperCoreWriteToProject` pas
  encore.
- **Sondes hors build** (compilation dans le scratchpad contre le FMTlib de `dev`) : un
  exécutable placé hors de `bin/Release` charge `C:\Windows\System32\onnxruntime.dll`, trop
  ancienne (« version [16] is not supported »). Copier `onnxruntime.dll` à côté de
  l'exécutable de sonde.
- **Chemin de sortie d'un cas d'intégration** : `Testing::getCaseFolder` crée `../../tests/IntegrationParser/<cas>`
  depuis le dossier de travail, et le nom d'un cas dépasse souvent 80 caractères. Lancée depuis le scratchpad, une
  sonde dépasse 260 caractères : `create_directories` échoue, ou FMT lève `FMTexc(53)` en ouvrant le fichier. La
  lancer depuis `build/release/bin/Release`, comme ctest, puis effacer les dossiers qu'elle a créés.
- **Carte de TWD_land** : ses unités ne se touchent qu'en un morceau. Un test de géométries en plusieurs morceaux
  écrit sa propre carte (section 5.12).
- **`setSolution` et sa tolérance** (section 5.14) : une superficie d'action `_LOCKEXEMPT` sous la tolérance n'est pas
  posée ; une cédule optimale rejouée ne redonne ses outputs qu'à la tolérance près. `boundSolution` résout à chaque
  appel : les périodes pas encore bornées changent de solution optimale, seul l'optimum se compare.
- **Sortie d'`IntegrationModels`** : le journal de MOSEK (ou de CLP) s'écrit dans la console de chaque cas ; lire le
  résultat dans les lignes `[  FAILED  ]` et `[       OK ]`, ou dans le résumé de `RunTests.bat`.
- **Erreurs converties en avertissements dans un test unitaire** (lot 8a) : `setErrorsToWarnings` agit sur le gestionnaire
  statique d'`FMTObject`, partagé par tous les objets du processus. Un cas qui s'en sert rend la liste vide à la fin (garde
  `ErrorsAsWarnings` de `Tests/Unit/Core/FMTFork.cpp`), sans quoi les cas suivants ne lèvent plus.
- **`FMTempty_action` est un avertissement** (`FMTEmptyAction` dérive de `FMTWarning`) : il ne lève pas, c'est l'erreur qui suit
  qui lève (lot 8a, `getGCBMActionId`). Vérifier le niveau d'une exception avant d'en attendre le code.
- **Compilation hors du dépôt depuis Git Bash** (lot 8a) : MSYS convertit `/W4` en chemin (`C:/Logiciels/Git/W4`). Passer
  `MSYS_NO_PATHCONV=1`, et alors des chemins Windows.
- **`FMTLookup.h` ne se compile pas seul** (lot 8b) : il emploie `std::string` et `std::vector` sans les inclure. Inclure
  `<string>` et `<vector>` avant lui.
- **Membres modèles non exportés** (lot 8b) : `FMTConstants::get` et `getAll`, instanciés explicitement dans
  `Source/FMTConstants.cpp`, ne sont pas dans les exports de `FMTlib.dll` (LNK2019), bien que la classe soit exportée (ligne de
  dette d'`Architecture.md`). Vérifier un export par `dumpbin /exports` avant d'écrire un cas.
- **Cache statique des yields complexes** (lot 8b) : `FMTYieldsCache` garde ses valeurs pour tout le processus. Un cas qui pose une
  valeur prend un nom de yield qu'aucun autre cas n'emploie, et aucun cas n'attend l'absence d'une clé qu'un autre a posée (défaut
  22, ligne de dette).
- **Gestionnaire d'exceptions et journal d'`FMTObject` dans un test** (lot 8b) : un cas qui les change rend un gestionnaire et un
  journal par défaut à la fin (garde `KeptLog` de `Tests/Unit/Core/FMTObject.cpp`). Les comptes d'avertissements sont dans le
  gestionnaire : un avertissement levé plus de dix fois dans le processus n'est plus journalisé, d'où un gestionnaire neuf.
- **Ordre d'une `boost::unordered_map`** (lot 8b) : `FMTConstants` écrit ses constantes dans l'ordre de sa table, qui change d'une
  copie à l'autre. Comparer des lignes triées.
- Plusieurs tests lèvent avec un nom de méthode copié d'un autre test (`"presolvetest"` dans
  `DoPlanning`) : lire le numéro de ligne plutôt que le nom.

## 8. Mesures

À refaire à la fin de chaque lot, après le build de Gabriel. Les nombres se lisent dans la
sortie console du passage de ctest : le total après « tests passed », le temps réel après
« Total Test time », et la somme des temps par test pour la durée cumulée. Ne pas les chercher
dans `LastTest.log`, que le moindre appel à ctest réécrit (section 7).


| Date | Tests base inscrits | Durée cumulée | Exécutables base | Sans vérification | Vérifications mortes | Lignes CSV ignorées | Désactivés | Exécutables BFEC seulement |
|---|---|---|---|---|---|---|---|---|
| 2026-09-16 (lot 0) | 80 (79 lignes + `UnitTestFMTexcelcache`) | 51 s cumulées (passage complet avec BFEC) | 48 | 17 | 3 | 2 | 0 | 25 |
| 2026-09-17 (lot 1) | 83 (79 lignes, 2 fixtures, `teststdconstraints` désactivé, `UnitTestFMTexcelcache`) | 9,3 s réelles ; 65,7 s cumulées | 48 | 17 | 3 | 0 | 1 | 25 |
| 2026-09-17 (lot 2) | 167 (156 lignes, 8 désactivées, 2 fixtures, `UnitTestFMTexcelcache`) | 13,8 s réelles ; 104,5 s cumulées | 51 | 17 | 3 | 0 | 8 | 25 |
| 2026-09-18 (complément lot 2) | 179 (165 lignes, 11 désactivées, 2 fixtures, `UnitTestFMTexcelcache`) | 13,7 s réelles ; 100,3 s cumulées | 51 | 17 | 3 | 0 | 11 | 25 |
| 2026-09-18 (lot 3) | 213 (195 lignes, 15 désactivées, 2 fixtures, `UnitTestFMTexcelcache`) | 12,9 s réelles ; 99,7 s cumulées | 51 | 17 | 3 | 0 | 15 | 25 |
| 2026-09-18 (lot 4) | 226 (208 lignes, 15 désactivées, 2 fixtures, `UnitTestFMTexcelcache`) | 22,0 s réelles ; 169,2 s cumulées | 52 | 17 | 3 | 0 | 15 | 25 |
| 2026-10-15 (#357, `15427a6d`) | 270 `all-public` (266 + 4 lignes `DISCOUNT*`) ; résumé : system 233 verts et 12 désactivés sur 245, performance 23, other 2 | 27,1 s réelles | — | — | — | — | 12 | — |
| 2026-10-15 (taux décimal, `62f9e288`) | 273 `all-public` (270 + 3 lignes `DISCOUNT*`) ; résumé : system 236 verts et 12 désactivés sur 248, performance 23, other 2 | 13,2 s réelles | — | — | — | — | 12 | — |
| 2026-10-20 (lot 5, `90ef4ad2`) | 300 `all-public` (280 + 20 cas `Integration.`) ; résumé : unit 7, integration 13 verts et 7 désactivés sur 20, system 236 verts et 12 désactivés sur 248, performance 23, other 2 | 13,8 s réelles | — | 13 | 2 | 0 | 19 | —
| 2026-10-21 (lot 7a, `0c35c546`) | 441 `all-public` (300 + 141 cas `Unit.`) ; résumé : unit 144 verts et 4 désactivés sur 148, integration 13 verts et 7 désactivés sur 20, system 236 verts et 12 désactivés sur 248, performance 23, other 2 | 15,5 s réelles | — | 13 | 2 | 0 | 23 | —
| 2026-10-22 (lot 7b, `355b2edb`) | 595 `all-public` (441 + 153 cas `Unit.` + 1 cas `Integration.`) ; résumé : unit 290 verts et 11 désactivés sur 301, integration 14 verts et 7 désactivés sur 21, system 236 verts et 12 désactivés sur 248, performance 23, other 2 | 16,5 s réelles | — | 13 | 2 | 0 | 30 | —
| 2026-10-22 (lot 7c, `5956123b`) | 719 `all-public` (595 + 119 cas `Unit.` + 5 cas `Integration.`) ; résumé : unit 409 verts et 11 désactivés sur 420, integration 16 verts et 10 désactivés sur 26, system 236 verts et 12 désactivés sur 248, performance 23, other 2 | 20,1 s réelles | — | 13 | 2 | 0 | 33 | —
| 2026-10-26 (fusion de `master`, #394 et #396 ; installation de Gabriel) | 724 (installation, `public`) ; unit 410 verts et 10 désactivés, integration 21 verts et 9 désactivés, system 237 verts et 12 désactivés, performance 23, fixtures 2 | 24 s (installation) | — | 13 | 2 | 0 | 31 | — |
| 2026-10-27 (lacunes 6 et 7, `50b67582`) | 740 `all-public` (724 + 15 cas `Integration.FMTSrModel` + 1 cas `Integration.FMTModelParser`) ; résumé : unit 410 verts et 10 désactivés sur 420, integration 35 verts et 11 désactivés sur 46, system 237 verts et 12 désactivés sur 249, performance 23, other 2 | 19 s réelles | — | 13 | 2 | 0 | 33 | — |
| 2026-10-27 (lot 8a, avant `2ca9a023`) | 920 `all-public` (740 + 180 cas `Unit.`) ; résumé : unit 590 verts et 10 désactivés sur 600, integration 35 verts et 11 désactivés sur 46, system 237 verts et 12 désactivés sur 249, performance 23, other 2 | 27,3 s réelles | — | 13 | 2 | 0 | 33 | — |
| 2026-10-27 (défaut 21, `3e6be3e6` ; installation de Gabriel) | 922 (installation, `public`) ; unit 591 verts et 11 désactivés, integration 35 verts et 11 désactivés, system 237 verts et 12 désactivés, performance 23, fixtures 2 | 25 s (installation) | — | 13 | 2 | 0 | 34 | — |
| 2026-10-27 (lot 8b, `12d46d92`) | 1 065 `all-public` (922 + 143 cas `Unit.`) ; résumé : unit 732 verts et 13 désactivés sur 745, integration 35 verts et 11 désactivés sur 46, system 237 verts et 12 désactivés sur 249, performance 23, other 2 | 28,3 s réelles | — | 13 | 2 | 0 | 36 | — |

- Tests base inscrits : `ctest --preset all-public -N`, autrefois `ctest ... -N -E "T:/"`
  (fixtures et tests désactivés compris).
- Durée : la référence est le temps réel de la suite base seule (`Total Test time (real)`).
  La somme des « Test time » de `LastTest.log` varie avec la charge : avec `-j 8`, les tests
  base lancés ensemble se ralentissent entre eux. Les 65,7 s du lot 1 ne se comparent donc pas
  aux 51 s du lot 0, mesurées dans le passage complet. Au lot 2, la somme vient de la sortie
  console : `LastTest.log` avait été réécrit (section 7).
- Exécutables base : premières colonnes distinctes de `basetests.csv` qui nomment une cible.
- Lignes CSV ignorées : alertes `no target named` à la configuration. Sans la sortie de la
  configuration, compter les lignes des CSV absentes de `ctest -N` (lot 2).
- Désactivés : lignes de `knownbugs.csv`.
- Depuis le lot 1 du chantier #349 (`Tests/Performance/ETAT.md`, 2026-09-25), la suite base compte
  aussi les lignes `FMTPerformanceTests.*` (étiquette `performance` ; 7 au lot 1, 13 au lot 2, 19
  au lot 3, 23 au lot 4), qui lisent TWD_land, dont le scénario `perfyields`, créé pour elles. Pour
  comparer avec les lignes ci-dessus, compter avec `ctest --preset system -N`, qui ne prend que
  l'étiquette `system` (fixtures comprises).

## 9. Réorganisation en trois niveaux (#350, depuis le 2026-10-08)

Section de travail de la réorganisation, **terminée le 2026-10-19 avec la phase 9b** (section 9.5).
Elle reste la référence de ce qui a été décidé et fait ; ce qui reste ouvert est en section 9.7.

### 9.1 Pourquoi

- **Lecture.** Tout se lance et se lit dans un seul passage : chaque test sur sa ligne, les tests
  Python, de base et longs mêlés. On voit mal ce qui a réussi ou non, et on ne peut pas lancer un
  niveau seul.
- **D'où vient le mélange.** `CMakeFMTVS2022vcpkg_GL.bat` configure dans `build/release`,
  construit, puis installe. L'installation (`cmake/BaseInstall.cmake`) enchaîne pytest en mode
  bavard, la suite R, puis un ctest sans filtre qui inclut, sur une machine qui a accès aux données
  privées, la campagne BFEC d'environ 168 minutes. Le `.bat` se termine sans `pause` : la fenêtre
  se ferme.
- **Ce que demande gcyr sur #350** (2026-09-23 et 2026-10-07) :
  - trois niveaux, dans les dossiers `Unit`, `Integration` et `System` ; nos tests actuels sont des
    tests système et doivent être renommés ;
  - Arrange / Act / Assert dans chaque test, écrits `// Arrange`, `// Act`, `// Assert` ;
  - noms Méthode + Condition + Comportement attendu, en PascalCase sans souligné, regroupés sous
    `<Classe>Test` : `TEST(FMTModelCacheTest, SetLengthWithValidPeriodUpdatesPlanningHorizon)` ;
  - les problèmes d'architecture que révèlent les tests unitaires se consignent dans
    `Documentation/Architecture.md`.

### 9.2 Décisions du 2026-10-08

1. **Tout l'existant va dans `Tests/System`** : les 62 tests de `Examples/C++`, les 15 de
   `FMTWrapperCore/tests`, les 3 d'`Excel/tests`, les suites Python et R. `Examples/C++`
   disparaît ; `Examples/` garde les modèles et les scripts d'exemple.
2. **`cmake --install` ne lance que la suite publique** (`INSTALL_TEST_SUITE=public` par défaut,
   `all` ou `none` sur demande). La campagne privée se lance à part, avant une version.
3. **Noms automatiques tout de suite, explicites ensuite** : un nom court généré pour chaque ligne
   CSV, puis un nom Méthode + Condition + Comportement donné famille par famille aux lignes
   publiques. Les lignes privées gardent un nom automatique.
4. **GoogleTest aussi pour le niveau Integration** ; une ligne CSV reste possible quand un cas se
   décrit mieux par des données.
5. **`Documentation/CodingStandards.md` passe de cinq niveaux de tests à trois**, comme le demande
   gcyr. Gabriel vérifiera s'il faut quelque chose de plus.
6. **Un résumé à la fin de chaque passage**, et un `.bat` qui reste ouvert.
7. **Une session par phase**, lancée depuis la session d'orchestration avec un prompt et un
   effort (section 9.4).
8. **#349 d'abord** : fait, clos le 2026-10-08 ; `new_test` a été fusionné dans `master`
   (PR #367).
9. **Valider la fusion de `master` avant tout** (phase 0) : on ne suppose pas que les tests
   arrivés par la fusion fonctionnent.

Maintenues : GoogleTest pour l'unitaire, un test ctest par cas, `DISABLED_` pour un défaut révélé,
en-tête de licence dans `Tests/`, aucune correction de FMTlib dans une phase de tests, et pas
d'étiquette `base`/`bfec` : le privé se reconnaît à `T:/` dans son nom et se filtre par là.

Proposés à gcyr en phase 3 : étiquettes toujours ancrées (`-L "^system$"` ; `-L` prend une
expression régulière, et `r` attraperait `core`) ; `// Act and Assert` quand un seul énoncé agit
et vérifie (`EXPECT_THROW`) ; en-tête de licence en commentaire `#` pour Python et R. Depuis la
phase 3 (2026-10-09), ces petits choix sont écrits, avec ceux que la phase a dû trancher (nouveaux
fichiers CMake, Python et R en UTF-8 ; classement des anciens niveaux service, wrapper et
compatibility), dans `Documentation/CodingStandards.md`, l'ancrage des étiquettes dans `AGENTS.md`.
Ils attendent la réponse de gcyr sur #350 : un refus se corrige dans le document, sans rouvrir le
reste.

**Décision de Gabriel (2026-10-15).** gcyr n'a pas répondu à la demande de relecture du 2026-10-09
sur #350. La table des nouveaux noms d'exécutables (`Tests/Support/RenamedTests.csv`) est donc
validée par Gabriel seul, et gcyr en est informé après coup par un commentaire sur #350.

**Décisions de Gabriel (2026-10-16), après une réponse de gcyr hors de #350.** gcyr préfère des
noms given/when/then qui disent ce que fait le test, et pas de `Test` dans les noms. La table du
2026-10-15 (`<Fonction>Test`) n'a pas été commitée ; elle est remplacée :

- noms en PascalCase sans souligné, comme le recommande GoogleTest : `Given<…>When<…>Then<…>`, par
  exemple `GivenAValidLpModelScheduleWhenSetSolutionThenObtainsAValidObjectiveValue` ; le seul
  souligné reste le préfixe `DISABLED_` ;
- un exécutable de test système porte la partie When, sans le mot `When` : l'action qu'il exerce,
  verbe en tête (`SetSolution`, `ReadScenario`). La table des 81 nouveaux noms a été validée par
  Gabriel telle quelle ;
- une suite GoogleTest porte le nom de la classe sous test, sans `Test` : `FMTModelCache` ;
- les noms given/when/then des cas arrivent avec les phases 8 (GoogleTest) et 9 (lignes CSV
  publiques), écrits à la main : un nom tiré des seuls arguments dirait les données, pas leur
  sens, et perdrait le `T:/` qui sépare les tests privés ;
- un tableau de ces noms sera revu avec gcyr plus tard.

**Décisions de Gabriel (2026-10-16), en phase 8.**

- La cible des tests unitaires de Core s'appelle `UnitCore`, niveau puis composant, sans `Test`, et
  non `FMTCoreUnitTests` ; la règle est écrite dans `CodingStandards.md` (Test names).
- Le témoin `FMTOperator::precedence` a deux cas : `*` passe avant `+`, et `^` a la priorité de `*`,
  le comportement connu de #346 fixé tel quel.
- GoogleTest a été installé par vcpkg dans le scratchpad pour compiler les témoins avant le build.
- Les DLL de GoogleTest suivent les autres DLL de vcpkg dans les paquets (section 7).

**Découpage de la phase 9 (proposé à Gabriel le 2026-10-19).**

- La phase 9 se fait en deux sessions : 9a, les colonnes `given` et `then`, le défaut de
  `getcsvtestcase` relevé en phase 7, et les noms des lignes de `knownbugs.csv` et des lignes
  `ReadScenario` ; puis 9b, les autres lignes publiques, l'avertissement des lignes publiques sans
  nom et le tableau des noms pour gcyr.
- Les vérifications qui manquent aux tests qui ne vérifient rien (section 2.3) sortent de la
  réorganisation et vont aux lots suivants (section 6.3). La règle Arrange / Act / Assert de
  `CodingStandards.md` s'applique quand un test est touché.

**Style des noms des lignes CSV (Gabriel, 2026-10-19, en phase 9a)**, validé sur douze propositions
et suivi par la phase 9b :

- le nom dit le sens de la ligne, pas ses données : ni chemin ni nom de fichier, et un nombre en
  chiffres seulement quand il est le sens (`AtPeriod2`, `AtAge0`) ; une borne se dit par son sens
  (`JustBelowItsLifespan`, `AtTheMinimumAge`) ;
- le Given dit le scénario et la situation : la racine par l'adjectif `Root` (`ARootYield`,
  `TheRootOutputs`), un autre scénario par ce qui le distingue (`WithTwoTargets`,
  `BesideANumericEquation`, `AwayFromNumericEquations`) plutôt que par son nom ; le Then dit ce
  qui est attendu (`IsOperable`, `InterpolatesLinearly`) ;
- les actions de TWD_land sont nommées par leur sens, en anglais : `COUPETOTALE` Clearcut, `AFIRE`
  Fire, `ARECUP` Salvage, `ACARIBOU` CaribouProtection, `PLANTATION` Planting, `_DEATH` Death ;
- une ligne désactivée garde le Then du comportement attendu, pas celui que FMT donne : les lignes
  de #347 et leurs témoins de `yieldoperatorsalone` ne diffèrent que par le Given.

**Compléments de la phase 9b (Gabriel, 2026-10-19)**, validés sur treize propositions :

- le Then dit ce que le test vérifie vraiment, lu dans la source de l'exécutable :
  `FindsTheKnownOptimum` pour un objectif comparé à une valeur, `FindsTheSameOptimumAsWithoutPresolve`
  pour `Presolve`, `RereadsItWithTheSameOptimum` pour `WriteModel` ;
- une ligne qui ne vérifie que l'absence d'erreur le dit, sans promettre de résultat :
  `EndsWithoutError`, ou ce que le test relit sans erreur quand il relit ce qu'il a écrit
  (`RereadsTheWrittenProjectWithoutError`) ; la liste est en section 2.3 ;
- les scénarios `_pass` du banc de l'interface (`Examples/Models/TWD_land/BANC_INTERFACE.md`) sont
  nommés par leur rôle dans le banc (`ThePlanningScenarioOfTheInterfaceBench`) : ce sont des copies
  d'autres scénarios, que leur contenu ne distinguerait pas de leur original ;
- `ReadInvalidScenario` garde le `RejectsIt` de la phase 9a ; le Given dit le défaut du scénario.

**Décision de Gabriel (2026-10-19), au lot 5 : premier composant du niveau Integration.** Les
allers-retours de `FMTModelParser` (écrire, relire, réécrire, comparer) et de `FMTScheduleParser` sont
des tests d'intégration au sens de `CodingStandards.md` (un composant avec sa vraie dépendance, les
formats de fichiers), écrits en GoogleTest dans `Tests/Integration/Parser/`, exécutable
`IntegrationParser`, étiquettes `integration;parser`, avec la fixture `ExamplesModels` pour lire
TWD_land (option `EXAMPLES_MODELS` de `registergoogletests`) et des sorties sous
`build/release/tests/IntegrationParser/`. Les exécutables qui écrivent et existaient déjà restent au
niveau système. Détail en section 5.7.

### 9.3 Cible

**Arborescence.**

```
Tests/
  CMakeLists.txt            agrégateur : fixture ExamplesModels, inclut les quatre suites
  ETAT.md                   ce fichier, déplacé en phase 6
  Support/
    TestSummary.cmake       phase 2 : résumé de fin de passage
    TestTools.h             depuis Examples/C++/tests/
    TestRegistration.cmake  phase 4 : la boucle CSV en fonctions (noms, étiquettes, contrôles)
    TestsDataSnapshot.cmake depuis cmake/
    RenamedTests.csv        phase 7 : ancien;nouveau nom d'exécutable
    RenamedTestCases.csv    phase 9b : ancien;nouveau nom ctest de chaque test public
  Unit/Core/                GoogleTest, cible UnitCore (phase 8)
  Integration/              GoogleTest, squelette vide au départ
  System/
    Cpp/                    les tests C++, basetests.csv, knownbugs.csv, BFECtests.csv (local)
      FMTWrapperCore/
      Excel/                MSVC seulement
    Python/                 depuis Examples/Python/tests, avec pytest.ini
    R/                      depuis Examples/R/tests
  Performance/              inchangé (#349)
```

**Étiquettes, le mécanisme de base.** Elles suffisent à segmenter, avec un appel direct de ctest :
`ctest --test-dir build/release -C Release -L "^unit$"`.

| Sorte | Étiquettes |
|---|---|
| Niveau, exactement une par test | `unit`, `integration`, `system`, `performance` |
| Interface, au niveau système | `cpp`, `python`, `r` |
| Composant | `core`, `wrappercore`, `excel` |
| Benchmarks, inchangées | `allocation`, `memory`, `bfec-perf` |

Les deux tests de fixture restent sans étiquette : ctest les ajoute de lui-même à tout passage qui
en dépend.

**Presets de test, un raccourci par-dessus** : `ctest --preset unit`. Gabriel garde son `.bat` pour
configurer et construire ; les presets de test pointent sur `build/release` par le preset de
configuration `release-gl`. Prérequis : que `CMakePresets.json` se lise — aujourd'hui CMake le
rejette en entier parce que le preset de build `release-mam` renvoie à un preset de configuration
`release` qui n'existe pas. La seule correction est ce nom (`release` → `release-mam`), sans
toucher aux paramètres du preset `mam` ; prévenir Marc-Alex.

| Preset | Filtre |
|---|---|
| `unit` | étiquette `^unit$` |
| `integration` | étiquette `^integration$` |
| `system` | étiquette `^system$`, sans `T:/` dans le nom |
| `system-private` | étiquette `^system$` et `T:/` dans le nom |
| `performance` | étiquette `^performance$`, une tâche à la fois |
| `all-public` | tout sauf `T:/` : la suite base d'aujourd'hui |

**Résumé de fin de passage.** À la fin de l'installation comme d'un niveau lancé seul, un tableau
par niveau (réussis, échoués, ignorés, désactivés, total), puis la liste concise des échecs et des
tests ignorés avec leur raison. Sortie de test, donc en anglais. Quand tout passe, le tableau seul.

**Noms.**

- Format `<Niveau>.<Suite>[.<Cas>]` : `Unit.FMTOperator.<Cas>`,
  `System.ReadScenario.<Cas>`, `System.Python.<Domaine>`. Un cas nommé à la main s'écrit
  `Given<…>When<…>Then<…>` (section 9.2, décisions du 2026-10-16).
- Nom automatique d'une ligne CSV : arguments 1 et 2 joints par `|`, le chemin
  `../../../../Examples/Models/<M>/<M>.pri` réduit à `<M>`, un chemin absolu réduit à
  `<lecteur>:/.../<dossier>/<fichier>` (le marqueur `T:/` reste), espaces remplacés par `_`, et
  au-delà de 60 caractères les 51 premiers suivis de `~` et de 8 caractères de l'empreinte SHA1 du
  cas réduit. Le fichier s'ajoute au dossier depuis la phase 1 (décision de Gabriel, 2026-10-08) :
  le dossier seul donnait le même nom aux trois lignes `ReadOaSchedulerParameters` de
  `BFECtests.csv`, dont les fichiers de paramètres partagent un dossier. Depuis la phase 9a, un
  chemin qui finit par `/` garde ce `/` (`<lecteur>:/.../<dossier>/`) : le fichier se lit après le
  dernier `/`, et non plus dans `CMAKE_MATCH_4`, que `if(MATCHES)` ne définit pas pour un groupe
  vide (défaut relevé en phase 7, section 9.5).
- Nom explicite (depuis la phase 9a) : deux colonnes facultatives après `doublevalue`, `given` et
  `then`, que les en-têtes de `basetests.csv` et de `knownbugs.csv` nomment. La configuration
  compose le cas `Given<given>When<exécutable>Then<then>`, de sorte que le When est toujours le
  nom de l'exécutable : `System.<exécutable>.Given<given>When<exécutable>Then<then>`. Les deux
  colonnes vont ensemble, et chacune suit `^[A-Z][A-Za-z0-9]*$` ; sinon la configuration s'arrête
  en nommant le fichier et la ligne (`checkcsvtestnameparts`). Le contrôle d'unicité de
  `registersystemtest` couvre ces noms. Une ligne sans ces colonnes, dont toute ligne à quatre
  colonnes, garde son nom automatique. Dans `BFECtests.csv`, elles sont ignorées, avec un
  avertissement qui donne leurs lignes : les lignes privées gardent leur nom automatique, et leur
  `T:/`. Le style des noms est en section 9.2 (2026-10-19). Depuis la phase 9b, les 240 lignes
  publiques ont un nom écrit à la main, et une ligne publique sans ces colonnes donne un
  avertissement qui nomme le fichier et la ligne ; une ligne privée n'en donne pas.
- Exécutables : l'action qu'ils exercent, verbe en tête, en PascalCase, sans `When` ni `Test`
  (section 9.2) — `testScenarioReading` → `ReadScenario`, `doplanning` → `DoPlanning`,
  `testWrapperCoreGetYield` → `WrapperCoreGetYield`. La table des 81 noms, colonnes `former;new`,
  est dans `Tests/Support/RenamedTests.csv` depuis la phase 7, validée par Gabriel (section 9.2). La configuration s'en sert pour lire une copie privée
  de `BFECtests.csv` restée aux anciens noms, avec un avertissement qui compte ses lignes ;
  `tools/testMigration/Update_PrivateTests.bat` renomme la copie.

### 9.4 Orchestration

- **Une session d'orchestration** garde les décisions et rédige, pour chaque phase, un prompt
  autonome : contexte, périmètre exact, fichiers, règles, vérifications attendues, ce qu'il ne faut
  pas toucher, effort recommandé.
- **Une session par phase**, ouverte à partir de ce prompt. Elle livre son diff, ses contrôles
  statiques et un message de commit Conventional Commits en anglais, puis s'arrête. Gabriel
  compile ; la session lance les tests ; Gabriel commite.
- **Retour à l'orchestration** : elle relit le résultat à partir des traces écrites — commit, ce
  fichier, sortie de ctest — et non de mémoire, puis prépare la phase suivante.
- **Une seule session de phase à la fois dans `new_test`** : deux sessions dans le même worktree
  mêlent leurs changements non commités.
- **Ordre** : phase 0, puis les phases 1 à 9. La correction de #357, seul vrai défaut (section
  2.7), se fait dans une session à part, de préférence avant la phase 6, qui déplace les CSV. La phase 3 peut se placer n'importe où avant la phase 7. Le lot 5 du chantier (section 6.3.2)
  attend la fin de la réorganisation.
- **Prévenir** la campagne `FMTWrapperCore` avant la phase 6, et Marc-Alex de la correction de son
  preset de build en phase 1. Chaque worktree qui a un `BFECtests.csv` le déplace dans
  `Tests/System/Cpp/` après la phase 6 (la configuration l'en avertit), puis lance le script de
  migration après la phase 7 : `tools/testMigration/Update_PrivateTests.bat`, ou
  `Update_PrivateTests.bat -Worktree <dossier>` pour un autre worktree, qui doit avoir la phase 7.

### 9.5 Phases et état

Une demande de tirage par phase, en français, qui renvoie à #350.

| Phase | Contenu | Effort | État |
|---|---|---|---|
| 0 | Valider la fusion de `master`, puis prendre la référence des comptes | `low` | fait (2026-10-08, `7adf1ddc`) |
| 1 | Étiquettes, presets, noms automatiques, sans rien déplacer | `medium` | fait (2026-10-08, `69ae400d`) |
| 2 | Résumé de fin de passage, `.bat` qui reste ouvert, `RunTests.bat <preset>`, installation filtrée par `INSTALL_TEST_SUITE` | `medium` | fait (2026-10-08, `ebb1a5d1`) |
| 3 | Conventions : `CodingStandards.md` (trois niveaux, AAA, noms), `Architecture.md` (dette révélée par les tests), renvois d'`AGENTS.md` | `medium` | fait (2026-10-09, `4ec09af3`) ; choix soumis à gcyr |
| 4 | Interrupteur unique `WITHOUT_TESTS`, squelette `Tests/`, boucle CSV en fonctions | `high` | fait (2026-10-09, `99dd05e4`) |
| 5 | Python et R dans ctest au niveau système : l'installation ne les lance plus qu'à travers ctest | `high` | fait (2026-10-13, `cae97f61`) ; R validé sous MSYS2 le 2026-10-14 |
| 6 | Déplacement des tests C++ dans `Tests/System` (`git mv` d'abord, chemins ensuite) | `medium` | fait (2026-10-15, `7886ca2b` et `7fdcea94`) ; 261 verts et 12 désactivés |
| 7 | Renommage des exécutables, migration des copies privées, en-têtes de licence | `high` | fait (2026-10-16, `5ae7fa75`, `31f3e288` et `ab5b27b3`) ; 261 verts et 12 désactivés |
| 8 | Socle GoogleTest dans `Tests/Unit/Core` et `Tests/Integration` (l'ancien lot 6) | `high` | fait (2026-10-19, `c1553d4b`) ; 268 verts, dont 7 unitaires, et 12 désactivés |
| 9a | Colonnes `given` et `then`, défaut de `getcsvtestcase` (phase 7), noms écrits à la main des 12 lignes de `knownbugs.csv` et des 112 lignes `ReadScenario` de `basetests.csv` | `high` | fait (2026-10-19, `791a09ae`) ; 268 verts, dont 7 unitaires, et 12 désactivés |
| 9b | Noms des autres lignes publiques, avertissement des lignes publiques sans nom, tableau des noms pour gcyr (section 9.2, découpage du 2026-10-19) | `high` | fait (2026-10-19, `fea23253`, table des anciens noms `f8676a84`) ; 268 verts, dont 7 unitaires, et 12 désactivés ; fin de la réorganisation (section 9.7) |

**Phase 0 en détail.** La fusion de `master` (`c2760ff4`, 2026-10-08) apporte, par rapport au
dernier état validé (`720010bb`, PR #367) :

- `975320eb` (gcyr) : un mode `loggerlifetime` dans `Examples/C++/testSolvers.cpp` et sa ligne
  `testSolvers;loggerlifetime;;`. Le test résout `LP3` en CLP puis copie 2000 fois le modèle : il
  n'échoue que si le processus plante, et rend 1, non 77, quand CLP manque ;
- dans le même commit, `FMTLpSolver` gagne un membre `m_solverLogger` (classe exportée, donc tout
  se recompile) et clone son journal à chaque construction ; gcyr note que ça peut casser des
  tests système ;
- `ded79a1a` résout un conflit dans `basetests.csv`.

Étapes : Gabriel reconfigure et reconstruit tout ; suite publique (attendu : l'état précédent plus
une ligne, 15 désactivés, aucun nouvel échec, durée de la nouvelle ligne notée) ; suite BFEC par
Gabriel ; le nouveau test est consigné tel quel, sans être modifié ; un échec devient une issue
documentée (règle 3) ; puis la référence hors du dépôt : `--show-only=json-v1`, `-N -E "T:/"`,
`--print-labels`, après avoir copié `LastTest.log`.

**Résultat (2026-10-08, sur `89f500d4`)** :

- suite publique : 259 tests inscrits, 244 verts, 15 désactivés, 16,3 s réelles avec `-j 8` —
  l'état validé précédent (243 verts) plus `testSolversloggerlifetime`, vert en 0,62 s ;
- référence des comptes : 357 tests inscrits, 259 publics, 98 privés ; seules existent les
  étiquettes des benchmarks (`allocation`, `bfec-perf`, `memory`, `performance`) ;
- `testSolversloggerlifetime` est consigné tel quel : il ne vérifie qu'une absence de plantage, rend
  1 et non 77 sans CLP, et ne peut être vu rouge qu'en retirant la correction de `FMTLpSolver` ;
- suite BFEC : à lancer par Gabriel, pour le changement du journal de `FMTLpSolver` sur les modèles
  réels.

**Phase 1 (2026-10-08, `69ae400d`)** :

- noms `System.<cible>[.<cas>]` et étiquettes `system;cpp` sur les 325 lignes CSV et
  `System.UnitTestFMTexcelcache`, qui reçoit aussi la fixture et le code 77 ; un chemin absolu
  garde son fichier (section 9.3) ; aucun doublon, aucune ligne CSV perdue (326 tests `system`) ;
- `--show-only=json-v1` comparé à la référence : mêmes commandes, dossiers de travail et
  propriétés pour les 357 tests ; seuls changent les noms, les étiquettes et, voulu, le test Excel
  (fixture, code 77, et donc sa place dans le `DEPENDS` de `ExamplesModelsUnchanged`) ;
- une étiquette de niveau par test, hormis les 2 fixtures et les 6 `bfec-perf` ; aucun nom public
  avec `T:/`, les 92 noms privés l'ont tous ;
- `ctest --preset <p> -N` : `unit` 0, `integration` 0, `system` 236, `system-private` 94,
  `performance` 25, `all-public` 259, 357 sans filtre. Les filtres par étiquette ajoutent les
  2 fixtures à leur compte : sans elles, 234, 92 et 23 ;
- `ctest --preset all-public` : 244 verts (fixtures comprises), 15 désactivés, 20,4 s réelles ;
  le plus long nom public fait 97 caractères (199 pour un nom privé de benchmark, inchangé), d'où
  une colonne de noms d'environ 120 caractères ; sortie redirigée, `shortProgress` n'abrège rien ;
- les presets se lisent (`cmake --list-presets=all`) ; le preset de build `release-mam` renvoie
  enfin à `release-mam` : prévenir Marc-Alex ;
- relecture : 85 des 259 noms publics sont tronqués par `~`, dont 79 lignes de
  `testScenarioReading`, dont la requête en deuxième argument dépasse à elle seule 60 caractères ;
  4 autres gardent un chemin relatif que la règle ne réduit pas (`../../tests/<cible>/...`). La
  phase 9, qui nomme `testScenarioReading` juste après `knownbugs.csv`, en règle l'essentiel ; pas
  de raison de compliquer la règle automatique. Les noms des benchmarks
  (`FMTPerformanceTests.*`) ne suivent pas encore le format `<Niveau>.<Suite>` :
  `Tests/Performance` reste hors du chantier.

**Phase 2 (2026-10-08, `ebb1a5d1`)** :

- `Tests/Support/TestSummary.cmake` imprime le tableau par niveau, puis les échecs et les ignorés
  avec leur raison ; `cmake/BaseInstallTests.cmake` l'appelle après le passage, `RunTests.bat
  <preset>` après le sien ; `CMakeFMTVS2022vcpkg_GL.bat` finit par `pause` ;
- dans le JUnit de ctest 3.31, un test qui n'a pas pu tourner s'écrit comme un test ignoré
  (`notrun`, élément `skipped`) : seul le message `SKIP_...` distingue un vrai test ignoré ;
- installation de Gabriel (`INSTALL_TEST_SUITE=public`) : 259 tests, aucun `T:/`, environ 20 s ;
  tableau system 219 verts et 15 désactivés, performance 23, other 2 (les fixtures), Total 244 verts
  et 15 désactivés ;
- `RunTests.bat system` : 221 verts et 15 désactivés sur 236, ctest « 0 tests failed out of 221 »,
  21,6 s ; `unit` : « no test matches this selection », code 0 ; `performance` : 25 verts (23 et
  les 2 fixtures), 26,5 s, une tâche à la fois ;
- tests inscrits identiques à la référence de la phase 1 : mêmes 357 noms, commandes et propriétés
  (`--show-only=json-v1`), et sortie de `ctest --preset <p> -N` identique pour les six presets ;
- défaut préexistant relevé, non corrigé : l'expression des badges de `BaseInstallTests.cmake`
  (`[^&gt;]*`) exclut le « t » de « https », si bien que les badges des README ne changent jamais.
  Démontré à la relecture : sur la ligne `MSVC ![](https://img.shields.io/badge/build-pass-...)`,
  `MSVC[^&gt;]*build[^&lt;]*` ne trouve rien et `MSVC[^>]*build[^<]*` trouve la ligne. Issue à
  ouvrir (règle 3). La corriger telle quelle ferait réécrire `README.md` et `README.fr.md` à chaque
  installation, `build-broken` compris : l'issue doit poser le choix entre corriger l'expression et
  retirer la mise à jour des badges de l'installation.

**Phase 3 (2026-10-09, `4ec09af3`)** :

- `CodingStandards.md` : trois niveaux et classement des cinq anciens, sous-sections
  « Arrange, Act, Assert », « Test names » et « Tests that reveal architectural debt », deux points
  de la liste de relecture, en-tête `#` de Python et R, encodage des nouveaux fichiers CMake, Python
  et R ; `AGENTS.md` n'y renvoie plus que ; `Architecture.md` : la dette révélée par les tests et
  trois lignes (comparateurs sans `FMTEXPORT`, `FMTYieldRequest` non exportée, chaîne d'exceptions
  lancée hors de tout `catch`), vérifiées dans le code ;
- les noms de gcyr sont cités d'après sa seconde image. La première nommait le même exemple
  `SetLengthUpdatesPlanningHorizon`, sans condition ;
- **`FMTModelParser::write` n'a pas sa ligne** : la lecture du code contredit la note de la
  section 7. Son `catch (...)` appelle `printExceptions`, dont `throwNested` (paramètre `rethrow`
  vrai par défaut) relance l'exception courante hors du build R, et `printExceptions` la relance à
  son tour ; un chemin invalide lève `FMTexc(FMTinvalid_path)`, de niveau `FMT_logic`, donc fatal.
  Non démontré par une exécution (pas de build dans cette phase) : une sonde A/B (écrire dans un
  dossier inexistant) tranchera avant de corriger la section 7 ou d'ajouter la ligne ;
- aucun build, aucun test : documentation seulement.

**Phase 4 (2026-10-09, `99dd05e4`)** :

- **un seul interrupteur** : `option(WITHOUT_TESTS ... OFF)` à la racine, avant `Excel/`, testé
  partout par sa valeur. `WITHOUT_TESTING` devient un alias déprécié : s'il est vrai,
  `message(DEPRECATION)`, et `WITHOUT_TESTS` vaut `ON` en variable normale, que
  `ExportAllVariablesToInstall` transmet aux scripts d'installation. Le cache de `WITHOUT_TESTS`
  n'est pas touché : `WITHOUT_TESTING` remis à `OFF` rend les tests ;
- **changement de comportement** : `-DWITHOUT_TESTS=false` (ou `OFF`) donne maintenant les tests.
  La racine testait `NOT DEFINED WITHOUT_TESTS` : toute valeur, fausse comprise, y retirait les
  tests, alors qu'`Excel/`, `FMTWrapperCore/`, `UI/` et les scripts d'installation, qui testaient la
  valeur, gardaient leurs exécutables de test et lançaient ctest à l'installation. Les scripts de
  version passent `-DWITHOUT_TESTS=true` : pour eux, rien ne change ;
- `Tests/CMakeLists.txt`, l'agrégateur : la fixture `ExamplesModels` (mêmes noms, mêmes commandes),
  les squelettes `Unit/`, `Integration/` et `System/`, puis `Performance/`, qui quitte la racine.
  `Tests/Performance/CMakeLists.txt` perd son garde `WITHOUT_TESTING` et son `enable_testing()` ;
  son corps est désindenté (`git diff -w` ne montre que le garde et le commentaire d'en-tête, devenu
  faux) ;
- `Tests/Support/TestRegistration.cmake` : `getcsvtestcase`, recopiée à l'identique ;
  `registersystemtest`, qui inscrit un test système (`add_test`, `system;cpp`, 77, fixture) sous un
  nom unique, contrôlé par une propriété GLOBAL par nom, `FMTTESTSorigin.<nom>`, avec un message qui
  nomme les deux origines ; `registercsvtests`, la boucle sur les CSV, aux règles inchangées.
  `Examples/C++/CMakeLists.txt` garde la création des exécutables et `FMTTESTSsharedoutputs`,
  `Excel/CMakeLists.txt` inscrit son test par `registersystemtest`, et la boucle de `UI/tests`, un
  dossier qui n'existe pas, disparaît ;
- **écart voulu au prompt de la phase** : `TestRegistration.cmake` est inclus par la racine, avant
  `Excel/`, et non par l'agrégateur. Placé avant `Excel/`, l'agrégateur faisait passer
  `Tests/Performance` avant `FMTWrapperCore/`, dont `include_directories` n'atteint que les cibles
  créées après lui : `FMTPerformanceTests` et `FMTBenchmarkHarness` perdaient
  `FMTWrapperCore/Include`, vu dans les `.vcxproj` générés. L'agrégateur reste donc à la place de
  l'ancien bloc, après `FMTWrapperCore/`. **Phase 6** : quand le test Excel aura quitté
  `Excel/CMakeLists.txt`, l'inclusion de `TestRegistration.cmake` pourra entrer dans l'agrégateur ;
- `enable_testing()` est appelé une fois, avant le premier `add_test`, celui d'Excel. Le bloc de
  couverture GNU, inchangé, garde son `include(CTest)`, qui l'appelle aussi ;
- **contrôles avant le build**, sur la configuration du `.bat` GL rejouée dans le scratchpad :
  - `--show-only=json-v1`, comparé par nom à celui pris avant toute modification : mêmes 357 noms ;
    commandes, dossiers de travail et propriétés identiques pour 356 tests ; pour
    `ExamplesModelsUnchanged`, le `DEPENDS` garde ses 335 éléments dans un autre ordre, celui des
    inscriptions. Aucun avertissement `no target named` ;
  - les 187 `.vcxproj`, `.filters` et `.sln` générés sont identiques, dossier de build et GUID
    normalisés ; seuls les 5 nouveaux fichiers CMake s'ajoutent aux entrées de la reconfiguration ;
  - variables exportées vers l'installation : les 14 variables que la boucle laissait fuir à la
    racine disparaissent, `WITHOUT_TESTS` (`OFF`) apparaît, `executablename` et `FMTexecutables` ont
    une autre dernière valeur ; aucun script d'installation ne lit ces variables ;
  - `-DWITHOUT_TESTS=ON`, `-DWITHOUT_TESTS=true` et `-DWITHOUT_TESTING=ON` : aucun test, aucun
    exécutable de test, mêmes 17 projets (bibliothèques et cibles utilitaires) ; l'alias affiche
    l'avertissement de dépréciation ; `-DWITHOUT_TESTS=false` : les 357 tests ;
  - doublon provoqué dans des copies des CSV, par un projet jetable qui inclut
    `TestRegistration.cmake` : « basetests.csv, line 220: the test name
    "System.testScenarioReading.TWD_land|ROOT|THEMES|3" is already given to basetests.csv, line
    91 », et, contre le test Excel, « extra.csv, line 1: the test name
    "System.UnitTestFMTexcelcache" is already given to Excel/CMakeLists.txt » ;
- **reste à corriger plus tard**, hors du périmètre de cette phase : les commentaires de
  `cmake/TestsDataSnapshot.cmake` (lignes 8 et 9) et de `TestTools.h` (ligne 10) nomment encore
  `Examples/C++/CMakeLists.txt` comme le lieu de la fixture et du code 77. À reprendre quand ces
  fichiers déménagent dans `Tests/Support` (section 9.3) ;
- **validation après le build de Gabriel** (2026-10-09) :
  - `--show-only=json-v1` de `build/release`, comparé par nom à la référence de la phase 1
    (`test-reorg-baseline\P1\tests.json`) : mêmes 357 noms ; commandes, dossiers de travail et
    propriétés identiques pour 356 tests, et le `DEPENDS` de `ExamplesModelsUnchanged` garde ses
    335 éléments dans un autre ordre ;
  - `ctest --preset <p> -N` : `unit` 0, `integration` 0, `system` 236, `system-private` 94,
    `performance` 25, `all-public` 259, avec les mêmes noms que les fichiers `preset_<p>.txt` de la
    référence ;
  - `RunTests.bat all-public` : « 0 tests failed out of 244 », 17,6 s réelles, et le tableau de la
    phase 2 (system 219 verts et 15 désactivés, performance 23, other 2 ; Total 244 verts et
    15 désactivés) ; fixture verte, `Examples/Models` inchangé. Le passage de l'installation
    (`INSTALL_TEST_SUITE=public`) donne le même tableau.

**Phase 5 (2026-10-13)** :

- **déplacements** par `git mv`, contenu inchangé : `Examples/Python/tests` et
  `Examples/Python/pytest.ini` dans `Tests/System/Python`, `Examples/R/tests` dans
  `Tests/System/R` ; 18 renommages à 100 % ;
- **inscription** : `registersystemtest` prend `INTERFACE <interface>` avant la commande, `cpp` par
  défaut. Un test par fichier `test<Domaine>` :
  - `System.Python.<Domaine>`, étiqueté `system;python`, là où l'installation construit la roue :
    cible `FMT`, `Python_EXECUTABLE` existant, sans `R_DIR` existant (la condition de
    `BaseInstallPython.cmake`) ni `WITHOUT_PYTHONLIB` ;
  - `System.R.<Domaine>`, étiqueté `system;r`, avec un `R_DIR` existant et son Rscript, cherché dans
    `R_DIR` seulement, sans le repli sur le `PATH` de l'installation : le paquet est installé dans ce
    R ;
  - `FMT_SOURCE_DIR`, et pour Python `PYTHONPATH` et `PYTHONDONTWRITEBYTECODE`, passent par
    `ENVIRONMENT`. Le corps des deux `CMakeLists.txt` est dans une fonction : aucune variable ne fuit
    vers l'installation ;
- **lanceurs** `runTestFile.py` et `runTestFile.R`, un fichier par appel :
  - Python : `FMT.pyd` de `bin/<type>` par `PYTHONPATH`, `FMT.__file__` affiché, pytest avec
    `-p no:cacheprovider` ; 77 sans pytest, ou si l'import échoue dans un arbre jamais installé
    (reconnu à l'absence de roue `FMT*.whl` dans `bin/<type>`) ; échec si aucun test n'est
    collecté, si l'import échoue dans un arbre installé, si `FMT.pyd` n'est pas construit ou s'il
    vient d'ailleurs : sans `FMT.pyd`, Python prend en silence la roue installée (vu dans la
    configuration jetable). La ligne `SKIPPED:` reste en ASCII et l'erreur d'import suit sur sa
    propre ligne : Windows la rédige en cp1252, que le résumé affichait mal ;
  - R : 77 si testthat ou FMT n'est pas installé, ou si la version installée diffère de
    `PROJECT_VERSION` ; 1 sur un échec, une erreur, ou si aucun test ne tourne. `runTests.R` reste
    pour lancer le dossier à la main ;
- **installation** : `BaseInstallPython.cmake` et `BaseInstallR.cmake` gardent pytest et testthat
  disponibles, construisent et installent les paquets, mais ne lancent plus aucune suite ; pip passe
  en `-q`. `BaseInstallTests.cmake` ne change pas ;
- **contrôles avant le build** :
  - configuration MSVC rejouée dans le scratchpad : 364 tests ; les 357 sont identiques à
    `P1\tests.json` par nom, commande, dossier et propriétés (hors indices `backtrace`), et seul le
    `DEPENDS` de `ExamplesModelsUnchanged` gagne les 7 tests Python ; aucun test R ;
  - lanceur Python à la main contre `build/release` : les 7 fichiers passent (15 fonctions) ; un
    fichier jetable qui échoue rend 1, un fichier sans test 1 ; 77 sans pytest (venv sans pip) et
    avec un `FMT.pyd` sans ses DLL ; 1 pour le même arbre avec une roue factice, et sans `FMT.pyd` ;
    rien d'écrit dans `Tests/System/Python` ;
  - passage ctest et résumé dans la configuration jetable : les 7 tests ignorés, avec leur raison ;
  - R, dans un projet jetable avec un faux `Rscript.exe` compilé par `cl` qui écrit ses arguments :
    7 tests `System.R.*`, leurs propriétés et la commande
    `Rscript --vanilla runTestFile.R <fichier> 1.3.0` ; aucun test Python avec `R_DIR` ; un `R_DIR`
    sans Rscript donne un avertissement et aucun test. Le lanceur R n'a jamais été exécuté
    (contrôle statique seulement) ;
- **validation après le build de Gabriel** (2026-10-13) :
  - passage de l'installation : system 226 verts et 15 désactivés, performance 23, other 2 ; Total
    251 verts et 15 désactivés ;
  - les 7 tests Python passent, 15 fonctions, avec `FMT.__file__` dans `bin\Release` ;
  - `ctest --preset <p> -N` : `all-public` 266, `system` 243, `system-private` 94, `performance`
    25, `unit` 0, `integration` 0 ; `-L "^python$"` : 7 tests et les 2 fixtures ; `-L "^r$"` : 0 ;
  - la console de l'installation n'a pas été relue par Claude : l'absence du bloc de pytest se
    déduit du script et du résumé ;
- **restes ignorés par git**, à supprimer à la main : `Examples/Python/tests/__pycache__` et
  `Examples/Python/.pytest_cache` ; `Examples/R/tests` est vide.

**À vérifier par Gabriel côté R**, au prochain build MSYS2 :

- `ctest --test-dir build/release -N -L "^r$"` : les 7 tests `System.R.*`, plus les 2 fixtures,
  et aucun `System.Python.*` ; pas d'avertissement « No Rscript was found » à la configuration ;
- l'installation ne lance plus testthat (plus d'étape « Run FMT R tests »), mais s'assure toujours
  qu'il est installé ;
- le résumé compte les 7 tests R dans `system` ; un test R ignoré y affiche sa raison (testthat ou
  FMT non installé, ou version installée différente de celle du build) ;
- jamais exécuté jusqu'ici : `test_file` qui charge `helperFMT.R`, et les colonnes `failed` et
  `error` de ses résultats. Vu rouge : une attente faussée dans une copie d'un fichier de test doit
  rendre 1.

**Validation côté R** (2026-10-14, build MSYS2 de Gabriel sur `9111b584`) :

- avant d'y arriver, quatre défauts du build MinGW, sans lien avec la phase, corrigés à part :
  `af301748` (git pour la pile COIN-OR privée de `FindOSI`), `009649ec` (cache d'un `build/release`
  configuré avant `e33ee6d0`, qui mêlait le COIN-OR de Rtools et la pile privée), `f224a2b2` et
  `9111b584` (fonctions de Windows 10 du harnais des benchmarks) ;
- configuration : `ctest -N -L "^r$"` donne les 7 tests `System.R.*` et les 2 fixtures ; seul
  avertissement `no target named` : `UnitTestFMToutputCache`, test Excel créé avec MSVC seulement,
  une fois le `BFECtests.csv` local de `/home/FMT` remplacé par la copie de `new_test` ;
- installation (`INSTALL_TEST_SUITE=public`, aucun `T:/`) : system 223 verts, 2 échecs et
  15 désactivés sur 240, performance 12 verts et 11 échecs sur 23, other 2 ; Total 237 verts,
  13 échecs, 15 désactivés sur 265 ;
- les 7 tests R passent contre le paquet de `R-4.5.0/library/FMT` : `helperFMT.R` est chargé
  (`testParser` et `testPlanning` lisent TWD_land par `fmtModelPath`), et les résultats de
  `test_file` sont lus. R avertit que testthat a été compilé avec R 4.5.3 ;
- non vérifié : la console de l'installation (absence de « Run FMT R tests »), un vu rouge d'un test
  R et le code 77 du lanceur R ;
- échecs hors de la phase :
  - `System.doplanning.TWD_land|fullcarbon|5` et `System.testJointScenarioReading.TWD_land|fullcarbon|…` :
    le scénario `fullcarbon` lit des yields `_PRED` par ONNX Runtime, absent du build R (MSYS2) ;
    échec attendu dans ce build seulement, selon Gabriel, le scénario passant sous MSVC. Ils
    échouent au lieu de rendre 77 (règle 8), `doplanning` par `std::terminate` (`0xc0000409`) :
    à reprendre quand on touche ces tests ;
  - 10 benchmarks `Flow.*` : mémoire privée de pointe d'environ 2,18 Go pour des bornes de 26 à
    491 Mo, déjà là dans les benchmarks les plus petits, alors que `Flow.Optimize` garde 1,9 Mo de
    tas au plus. OpenBLAS et libgomp, le runtime OpenMP de GCC, sont liés en statique dans
    `libFMTlib.dll` et dans `FMT.dll` (le `-lblas` de `build-coin-stack.sh`) ; les en-têtes PE ne
    demandent ni pile ni tas inhabituels. A/B sur `Flow.Optimize` (2026-10-15) :
    `OPENBLAS_NUM_THREADS=1` ne change rien (2 190,9 Mo), `OMP_NUM_THREADS=1` donne 173,6 Mo. La
    mémoire suit donc le nombre de fils OpenMP, environ 65 Mo par fil, 32 fils par défaut sur ce
    poste ; mécanisme probable, non lu dans le code : les tampons qu'OpenBLAS, compilé avec OpenMP,
    prépare pour chaque fil au chargement. Chaque session R qui charge FMT est concernée : issue
    #373 (2026-10-15, règle 3). Même à 173,6 Mo, `Flow.Optimize` dépasse sa borne de 32 Mo, calibrée sous
    MSVC : les bornes de mémoire sont propres à MSVC. Décision de Gabriel (2026-10-15) : des bornes
    propres à MinGW, dans `Tests/Performance/performance-mingw.csv`, avec `OMP_NUM_THREADS=32`
    (section 7.8 de `Tests/Performance/ETAT.md`) ;
  - `Mask.FromString` : 11 allocations par appel pour une borne de 7, mesurée avec la bibliothèque
    standard de MSVC ; libstdc++ alloue autrement. Borne de 11 dans `performance-mingw.csv`.

**À retenir pour la phase 7 (relevé le 2026-10-09).** Les sources de test à déplacer ne partagent pas
un encodage. Sur les 82 fichiers suivis (sources C++ des trois dossiers et `TestTools.h`), 63 sont
en ASCII, 6 en cp1252 (`planningtest`, `replanningtest`, `sumandavgtest`, `testVersionFeature`,
`testmodelwriter`, `testreadwriteproject`) et 13 en UTF-8, dont 8 avec BOM. Seul
`testVersionFeature.cpp` a déjà l'en-tête de licence. Les 42 sources C++ de `Tests/Performance` sont
en cp1252 avec l'en-tête. L'en-tête s'écrit dans l'encodage de chaque fichier, après le BOM s'il y en
a un.

**Le filtre de l'installation passe de la phase 5 à la phase 2** (2026-10-08). La phase 2 réécrit
déjà l'appel de ctest dans `cmake/BaseInstallTests.cmake` ; le filtre n'y ajoute que quelques
lignes, et c'est lui qui retire de l'installation de Gabriel la campagne privée d'environ
168 minutes. Rien ne le lie aux suites Python et R, qui restent en phase 5.

**La session des défauts s'est réduite à #357** (précision de la section 2.7), le seul vrai
défaut, corrigé le 2026-10-15, avant la phase 6 (section 2.7, item 3) :

- `_DISCOUNTFACTOR` lit enfin son taux, et `HALF` prend l'exposant `période - 0,5`, convention
  retenue par Gabriel ;
- les trois lignes `DISCOUNT*` sont passées de `knownbugs.csv` à `basetests.csv`, valeurs
  inchangées, et quatre lignes s'y ajoutent pour couvrir `FULL`, `NONE` et `HALF` sur au moins
  deux périodes ;
- commit `15427a6d` ; rouge consigné en section 2.7 (build du 2026-10-14) ;
- validation après le build de Gabriel (2026-10-15) : les 7 lignes `DISCOUNT*` passent ;
  `tools/tests/RunTests.bat all-public` : « 0 tests failed out of 258 », 27,1 s réelles ;
  tableau system 233 verts et 12 désactivés sur 245, performance 23, other 2 ; Total 258 verts et
  12 désactivés sur 270 ; fixture `ExamplesModelsUnchanged` verte ;
- la vérification a révélé un autre défaut, le taux décimal tronqué (section 2.7, item 5),
  corrigé dans la même session à la demande de Gabriel : trois lignes de plus dans
  `basetests.csv` et le scénario `discountconstant` (commit `62f9e288`). Après le build de
  Gabriel (2026-10-15) : `RunTests.bat all-public`, « 0 tests failed out of 261 », 13,2 s
  réelles ; tableau system 236 verts et 12 désactivés sur 248, performance 23, other 2 ; Total
  261 verts et 12 désactivés sur 273 ; fixture `ExamplesModelsUnchanged` verte.

**Phase 6 (2026-10-15, `7886ca2b` puis `7fdcea94`)** :

- **déplacements** par `git mv`, contenu inchangé, seuls dans un premier commit : les 63 sources
  d'`Examples/C++`, son `CMakeLists.txt`, `basetests.csv` et `knownbugs.csv` dans
  `Tests/System/Cpp/` ; les 15 sources de `FMTWrapperCore/tests` dans
  `Tests/System/Cpp/FMTWrapperCore/` ; les 3 d'`Excel/tests` dans `Tests/System/Cpp/Excel/` ;
  `TestTools.h` et `cmake/TestsDataSnapshot.cmake` dans `Tests/Support/` ; ce fichier dans
  `Tests/`. 87 renommages à 100 % ;
- **CMake**, dans le second commit :
  - `Tests/System/Cpp/CMakeLists.txt` crée, dans une fonction (aucune variable ne fuit vers
    l'installation), les exécutables des trois dossiers : ceux de `FMTWrapperCore/` si la cible
    `FMTWrapperCore` existe, ceux d'`Excel/` si `FMTExcelWrapper` existe, donc avec MSVC et ses
    sources, avec les mêmes liens et dossiers d'en-têtes qu'avant ; il inscrit le test Excel, puis
    les lignes des CSV. `Tests/System/CMakeLists.txt` l'inclut avant Python et R ;
  - `Tests/CMakeLists.txt` inclut `TestRegistration.cmake` (fin de l'écart de la phase 4) ; la
    racine appelle `enable_testing()` juste avant l'agrégateur, toujours après `FMTWrapperCore/`,
    et n'inclut plus `Examples/C++/CMakeLists.txt` ; les blocs de test de
    `FMTWrapperCore/CMakeLists.txt` et d'`Excel/CMakeLists.txt` disparaissent ;
  - étiquettes de composant : propriété de cible `FMTTESTScomponent`, que `registersystemtest`
    ajoute aux étiquettes ; 16 tests `wrappercore` (10 publics) et 2 `excel` (1 public) ;
  - `BFECtests.csv` se lit dans `Tests/System/Cpp/` ; à défaut, la copie d'`Examples/C++/tests/`
    est lue en tête de liste, là où le glob la mettrait, avec un avertissement qui donne son
    nouveau chemin. `.gitignore` garde l'ancien chemin et ajoute le nouveau ;
- **chemins et références** : `cmake/ConfigFunctions.cmake` et `Tests/Performance/CMakeLists.txt`
  (dossier de `TestTools.h`), les commentaires de `TestTools.h`, de `TestsDataSnapshot.cmake`, de
  `TestRegistration.cmake` et du harnais des benchmarks (`BenchmarkSuite.h`, commentaire seulement),
  `AGENTS.md`, `Documentation/Architecture.md`, les sections vivantes de ce fichier, de
  `FMTWrapperCore/ETAT.md` et de `Tests/Performance/ETAT.md`, `INTERFACE_MIGRATION.md` et le
  commentaire du `._yld` de `perfyields`. Les mentions datées restent telles quelles ;
- **`include_directories` atteint aussi les cibles déjà créées**, en dernier : dans la
  configuration d'avant, les 3 tests Excel, créés avant `FMTWrapperCore/`, avaient déjà
  `FMTWrapperCore/Include`, en dernière place, comme `FMTlib`, `FMT` et `FMTExcelWrapper` ; les
  cibles créées après l'ont en première place. Cela nuance la note de la phase 4 (« n'atteint que
  les cibles créées après lui ») : l'ordre de création décide de la place du dossier, pas de sa
  présence. Le cas des benchmarks de la phase 4 n'a pas été rejoué ;
- **contrôles avant le build**, sur la configuration du `.bat` GL rejouée dans le scratchpad avant
  toute modification, puis après :
  - `--show-only=json-v1` comparé par nom : mêmes 371 tests ; commandes, dossiers de travail et
    propriétés identiques, sauf le chemin du script des deux fixtures
    (`Tests/Support/TestsDataSnapshot.cmake`), l'ordre du `DEPENDS` de `ExamplesModelsUnchanged`
    (mêmes 352 éléments, dans l'ordre des inscriptions) et les étiquettes de composant. Mêmes
    comptes par preset : `all-public` 273, `system` 250, `system-private` 94, `performance` 25,
    `unit` 0, `integration` 0. Aucun avertissement `no target named`. Même résultat avec
    `BFECtests.csv` à l'ancien emplacement (avertissement vu) ou au nouveau (copie jetable) ;
  - les 189 `.vcxproj`, `.filters` et `.sln` sont identiques, dossier de build, GUID et chemins
    déplacés normalisés, sauf deux différences d'ordre : les entrées de reconfiguration (mêmes
    96 fichiers, rangés par chemin) et, pour les 3 tests Excel, `FMTWrapperCore/Include`, qui
    passe de la dernière à la première place ; aucun nom d'en-tête de `FMTWrapperCore/Include`
    n'existe dans les autres dossiers d'en-têtes de ces tests ;
  - variables exportées vers l'installation : `FMTexecutables`, `FMTWrapperCore_tests`,
    `FMTTESTSfiles`, `FMTTESTSsharedoutputs` et `pathtosource` disparaissent, `executablename` a une
    autre dernière valeur ; aucun script d'installation ne les lit ;
  - `-DWITHOUT_TESTS=ON` : aucun test, les mêmes 17 projets ;
  - arborescence prévue sous `tests/` identique ; son plus long chemin reste
    `tests/Spatialyexplicitsimulation_doplanning`, les dossiers de sortie portant le nom des
    cibles, inchangé ;
- **validation après le build de Gabriel** (2026-10-15, sur `7fdcea94`) :
  - `7886ca2b` ne contient que les 87 renommages à 100 %, `7fdcea94` les 20 fichiers modifiés ;
    `git log --follow Tests/System/Cpp/testScenarioReading.cpp` remonte aux commits d'`Examples/C++` ;
  - `BFECtests.csv` est dans `Tests/System/Cpp/`, ignoré par git ; `Examples/C++` n'existe plus ;
  - `--show-only=json-v1` de `build/release`, comparé par nom à
    `test-reorg-baseline\P6-before\tests.json` : mêmes 371 tests ; commandes, dossiers de travail et
    propriétés identiques, hormis le chemin du script des deux fixtures, l'ordre du `DEPENDS` de
    `ExamplesModelsUnchanged` et les étiquettes `wrappercore` (16 tests) et `excel` (2) ;
  - `ctest --preset <p> -N` : `all-public` 273, `system` 250, `system-private` 94, `performance` 25,
    `unit` 0, `integration` 0, avec les mêmes noms que les fichiers `preset_<p>.txt` de la
    référence ; `-L "^wrappercore$"` 18 et `-L "^excel$"` 4, fixtures comprises ;
  - passage de l'installation (`INSTALL_TEST_SUITE=public`, aucun `T:/`) : 0 échec sur 273, 18 s ;
    tableau system 236 verts et 12 désactivés sur 248, performance 23, other 2 ; Total 261 verts et
    12 désactivés ; fixture `ExamplesModelsUnchanged` verte. Les tests `wrappercore` publics et
    `System.UnitTestFMTexcelcache` passent ;
  - plus long chemin sous `build/release/tests` : 92 caractères depuis `build/release`, comme
    avant le build ;
  - non relu : la console de la configuration. L'absence d'avertissement `no target named` se
    déduit des 371 tests identiques, celle de l'avertissement de l'ancien emplacement de
    `BFECtests.csv` du fichier déplacé avant la configuration.

**Phase 7 (2026-10-15 et 2026-10-16)** :

- **table** : `Tests/Support/RenamedTests.csv`, 81 lignes `former;new`, hors du glob qui inscrit
  les tests. Une première table en `<Fonction>Test` (2026-10-15) n'a pas été commitée ; la table
  retenue nomme chaque exécutable par l'action qu'il exerce, la partie When de ses tests
  (section 9.2, décisions du 2026-10-16), et Gabriel l'a validée telle quelle. Quatre renommages
  ne changent que la casse (`CreateModel`, `DoPlanning`, `ForceSolution`, `GetCarbonPredictors`) :
  `core.ignorecase` étant vrai, ils passent par un nom intermédiaire, en deux `git mv` ;
- **déplacements** par `git mv`, contenu inchangé, seuls dans un premier commit : 81 renommages à
  100 % ;
- **noms**, dans le deuxième commit :
  - première colonne de `basetests.csv` (228 lignes) et de `knownbugs.csv` (12),
    `FMTTESTSsharedoutputs` et les tests Excel de `Tests/System/Cpp/CMakeLists.txt` ;
  - dossiers de sortie `../../tests/<exécutable>` : dans 37 sources, et en argument dans les
    lignes 6, 71 et 76 de `basetests.csv` (décision de Gabriel, 2026-10-15). La configuration ne
    crée plus le dossier d'un ancien nom : `maptoFMTforest.exe` plante (0xC0000409, GDAL ne peut
    créer `THEME1.tif`) quand le sien manque, vu dans un miroir du scratchpad ;
    `WrapperCoreWriteToProject` et `WrapperCoreRasterize` créent le leur. Les lignes 19
    (`../../tests/planning`) et 89 (`testWrapperCoreRasterizationPass`) ne nomment pas un
    exécutable et ne changent pas ;
  - le dossier de la ligne 71 est dans son deuxième argument : son nom ctest change aussi, et
    tient maintenant sans empreinte,
    `System.WrapperCoreWriteToProject.TWD_land|actionseries|../../tests/WrapperCoreWriteToProject/`,
    le plus long nom public (93 caractères, 97 avant) ;
  - `registercsvtests` lit une ligne à l'ancien nom sous son nouveau nom (`getrenamedtests`), avec
    un avertissement par fichier qui compte ces lignes ;
  - `tools/testMigration/Update-PrivateTests.ps1`, lancé par `Update_PrivateTests.bat`, en ASCII
    pour Windows PowerShell 5.1 : il déplace au besoin la copie d'`Examples/C++/tests`, renomme la
    première colonne et les dossiers `../../tests/<ancien nom>` des arguments, garde une
    sauvegarde `BFECtests.csv.<date>-<heure>.bak` ignorée par git, refuse un worktree sans la
    table et n'écrit que des comptes et des chemins ;
  - règle des noms réécrite dans `CodingStandards.md` (section Test names) : given/when/then en
    PascalCase sans souligné, sans `Test` ; suite GoogleTest au nom de la classe ; exécutable au
    nom de son action ;
  - références : `AGENTS.md`, `PerformanceTesting.md`, `tools/README.md`, `BANC_INTERFACE.md`,
    les commentaires de 12 scénarios de TWD_land, les sections vivantes de ce fichier, de
    `Tests/Performance/ETAT.md` et de `FMTWrapperCore/ETAT.md`, `INTERFACE_MIGRATION.md`, les
    commentaires des sources de test et des benchmarks, `HeapCorruptionDebug.bat` et l'exemple de
    `RunPrivateTests.bat`. `README.md` et `README.fr.md` ne nomment aucun exécutable de test (leur
    `doplanning` est la méthode de Python et de R) ;
  - restent : les chaînes de nom de méthode passées aux exceptions (`"presolvetest"`,
    `"FMTsetsolution"`...), qui changeraient la sortie des tests, et les classes `Testing::UnitTest*`
    des sources. `UnitTestFMTParser` et `UnitTestFMTmodelcache` sont amies dans des en-têtes de
    FMTlib : ligne de dette dans `Architecture.md` ;
- **en-têtes de licence**, dans le troisième commit : 80 sources C++ (`QueryVersionFeature.cpp`
  l'avait), `TestTools.h`, `TestsDataSnapshot.cmake`, 8 fichiers Python et 9 fichiers R
  (`runTestFile.py` et `runTestFile.R` l'avaient). Chacun dans son encodage : les 63 fichiers C++
  en ASCII passent en cp1252, les fichiers CMake, Python et R en UTF-8 ; les 13 sources en UTF-8
  le restent, dont 8 avec BOM, l'en-tête après lui (7 de ces 8 avaient un corps en ASCII).
  Vérifié par fichier : seul l'en-tête s'ajoute, CRLF partout, « Québec » lisible dans
  l'encodage du fichier, aucun U+FFFD. Les renvois à des lignes de ces sources (section 2.4,
  section 2.8 de `Tests/Performance/ETAT.md`) avancent de 7 ;
- **contrôles avant le build**, sur la configuration du `.bat` GL rejouée dans le scratchpad
  avant toute modification, puis après :
  - `--show-only=json-v1` : mêmes 371 tests ; chaque nom se déduit de l'ancien par la table, sauf
    celui de la ligne 71 ; commandes identiques hormis le nom de l'exécutable et le dossier des
    lignes 6, 71 et 76 ; propriétés identiques, `RESOURCE_LOCK` et `DEPENDS` traduits. Comptes
    par preset inchangés : `all-public` 273, `system` 250, `system-private` 94, `performance` 25,
    `unit` 0, `integration` 0. Aucun avertissement `no target named` ;
  - avertissement de traduction vu sur la copie privée de `new_test`, encore aux anciens noms
    (92 lignes). Dans un projet jetable qui inclut `TestRegistration.cmake`, une copie migrée
    donne les mêmes 92 tests, sans avertissement ; 6 changent aussi par leur dossier
    (5 `ReplanOnDemand`, 1 `ConvertPointsToRasters`) ;
  - script de migration, par le `.bat`, sur des copies jetables : 92 lignes renommées, 6 dossiers,
    sauvegarde identique à l'original, second passage sans changement, refus sans table (code 1) ;
  - les 189 `.vcxproj`, `.filters` et `.sln` sont identiques, anciens noms remplacés, sauf
    l'ordre des entrées de `ALL_BUILD.vcxproj` et de `FMT.sln`, rangées par nom ;
  - dossiers créés sous `tests/` : les 77 d'avant, renommés. Plus long chemin prévu : 91
    caractères depuis `build/release` (`tests/WrapperCoreRunPlanning/...`), 146 en absolu ;
  - script de la fixture avec son en-tête : `save` puis `compare` rendent 0, et 1 après une
    modification ;
- **défaut préexistant relevé, non corrigé** (`getcsvtestcase`, phase 1) : la condition
  `NOT CMAKE_MATCH_4 STREQUAL ""` lit une variable que `if(MATCHES)` ne définit pas quand le
  dernier groupe est vide ; elle prend alors la valeur de la portée appelante, ou le nom de la
  variable. Dans la configuration MSVC, cette valeur existe et est vide : les 2 lignes privées dont
  le chemin finit par `/` n'ont pas de `/` final dans leur nom. Sans elle, elles l'ont (démontré
  par `cmake -P` et le projet jetable). À corriger à part, sans lire `CMAKE_MATCH_4` quand le
  groupe peut être vide : par exemple, tester si le champ finit par `/`.
- **validation après le build de Gabriel** (2026-10-16, sur `ab5b27b3`) :
  - les trois commits reproduisent les arbres vérifiés avant la livraison : `5ae7fa75`, 81
    renommages à 100 % ; `31f3e288`, 75 fichiers ; `ab5b27b3`, 101 fichiers.
    `git log --follow Tests/System/Cpp/ReadScenario.cpp` remonte aux commits d'`Examples/C++` ;
  - copie privée de `new_test` migrée par le script : 92 lignes, toutes aux nouveaux noms ;
    sauvegarde identique à l'original ; les deux fichiers ignorés par git ;
  - `--show-only=json-v1` de `build/release`, comparé à `test-reorg-baseline\P6-before\tests.json`,
    noms traduits par la table : mêmes 371 tests, arguments, dossiers de travail et propriétés,
    hormis les étiquettes de composant, le chemin du script des fixtures et l'ordre du `DEPENDS`
    (phase 6), et le dossier renommé de 9 lignes (3 publiques, 6 privées), dont 7 changent aussi
    de nom par ce dossier ;
  - 44 commandes ne diffèrent que par la casse de l'exécutable (`doplanning.exe` au lieu de
    `DoPlanning.exe`, et de même pour `CreateModel`, `ForceSolution` et `GetCarbonPredictors`) :
    reconstruit le 2026-10-16, le fichier garde la casse de l'ancien, que Windows conserve quand
    l'éditeur de liens l'écrase. `bin/Release` garde aussi les exécutables des 77 autres anciens
    noms. Sans effet sur ctest, qui suit les cibles ; supprimer ces fichiers rend la bonne casse au
    build suivant ;
  - `ctest --preset <p> -N` : `all-public` 273, `system` 250, `system-private` 94, `performance` 25,
    `unit` 0, `integration` 0 ;
  - aucun avertissement de traduction, `no target named` ni d'ancien emplacement, vu en rejouant la
    configuration dans le scratchpad sur la copie migrée ; la console de la configuration de
    Gabriel n'a pas été relue ;
  - `ctest --preset all-public` : « 0 tests failed out of 261 », 13,7 s réelles ; tableau system
    236 verts et 12 désactivés sur 248, performance 23, other 2 ; Total 261 verts et
    12 désactivés sur 273 ; fixture `ExamplesModelsUnchanged` verte. Le passage de l'installation
    (`LastTest.log` copié dans `test-reorg-baseline\P7-after`) compte aussi 273 tests sans échec ;
  - plus long chemin sous `build/release/tests` : 91 caractères, 146 en absolu, comme prévu ;
  - reste à migrer les copies privées du clone `/home/FMT` de rtools45 et du worktree `dev`, une
    fois qu'ils auront la phase 7.

**Phase 8 (2026-10-16)** :

- **dépendance** : `gtest` dans `vcpkg.json` (1.14.0 à la baseline du manifeste).
  `Tests/CMakeLists.txt` appelle `find_package(GTest CONFIG QUIET)` et n'inclut `Unit/` et
  `Integration/` que s'il trouve GoogleTest ; sinon, l'avertissement « GoogleTest was not found: the
  unit and integration tests are not registered », et le reste se configure comme avant. Avec
  `-DWITHOUT_TESTS=ON`, `Tests/CMakeLists.txt` n'est pas lu : `find_package(GTest)` n'est pas appelé ;
- **inscription** : `registergoogletests(<cible> <niveau> <composant> <sources>)`, dans
  `TestRegistration.cmake` :
  - l'exécutable vient de `createnamedexecutable`, le corps de `createexecutable` mis à part pour
    recevoir un nom et plusieurs sources (mêmes liens et dossiers, `bin/<type>`, mimalloc avec
    `WITH_MIMALLOC`), puis il est lié à `GTest::gtest` et `GTest::gtest_main` ;
  - `gtest_discover_tests` en `DISCOVERY_MODE PRE_TEST` (le build ne lance rien), depuis
    `bin/<type>`, délai de découverte de 60 s, préfixe `<Niveau>.`, étiquettes `<niveau>;<composant>` ;
    un cas `DISABLED_` est inscrit désactivé, sous son nom sans le préfixe ;
  - unicité : les noms ne sont connus qu'au passage de ctest. Un script `<cible>_names.cmake`, lu
    par ctest après les cas de chaque exécutable, arrête ctest (code 8) quand un nom est déjà donné à
    un cas d'un autre exécutable GoogleTest, en nommant les deux. Un nom système commence par
    `System.`, et GoogleTest refuse deux cas du même nom dans un exécutable ;
- **cible** `UnitCore` (`Tests/Unit/Core/CMakeLists.txt`) : tous les `.cpp` du dossier, une source
  par classe, au nom de la classe. `Tests/Integration/CMakeLists.txt` dit comment y inscrire un
  composant, sans exécutable ;
- **témoins**, 7 cas en cp1252 avec l'en-tête de licence, en Arrange / Act / Assert :
  - `FMTOperator` : `GivenMultiplyAndAddWhenPrecedenceThenMultiplyComesFirst` et
    `GivenPowerAndMultiplyWhenPrecedenceThenTheyAreEqual` (#346, comportement connu) ;
  - `FMTBounds`, par `FMTAgeBounds` : `GivenBoundsFromTenToTwentyWhenInThenAcceptsBothBounds`,
    `GivenBoundsFromTenToTwentyWhenInThenRejectsTheValuesJustOutside` et
    `GivenUnusedBoundsWhenInThenAcceptsAnyValue` ;
  - `FMTSpec` : `GivenAnAgeRangeWhenAllowWithoutYieldThenAllowsBothAgeBounds` et
    `GivenAnAgeRangeWhenAllowWithoutYieldThenRefusesTheAgesJustOutside` ;
  - aucun défaut révélé, donc aucun cas `DISABLED_` ni issue ;
- **dette révélée** : `FMTBounds<int>` utilisé directement à côté de `FMTSpec` ne se lie pas
  (section 7) ; ligne dans `Documentation/Architecture.md` ;
- **documentation** : `AGENTS.md` (carte du dépôt, inscription, étiquettes, presets, « Adding a unit
  or integration test » ; « Adding a C++ test » devient « Adding a C++ system test »),
  `CodingStandards.md` (nom d'un exécutable GoogleTest), `Architecture.md`, et les sections 0.4, 6.3,
  6.3.3, 7, 9.2 et 9.3 de ce fichier ;
- **contrôles avant le build**, sur la configuration du `.bat` GL rejouée dans le scratchpad :
  - sans GoogleTest (`vcpkg_installed` de `build/release`) : l'avertissement attendu ; 371 tests
    identiques à `test-reorg-baseline\P7-after\tests.json` (hormis la casse de 44 exécutables,
    connue en phase 7) et, sans aucune différence, à une configuration prise avant le diff ; aucun
    test `Unit.` ; mêmes noms par preset que P7-after. Les 189 `.vcxproj`, `.filters` et `.sln` sont
    identiques à ceux d'avant, sauf les entrées de reconfiguration de 91 d'entre eux (`Unit/` et
    `Integration/` ne sont plus lus, `GoogleTest.cmake` l'est) ;
  - `-DWITHOUT_TESTS=ON` : aucun test, pas de `GTest` dans le cache, les 8 projets des bibliothèques
    et des cibles utilitaires ;
  - variables exportées vers l'installation : seules s'ajoutent celles de `find_package(GTest)`
    (`GTest_DIR`, `GTest_FOUND`...), qu'aucun script d'installation ne lit ;
  - avec GoogleTest, installé par vcpkg dans le scratchpad (`-DGTest_DIR`) : `UnitCore` est généré,
    seul ajout aux projets ; compilé par `cl` avec les options de son `.vcxproj` et lié au
    `FMTlib.lib` de `build/release`, il passe ses 7 cas. ctest découvre les 7 cas
    `Unit.<Classe>.Given…`, étiquetés `unit;core`, et les 371 autres tests restent identiques. `-N` :
    `unit` 7, `all-public` 280, `system` 250, `system-private` 94, `performance` 25, `integration` 0 ;
    `-L "^unit$"` : 7 verts, et le résumé montre une ligne `unit` ;
  - vu rouge : une copie faussée, attentes inversées, hors du dépôt, fait échouer les 12 attentes,
    donc les 7 cas ;
  - `/W4 /permissive-` : aucun avertissement dans les sources des tests (24 dans les en-têtes de
    FMTlib, préexistants) ;
  - projet jetable, vrai GoogleTest et fausse `FMTlib` : niveau `Integration`, étiquettes
    `integration;gdal`, cas réussi, en échec, `DISABLED_` et `GTEST_SKIP` vus dans ctest et dans le
    résumé ; avec deux exécutables qui ont le même cas, ctest s'arrête sur ce nom (code 8) ;
- **validation après le build de Gabriel** (2026-10-19, sur `c1553d4b`) :
  - le commit contient les 14 fichiers livrés ; la configuration a installé gtest dans le
    `vcpkg_installed` de `build/release` (`GTest_DIR` dans le cache), et `gtest.dll`,
    `gtest_main.dll` et `gmock.dll` sont dans `bin/Release` à côté de `UnitCore.exe` ;
  - passage de l'installation (`INSTALL_TEST_SUITE=public`, aucun `T:/`) : 0 échec sur 280, 20 s ;
    tableau unit 7 verts, system 236 verts et 12 désactivés sur 248, performance 23, other 2 ; Total
    268 verts et 12 désactivés. `LastTest.log`, `InstallTests.xml` et `InstallTests.json` copiés dans
    `test-reorg-baseline\P8-after`, avec les listes par preset et `--show-only=json-v1` ;
  - `ctest --preset <p> -N` : `unit` 7, les cas `Unit.<Classe>.Given…When…Then` étiquetés
    `unit;core` (`-L "^core$"` : 7) ; `integration` 0 ; `all-public` 280 ; `system` 250,
    `system-private` 94 et `performance` 25, avec les mêmes noms que P7-after ;
  - `--show-only=json-v1` comparé à `test-reorg-baseline\P7-after\tests.json` : les 371 tests
    d'avant identiques (commandes, dossiers de travail, propriétés), et les 7 cas en plus, lancés
    depuis `bin/Release` ;
  - `RunTests.bat unit` : 7 verts, tableau d'une ligne `unit` ; `RunTests.bat all-public` : « 0 tests
    failed out of 268 », 12 désactivés, 17,2 s réelles, fixture `ExamplesModelsUnchanged` verte ;
  - non relue : la console de la configuration de Gabriel. L'absence de l'avertissement de
    GoogleTest se déduit de `GTest_DIR` et des 7 cas inscrits ;
  - l'issue de la dette de `FMTBounds<int>` est rédigée (reproduction minimale sans GoogleTest, quatre
    cas A à D, origine `6bd900dc`), à ouvrir par Gabriel.

**Phase 9a (2026-10-19, `791a09ae`)** :

- **mécanisme**, dans `TestRegistration.cmake` (section 9.3) : `registercsvtests` lit `given` et
  `then` après `doublevalue` ; `checkcsvtestnameparts` arrête la configuration sur une colonne
  seule ou hors du motif, en nommant le fichier et la ligne, même quand la cible de la ligne manque.
  L'en-tête `TEST` est sauté avant tout contrôle. Une colonne vide ne compte pas : les lignes 69 et
  71 de `basetests.csv`, qui ont déjà des colonnes vides en trop, gardent leur nom automatique.
  Dans `BFECtests.csv`, un seul avertissement par fichier donne les lignes ignorées ;
- **défaut de `getcsvtestcase`** (phase 7) : le fichier d'un chemin absolu se lit après le dernier
  `/` par `string(REGEX MATCH)`, placé après la lecture des groupes 1 et 3, qu'il redéfinit. Les
  2 lignes privées dont le chemin finit par `/` gagnent ce `/` dans leur nom (`ReadDatabaseSections`,
  et `DoSpatialPlanning`, dont l'empreinte change), sans doublon ;
- **noms** : 124 lignes, et non 135 comme l'annonçait le prompt de la phase, qui comptait deux fois
  les 11 lignes `ReadScenario` de `knownbugs.csv` (12 + 123) : les 112 lignes `ReadScenario` de
  `basetests.csv` et les 12 lignes de `knownbugs.csv`. Style validé par Gabriel sur douze
  propositions (section 9.2). Noms de 75 à 114 caractères, `System.` compris. Vérifié par script
  contre `HEAD` : quatre premières colonnes intactes, autres lignes inchangées, CRLF et ASCII,
  chaque given et then dans le motif, 240 noms publics uniques ;
- **références** : `CodingStandards.md` (Test names : les deux formes du cas), `AGENTS.md` (Adding a
  C++ system test), sections 1.3, 2.1, 9.2 et 9.3 de ce fichier. La section 2.7 ne cite aucun nom
  ctest ;
- **contrôles avant le build**, sur la configuration du `.bat` GL rejouée dans le scratchpad avant
  le diff, puis après :
  - avant : 371 tests identiques à `test-reorg-baseline\P8-after\tests.json`, hormis la casse de
    44 exécutables (phase 7). Les 7 cas `Unit.` ne sont découverts qu'avec le `UnitCore.exe` du
    build copié dans le rejeu, le `bin/Release` du build dans le `PATH` ;
  - après : les mêmes 371 tests et 7 cas `Unit.` ; seuls changent les noms des 124 lignes et des
    2 lignes privées. Commandes, dossiers de travail et propriétés identiques à la référence
    rejouée et à P8-after, `DEPENDS` de `ExamplesModelsUnchanged` traduit compris, dans le même
    ordre ;
  - `ctest -N` par preset : `unit` 7, `integration` 0, `system` 250, `system-private` 94,
    `performance` 25, `all-public` 280, avec les mêmes noms que les fichiers `preset_<p>.txt` de
    P8-after, une fois les noms traduits. Les 12 lignes désactivées le restent, sous leur nouveau
    nom ;
  - aucun avertissement `no target named`, ni de colonnes ignorées : la copie privée de `new_test`
    n'a pas de colonnes `given` et `then` ;
  - projet jetable qui inclut `TestRegistration.cmake`, sur des copies des CSV : un given sans then,
    un then sans given, un nom qui commence par une minuscule ou qui contient un souligné ou une
    espace arrêtent la configuration sur `basetests.csv, line 91`. Deux lignes au même nom l'arrêtent
    sur le contrôle d'unicité. Une copie de `BFECtests.csv` avec un given et un then sur deux lignes
    donne l'avertissement (« lines 2, 3 ») et les mêmes 92 noms privés que le rejeu ;
- **validation après le build de Gabriel** (2026-10-19, reconfiguration par
  `CMakeFMTVS2022vcpkg_GL.bat`, avant le commit) :
  - passage de l'installation (`INSTALL_TEST_SUITE=public`, aucun `T:/`) : 0 échec sur 280, 18 s ;
    268 exécutés et 12 désactivés, ceux-ci sous leur nouveau nom. `LastTest.log`,
    `InstallTests.xml`, `InstallTests.json` et `LastTestsDisabled.log` copiés dans
    `test-reorg-baseline\P9a-after`, avec `--show-only=json-v1`, les étiquettes et les listes par
    preset ;
  - `--show-only=json-v1` de `build/release` comparé à P8-after : les 371 tests d'avant, et les
    mêmes 7 cas `Unit.` ; seuls changent les noms des 124 lignes et des 2 lignes privées ;
    commandes, dossiers de travail et propriétés identiques, `DEPENDS` traduit compris. Les 378 noms
    sont ceux du rejeu d'après, qui ne diffère que par la casse de 44 exécutables (phase 7) ;
  - `ctest --preset <p> -N` : `unit` 7, `integration` 0, `system` 250, `system-private` 94,
    `performance` 25, `all-public` 280, avec les mêmes noms que P8-after une fois les noms
    traduits ; aucun nom de `all-public` ne contient `T:/` ;
  - `RunTests.bat all-public` : « 0 tests failed out of 268 », 16,5 s réelles ; tableau unit
    7 verts, system 236 verts et 12 désactivés sur 248, performance 23, other 2 ; Total 268 verts
    et 12 désactivés sur 280. Les 112 lignes `ReadScenario` nommées passent, les 12 lignes de
    `knownbugs.csv` restent désactivées ; fixture `ExamplesModelsUnchanged` verte ;
  - non relue : la console de la configuration de Gabriel. L'absence d'avertissement
    `no target named` se déduit des 371 tests identiques, et le rejeu du scratchpad n'en avait
    aucun.

**Phase 9b (2026-10-19)** :

- **noms** : les 116 lignes publiques de `basetests.csv` encore sans nom, sur 51 exécutables
  (`DoPlanning` 24 ; `ReadScenariosTogether`, `SimulateNonSpatially`, `PlanScenario`,
  `ReadInvalidScenario` et `Presolve` 7 chacun ; `WriteModel` 5 ; puis 1 à 3), reçoivent un nom
  écrit à la main, dans le style de la section 9.2 et ses compléments de la phase 9b, validés par
  Gabriel sur treize propositions. Le Then de chaque ligne vient de la lecture de la source de son
  exécutable. Les lignes 69 (`WrapperCoreGetAllMasks`) et 71 (`WrapperCoreWriteToProject`) perdent
  leurs colonnes vides en trop : leur given et leur then sont en colonnes 5 et 6. Les 240 lignes
  publiques sont nommées ; noms de 75 à 139 caractères, `System.` compris. Vérifié par script
  contre `HEAD` : quatre premières colonnes intactes, 116 lignes changées et aucune autre, CRLF et
  ASCII, chaque given et then dans le motif, 240 noms publics uniques, aucun ne contient `Test` ;
- **vérifications qui manquent** : 22 lignes ne vérifient que l'absence d'erreur, et leur nom le
  dit (section 2.3). `ReplanConcurrently` relit la cédule de son premier réplicat, ce que
  l'inventaire du lot 0 n'avait pas vu, et la ligne `mai` de `DoPlanning` attend l'objectif d'un
  modèle vidé par le présolve ;
- **avertissement** : dans `registercsvtests`, une ligne d'un autre fichier que `BFECtests.csv`
  sans `given` ni `then` donne `<fichier>, line <n>: this public row has no given and then
  columns, and gets a name built from its arguments: name it by hand`. Elle garde son nom
  automatique. Les lignes privées gardent le leur sans avertissement ;
- **tableau pour gcyr** : un commentaire en français, rédigé pour #350, que Gabriel publie : le
  style, les 240 noms publics par exécutable dans un bloc `<details>`, et ce qui reste à décider
  avec gcyr (section 9.7) ;
- **table des anciens noms** (demande de Gabriel, 2026-10-19) : `Tests/Support/RenamedTestCases.csv`,
  `former;new`, en ASCII et CRLF, donne pour chaque test public de `master` le nom qu'il porte
  aujourd'hui. Les noms intermédiaires des phases 1 à 8 ne sont jamais arrivés dans `master`, qui
  ne diffère du commit de la référence P0 (`89f500d4`) que par la documentation : les anciens noms
  sont ceux de `test-reorg-baseline\P0`. Les 259 tests de P0 ont été appariés par leur commande
  (exécutable et dossier de sortie traduits par `RenamedTests.csv`, chemins normalisés) aux 280 tests
  publics du rejeu d'après : chacun a exactement un successeur, dont 25 au nom inchangé (les
  2 fixtures et les 23 benchmarks). 28 lignes ont un ancien nom vide : les tests ajoutés pendant la
  réorganisation, soit les 7 cas `Unit.`, les 7 lignes `ReadScenario` de `_DISCOUNTFACTOR` de #357
  et de #374, les 7 tests `System.Python.<Domaine>`, et les 7 tests `System.R.<Domaine>`, qui ne
  sont inscrits que par un build avec R. Aucune ligne privée, aucun chemin `T:/` ni nom de modèle
  privé. CMake ne lit pas ce fichier ; un test ajouté après la réorganisation n'y entre pas ;
- **références** : `CodingStandards.md` (Test names) et `AGENTS.md` (Adding a C++ system test)
  disent qu'une ligne publique sans nom donne un avertissement ; `CodingStandards.md` (Test names)
  et `AGENTS.md` (Testing) citent `RenamedTestCases.csv` ; sections 2.1, 2.3, 9.2, 9.3 et 9.7 de
  ce fichier, et son en-tête ;
- **contrôles avant le build**, sur la configuration du `.bat` GL rejouée dans le scratchpad avant
  le diff, puis après :
  - avant : 378 tests, dont les 7 cas `Unit.` (le `UnitCore.exe` du build copié dans le rejeu),
    identiques à `test-reorg-baseline\P9a-after\tests.json`, hormis la casse de 44 exécutables
    (phase 7) ; mêmes noms par preset ;
  - après : les mêmes 378 tests, dans le même ordre ; seuls changent les noms des 116 lignes.
    Commandes, dossiers de travail et propriétés identiques à P9a-after et au rejeu d'avant,
    `DEPENDS` de `ExamplesModelsUnchanged` traduit compris ;
  - `ctest -N` par preset : `unit` 7, `integration` 0, `system` 250, `system-private` 94,
    `performance` 25, `all-public` 280, avec les mêmes noms que P9a-after une fois les noms
    traduits ; 12 désactivés ; aucun nom de `all-public` ne contient `T:/` ;
  - les 240 noms publics des lignes CSV ont leur Given. Restent sans Given `System.UseExcelCache`,
    inscrit hors CSV, et les 7 tests `System.Python.<Domaine>` : leur nom est à décider avec gcyr
    (section 9.7) ;
  - console de la configuration : aucun avertissement `no target named` ni de ligne publique sans
    nom ; seul reste l'avertissement de développement de Boost (CMP0167), préexistant ;
  - projet jetable qui inclut `TestRegistration.cmake`, sur des copies des CSV : les copies telles
    quelles donnent 332 tests et aucun avertissement, les 92 lignes privées comprises. Une copie qui
    retire le nom de la ligne 90 de `basetests.csv` et de la ligne 3 de `knownbugs.csv`, et ajoute
    une ligne 230 sans nom, donne trois avertissements, qui nomment `basetests.csv, line 90`,
    `basetests.csv, line 230` et `knownbugs.csv, line 3`. Ces lignes prennent leur nom automatique
    (`System.SolveWithEachSolver.loggerlifetime`), et les lignes privées ne donnent aucun avertissement ;
- **commits** : `fea23253` (noms, avertissement, références), puis `f8676a84` (table des anciens
  noms), tous deux de Gabriel ;
- **validation après le build de Gabriel** (2026-10-19, reconfiguration par
  `CMakeFMTVS2022vcpkg_GL.bat` sur `f8676a84`) :
  - passage de l'installation (`INSTALL_TEST_SUITE=public`, aucun `T:/`) : 0 échec sur 280, 20 s ;
    268 exécutés et 12 désactivés. `LastTest.log`, `InstallTests.xml`, `InstallTests.json` et
    `LastTestsDisabled.log` copiés dans `test-reorg-baseline\P9b-after` avant tout appel de ctest,
    avec `--show-only=json-v1`, les étiquettes, les listes par preset et la sortie de
    `RunTests.bat all-public` ;
  - `--show-only=json-v1` de `build/release` comparé à P9a-after : les mêmes 378 tests, dans le même
    ordre ; seuls changent les noms des 116 lignes. Commandes, dossiers de travail et propriétés
    identiques, `DEPENDS` traduit compris. Comparé au rejeu d'après, il ne diffère que par la casse de
    44 exécutables (phase 7) ;
  - `ctest --preset <p> -N` : `unit` 7, `integration` 0, `system` 250, `system-private` 94,
    `performance` 25, `all-public` 280, avec les mêmes noms que P9a-after une fois les noms
    traduits ; 12 désactivés ; aucun nom de `all-public` ne contient `T:/`. Les 240 noms publics des
    lignes CSV ont leur Given ; les noms d'aujourd'hui de `RenamedTestCases.csv`, tests R mis à
    part, sont exactement les 280 noms de `all-public` ;
  - `RunTests.bat all-public` : « 0 tests failed out of 268 », 12,7 s réelles ; tableau unit
    7 verts, system 236 verts et 12 désactivés sur 248, performance 23, other 2 ; Total 268 verts
    et 12 désactivés sur 280. Les 116 lignes renommées passent ; fixture `ExamplesModelsUnchanged`
    verte, `git status` propre ;
  - non relue : la console de la configuration de Gabriel. L'absence d'avertissement
    `no target named` ou de ligne publique sans nom se déduit des 378 tests identiques au rejeu, qui
    n'en avait aucun, et des CSV, dont toutes les lignes publiques ont leurs deux colonnes.

### 9.6 Vérifications communes

- Comptes par niveau (`ctest -N -L "^<niveau>$"`) et par preset, comparés à la référence de la
  phase 0.
- Comparaison avec `--show-only=json-v1` : mêmes commandes, dossiers de travail et propriétés ;
  seules changent les étiquettes, puis une fois les noms. Après un déplacement ou un renommage, même
  nombre de tests.
- Chaque test porte exactement une étiquette de niveau, hormis les fixtures,
  `FMTPerformanceTests.Mimalloc` et les `bfec-perf`. Aucun nom de la suite publique ne contient
  `T:/`.
- La suite publique donne le même résultat qu'avant, la fixture reste verte, et la configuration
  n'émet aucun avertissement `no target named`.
- Après les déplacements : `git diff -M --stat` montre des renommages à 100 %, et le plus long
  chemin sous `build/release/tests` reste sous 260 caractères.

### 9.7 Fin de la réorganisation et ce qui reste ouvert

La réorganisation est terminée avec la phase 9b (2026-10-19). Les tests vivent dans `Tests/`, par
niveau : `Unit`, `Integration` (vide), `System` et `Performance`. Chaque test porte son étiquette de niveau, les
presets lancent un niveau seul, et chaque passage finit par un résumé. Les exécutables portent le
nom de leur action, et les 240 lignes CSV publiques comme les 7 cas GoogleTest ont un nom
Given/When/Then écrit à la main ; `Tests/Support/RenamedTestCases.csv` donne, pour chaque test
public de `master`, son nom d'aujourd'hui. Restent ouverts :

- **la revue commune des tests BFEC** (section 2.7) : ce qu'on ramène dans la suite publique et sous
  quelle forme, et le sort des lignes de #346, #347 et #358 de `knownbugs.csv`. Pour #346, deux
  tests se contredisent : le cas unitaire activé `Unit.FMTOperator.GivenPowerAndMultiplyWhenPrecedenceThenTheyAreEqual`
  et les deux lignes désactivées `…ThenAppliesThePowerFirst`. La revue gardera l'un des deux ;
- **la réponse de gcyr** sur #350 :
  - à la demande de relecture du 2026-10-09 (choix de la phase 3, section 9.2) ;
  - au tableau des noms de la phase 9b, que Gabriel publie ;
  - aux deux points à décider avec lui : les noms automatiques des 92 lignes privées, et les noms
    des tests Python et R, un test par fichier (`System.Python.<Domaine>`, `System.R.<Domaine>`),
    comme `System.UseExcelCache`, seul test C++ inscrit hors CSV ;
- **la migration des copies privées** de `BFECtests.csv` du clone `/home/FMT` (rtools45) et du
  worktree `dev`, quand ils auront la phase 7 : `tools/testMigration/Update_PrivateTests.bat`
  (section 9.4) ;
- **la validation R des phases 7 à 9 par Gabriel**, sous MSYS2 ; la dernière validation R consignée
  ici est celle de la phase 5 (2026-10-14) ;
- **les lots suivants** (section 6.3) : le lot 5 (écriture et relecture) d'abord, puis l'inventaire
  par classe reporté de la phase 8, les lots 7 à 9 et la fermeture de #350. Les 22 lignes de la
  section 2.3 qui ne vérifient que l'absence d'erreur y attendent leurs vérifications ; la règle
  Arrange / Act / Assert s'applique à tout test qu'on touche (section 9.2). Le lot 5, livré le
  2026-10-19 (section 5.7), a ouvert le niveau Integration et ramené ces lignes à 16.
