# État de la migration FMTWrapper -> FMTWrapperCore

> Fichier de suivi. Il est mis à jour **à la fin de chaque lot**. Une session qui démarre
> à froid doit pouvoir savoir d'ici quoi faire, sans relire l'historique.

## 1. Objectif et patron

`FMTWrapper` (cible `UI/`, C++/CLI) doit se réduire à de la **traduction C# <-> C++**.
Toute la logique vit dans `FMTWrapperCore` (C++ pur), où elle est testable par exécutable.

Trois règles, vérifiables mécaniquement :

1. **Aucune logique dans `FMTWrapper`** : conversion, `try/catch`, émission d'événements.
2. **Aucun objet FMT visible depuis `FMTWrapper`** : plus un seul `Core::`, `Models::`,
   `Parser::`, `Parallel::`, `Spatial::`, `Heuristics::`, `Exception::`, `Logging::`
   dans le wrapper (`UI/Include`, `UI/Source`).
   *Seule exception* : les DTO `FMTWrapperCore::*Parameters` / `*Results`, qui ne
   contiennent que des types `std`. C'est le contrat de traduction : le wrapper doit
   le voir.
3. **Le wrapper ne parle au Core que par `Controller`** : il n'inclut que `Controller.h`
   et n'y nomme que `FMTWrapperCore::Controller` et les DTO (lot 5b).

### Patron par domaine

