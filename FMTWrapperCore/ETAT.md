# État de la migration FMTWrapper -> FMTWrapperCore

> Fichier de suivi. Il est mis à jour **à la fin de chaque lot**. Une session qui démarre
> à froid doit pouvoir savoir d'ici quoi faire, sans relire l'historique.

## 1. Objectif et patron

`FMTWrapper` (cible `UI/`, C++/CLI) doit se réduire à de la **traduction C# <-> C++**.
Toute la logique vit dans `FMTWrapperCore` (C++ pur), où elle est testable par exécutable.

Deux règles, vérifiables mécaniquement :

1. **Aucune logique dans `FMTWrapper`** : conversion, `try/catch`, émission d'événements.
2. **Aucun objet FMT visible depuis `FMTWrapper`** : plus un seul `Core::`, `Models::`,
   `Parser::`, `Parallel::`, `Spatial::`, `Heuristics::`, `Exception::`, `Logging::`
   dans le wrapper (`UI/Include`, `UI/Source`).
   *Seule exception* : les DTO `FMTWrapperCore::*Parameters` / `*Results`, qui ne
   contiennent que des types `std`. C'est le contrat de traduction : le wrapper doit
   le voir.

### Patron par domaine

| Fichier | Contenu |
|---|---|
| `FMTWrapperCore/Include/<Domaine>.h` | forward-decl FMT, `struct <X>Parameters` (types `std`), `struct <X>Results` (`success`, `errorMessage`, données), `class __declspec(dllexport) <X>` |
| `FMTWrapperCore/Source/<Domaine>.cpp` | la logique ; erreurs via `Exception::FMTFreeExceptionHandler().raiseFromCatch(...)` |
| `UI/Source/<Fichier>.cpp` | `ConvertirParametres` (C# -> `Parameters`), l'appel Core, `_EnvoyerResultats...` (`Results` -> `RetourJson`/`FeedBack`) |

Référence vivante : `SES.h` / `SES.cpp` / `UI/Source/SimulationSpatialeExplicite.cpp`.

`<X>Results` n'existe que si l'opération produit des données en mémoire. Sinon l'entrée
retourne `void` et les erreurs remontent par exception : c'est le cas de `Rasterization`
et des méthodes `...FromCache` de `Transformation`.

### Convention des doubles entrées

C'est elle qui permet la règle 2 sans sacrifier la testabilité. Chaque classe Core expose :

```cpp
// Entrée pure : prend les objets FMT. Utilisée par les tests C++.
static Results run(const Parameters& p_params, const Models::FMTModel& p_model);
// Entrée indexée : résout le modèle via FMTFormCache. Utilisée par le wrapper.
static Results run(const Parameters& p_params, int p_modelIndex);
```

Le wrapper n'appelle **que** la seconde, et ne touche donc jamais un `FMTModel`.
Les tests appellent la première et n'ont pas à peupler le singleton.

### Règles transversales

- Les signatures publiques de `FMTForm` ne changent pas : le UI .NET externe en dépend.
- Journalisation : le wrapper clone le logger et fait `model.passInLogger(...)` ; le Core
  journalise à travers le modèle. La progression reste en temps réel.
- Chaque lot laisse l'ensemble compilable et ce fichier à jour.

## 2. Inventaire

### Infrastructure

| Classe | Emplacement | Statut |
|---|---|---|
| `FMTFormCache` | `FMTWrapperCore/{Include,Source}` | migré (lot 1) |
| `FMTFormLogger` | `FMTWrapperCore/{Include,Source}` | migré (lot 1) |
| `FMTExceptionHandlerWarning` | `FMTWrapperCore/{Include,Source}` | migré (lot 1) |

### Classes du Core

| Classe | Rôle |
|---|---|
| `Environment` | Capacités de FMT, sans modèle : solveurs, pilotes GDAL, changelog, exceptions |
| `ModelQuery` | Interrogation d'un modèle : yields, masques, thèmes, actions, cédules, écriture de projet |
| `Selection` | Filtrage des contraintes et outputs par nom |
| `Transformation` | Agrégation, séparation et construction d'actions |
| `SES` | Simulation spatiale explicite et optimisation par recuit simulé |
| `Rasterization` | Rastérisation d'un fichier vectoriel selon les thèmes d'un modèle |
| `OperatingArea` | Planification des aires d'opération (calendrier de COS) |
| `AreaVariability` | Variabilité de l'aire initiale selon des proportions par masque |

`Tools` a été éclaté au lot 2 en `Environment` + `ModelQuery` et n'existe plus.

### Domaines

| Fichier UI | L. | Statut | Classe Core | Reste à faire |
|---|---|---|---|---|
| `SimulationSpatialeExplicite.cpp` | 207 | **migré** | `SES` | -- |
| `OptimisationSpatialeExplicite.cpp` | 105 | **migré** | `SES` | -- |
| `Transformations.cpp` | 136 | **migré** | `Transformation`, `ModelQuery` | -- |
| `FMTFormOutils.cpp` | 599 | partiel | `ModelQuery`, `Environment`, `Selection` | 2 helpers privés délégués (`_ObtenirArrayOutputsSelectionnees`, `_ObtenirSEQ`), à supprimer au lot 6 |
| `Raterisation.cpp` | 38 | **migré** | `Rasterization` | -- |
| `OperatingAreaScheduling.cpp` | 60 | **migré** | `OperatingArea` | -- |
| `InitialAreaVariability.cpp` | 86 | **migré** | `AreaVariability` | -- |
| `Plannification.cpp` | 165 | à faire | `Planning` | lot 6 |
| `FMTForm.cpp` | 228 | partiel | `FMTFormCache` | `Cache_AjouterScenarios` et `SetErrorsToWarnings` à déplacer (lot 7) ; le reste est managé par nature (délégué, `IntPtr`) |

### Mesure de la règle 2

À refaire à la fin de chaque lot. Elle compte les types de FMTlib visibles dans le
wrapper lui-même (`UI/Include`, `UI/Source`) ; les tests de `UI/tests` sont comptés à part.

```
grep -rhoE "\b(Core|Models|Parser|Parallel|Spatial|Heuristics|Exception|Logging)::FMT" UI/Include UI/Source | wc -l
```

Le `\b` est indispensable : sans lui, `Core::FMT` correspond aussi à l'intérieur de
`FMTWrapperCore::FMTFormCache`, qui est une classe du Core et a toute sa place dans le
wrapper. **Les valeurs publiées aux lots 1 et 2 (241 et 145) étaient faussées de cette
façon, et comptaient en plus `UI/tests`.** La série ci-dessous est recalculée avec la
commande corrigée, l'état d'origine à partir de `git show HEAD`.

| État | Occurrences |
|---|---|
| Avant la migration | 180 |
| Après le lot 1 | 138 |
| Après le lot 2 | 90 |
| Après le lot 3 | 86 |
| Après le lot 4 | 64 |
| Après le lot 5 | 45 |

Répartition après le lot 5 : `Plannification.cpp` 33, `FMTForm.cpp` 5, `FMTFormOutils.cpp` 4, `FMTForm.h` 3.

`UI/tests` : 23 occurrences, dans `UnitTestFMTFormLogger.cpp` (voir le suspens du lot 1).

Objectif : zéro à la fin du lot 7. Le lot 6 vide le dernier fichier de domaine et les
deux helpers privés restants de `FMTForm` ; ce qui reste ensuite est dans `FMTForm.cpp`.

## 3. Journal des lots

### Lot 1 -- Infrastructure : le cache passe dans le Core (2026-09-09)

Déplacement pur, sans changement de logique, des trois classes d'infrastructure de `UI/`
vers `FMTWrapperCore/`. Elles étaient **déjà 100 % natives** : le seul contact managé était
deux signatures en `System::IntPtr` sur `FMTFormCache`, plus un `#include <msclr...>`
inutilisé dans `FMTFormLogger.cpp`.

- 6 fichiers déplacés (`FMTFormCache`, `FMTFormLogger`, `FMTexceptionhandlerwarning`).
- Namespace `Wrapper` -> `FMTWrapperCore`.
- `public class` (syntaxe C++/CLI) -> `class __declspec(dllexport)`.
- `InitializeLogger` / `RecoverLoggerAndHandler` prennent un `void*` au lieu d'un
  `System::IntPtr` ; `FMTForm.cpp` fait le `.ToPointer()`.
- `#include "stdafx.h"` (PCH de `UI/`) retiré des fichiers déplacés.
- `-DFMTLIBIMPORT` ajouté sur la cible `FMTWrapperCore` (voir Pièges) : révélé par le
  premier `ResetThread()` compilé hors de `UI/`.

La mesure de la règle 2 est tenue à jour en section 2.

Deux suspens :

- Le renommage des classes (`ModelCache` / `CallbackLogger` / `WarningExceptionHandler`)
  est volontairement reporté -- mélanger déplacement et renommage rendrait le diff
  illisible.
- `UI/tests/UnitTestFMTFormLogger.cpp` teste désormais une classe du Core depuis le
  répertoire de tests du wrapper. Il est **laissé sur place volontairement** : il tourne
  sur un modèle public (`Examples/Models/TWD_land`) et il est enregistré dans ctest via
  `UI/CMakeLists.txt`. Le déplacer vers `FMTWrapperCore/tests/` le sortirait de ctest,
  puisque les tests du Core n'y sont pas enregistrés. À trancher le jour où les tests base
  du Core basculeront dans ctest.

### Lot 2 -- `Tools` éclaté, entrées indexées, wrapper aminci (2026-09-09)

**Statut** : compilé le 2026-09-10, après trois corrections (macro Windows `ERROR`,
`FMTTheme::validate` non-const, constructeur et destructeur de `FMTFormCache` sortis du
header -- voir Pièges). **Validé** le 2026-09-11 : SES fonctionne dans l'interface et
`testWrapperCoreSES` passe, après l'unification du filtrage -- seul changement de
comportement potentiel du lot.

- **`Tools` supprimé**, remplacé par `Environment` (sans modèle) et `ModelQuery`
  (interrogation d'un modèle). `testWrapperCoreTools` devient
  `testWrapperCoreEnvironment` ; les autres tests pointent vers `ModelQuery.h`.
- **`Selection` créé** ; `SES::filterConstraints` / `filterOutputs` supprimés au profit
  de `Selection::selectConstraints` / `selectOutputs`. La duplication wrapper/SES
  n'existe plus.
- **Entrées indexées** ajoutées sur `SES` (`RunSES` / `RunOptimization` par index),
  `Transformation` (suffixe `FromCache`) et `ModelQuery` (surcharge `int`).
- **Wrapper aminci** : `FMTFormOutils.cpp` 1009 → 618 lignes,
  `SimulationSpatialeExplicite.cpp` 276 → 207, `OptimisationSpatialeExplicite.cpp`
  159 → 105, `Transformations.cpp` 161 → 136. Le clone du logger, la lecture des
  cédules et la journalisation de progression de SES sont passés dans le Core.

Corrections faites au passage, à signaler en revue :

- `FMTForm::_InscrireLigneFichierTexte` était **du code mort** (aucun appelant) :
  supprimé, ainsi que `_ObtenirArrayContraintes` dont le seul appelant a migré.
- `ObtenirNombrePeriodes` faisait `.back()` sur un vecteur potentiellement vide, soit
  un **comportement indéfini** et non une exception. `ModelQuery::getPeriodsCount`
  retourne maintenant 0 dans ce cas.
- `Transformations.cpp` marshalait `p_outputDirPath` et `p_pri_name` dans des variables
  jamais utilisées : retiré (les paramètres restent dans la signature publique).
- Les 18 caractères U+FFFD de `Tools.h` / `Tools.cpp` (accents perdus lors d'une
  conversion passée) ont disparu avec la réécriture des commentaires.
- `TransformationCore.h` n'avait aucune garde d'inclusion : ajoutée.

Comportements délibérément **préservés** :

- `ObtenirListeOutputs` retourne une liste vide quand le cache est vide, au lieu de
  lever comme les autres interrogations : l'interface peuple sa liste d'outputs avant
  qu'un scénario soit chargé. Le cas est traité explicitement dans
  `ModelQuery::getOutputsNames(int)`.
- Les messages accentués déplacés du wrapper vers `SES.cpp` l'ont été **octet pour
  octet** (les deux fichiers sont en UTF-8), donc leur affichage est inchangé.
- Chaque méthode garde la conversion sortante qu'elle utilisait
  (`_convertToSystemString` pour l'UTF-8, `gcnew System::String` ou
  `marshal_as<System::String^>` ailleurs) : elles ne sont pas équivalentes pour les
  accents.

Quatre helpers privés à types FMT restent dans `FMTForm` parce que les fichiers des
lots 4/5/6 les appellent encore. Ils sont réduits à une délégation vers `Selection` ou
`ModelQuery` — la logique dupliquée a donc déjà disparu — et seront supprimés avec leur
dernier appelant : `_ObtenirOutputSelectionnee` (lot 4),
`_ObtenirArrayContraintesSelectionnees` (lot 5), `_ObtenirArrayOutputsSelectionnees` et
`_ObtenirSEQ` (lot 6).

### Lot 3 -- `Raterisation` -> `Rasterization` (2026-09-10)

- **`Rasterization` créé** : `RasterizationParameters`, `rasterize(params, modèle)` et son
  entrée indexée. Même enchaînement qu'avant : `vectormaptoFMTforest` puis
  `writeForestExtended`.
- Les constantes en dur de l'ancien code sont nommées dans `Rasterization.cpp` :
  `AGE_FACTOR = 1.0`, `AREA_FACTOR = 0.0001` (m² -> ha), `MINIMAL_AREA = 0.0`.
  Valeurs inchangées.
- **Pas de `RasterizationResults`** : l'opération n'écrit que sur disque. L'entrée retourne
  `void` et les erreurs remontent par exception, comme le wrapper les traitait déjà.
- **`Raterisation.cpp` aminci** : 38 lignes, plus un seul type de FMTlib. `fichierPri`,
  jamais utilisé, reste dans la signature publique.
- **`testWrapperCoreRasterization` ajouté**, sur les données publiques `TWD_land` par
  défaut. Il supprime `AGE.tif` et `THEME<i>.tif` avant l'appel, vérifie qu'ils sont
  réécrits, puis les relit avec `readRasters` et exige une forêt non vide.
- **Mesure de la règle 2 corrigée** : l'expression régulière comptait
  `FMTWrapperCore::FMTFormCache` comme un type de FMTlib et incluait `UI/tests`, ce qui
  rendait l'objectif de zéro inatteignable. Série recalculée en section 2. Le
  recensement a aussi fait apparaître le lot 7 (`FMTForm.cpp`).

Trouvé en passant, **non corrigé** car dans FMTlib : le point-virgule parasite de
`FMTAreaParser::writeForestExtended` (voir Pièges).

### Lot 4 -- `OperatingAreaScheduling` -> `OperatingArea` (2026-09-11)

- **`OperatingArea` créé** : `OperatingAreaParameters`, `OperatingAreaResults` et
  `schedule(params, modèle)` avec son entrée indexée. Le Core journalise par
  `Models::FMTModel::getLogger()` (statique), qui atteint le même `FMTFormLogger` que le
  wrapper : l'entrée pure est testable sans le cache, messages compris.
- Les deux refus du modèle (agrégat réservé déjà utilisé, aucune action `YOUVERT`)
  deviennent `success = false` et `errorMessage`, journalisés comme avant. Toute autre
  erreur remonte par exception.
- `ModelQuery::_getFullMask` devient public (`getFullMask`) : le wrapper reconstruisait
  le même masque « tout `?` » à la main. Ajout d'une garde contre un modèle sans thème.
- `_ObtenirOutputSelectionnee` supprimé de `FMTForm` ; le Core appelle
  `Selection::findOutput`.
- **`OperatingAreaScheduling.cpp` réécrit** en traduction pure (131 -> 60 lignes), et
  converti de cp1252 en UTF-8 au passage, par réécriture complète.
- **Régression potentielle du lot 2 corrigée** : depuis le lot 2,
  `_ObtenirOutputSelectionnee` marshalait `returnTimeOutput`, et `marshal_as` lève sur
  `nullptr` ; le code d'origine le comparait sans le marshaler. Le wrapper passe
  maintenant un nom vide quand l'interface n'en envoie pas.
- **`testWrapperCoreOperatingArea` ajouté**, privé : aucun modèle public n'a de yield
  `YOUVERT`. Mêmes arguments que `testOAschedulertask`, et même vérification : le modèle
  final écrit par la tâche doit se relire et rejouer sa cédule.

Constats consignés, comportement conservé tel quel :

- `numeroTheme` est transmis sans conversion à `readOaSchedulerParameters`, qui attend un
  indice 0-based ; l'exemple `testOAschedulertask`, lui, retranche 1 d'un numéro 1-based
  dans sa propre copie de `getOperatingArea`.
- La tâche écrit son modèle final dans le dossier de résultat (le parent de
  `<dossier>\Retour.txt`), et ses sorties de temps de retour sous `<dossier>\Retour\`.
  `FMTTaskHandler::conccurentRun()` appelle lui-même `finalize()`.
- Les messages de progression sont passés en UTF-8, comme ceux de SES : voir Pièges,
  « Encodage des messages ».

### Lot 5 -- `InitialAreaVariability` -> `AreaVariability` (2026-09-11)

- **`AreaVariability` créé** : `AreaVariabilityParameters`, `AreaVariabilityResults` et
  `run(params, modèle)` avec son entrée indexée, sur le patron du lot 4 (journalisation
  par le logger statique de FMT).
- **Découpage de la table** : le wrapper convertit `ListeInformations` telle quelle en
  `std::vector<std::vector<std::string>>`, en distinguant table absente et table vide ;
  le Core l'interprète (en-tête ignoré, dernière colonne = proportion, reste = masque).
- **Décisions de Gabriel appliquées** :
  - la construction des masques reproduit fidèlement le comportement historique : une
    colonne dont la valeur égale la proportion est écartée du masque. Le bug est suivi
    dans une issue, et un commentaire le signale dans `AreaVariability.cpp` ;
  - l'`exit(-1)` sur des masques qui se recoupent, qui fermait tout le processus de
    l'interface, devient une exception FMT (`FMTfunctionfailed`). Son message nomme le
    développement fautif et le nombre de masques qui le recouvrent.
- Les trois refus (table absente, table vide, aucun output demandé) deviennent
  `success = false` et `errorMessage`, avec les messages d'avant. Une ligne vide dans la
  table lève une exception explicite, là où l'ancien code levait une
  `IndexOutOfRangeException`.
- Le wrapper convertit les `nullptr` en valeurs vides : le Core produit alors ses propres
  messages, au lieu d'une `NullReferenceException` qui dépendait de l'endroit où le
  `nullptr` était lu.
- `_ObtenirArrayContraintesSelectionnees` supprimé de `FMTForm` ; le Core appelle
  `Selection::selectConstraints`.
- **`InitialAreaVariability.cpp` réécrit** en traduction pure (130 -> 86 lignes). Ses
  messages étaient déjà en UTF-8 : ils sont passés octet pour octet dans le Core.
- **`testWrapperCoreAreaVariability` ajouté**, sur les données publiques `TWD_land`
  (scénario `LP`, masques de l'exemple `testareavariabilities`). Il couvre le cas
  nominal, les trois refus et l'exception des masques qui se recoupent.

Suspens : trois copies de `_toStdVector` vivent maintenant dans le wrapper
(`FMTFormOutils.cpp`, `Transformations.cpp`, `InitialAreaVariability.cpp`) -- à réunir
dans un header de conversion (voir « Ensuite »).

## 4. Prochain lot

### Lot 6 -- `Plannification` / `Replanification` -> `Planning`

Le plus lourd : deux entrées, des tâches parallèles, et le dernier fichier en cp1252.

1. `PlanningParameters` / `ReplanningParameters` et une classe `Planning`, avec
   `plan(params, modèles)` et `replan(params, stratégique, stochastique, tactique)`, plus
   leurs entrées indexées.
2. **Plannification** : un `FMTPlanningTask` reçoit un modèle par scénario (solveur,
   `LENGTH`, `TOLERANCE` 0,01, outputs retenus, `FORCE_PARTIAL_BUILD` selon la relecture
   de cédule), puis `FMTTaskHandler::conccurentRun`. Deux détails à reproduire :
   - le drapeau de relecture est lu par `playback[scenarios->IndexOf(scen)]` : un scénario
     présent deux fois reprend le drapeau de sa première occurrence ;
   - threads par scénario : `nbreProcessus / scenarios->Count` si
     `scenarios->Count <= nbreProcessus`, sinon 1.
3. **Replanification** : modèles global (`FMTLpModel`), stochastique (`FMTNssModel`) et
   local (`FMTLpModel`), `FMTReplanningTask`, puis `FMTTaskHandler::onDemandRun`. Elle
   appelle `settasklogginglevel` / `setdefaultlogginglevel`, propres à `FMTFormLogger` :
   l'entrée pure ne doit les appliquer que si le logger en est un (`dynamic_cast`), pour
   rester testable. `p_writeSchedule` n'est pas utilisé.
4. `layersoptions` : `SEPARATOR=SEMICOLON` quand le pilote est `CSV`, dans les deux entrées.
5. `_ObtenirArrayOutputsSelectionnees` et `_ObtenirSEQ` disparaissent, et avec eux les
   déclarations avancées `Core::` de `FMTForm.h`.
6. **Fichier cp1252** : réécriture complète ; ses messages passent en UTF-8, comme au lot 4.
7. Test : `TWD_land` a des scénarios publics de replanification (`Globalreplanning`,
   `Globalfire`, `Localreplanning`, déjà utilisés par `UnitTestFMTFormLogger`), et `LP`
   pour la plannification.

Critère de sortie : `Plannification.cpp` ne contient plus un seul type de FMTlib
(33 aujourd'hui), et `FMTForm` plus aucun helper privé à types FMT.

### Ensuite

- **Lot 7 -- `FMTForm.cpp`** : `Cache_AjouterScenarios` (lecture du projet avec
  `FMTModelParser`) et `SetErrorsToWarnings` (conversion en `Exception::FMTexc`) passent
  dans `FMTFormCache`, qui reçoit des `std::string` et des `int`. Ce sont les derniers
  types de FMTlib du wrapper une fois le lot 6 fait.
- **Nettoyage** : réunir les conversions du wrapper (`_toStdString`, `_toStdVector`) dans
  un header ; renommer les classes d'infrastructure (`ModelCache` / `CallbackLogger` /
  `WarningExceptionHandler`), reporté depuis le lot 1.

## 5. Pièges connus

- **Encodage cp1252** -- `UI/Source/Plannification.cpp` est le dernier fichier en cp1252
  (`OperatingAreaScheduling.cpp` a été converti au lot 4) ; tous les autres fichiers de
  `UI/` et de `FMTWrapperCore/` sont en UTF-8 sans BOM. L'éditer avec un outil qui suppose
  l'UTF-8 corrompt les accents : le réécrire entièrement, ou l'éditer au niveau octet.
- **Normaliser un fichier en Python** : lire *avant* d'ouvrir en écriture.
  `open(p, 'wb').write(f(open(p, 'rb').read()))` tronque le fichier avant de le lire et
  l'écrit vide -- vu au lot 4 sur `OperatingArea.h/.cpp`, restaurés aussitôt.
- **Encodage des messages** -- MSVC compile sans `/utf-8` : un fichier UTF-8 sans BOM est
  relu en cp1252 et ses octets passent tels quels dans le binaire. Deux chemins les
  décodent ensuite différemment :
  - `_convertToSystemString` (valeurs de retour : changelog, descriptions d'exceptions)
    décode en **UTF-8** ;
  - `_toFeedback`, par lequel passent tous les messages du logger, fait
    `gcnew System::String(const char*)`, qui décode dans la **page ANSI** (cp1252) sous
    .NET Framework.
  La convention de l'équipe est l'UTF-8 : `SimulationSpatialeExplicite.cpp` a été converti
  le 2025-09-19 (`f71f5dd5`), et SES fonctionne dans l'interface. Les messages déplacés au
  lot 4 sont donc en UTF-8 et se comportent désormais comme ceux de SES. **À vérifier une
  fois dans l'interface** : si les accents des messages de progression y paraissent
  déformés (« DÃ©marrage »), la correction est unique et tient dans `_toFeedback` -- mais
  elle toucherait aussi les messages de FMTlib qui citent le contenu des fichiers de
  modèle, dont l'encodage est à vérifier avant.
- **Filtrage des contraintes / outputs** : deux implémentations divergentes coexistent.
  `FMTForm::_ObtenirArrayContraintesSelectionnees` retire `\r`/`\n`, applique `TrimEnd`,
  suit l'ordre du modèle et **ne fait pas de `break`**. `SES::filterConstraints` fait une
  égalité stricte, suit l'ordre des noms sélectionnés et `break`. Les unifier change
  l'ordre et le nombre de contraintes passées à `setConstraints` dans SES, **et l'ordre
  influence la matrice LP**. Valider par `testWrapperCoreSES` avant/après ; si les sorties
  diffèrent, garder deux méthodes aux sémantiques distinctes et documentées.
- **`public class`** est une syntaxe C++/CLI : à abandonner dans tout code déplacé.
- **Macros Windows** -- `windows.h` arrive par inclusion transitive (GDAL via
  `FMTModelParser.h`) et définit `ERROR`, `DELETE`, `IN`, `OUT`, `NEAR`, `FAR`.
  Une variable nommée `ERROR` donne un `C2059: erreur de syntaxe : 'constante'`,
  parce que `#define ERROR 0` la transforme en littéral. Vu au lot 2 dans
  `Environment::getErrorsToIgnore`.
- **`FMTTheme::validate`** prend le masque par référence **non-const** : il le
  tronque quand la chaîne porte plus d'attributs qu'il n'y a de thèmes. Lui passer
  une copie locale, comme le faisait le wrapper.
- **Les accesseurs de `FMTModel` retournent par valeur** (`getThemes`, `getActions`,
  `getYields`, `getOutputs`, `getConstraints`, `getArea`). Les chaîner
  (`getThemes().at(i).getBaseAttributes()`) reste licite mais fragile, puisque
  `getBaseAttributes` rend une référence dans un temporaire : passer par une
  variable locale.
- **`FMTLIBIMPORT` est obligatoire sur toute cible qui consomme FMTlib.** `FMTEXPORT`
  (`Include/FMTutility.h`) ne vaut `__declspec(dllimport)` que si `FMTLIBIMPORT` est
  défini ; sinon la macro est vide. Les *appels de fonctions* se résolvent quand même
  via les thunks de l'import library, ce qui masque le problème -- mais les *données
  statiques* d'une classe FMTlib, elles, ne se résolvent pas :
  `LNK2019 ... FMTExceptionHandler::m_mainthreadid`. `FMTWrapperCore` était la seule
  cible sans ce flag (la ligne existait, commentée, et visait `FMTWrapper` par
  copier-coller). Corrigé au lot 1. Symptôme à reconnaître : un `LNK2019` sur un
  `static` d'une classe FMTlib, alors que les fonctions de la même classe passent.
- **`__declspec(dllexport)`** obligatoire sur toute classe Core consommée par le wrapper.
- **Exporter une classe instancie ses membres implicites dans chaque client.** Une
  classe exportée dont un membre est un `std::unique_ptr<T>` avec `T` seulement
  déclaré en avant ne peut plus définir constructeur ni destructeur `= default`
  dans le header : chaque unité de compilation cliente les instancie et échoue sur
  `can't delete an incomplete type`. Les déclarer dans le header et les définir
  `= default` dans le .cpp, là où le type est complet. Vu au lot 2 sur
  `FMTFormCache::m_Models`. **Ne pas** « corriger » en incluant `FMTModel.h` dans
  le header : ça exposerait le type à tout le wrapper, contre la règle 2.
- **Signatures `FMTForm` figées** : le UI .NET externe en dépend.
- **Bug de FMTlib repéré, non corrigé** : `FMTAreaParser::writeForestExtended`
  (`Source/FMTAreaParser.cpp`) contient
  `if (!_isMapWithSameThemes(p_themes, p_VectorFile));` -- le point-virgule rend le `if`
  vide, et le bloc `setLastThemeWithArea` qui suit s'exécute **toujours**. Hors périmètre
  de la migration : `Rasterization` reproduit fidèlement le comportement actuel. À corriger
  dans FMTlib en vérifiant l'effet sur les rasters produits.
- **`FMTlib` est SHARED** : un seul `FMTlib.dll` dans le processus, donc les statiques
  `FMTModel::getLogger()` / `getExceptionHandler()` restent partagées entre
  `FMTWrapper.dll` et `FMTWrapperCore.dll`. C'est ce qui rend le lot 1 sûr.
- **`Excel/`** a son propre `Wrapper::FMTModelCache` (~2600 lignes, dérive de `FMTLpModel`),
  sans rapport avec `FMTFormCache`. Hors périmètre, mais c'est la raison du choix du
  namespace `FMTWrapperCore` plutôt que `Wrapper`.

## 6. Tests

**Base et BFEC restent séparés.** L'interface n'étant pas publique sur le dépôt, les tests
du Core restent **manuels et privés**.

- Convention existante : `Examples/C++/tests/basetests.csv` (modèles publics de
  `Examples/Models/`, enregistrés dans ctest) contre `Examples/C++/tests/BFECtests.csv`
  (modèles privés sur `T:\`).
- Les tests `FMTWrapperCore/tests/*` sont des exécutables lancés à la main, sur le patron
  de `testWrapperCoreSES.cpp` : paramètres via un JSON en `argv[1]`, valeurs de référence
  en `argv[2]`, repli codé en dur sur un modèle `T:\`.
- **Ne pas** ajouter d'`add_test()` dans `FMTWrapperCore/CMakeLists.txt` : l'absence est
  volontaire tant que les modèles sont privés. Le jour où une interface publique existera,
  les tests base basculeront dans ctest.
- Chaque lot ajoute son `testWrapperCore<Domaine>.cpp` selon ce patron. La convention des
  doubles entrées garantit que les tests n'ont jamais à peupler le singleton.
- Quand un jeu de données public existe, le test l'utilise par défaut et reprend la forme
  d'arguments de son cousin de `basetests.csv` : `testWrapperCoreRasterization` tourne sur
  `Examples/Models/TWD_land` avec les mêmes arguments que `maptoFMTforest`. Il reste
  manuel, mais rejoindra ctest en une ligne le moment venu. Les tests SES, qui n'ont que
  des modèles privés, prennent un JSON.
- Sinon, le test tourne sur un modèle privé et reprend la forme d'arguments de son cousin
  de `BFECtests.csv` : `testWrapperCoreOperatingArea` a les mêmes arguments que
  `testOAschedulertask`, aucun modèle public n'ayant de yield `YOUVERT`.