| Fichier | Contenu |
|---|---|
| `FMTWrapperCore/Include/<Domaine>Types.h` | les DTO, types `std` uniquement : `struct <X>Parameters`, `struct <X>Results` (`success`, `errorMessage`, données) |
| `FMTWrapperCore/Include/<Domaine>.h` | le service : forward-decl FMT, `class FMTWRAPPERCOREEXPORT <X>` et ses entrées pures |
| `FMTWrapperCore/Source/<Domaine>.cpp` | la logique ; erreurs via `raiseFromCatch` du gestionnaire d'exceptions |
| `Controller.h` / `Controller.cpp` | une méthode par opération système de `FMTForm` : résout l'index de scénario, délègue à l'entrée pure |
| `UI/Source/<Fichier>.cpp` | `ConvertirParametres` (C# -> `Parameters`), l'appel du contrôleur, `_EnvoyerResultats...` (`Results` -> `RetourJson`/`FeedBack`) |

Référence vivante : `OperatingAreaTypes.h`, `OperatingArea.h` / `.cpp`,
`Controller::scheduleOperatingAreas` et `UI/Source/OperatingAreaScheduling.cpp`.

`<X>Results` n'existe que si l'opération produit des données en mémoire. Sinon l'entrée
retourne `void` et les erreurs remontent par exception : c'est le cas de `Rasterization`
et des transformations.

### Contrôleur façade

`FMTWrapperCore::Controller` (lot 5b) applique le patron *Controller* de GRASP (Larman) :
un objet hors de l'interface reçoit les opérations système et les délègue.

- **Les services n'ont que des entrées pures** : elles prennent les objets FMT et ne
  connaissent pas le cache. Les tests C++ les appellent directement.
- **Le contrôleur** ne reçoit que des types `std`, des DTO et des index de scénario. Il
  résout l'index dans `FMTFormCache`, puis délègue à l'entrée pure : il coordonne sans
  calculer. Dans le Core, seuls `Controller.cpp` et `FMTFormCache.cpp` utilisent le cache.
- **`Controller.h` n'inclut que les `<Domaine>Types.h`** : le wrapper ne voit ni les
  services ni la moindre déclaration de FMTlib.
- Il est sans état et ses méthodes sont statiques : l'état de la session (scénarios,
  logger, gestionnaire d'exceptions) reste dans `FMTFormCache`.

```cpp
// Service : entrée pure, testable sans cache.
static OperatingAreaResults schedule(const OperatingAreaParameters& p_params, const Models::FMTModel& p_model);

// Contrôleur : résout l'index, délègue.
OperatingAreaResults Controller::scheduleOperatingAreas(const OperatingAreaParameters& p_params, int p_modelIndex)
{
    return OperatingArea::schedule(p_params, getCachedModel(p_modelIndex));
}
```

### Règles transversales

- Les signatures publiques de `FMTForm` ne changent pas : le UI .NET externe en dépend.
- Journalisation : les entrées pures écrivent dans le logger statique de FMT
  (`Models::FMTModel::getLogger()`) : le `FMTFormLogger` dans l'interface, le logger par
  défaut dans un test. La progression reste en temps réel.
- Toute classe du Core porte `FMTWRAPPERCOREEXPORT` (`FMTWrapperCoreExport.h`) ; les DTO,
  sans fonction membre hors ligne, n'en ont pas besoin.
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
| `Controller` | Contrôleur façade : le seul point d'entrée du wrapper, une méthode par opération de `FMTForm` |
| `Environment` | Capacités de FMT, sans modèle : solveurs, pilotes GDAL, changelog, exceptions |
| `ModelQuery` | Interrogation d'un modèle : yields, masques, thèmes, actions, cédules, écriture de projet |
| `Selection` | Filtrage des contraintes et outputs par nom |
| `Transformation` | Agrégation, séparation et construction d'actions |
| `SES` | Simulation spatiale explicite et optimisation par recuit simulé |
| `Rasterization` | Rastérisation d'un fichier vectoriel selon les thèmes d'un modèle |
| `OperatingArea` | Planification des aires d'opération (calendrier de COS) |
| `AreaVariability` | Variabilité de l'aire initiale selon des proportions par masque |
| `Planning` | Planification et replanification de scénarios, par les tâches parallèles de FMT |

`Tools` a été éclaté au lot 2 en `Environment` + `ModelQuery` et n'existe plus. Depuis le
lot 5b, les DTO de chaque domaine sont dans `<Domaine>Types.h`, et
`FMTWrapperCoreExport.h` définit `FMTWRAPPERCOREEXPORT`.

### Domaines

| Fichier UI | L. | Statut | Service, appelé par `Controller` | Reste à faire |
|---|---|---|---|---|
| `SimulationSpatialeExplicite.cpp` | 207 | **migré** | `SES` | -- |
| `OptimisationSpatialeExplicite.cpp` | 105 | **migré** | `SES` | -- |
| `Transformations.cpp` | 135 | **migré** | `Transformation`, `ModelQuery` | -- |
| `FMTFormOutils.cpp` | 495 | **migré** | `ModelQuery`, `Environment` | -- |
| `Raterisation.cpp` | 38 | **migré** | `Rasterization` | -- |
| `OperatingAreaScheduling.cpp` | 60 | **migré** | `OperatingArea` | -- |
| `InitialAreaVariability.cpp` | 86 | **migré** | `AreaVariability` | -- |
| `Plannification.cpp` | 141 | **migré** | `Planning` | -- |
| `FMTForm.cpp` | 206 | **migré** | `FMTFormCache`, `Environment` | -- (le délégué et l'`IntPtr` restent : managés par nature) |

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
| Après le lot 5b | 40 |
| Après le lot 6 | 0 |

`UI/tests` : 23 occurrences, dans `UnitTestFMTFormLogger.cpp` (voir le suspens du lot 1).

**Objectif atteint au lot 6** : plus un seul type de FMTlib dans le wrapper.

### Mesure de la règle 3

Les noms du Core que le wrapper utilise, hors `Controller` et DTO :

```
grep -raoE "FMTWrapperCore::\w+" UI/Include UI/Source | grep -vE "::(Controller|\w+Parameters|\w+Results)$" | sort | uniq -c
```

**À zéro depuis le lot 6** : la commande ne retourne rien, et aucun fichier du wrapper
n'inclut d'autre en-tête du Core que `Controller.h`.

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

### Lot 5b -- Contrôleur façade et macro d'export (2026-09-14)

**Statut** : livré le 2026-09-14, **pas encore compilé**. Refactor sans changement de
comportement attendu, décidé avec Gabriel avant le lot 6 pour que celui-ci s'écrive
directement dans la forme finale. Il absorbe le lot 7.

- **`FMTWrapperCoreExport.h`** définit `FMTWRAPPERCOREEXPORT` : `dllexport` quand
  `FMTWrapperCore_EXPORTS` est défini (CMake le fait pour la cible SHARED), `dllimport`
  sinon, `visibility("default")` hors Windows. Il remplace les 11 `__declspec(dllexport)`
  codés en dur, que les clients voyaient à tort (voir Pièges).
- **`Controller` créé** : 36 méthodes statiques, une par opération système de `FMTForm`.
  Il reprend les entrées indexées et les `...FromCache` des lots 2 à 5, tout l'accès au
  cache de `FMTForm.cpp` (logger, gestionnaire d'exceptions, ajout, retrait et vidage des
  scénarios) et la partie native de `FMTForm::_raiseFromCatch` (`logCurrentException`,
  `openErrorLocation`).
- **Lot 7 absorbé** : `Cache_AjouterScenarios` (lecture du projet) et
  `SetErrorsToWarnings` (conversion en `Exception::FMTexc`, 10 avertissements par défaut)
  passent dans `Controller::addScenarios` et `Controller::setErrorsToWarnings`.
- **Les services n'ont plus que des entrées pures**, et aucun n'inclut plus
  `FMTFormCache.h`. `ModelQuery::getPeriodsCount` gagne son entrée pure.
- **DTO séparés** dans `SESTypes.h`, `RasterizationTypes.h`, `OperatingAreaTypes.h` et
  `AreaVariabilityTypes.h`, les seuls en-têtes que `Controller.h` inclut.
  `OutputsData::outputObjects` (`std::vector<Core::FMTOutput>`, « pour usage ultérieur »)
  en sort : c'était le seul type FMT d'un DTO, et SES ne s'en servait qu'en interne ;
  `calculateOutputs` rend désormais la sélection par un paramètre de sortie.
- **Journalisation de SES ramenée dans les entrées pures.** La nouvelle surcharge
  `SES::RunSES(params, modèle)` reprend l'ancienne entrée indexée dans le même ordre :
  clone du logger courant, lecture des cédules, `passInLogger`, messages. Les messages de
  l'ancienne `RunOptimization` indexée passent dans son entrée pure :
  `testWrapperCoreSA` journalise donc maintenant sa progression.
- **Wrapper** : `FMTForm.cpp` et les fichiers de domaine n'incluent plus que
  `Controller.h`. `FMTForm.cpp` n'a plus un seul type de FMTlib (règle 2 : 45 -> 40).
- Divers : déclarations avancées mortes retirées de `FMTForm.h` (`Core::FMTConstraint`,
  `Models::FMTSeModel`) ; seuil `m_GET_ALL_MASKS_THRESHOLD` sorti du header de
  `ModelQuery` (détail d'implémentation, et pas de donnée statique dans une classe
  importée) ; quatre U+FFFD corrigés dans les commentaires de `TransformationCore.cpp`.

Différences observables, toutes sur des chemins d'erreur :

- un index demandé sur un cache vide lève désormais le même `FMTrangeerror` (« no scenario
  in cache for index N ») pour toutes les opérations ; SES, la rastérisation, les aires
  d'opération, la variabilité et les transformations levaient celui de
  `FMTFormCache::getModel` ;
- `RunSES` et `RunOptimization` ne vérifient plus que le logger installé est un
  `FMTFormLogger` ; dans l'interface, il l'est toujours ;
- l'horodatage de l'optimisation est journalisé après la résolution du scénario, et manque
  donc si l'index est invalide ;
- le message « FMT -> Erreur d'optimisation » disparaît : sa branche était morte, puisque
  `RunOptimization` ne retourne jamais `success = false` ;
- les noms de méthode cités dans les piles d'erreurs changent
  (`ModelQuery::_getCachedModel` -> `Controller::getCachedModel`, etc.).

Transitoire jusqu'au lot 6 : `_ObtenirSEQ` résout lui-même le scénario par `FMTFormCache`,
puisque `ModelQuery` n'a plus d'entrée indexée et que le contrôleur ne peut pas retourner
de `Core::FMTSchedule`.

À faire par Gabriel : reconfigurer CMake (7 nouveaux fichiers), compiler `FMTWrapperCore`,
`FMTWrapper` et les tests, puis un aller-retour dans l'interface : chargement d'un
scénario, SES, et une erreur provoquée pour exercer `_raiseFromCatch`.

### Lot 6 -- `Plannification` / `Replanification` -> `Planning` (2026-09-15)

**Statut** : livré le 2026-09-15, **pas encore compilé**. Dernier lot de domaine : les
règles 2 et 3 sont à zéro.

- **`Planning` créé**, avec `PlanningTypes.h` (`PlanningParameters`,
  `ReplanningParameters`) et deux entrées pures : `plan(params, modèles, relectures,
  rapporteur)` et `replan(params, stratégique, stochastique, tactique)`. Le contrôleur gagne
  `plan` et `replan`, qui reçoivent les index de scénario.
- **Plannification** reproduite pas à pas : un `FMTLpModel` par scénario,
  `setStrictlyPositivesOutputsMatrix`, `LENGTH`, threads partagés entre les scénarios
  s'il y en a assez (sinon 1), `TOLERANCE` 0,01 sur `FMTModel`, outputs retenus,
  `FORCE_PARTIAL_BUILD` selon la relecture, puis `conccurentRun`. Un scénario présent deux
  fois reprend le drapeau de sa première occurrence : le service le retrouve par identité
  du modèle, le contrôleur résolvant un même index vers le même modèle du cache.
- **Décisions de Gabriel appliquées** :
  - une relecture de cédule en échec reste **signalée sans interrompre** la
    planification, le scénario étant lancé avec une cédule vide. Le service confie
    l'erreur à un `Planning::ErrorReporter` appelé depuis son bloc `catch` ; le contrôleur
    y branche `logCurrentException` et `openErrorLocation`, ce que faisait
    `_raiseFromCatch`. Sans rapporteur (appel direct au service), l'erreur remonte ;
  - le niveau du logger de l'interface est **rétabli même si la replanification lève** :
    un garde (`ScopedTaskLoggingLevel`) remplace l'appel à `setdefaultlogginglevel`, qui
    ne s'exécutait qu'après un succès.
- **Replanification** : modèles global, stochastique et local paramétrés comme avant,
  `FMTReplanningTask`, `setReplicates`, puis `onDemandRun`. Le niveau des tâches ne
  s'applique qu'à un `FMTFormLogger` (`dynamic_cast`) : un test garde son logger.
- **`indProduireSolution` commande l'écriture des cédules des réplicats**
  (`writeSchedule` de `FMTReplanningTask`) depuis la première version de l'interface
  (`eeebbdaf`, 2024) ; `p_writeSchedule` n'a jamais été branché. Comportement conservé,
  et consigné dans le wrapper.
- **`Plannification.cpp` réécrit** en traduction pure (165 -> 141 lignes) et converti de
  cp1252 en UTF-8 : c'était le dernier fichier cp1252 du wrapper. Ses messages sont passés
  dans le Core à l'identique, en UTF-8.
- `_ObtenirArrayOutputsSelectionnees`, `_ObtenirSEQ` et les déclarations avancées `Core::`
  de `FMTForm.h` disparaissent, avec les inclusions transitoires de `FMTFormOutils.cpp` et
  son `_toStdVector` devenu inutile. Le stub vide `FMTPlanningTaskParameters.h` est
  supprimé.
- **`testWrapperCorePlanning` ajouté**, sur une **copie** de `TWD_land` (voir Pièges) :
  planification de `LP` (optimisé, cédule réécrite), `LP3` (rejoué) et
  `Globalreplanning` (sans cédule : relecture signalée, planification poursuivie) ;
  replanification des scénarios de `replanningtest`, cédules des réplicats comprises.

Différences observables, toutes sur des chemins d'erreur :

- une relecture de cédule en échec apparaît une fois dans l'interface au lieu de deux :
  `_raiseFromCatch` la renvoyait aussi directement par `FeedBack`, en plus du logger ;
- les index de scénario sont tous résolus avant la première journalisation : un index
  invalide lève avant l'horodatage et les messages des scénarios précédents.

## 4. Prochain lot

Les lots de domaine sont terminés : les règles 2 et 3 sont à zéro depuis le lot 6. Le
prochain travail est sa validation -- compilation, `testWrapperCorePlanning`, puis
Plannification et Replanification dans l'interface -- et, au passage, l'affichage des
accents des messages de progression (voir Pièges, « Encodage des messages »).

### Ensuite

- **Nettoyage** : réunir les conversions du wrapper (`_toStdString`, `_toStdVector`) dans
  un header ; renommer les classes d'infrastructure (`ModelCache` / `CallbackLogger` /
  `WarningExceptionHandler`), reporté depuis le lot 1.
- **Portabilité du Core**, s'il doit un jour compiler hors Windows :
  `FMTExceptionHandlerWarning::tryfileopener` lance Notepad++ par `windows.h`, du
  comportement d'interface à isoler sous `#ifdef _WIN32` ou à rendre au wrapper ; et deux
  inclusions n'ont pas la casse du fichier (`"FMTExceptionHandlerWarning.h"` pour
  `FMTexceptionhandlerwarning.h`, dans `FMTFormCache.cpp` et
  `FMTexceptionhandlerwarning.cpp`), sans effet sous Windows.
- **`UI/tests/UnitTestFMTFormLogger.cpp`** teste une classe du Core depuis les tests du
  wrapper (suspens du lot 1) : à déplacer le jour où les tests base du Core rejoindront
  ctest.

## 5. Pièges connus

- **Encodage cp1252** -- depuis le lot 6, le wrapper (`UI/`) et les sources du Core
  (`FMTWrapperCore/Include`, `FMTWrapperCore/Source`) sont en UTF-8 sans BOM
  (`Plannification.cpp` était le dernier en cp1252). Restent en cp1252 deux tests du Core,
  `testWrapperCoreGetMaxAge.cpp` et `testWrapperCoreSplitActions.cpp`, et des fichiers
  ailleurs dans FMT (ex. `Source/FMTReplanningTask.cpp`, `Examples/C++/planningtest.cpp`) :
  les éditer avec un outil qui suppose l'UTF-8 corrompt les accents ; les réécrire
  entièrement, ou les éditer au niveau octet.
- **La planification écrit dans le projet** -- quand `FMTPlanningTask` reçoit le fichier
  primaire, `FMTParallelWriter::getAndWrite` réécrit
  `Scenarios/<scénario>/<projet>._seq` pour chaque scénario optimisé. Planifier sur
  `Examples/Models` modifie donc des fichiers suivis : un test travaille sur une copie,
  comme `testWrapperCorePlanning`. Et `FMTParser::createOGRDataset` tente de supprimer la
  destination des sorties si elle existe déjà.
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
  le 2025-09-19 (`f71f5dd5`), et SES fonctionne dans l'interface. Les messages déplacés aux
  lots 4 et 6 sont donc en UTF-8 et se comportent désormais comme ceux de SES. **À vérifier une
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
- **`FMTWRAPPERCOREEXPORT`** sur toute classe du Core appelée depuis un autre binaire : le
  wrapper (par `Controller`) et les exécutables de test (par les entrées pures). Pas sur
  les DTO. **Ne pas** utiliser `FMTEXPORT` : il vaut `dllimport` dans `FMTWrapperCore`,
  qui définit `FMTLIBIMPORT`. **Ne pas** coder `__declspec(dllexport)` en dur : les
  clients doivent voir `dllimport`, et GCC hors Windows ne connaît pas `__declspec`.
- **Rien de FMTlib dans `Controller.h`** : une opération qui retournerait un objet FMT
  n'a pas sa place dans le contrôleur. Vu au lot 5b avec `_ObtenirSEQ`, qui retourne des
  `Core::FMTSchedule` : il est resté dans le wrapper jusqu'au lot 6, qui l'a supprimé.
- **Exporter une classe instancie ses membres implicites dans chaque unité de
  compilation qui la voit exportée** : chaque client jusqu'au lot 5b, qui voyait
  `dllexport` en dur ; chaque source du Core depuis. Une classe exportée dont un membre est un `std::unique_ptr<T>` avec `T` seulement
  déclaré en avant ne peut plus définir constructeur ni destructeur `= default`
  dans le header : chaque unité de compilation concernée les instancie et échoue sur
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
- Chaque lot ajoute son `testWrapperCore<Domaine>.cpp` selon ce patron. Les tests
  appellent les entrées pures des services, jamais `Controller` : ils n'ont pas à peupler
  le singleton.
- Un test qui écrit dans un projet (planification) travaille sur une copie du jeu de
  données, jamais sur `Examples/Models` : voir `testWrapperCorePlanning`.
- Quand un jeu de données public existe, le test l'utilise par défaut et reprend la forme
  d'arguments de son cousin de `basetests.csv` : `testWrapperCoreRasterization` tourne sur
  `Examples/Models/TWD_land` avec les mêmes arguments que `maptoFMTforest`. Il reste
  manuel, mais rejoindra ctest en une ligne le moment venu. Les tests SES, qui n'ont que
  des modèles privés, prennent un JSON.
- Sinon, le test tourne sur un modèle privé et reprend la forme d'arguments de son cousin
  de `BFECtests.csv` : `testWrapperCoreOperatingArea` a les mêmes arguments que
  `testOAschedulertask`, aucun modèle public n'ayant de yield `YOUVERT`.
