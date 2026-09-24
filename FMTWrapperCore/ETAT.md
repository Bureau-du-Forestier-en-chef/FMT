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
   *Seule exception* : les DTO `FMTWrapper::Backend::*Parameters` / `*Results`, qui ne
   contiennent que des types `std`. C'est le contrat de traduction : le wrapper doit
   le voir.
3. **Le wrapper ne parle au Core que par `Controller`** : il n'inclut que `Controller.h`
   et n'y nomme que `FMTWrapper::Backend::Controller`, les DTO et les types
   d'événements (lots 5b et C).

### Patron par domaine

| Fichier | Contenu |
|---|---|
| `FMTWrapperCore/Include/<Domaine>Types.h` | les DTO, types `std` uniquement : `struct <X>Parameters`, `struct <X>Results` (`success`, `errorMessage`, données) |
| `FMTWrapperCore/Include/<Domaine>.h` | le service : forward-decl FMT, `class FMT_WRAPPER_CORE_EXPORT <X>` et ses entrées pures |
| `FMTWrapperCore/Source/<Domaine>.cpp` | la logique ; erreurs via `raiseFromCatch` du gestionnaire d'exceptions |
| `FMTWrapperCore/{Include,Source}/<Domaine>UseCases.*` | le cas d'utilisation : reçoit DTO et index de scénario, résout l'index dans `ModelCache`, appelle les services, met le cache à jour |
| `Controller.h` / `Controller.cpp` | une méthode par opération système de `FMTForm` : délègue au cas d'utilisation, sans logique ni type FMT |
| `UI/Source/<Fichier>.cpp` | `ConvertirParametres` (C# -> `Parameters`), l'appel du contrôleur, `_EnvoyerResultats...` (`Results` -> `RetourJson`/`FeedBack`) |

Référence vivante : `OperatingAreaTypes.h`, `OperatingArea.h` / `.cpp`,
`SpatialUseCases::scheduleOperatingAreas`, `Controller::scheduleOperatingAreas` et
`UI/Source/OperatingAreaScheduling.cpp`.

`<X>Results` n'existe que si l'opération produit des données en mémoire. Sinon l'entrée
retourne `void` et les erreurs remontent par exception : c'est le cas de `Rasterization`
et des transformations.

### Contrôleur façade et cas d'utilisation

`FMTWrapper::Backend::Controller` (lot 5b) applique le patron *Controller* de GRASP (Larman) :
un objet hors de l'interface reçoit les opérations système et les délègue. Depuis le lot B, il
ne fait que cela.

- **Le contrôleur** ne reçoit et ne rend que des types `std`, des DTO et des index de
  scénario. Chacune de ses 38 méthodes tient en une délégation : aucune logique, aucune
  structure de contrôle, et aucun en-tête de FMT n'atteint son implémentation.
- **Les cas d'utilisation** (`<Domaine>UseCases`) portent la coordination : résolution des
  index dans `ModelCache`, appel des services, mise à jour du cache. Internes à la
  bibliothèque, ils ne sont pas exportés.
- **Les services n'ont que des entrées pures** : elles prennent les objets FMT et ne
  connaissent pas le cache. Les tests C++ les appellent directement.
- **`Controller.h` n'inclut que les `<Domaine>Types.h`** : le wrapper ne voit ni les
  services ni la moindre déclaration de FMTlib.
- Dans le Core, `ModelCache` n'est utilisé que par les cas d'utilisation et par lui-même.
- `Environment` ne dépend d'aucun scénario et son interface est déjà en types `std` : le
  contrôleur l'appelle sans passer par un cas d'utilisation.
- Le contrôleur est sans état et ses méthodes sont statiques : l'état de la session
  (scénarios, logger, gestionnaire d'exceptions) reste dans `ModelCache`.

```cpp
// Service : entrée pure, testable sans cache.
static OperatingAreaResults schedule(const OperatingAreaParameters& p_params, const Models::FMTModel& p_model);

// Cas d'utilisation : résout l'index, appelle le service.
OperatingAreaResults SpatialUseCases::scheduleOperatingAreas(const OperatingAreaParameters& p_params, int p_modelIndex)
{
    return OperatingArea::schedule(p_params, ModelCache::GetInstance()->getModel(p_modelIndex));
}

// Contrôleur : délègue.
OperatingAreaResults Controller::scheduleOperatingAreas(const OperatingAreaParameters& p_params, int p_modelIndex)
{
    return SpatialUseCases::scheduleOperatingAreas(p_params, p_modelIndex);
}
```

### Règles transversales

- Les signatures publiques de `FMTForm` ne changent pas : le UI .NET externe en dépend.
- Journalisation : les entrées pures écrivent dans le logger statique de FMT
  (`Models::FMTModel::getLogger()`) : le `CallbackLogger` dans l'interface, le logger par
  défaut dans un test. La progression reste en temps réel.
- Toute classe du Core appelée depuis un autre binaire porte `FMT_WRAPPER_CORE_EXPORT`
  (`FMTWrapperCoreExport.h`) ; les DTO, sans fonction membre hors ligne, et les cas
  d'utilisation, internes à la bibliothèque, n'en ont pas besoin.
- Le code du Core vit dans l'espace de noms `FMTWrapper::Backend` (lot A) ; la bibliothèque,
  sa cible CMake et son dossier gardent le nom `FMTWrapperCore`.
- Ce que le Core rapporte à une interface passe par des événements typés (`Events.h`) et un
  point de publication unique (`EventPublisher`) : aucun rappel non typé ne traverse la
  frontière (lot C).
- Langue : commentaires, documentation Doxygen et sorties des tests en **anglais** ;
  messages affichés aux utilisateurs (journal, refus, textes d'exception) et ce fichier en
  **français**.
- Les conversions entrantes .NET -> `std` du wrapper passent toutes par
  `UI/Include/Conversions.h` (`Wrapper::Conversions`), qui rend une valeur vide pour
  `nullptr`. Les conversions sortantes gardent chacune leur décodage (voir lot 2).
- Chaque lot laisse l'ensemble compilable et ce fichier à jour.

## 2. Inventaire

### Infrastructure

| Classe | Emplacement | Statut |
|---|---|---|
| `ModelCache` | `FMTWrapperCore/{Include,Source}` | migré (lot 1) ; ex-`FMTFormCache`, renommé au nettoyage |
| `CallbackLogger` | `FMTWrapperCore/{Include,Source}` | migré (lot 1) ; ex-`FMTFormLogger`, renommé au nettoyage |
| `WarningExceptionHandler` | `FMTWrapperCore/{Include,Source}` | migré (lot 1) ; ex-`FMTExceptionHandlerWarning`, renommé au nettoyage |

### Classes du Core

| Classe | Rôle |
|---|---|
| `Controller` | Contrôleur façade : le seul point d'entrée du wrapper, une méthode par opération de `FMTForm`, chacune une délégation |
| `EventPublisher` | Point de publication unique des événements : abonnement, désabonnement, diffusion (lot C) |
| `SessionUseCases`, `ScenarioUseCases`, `QueryUseCases`, `TransformationUseCases`, `SpatialUseCases`, `PlanningUseCases` | Cas d'utilisation (lot B) : résolution des index de scénario, coordination des services, mise à jour du cache |
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
`FMTWrapperCoreExport.h` définit `FMT_WRAPPER_CORE_EXPORT`. Les types d'événements sont
dans `Events.h` (lot C).

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
| `FMTForm.cpp` | 206 | **migré** | `ModelCache`, `Environment` | -- (le délégué et l'`IntPtr` restent : managés par nature) |

### Mesure de la règle 2

À refaire à la fin de chaque lot. Elle compte les types de FMTlib visibles dans le
wrapper lui-même (`UI/Include`, `UI/Source`) ; les tests de `UI/tests` sont comptés à part.

```
grep -rhoE "\b(Core|Models|Parser|Parallel|Spatial|Heuristics|Exception|Logging)::FMT" UI/Include UI/Source | wc -l
```

Le `\b` est indispensable : sans lui, `Core::FMT` correspondait aussi à l'intérieur de
`FMTWrapperCore::FMTFormCache` (aujourd'hui `ModelCache`), une classe du Core qui avait
toute sa place dans le wrapper. **Les valeurs publiées aux lots 1 et 2 (241 et 145) étaient faussées de cette
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

`UI/tests` : n'existe plus depuis le 2026-09-16. Ses 23 occurrences étaient dans
`UnitTestCallbackLogger.cpp` (ex-`UnitTestFMTFormLogger.cpp`), passé dans
`FMTWrapperCore/tests/` (suspens du lot 1).

**Objectif atteint au lot 6** : plus un seul type de FMTlib dans le wrapper.

### Mesure de la règle 3

Les noms du Core que le wrapper utilise, hors `Controller` et DTO :

```
grep -raoE "FMTWrapper::Backend::\w+" UI/Include UI/Source | grep -vE "::(Controller|\w+Parameters|\w+Results|\w*Event|EventHandler|SubscriptionId)$" | sort | uniq -c
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
  *Rectifié le 2026-09-16* : les tests du Core rejoignent ctest par les CSV de tests
  (section 6), le déplacement ne le sortirait donc pas de ctest.
  **Résolu le 2026-09-16** : déplacé dans `FMTWrapperCore/tests/` et inscrit dans
  `basetests.csv`.

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

**Statut** : livré le 2026-09-14, compilé avant le lot 6. Refactor sans changement de
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

**Statut** : livré le 2026-09-15, compilé avant le nettoyage. Dernier lot de domaine : les
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

### Nettoyage -- renommages, conversions et commentaires en anglais (2026-09-15)

**Statut** : livré et compilé le 2026-09-15. **Validé en partie** le 2026-09-16 : ctest
passe (166 tests sur 166, dont `UnitTestCallbackLogger` et les tests du Core inscrits dans
les CSV) et l'interface fonctionne à première vue ; le reste est en section 4. Lancé par
Gabriel après la compilation du lot 6.

- **Classes d'infrastructure renommées**, fichiers compris : `FMTFormCache` -> `ModelCache`,
  `FMTFormLogger` -> `CallbackLogger`, `FMTExceptionHandlerWarning` ->
  `WarningExceptionHandler`, ainsi que les accesseurs `GetFormLogger` -> `GetCallbackLogger`
  et `GetFormHandler` -> `GetWarningHandler`. Le test `UnitTestFMTFormLogger` devient
  `UnitTestCallbackLogger`, nom ctest compris. Les inclusions à la mauvaise casse de
  `FMTexceptionhandlerwarning.h` disparaissent avec le renommage. Le journal ci-dessus garde
  les anciens noms.
- **Conversions du wrapper réunies** dans `UI/Include/Conversions.h`
  (`Wrapper::Conversions`) : `toStdString`, `toStdVector`, `toManagedList` et `fromUtf8`
  remplacent les copies locales de `Transformations.cpp`, `InitialAreaVariability.cpp`,
  `Plannification.cpp` et `FMTFormOutils.cpp`, ainsi que le membre
  `FMTForm::_convertToSystemString`. Seule différence : un élément `nullptr` dans une liste
  devient une chaîne vide dans `Transformations.cpp` et `Plannification.cpp`, où
  `marshal_as` levait.
- **Commentaires, documentation et sorties des tests en anglais** (décision de Gabriel),
  dans tout `FMTWrapperCore/` et le wrapper ; les messages affichés aux utilisateurs et les
  textes d'exception restent en français. 279 blocs traduits un à un, puis, pour chacun des
  50 fichiers touchés, vérification que le code sans commentaires est identique. Au passage :
  - les deux tests encore en cp1252 (`testWrapperCoreGetMaxAge.cpp`,
    `testWrapperCoreSplitActions.cpp`) sont devenus ASCII, et les accents corrompus
    (U+FFFD) des commentaires de plusieurs tests ont disparu avec eux ;
  - la docstring de `FMTForm::ObtenirNomSolveur`, qui annonçait la liste des solveurs, est
    corrigée.
- Pour git, `FMTFormLogger.cpp` -> `CallbackLogger.cpp` n'est semblable qu'à 40 % après la
  traduction : il apparaît comme une suppression et un ajout, et `git log --follow` ne
  remonte au-delà du nettoyage qu'avec `-M40%`. Les six autres renommages sont reconnus.

### Tests dans ctest et conversions entrantes (2026-09-16)

**Statut** : livré le 2026-09-16, compilé et validé le 2026-09-18 : les sept tests
nouvellement inscrits passent dans ctest, `testWrapperCoreGetYield` compris, dans ses deux
lignes.

- **Tests de la migration dans ctest**, par les CSV (section 6) :
  - `basetests.csv` : `testWrapperCoreRasterization` (arguments de `maptoFMTforest`),
    `testWrapperCoreAreaVariability`, `testWrapperCorePlanning`,
    `testWrapperCoreEnvironment` et `UnitTestCallbackLogger` ;
  - `BFECtests.csv`, local : `testWrapperCoreOperatingArea`, avec les arguments de la
    première ligne de `testOAschedulertask`. La ligne morte `UnitTestFMTFormLogger` en est
    retirée ;
  - `testWrapperCoreSA` reste hors ctest : son repli est un gabarit `TODO`, et aucun JSON
    de recuit simulé n'existe sous `Modeles_test`.
- **`testWrapperCoreGetYield` réparé**, jamais exécuté par ctest jusqu'ici :
  - ses deux lignes nomment enfin la cible. Celle de `basetests.csv` passe à la forme
    d'arguments en vigueur depuis 2025 : `TEST1` du scénario `equation`, la constante
    `EXP(1.970 - 0.405 * LN(0.3)) + 17.71`, soit 29,3868586 ;
  - la valeur attendue était lue par `std::stoi` : « 54.53 » devenait 54. Elle est lue par
    `std::stod` et comparée avec une tolérance relative de 1e-6, un yield d'équation étant
    calculé.
- **`UnitTestCallbackLogger.cpp` déplacé** de `UI/tests/` vers `FMTWrapperCore/tests/`, à
  l'identique (suspens du lot 1) : il n'inclut rien du wrapper. `UI/tests/`, vide, est
  supprimé. La chaîne MSYS2 compile désormais ce test aussi.
- Les en-têtes des tests inscrits ne les disent plus « manuels ».
- **Conversions entrantes alignées** : les 46 appels directs à
  `marshal_as<std::string>` (8 fichiers) passent par `Conversions::toStdString`, et les
  boucles de listes de `SimulationSpatialeExplicite.cpp` et
  `OptimisationSpatialeExplicite.cpp` par `Conversions::toStdVector`. Le seul `marshal_as`
  restant est la conversion sortante de `getActionsNames`. Contrôle : en défaisant les
  remplacements, chaque fichier redevient identique à l'original.

Différence observable, sur `nullptr` seulement : l'interface ne lève plus
d'`ArgumentNullException`. Le Core reçoit une chaîne vide, comme pour une zone de texte
vide, et une liste `nullptr` de SES ou du recuit simulé devient une liste vide. Le Core ne
refuse pas toutes les valeurs vides : d'après le code, un dossier de résultat vide pour les
aires d'opération écrit sous `\Retour`, à la racine du lecteur, et un nom de scénario vide
pour les transformations écrit directement dans `Scenarios/`. Ces cas existaient déjà pour
une chaîne vide (voir « Ensuite »).

### Macro d'export renommée (2026-09-17)

**Statut** : livré le 2026-09-17, compilé le 2026-09-18. Demandé par Gabriel.

- `FMTWRAPPERCOREEXPORT` devient `FMT_WRAPPER_CORE_EXPORT` : sa définition dans
  `FMTWrapperCoreExport.h` et les 13 classes du Core qui le portent. Le journal ci-dessus
  garde l'ancien nom.
- Inchangés : `FMTWrapperCore_EXPORTS`, que CMake définit lui-même pour la cible SHARED, et
  la garde d'inclusion `FMTWRAPPERCORE_EXPORT_HEADER`, au format de celles des autres
  en-têtes du Core.
- Vérifié avec `cl` de VS 2022 (`/W4 /permissive-`), sans le build : `Controller.h` compile
  sans avertissement, et la macro s'étend en `dllexport` avec `FMTWrapperCore_EXPORTS`, en
  `dllimport` sans.

### Lot A -- espace de noms `FMTWrapper::Backend` (2026-09-17)

**Statut** : livré le 2026-09-17, compilé le 2026-09-18. Premier lot de la revue de la
PR #345 (section 4).

- `namespace FMTWrapperCore` devient `namespace FMTWrapper::Backend` (29 déclarations) et
  `FMTWrapperCore::` devient `FMTWrapper::Backend::` (135 noms qualifiés, commentaires et noms
  de méthode des messages d'erreur de `TransformationCore.cpp` compris), dans 56 fichiers du
  Core, de ses tests et du wrapper. Le journal ci-dessus garde l'ancien nom.
- Inchangés : la bibliothèque, sa cible CMake et son dossier `FMTWrapperCore`,
  `FMTWrapperCoreExport.h`, `FMTWrapperCore_EXPORTS` et les gardes d'inclusion.
- `Core` était exclu : voir Pièges, « Pas d'espace de noms `Core` dans le Core ».
- Contrôles : en remettant l'ancien nom, le diff s'annule ligne à ligne. Compilé sans édition
  de liens, dans le scratchpad, avec les options du build (`cl` de VS 2022, `/W1`) : les 13
  sources du Core, ses 15 tests et les 11 sources du wrapper en `/clr` passent sans erreur ni
  avertissement, et les objets portent le nouveau nom.

### Lot B -- contrôleur sans logique (2026-09-17)

**Statut** : livré le 2026-09-17. **Compilé, et la suite ctest passe** le 2026-09-18 :
173 tests sur 173. Deuxième lot de la revue de la PR #345 (section 4).

- **Six cas d'utilisation créés** : `SessionUseCases` (journal, gestionnaire d'exceptions,
  pile d'erreur), `ScenarioUseCases` (lecture, retrait et écriture des scénarios),
  `QueryUseCases` (les douze interrogations), `TransformationUseCases`, `SpatialUseCases`
  (SES, recuit simulé, rastérisation, aires d'opération) et `PlanningUseCases` (planification,
  replanification, variabilité de l'aire initiale).
- **`Controller.cpp` ne contient plus que des délégations** : chacune de ses 38 méthodes tient
  en une instruction. Il n'inclut plus que `Controller.h`, `Environment.h` et les six cas
  d'utilisation, tous en types `std` ; plus un seul type de FMT n'y apparaît.
- **`DEFAULT_MAX_WARNINGS` quitte l'espace anonyme du `.cpp`** : il est déclaré dans
  `SessionTypes.h`, que `Controller.h` inclut, et la règle « zéro ou moins = valeur par
  défaut » est appliquée par `SessionUseCases`.
- Le refus du cache vide disparaît du contrôleur : `ModelCache::getModel` lève déjà
  `FMTrangeerror` pour tout index hors limites, cache vide compris. Seul le message change,
  « no scenario for index N in cache », levé depuis `ModelCache::getModel`.
- Les nouveaux fichiers portent l'en-tête de licence LiLiQ-R exigé par `CodingStandards.md`,
  que les fichiers existants du Core n'ont pas (voir « Ensuite »).
- Signatures publiques inchangées : le wrapper n'est pas touché.
- Contrôles : contrôleur sans type FMT ni en-tête qui en expose, 38 méthodes d'une seule
  instruction et sans structure de contrôle, cas d'utilisation non exportés, 38 méthodes
  appelées par le wrapper. Compilé sans édition de liens avec les options du build (`cl` de
  VS 2022, `/W1`) : 19 sources du Core, 15 tests et 11 sources du wrapper en `/clr`, sans
  erreur ni avertissement.
- `Architecture.md` : la couche « Application controllers and services » décrit maintenant le
  contrôleur, les cas d'utilisation et les services ; la dette « cache dans la couche
  interface » est retirée, le cache vivant dans le Core depuis le lot 1.
- **Hors migration**, deux corrections de FMTlib livrées avec ce lot, parce qu'elles
  bloquaient le build et viennent de master (`4c61d509`) : une déclaration de `inputs` en
  double dans `FMTYieldModelNn.cpp`, et `std::discrete_distribution<double>` dans
  `FMTYieldModelRandom.cpp`, que MSVC 14.44 refuse désormais, le paramètre de patron étant
  celui de l'indice tiré et devant être entier. Aucun avertissement, de `/W1` à `/W4`.

### Lot C -- événements typés (2026-09-18)

**Statut** : livré le 2026-09-18, **pas encore compilé par Gabriel**. Dernier lot de la revue
de la PR #345 (section 4).

- **Le `void*` disparaît.** `Controller::initializeLogger` ne prend plus que le chemin du
  journal, `recoverLoggerAndHandler` ne prend plus rien, et deux opérations s'ajoutent :
  `subscribe`, qui rend un identifiant, et `unsubscribe`. Le `typedef logfunc` en `__stdcall`,
  le délégué managé et l'`IntPtr` du wrapper sont supprimés.
- **Modèle d'événements** (`Events.h`) : `LogEvent`, un message, et `ErrorEvent`, une pile
  d'erreur formatée, réunis dans un `std::variant`, avec
  `EventHandler = std::function<void(const Event&)>`. Rapporter autre chose, c'est ajouter une
  structure ; un abonné écrit avec `std::get_if` ignore ce qu'il ne connaît pas.
- **Point de publication unique** (`EventPublisher`) : la liste d'abonnés est remplacée plutôt
  que modifiée, si bien que publier ne copie qu'un `shared_ptr`, sans allocation sur le chemin
  qui porte chaque ligne de journal. Les abonnés sont appelés hors du verrou, sur le thread qui
  publie. Le cache possède le publieur et le passe au `CallbackLogger`.
- **Qui publie quoi** : `CallbackLogger::_cout` publie un `LogEvent` là où il appelait le
  pointeur de fonction ; `SessionUseCases::logCurrentException` publie un `ErrorEvent` après
  avoir journalisé la pile.
- **Côté wrapper**, `makeEventHandler` construit l'abonné dans une fonction libre, avec
  `gcroot<FMTForm^>` (Pièges, « Lambda dans une classe managée »). `_raiseFromCatch` n'envoie
  plus lui-même les deux lignes d'erreur : elles viennent de l'`ErrorEvent`, par
  `_toErrorFeedback`. L'abonnement est pris une seule fois, à la première assignation du
  journal.
- **Sortie inchangée pour l'interface .NET** : mêmes lignes, dans le même ordre, toujours
  décodées dans la page ANSI. `RecoverFromCrash` ne recrée plus de délégué, la liste d'abonnés
  survivant à l'incident.
- `unsubscribe` n'est appelé nulle part dans le wrapper -- le formulaire vit aussi longtemps
  que l'interface -- mais `UnitTestCallbackLogger` s'en sert, et une autre interface en aura
  besoin.
- Contrôles : plus un seul `void*`, `logfunc` ou délégué ; 40 méthodes du contrôleur, chacune
  une délégation ; règles 2 et 3 à zéro, les types d'événements rejoignant les DTO dans le
  contrat. Compilé sans édition de liens (`cl`, `/W1`) : 20 sources du Core, 15 tests et 11
  sources du wrapper en `/clr`, sans erreur ni avertissement. À `/W3`, `EventPublisher` ajoute
  un C4251, comme les 265 que produisent déjà les classes exportées de FMTlib et du Core.

### Correctif -- le refus de la simulation spatiale (2026-09-22)

**Statut** : livré le 2026-09-22, **pas encore compilé par Gabriel**.

`SES::RunSES` a deux sorties anticipées, quand la cédule est vide et quand elle ne couvre pas
les périodes demandées. Elles posaient `success = false` et remplissaient `errorMessage` sans
rien journaliser ni lever d'exception ; la surcharge indexée annonçait ensuite « Simulation
terminée avec succès » sans condition, et `FMTForm::SimulationSpatialeExplicite` rendait
`true` sans lire le drapeau. Trois occasions de voir le refus, trois manquées : l'interface
annonçait une simulation réussie alors que rien n'avait été simulé.

Démontré sur `sse_pass`, dont la cédule s'arrête à la période 10 : demander 5 périodes donne
27 lignes de journal, le rapport des contraintes et 18 fichiers ; en demander 20 donne 5
lignes, aucun fichier, aucun message, et le code de retour zéro. Le défaut datait de
`98c2bdd8`, la première migration de SES, avant les lots A, B et C.

- `SpatialUseCases::runSpatialSimulation` publie `errorMessage` comme `ErrorEvent`. Le refus
  emprunte donc le chemin du lot C et sort par `_toErrorFeedback`, sur deux lignes, comme
  toute autre erreur. Publier ici plutôt que dans `SES` garde le service libre du cache : les
  tests C++ appellent toujours l'entrée pure sans peupler le singleton.
- `SES::RunSES`, surcharge indexée, rend le résultat tout de suite quand il porte un refus,
  au lieu d'annoncer un succès.
- `FMTForm::SimulationSpatialeExplicite` rend `RESULTS.success`, comme le fait déjà
  `OptimisationSpatialeExplicite`. Sa documentation Doxygen promettait déjà « True if the
  simulation completed successfully » : le code la rejoint.
- Les sept modules ont été revus : les quatre dont le Core rend un DTO lisent maintenant son
  drapeau. Des trois dont l'entrée est `void`, la rastérisation et les transformations
  gardent leur `return true` à bon droit, leurs échecs passant par `raiseFromCatch`. **La
  planification, non** : un scénario irréalisable n'est ni une exception ni un drapeau, et
  reste ouvert -- voir « Validations en attente ».
- Compilé sans édition de liens (`cl`, `/W1`) : 20 sources du Core, 15 tests et 11 sources du
  wrapper en `/clr`, sans erreur ni avertissement. Contrôles d'architecture au vert.
  **La nouvelle publication n'est pas observable sans édition de liens** : `testWrapperCoreSES`
  appelle l'entrée pure, pas celle qui passe par le cache. C'est l'aller-retour dans
  l'interface qui la validera, en demandant plus de périodes que la cédule de `sse_pass` n'en
  couvre.

## 4. Prochain lot

Les lots de domaine sont terminés depuis le lot 6. La revue de la PR #345 ouvre trois lots :
A (livré), puis B et C, dans cet ordre.

### Revue de la PR #345 : décisions de Gabriel (2026-09-17)

| Commentaire de gcyr | Décision | Lot |
|---|---|---|
| Nom de la macro d'export | `FMT_WRAPPER_CORE_EXPORT` | fait (`8fb920c1`) |
| `FMTException`, `FMTModel` et `FMTModelParser` dans `Controller.cpp` | **Non négociable** : le contrôleur ne contient aucune logique, ne connaît rien de FMT et n'accepte ou ne renvoie que des DTO | B, livré |
| `DEFAULT_MAX_WARNINGS` dans le `.h` | Accepté | B, livré |
| Espace de noms `FMTWrapper::Core` | `FMTWrapper::Backend` : `Core` entre en conflit avec FMTlib (Pièges) | A, livré |
| Callbacks et `void*` remplacés par des événements | Accepté : événements typés et liste d'abonnés | C, livré |

**Lot B -- contrôleur sans logique.** Livré le 2026-09-17 : voir le journal, section 3.

**Lot C -- événements.** Livré le 2026-09-18 : voir le journal, section 3.

### Banc de scénarios pour l'interface (2026-09-21)

`Examples/Models/TWD_land/Scenarios` porte un scénario `_pass` et un `_fail` par module de
l'interface. **La marche à suivre vit avec le modèle**, dans
`Examples/Models/TWD_land/BANC_INTERFACE.md` : rôle de chaque scénario, réglages à saisir,
message attendu, et ce que les essais réécrivent dans le projet. Cette section-ci garde ce qui
regarde la migration -- l'état des validations et les défauts relevés. Le modèle est minuscule -- huit peuplements, trois thèmes -- donc chaque essai se
compte en secondes, et les scénarios reprennent ceux qui existaient déjà (`COS`, `LP`,
`Spatial`, les trois de la replanification) quand ils conviennent.

| Module | `_pass` | `_fail` | Ce que le `_fail` doit produire |
|---|---|---|---|
| Calendrier de COS | `cos_pass` | `cos_fail` | « Aucune action dans le modèle n'a de yield youvert » ; la fonction rend faux, sans exception |
| Planification | `planification_pass` | `planification_fail` | `FMTexc(61)` « Une contrainte rend le problème d'optimisation infaisable. OSUPPL >= 1 at period 1 » |
| Planification, cas muet | -- | `planification_fail_solveur` | « infeasible scenario » au journal, puis un succès : le défaut ouvert, voir « Validations en attente » |
| Replanification | `replanification_strategique` + `replanification_stochastique` + `replanification_tactique` | `replanification_tactique_fail`, à la place du tactique | `FMTexc(77)` « infeasible model named replanification_tactique_fail on replicate N », un avertissement |
| Rastérisation | `rasterisation_pass` | `rasterisation_fail` | « A referenced attribute is missing or undefined. UNITE3 at theme 1 at line 5 » |
| Transformation | `transformation_pass` | `transformation_fail` | pile pointant `transformation_fail/TWD_land._seq` ligne 6, et Notepad++ s'ouvre dessus |
| Simulation spatialement explicite | `sse_pass` | `spatial_fail_rasters` | `FMTexc(19)` « A referenced attribute is missing or undefined. UNITE3 at theme 1 », levée à la lecture des rasters |
| Optimisation spatialement explicite | `ose_pass` | `spatial_fail_rasters` | la même : le scénario sert aux deux modules |
| Spatial, cas infaisables | -- | `sse_infaisable`, `ose_infaisable` | **pas d'erreur** : le traitement réussit, seul le rapport des contraintes change -- `OVOLREC >= 1000000` brisée, contre 0 % pour les `_pass` |
| Chargement du scénario | -- | `chargement_fail` | pile pointant `chargement_fail/TWD_land._opt` ligne 8, et Notepad++ s'ouvre dessus |

Chaque module a un scénario qui fait remonter une **erreur** dans l'interface : c'est ce qui
prouve que le chemin d'erreur du lot C fonctionne, quel que soit le message. Les scénarios
`_infaisable`, eux, ne produisent qu'un rapport, le Core n'ayant pas de quoi refuser --
les garder en tête pour ne pas conclure trop vite que le journal est muet.

`transformation_fail` et `chargement_fail` valident le lot C de bout en bout : ils produisent
une pile de la forme `In <fichier> at line <n> FMTsection`, la seule que
`WarningExceptionHandler::tryfileopener` sait ouvrir, donc Notepad++ doit s'ouvrir dessus.

**Ce qu'il faut saisir**

- **Rastérisation et COS** : carte `Carte/TWD_land.shp`, champ d'âge `AGE`, champ de
  superficie `SUPERFICIE`.
  **Vider le champ Stanlock**, que l'interface remplit d'office avec `STANLOCK` : aucune des
  deux cartes de TWD_land ne porte ce champ, et `FMTParser::_getWSFields` lève
  `FMTexc(28) A required field is missing from the dataset` dès que le nom est fourni et
  introuvable. Vide, il est simplement ignoré.
  La résolution se saisit en **hectares par pixel**, pas en mètres : le journal de l'interface
  montre `0.04` devenir « Résolution FMT : 20 ». Pour retrouver les 1420 mètres du test
  automatisé, saisir `201.64`.
  **Le répertoire des rasters proposé par défaut est `TWD_land/rasters`**, celui-là même où
  `sse_pass` et `ose_pass` vont lire. Écrire ailleurs, ou remettre le banc en état avec
  `git checkout -- Examples/Models/TWD_land/rasters`.
- **COS** : fichier de paramètres `Scenarios/cos_pass/parametres_cos.csv` (une copie est dans
  `cos_fail`), qui découpe les aires d'opération sur le thème 3, celui des UTR. Le Core passe
  le numéro de thème tel quel à `readOaSchedulerParameters`, qui compte à partir de zéro : si
  les aires ne sont pas reconnues, essayer 2 plutôt que 3.
  L'en-tête porte les huit colonnes, `OA;OPT;RET;MAXRET;REP;OPR;NPE;GUP` : le Core n'exige que
  les cinq premières, mais l'interface refuse le fichier sans `NPE` ni `GUP`. Les deux sont à
  zéro exprès -- au-dessus, `getOperatingArea` lit les blocs voisins dans le fichier de formes,
  ce que ce modèle n'a jamais exercé.
- **Simulation et optimisation spatiales** : dossier de rasters `rasters/`, 5 périodes, sortie
  `OVOLREC`, niveau total, périodes 1 à 5, pilote `CSV`. Pour l'optimisation, les valeurs de
  `sasolve` : 500 000 mouvements, 3 000 acceptés, 5 000 par cycle.
  `spatial_fail_rasters` sert aux deux modules et se lance avec les mêmes réglages que le
  `_pass` correspondant : son modèle nomme `UNITE4` un attribut que les rasters, la carte et
  l'inventaire appellent `UNITE3`, et `readRasters` lève.
  **Aucun réglage, en revanche, ne fera « échouer » `ose_infaisable`** : `SES::RunOptimization`
  n'a pas de sortie de refus, elle lève ou elle pose `success = true`. Ce qui le distingue du
  `_pass` est le rapport des contraintes, à lire dans le journal :
  « OVOLREC >= 1000000.000000 1.._LENGTH(...) » et « Percentage of infeasible constraints
  50 % », contre « 0 % » pour `ose_pass`. Vérifié aux deux horizons, 5 et 10 périodes.
  Pour voir le refus de la
  simulation, demander 20 périodes sur `sse_pass`, dont la cédule s'arrête à la période 10 :
  depuis le correctif du 2026-09-22, le refus doit sortir sur deux lignes et la fonction doit
  rendre faux.
- **Planification** : 5 périodes, sortie `OVOLREC` -- c'est le nom que porte ce modèle, et
  `Selection::selectOutputs` rend une liste vide sans rien dire pour un nom qu'il ne trouve
  pas. `planification_pass` a une cédule, donc il se planifie et se rejoue. Les deux scénarios
  d'échec n'en ont pas et **doivent être lancés sans rejouer la cédule** : avec « rejouer »
  coché, le modèle est construit mais jamais résolu, donc jamais déclaré irréalisable, parce
  que `FMTPlanningTask::work` ne teste la faisabilité que lorsqu'il résout.
  Les deux couvrent les deux régimes d'infaisabilité, à tous les horizons de 1 à 10 :
  `planification_fail` porte une contrainte que FMT voit impossible **en posant la matrice**
  -- `oSuppl` n'a aucune variable en période 1, la plantation n'étant opérable qu'entre 1 et
  3 ans alors que l'inventaire commence à 7 ans -- et lève une `FMTError` fatale ;
  `planification_fail_solveur` porte une contrainte que **seul le solveur** peut rejeter, et
  que FMT se contente de journaliser.
- **Replanification** : `replanification_strategique` en stratégique, `replanification_stochastique` en
  stochastique, `replanification_tactique` en tactique ; 10 périodes, 3 de replanification,
  variabilité 0,5, 2 réplicats au minimum comme au maximum, sortie `OVOLREC`, niveau TOTALE,
  CSV, « produire la solution » cochée. Ce sont les valeurs que `testWrapperCorePlanning`
  passe à `Planning::replan`, et il passe. Les deux solveurs conviennent : MOSEK a été
  vérifié avec `replanner`, CLP par les deux tests automatisés.
  **Si la replanification échoue**, chercher « `FMTexc(58)` ... No outputs to write » dans le
  journal : la liste des sorties retenues est vide, et elle est cherchée dans les sorties du
  modèle **stratégique**, pas dans celles du tactique.
- **Transformation** : dans « Agrégats et actions », cocher **les trois agrégats et eux
  seuls** -- `ARECOLTE` (COUPETOTALE et ARECUP), `ASYLVICULTURE` (PLANTATION) et
  `APERTURBATION` (AFIRE et ACARIBOU).
  La liste mêle les agrégats et les actions, et ce qu'on y coche doit **partitionner** les
  actions du modèle : chacune exactement une fois, nommée directement ou par un agrégat. Trop
  peu, et l'agrégation refuse avec « Missing aggregate for actions
  AFIRE,ARECUP,COUPETOTALE,ACARIBOU,PLANTATION » -- le message énumère toutes les actions, pas
  seulement les manquantes. Trop, et elle refuse avec « Action AFIRE already in aggregate » :
  `FMTActionComparator::getAllAggregates` reconnaît un nom aussi bien comme action que comme
  agrégat, donc cocher `AFIRE` et `APERTURBATION` compte AFIRE deux fois.
  Une partition mixte fonctionne : `ARECOLTE`, `AFIRE`, `ACARIBOU`, `PLANTATION` passe.
  `_DEATH` est sans effet, l'agrégation l'ajoutant d'elle-même. **Ne pas cocher que des
  actions, sans aucun agrégat** : les cinq actions seules font mourir le traitement sans
  message, ce qui n'est pas diagnostiqué.
  L'ordre des actions, lui, n'a aucun effet : `Transformation::aggregateAllActions` reçoit
  `p_order` et ne s'en sert pas, l'ordre remis à FMT étant celui que
  `getSchedulesPriorities` calcule depuis les cédules. Antérieur à la migration --
  `56c381ff`, gcyr, 2024-10-25 --, à décider plus tard : retirer le paramètre, ou honorer ce
  que l'utilisateur choisit.
- **Partout** : garder la liste d'erreurs ignorées par défaut, celle que rend
  `Controller::getErrorsToIgnore`. Sans elle, des avertissements du modèle de base
  (`_DEATH` non défini, yield redéfini) empêchent les scénarios de se lire.

**Ce que j'ai vérifié** avec les exécutables du build du 2026-09-17, donc sans le lot C :

- `planification_pass` se résout (65 733), `planification_fail` non : `doPlanning` rend faux.
- `rasterisation_pass` passe `testWrapperCoreRasterization` (8 peuplements relus des rasters) ;
  `rasterisation_fail` échoue sur l'attribut absent de la carte.
- `transformation_pass` passe `testWrapperCoreAggregateAllActions` et écrit son scénario ;
  `transformation_fail` s'arrête sur la cédule, à la ligne voulue.
- `sse_pass` : 0 % de contraintes brisées ; `sse_infaisable` : la contrainte impossible, 20 %.
- `ose_pass` : 0 % de contraintes brisées et une récolte réelle (46 000 à 112 000 par période) ;
  `ose_infaisable` : la contrainte impossible, 50 %.
- `chargement_fail` s'arrête à la lecture, sur la bonne ligne.
- `cos_pass` franchit le test du yield youvert, `cos_fail` s'y arrête ; le fichier de paramètres
  se lit sans erreur.
- Les trois scénarios de replanification sont des copies octet pour octet de
  `Globalreplanning`, `Globalfire` et `Localreplanning`, que `testWrapperCorePlanning` fait
  passer.

Deux cases du tableau restent vides : la génération du calendrier de COS et la réaction de la
replanification à un modèle tactique sans solution. Elles sont inscrites en « Validations en
attente », plus bas.
- `testWrapperCorePlanning` passe avec le banc en place : les nouveaux dossiers ne dérangent
  aucun test existant, `readproject` ne lisant que les scénarios qu'on lui nomme.

**Le banc est gardé par ctest** : neuf lignes de `basetests.csv` inscrivent les scénarios
`_pass`, pour qu'ils ne pourrissent pas. Huit sont des `doplanning`, qui prouvent que le
scénario se lit et se résout, et la neuvième passe `rasterisation_pass` par
`testWrapperCoreRasterization`. Les valeurs ont été mesurées deux fois chacune, avec MOSEK.
Les scénarios `_fail`, eux, ne sont pas inscriptibles : aucun harnais n'exprime « doit
échouer ».

**Pièges**

- **Les trois scénarios de replanification sont des copies** de `Globalreplanning`,
  `Globalfire` et `Localreplanning` : le banc se lit tout seul dans la liste de l'interface,
  au prix d'une dérive possible si les originaux changent. Les remplacer par leurs originaux
  ne coûte qu'une ligne du tableau.
- **La planification et les transformations écrivent dans le projet.** La planification
  réécrit la cédule des scénarios qu'elle optimise, `planification_pass` compris : un essai à
  l'horizon 1 ramène sa cédule de dix périodes à une seule, et le banc perd son cas de
  rejeu. Les transformations, elles, ajoutent un scénario et réécrivent le `.pri`. Travailler
  sur une copie de `TWD_land`, ou remettre le banc en état après coup avec
  `git checkout -- Examples/Models/TWD_land/Scenarios`.
- **Un refus du Core n'est pas une exception.** Quatre opérations rendent un DTO qui porte
  `success` et `errorMessage` au lieu de lever : la simulation et l'optimisation spatiales,
  les aires d'opération et la variabilité de l'aire initiale. Arriver au `return true` du
  wrapper ne prouve donc pas que l'opération a eu lieu : il faut lire le drapeau et faire
  remonter le message. La simulation spatiale l'a oublié pendant toute la migration -- voir
  le correctif du 2026-09-22, section 3.
- **Le fichier de paramètres des aires d'opération perd deux de ses colonnes** (issue à
  ouvrir, le code est de gcyr ; sans rapport avec la migration). `MAXRET` d'abord :
  `readOaSchedulerParameters` initialise `useRETasMAXRET` à vrai sans jamais le remettre à
  faux, si bien que `MAXRET` vaut toujours `RET` -- démontré avec `RET=5` et `MAXRET=2`,
  l'avertissement « MAXRET value for UTR1 is less than RET value » ne sort pas. Introduit par
  `f4c0fb76` (2026-07-22). Le périmètre de voisinage ensuite : `writeOaSchedulerParameters`
  écrit la colonne `NEP` (ligne 1891) alors que la lecture cherche `NPE` (ligne 2087), et le
  fichier sort par le pilote CSV, celui par défaut de `createOGRDataset`. FMT ne sait donc pas
  relire le fichier qu'il écrit -- démontré avec la même valeur `0.5` : sous `NPE`, « Lecture
  des blocs voisins. » apparaît ; sous `NEP`, rien.

### Validations en attente

**Tout ce qui est livré compile, et la suite ctest passe** : 173 tests sur 173 le 2026-09-18,
lots A et B compris, avec les sept tests du Core inscrits au lot « Tests dans ctest ». Restent
à valider :

- **Les lots 3 à 6 par leurs tests** : fait le 2026-09-18. `Rasterization`,
  `AreaVariability`, `Planning`, `OperatingArea`, `Environment` et les deux lignes de
  `GetYield` passent, ces dernières pour la première fois depuis 2024.
- **Dans l'interface**, avec le banc de scénarios ci-dessous. Au 2026-09-24, Gabriel a vu
  remonter une erreur dans la planification, la replanification, la rastérisation, la
  transformation et l'optimisation spatiale, cette dernière par `spatial_fail_rasters`. Restent :
  - **la simulation spatiale**, bloquée par la validation de son écran, qui exige
    `rasters/STANLOCK.tif` même quand l'option de verrou est décochée. Le Core, lui, ne le lit
    que si elle est cochée (`SES::prepareInitialForest`), `testWrapperCoreSES` fait tourner
    `sse_pass` sans ce fichier, et l'écran de l'optimisation spatiale démarre sur le même
    dossier de rasters alors que les deux modules appellent la même fonction. La validation de
    l'écran est donc en trop : défaut du UI .NET, hors de ce dépôt ;
  - le calendrier de COS et `chargement_fail`, jamais lancés ;
  - l'ouverture du fichier fautif dans Notepad++ et `RecoverFromCrash`, que
    `transformation_fail` et `chargement_fail` doivent déclencher ;
  - la replanification : vérifier que le niveau du journal est rétabli ensuite, même après
    une erreur ;
  - les accents des messages de progression (Pièges, « Encodage des messages »).
- **La génération du calendrier de COS**, qu'aucun test ne couvre sur un modèle public.
  `testOAschedulertask` et `testWrapperCoreOperatingArea` figent le numéro de thème -- 14 et 13,
  pour des modèles privés -- et la sortie de temps de retour `OATTEINTE7M`. Sur TWD_land, qui n'a
  que trois thèmes, les masques d'aires d'opération deviennent tous `? ? ?` et se recoupent.
  Vérifié quand même : `cos_pass` a bien une action qui lit le yield youvert et `cos_fail` non,
  et `parametres_cos.csv` se lit sans erreur. Reste à voir le calendrier lui-même, dans
  l'interface ou en rendant le numéro de thème paramétrable.
- ~~**La réaction de la replanification à un modèle tactique sans solution.**~~ Vérifiée le
  2026-09-24 avec `replanner` : elle rapporte « `FMTexc(77)` Replanning completed but some
  assumptions or constraints generated warnings. Ignoring: infeasible model named
  replanification_tactique_fail on replicate 2 at replanning period 1 », une fois par réplicat. Le
  message sort, mais c'est un **avertissement par construction** -- `FMTReplanningWarning`
  dérive de `FMTWarning` --, donc il n'est jamais levé et la replanification se termine en
  succès. Décocher la liste d'erreurs ignorées n'y change rien : cette liste rétrograde des
  erreurs en avertissements, elle ne promeut pas l'inverse. C'est vraisemblablement voulu, un
  réplicat pouvant échouer pendant que les autres aboutissent. (Mon échec initial de
  `replanner` venait d'un chemin de sortie que MSYS avait mutilé, pas des scénarios.)
- **Un scénario irréalisable ne remonte pas de la planification.** Observé dans l'interface
  le 2026-09-23 sur `planification_fail`, horizon 10, sans rejeu : le journal porte bien
  « infeasible scenario planification_fail », mais le traitement se termine en succès, parce
  que `Controller::plan` et `Planning::plan` sont `void` et que `FMTForm::Plannification` rend
  `true` sans condition. Le Core ne peut pas faire mieux aujourd'hui : seul
  `FMTPlanningTask::work` connaît le verdict, le gestionnaire clone la tâche
  (`maintask.clone()`), la scinde par fil d'exécution et ne finalise que la dernière, et rien
  ne rapporte le résultat à l'appelant. Le correctif tient dans FMTlib : `FMTPlanningTask`
  partage déjà un état entre ses clones par `m_ResultsWriter`, un `shared_ptr` recopié tel
  quel dans son constructeur de copie ; un `shared_ptr<vector<string>>` des scénarios
  irréalisables, rempli dans le `else` de `work()` et lu par un accesseur, reviendrait par
  l'objet de l'appelant. Décision en attente : correctif ici ou issue pour gcyr, c'est son
  fichier. La replanification, elle, lève bien -- « Infeasible Global model »,
  `FMTReplanningTask.cpp:126`.
- **La chaîne MSYS2** (`CMakeFMTMSYS2rcran45.sh`) : le `CMakeLists.txt` racine inclut
  `FMTWrapperCore` sans condition MSVC, donc GCC compile le Core et ses tests. Aucune
  compilation MSYS2 n'est consignée depuis le début de la migration. La recherche des
  extensions MSVC courantes (`std::exception("...")`, fonctions `_s`, `#pragma warning`) ne
  relève rien, et `windows.h` comme `WinExec` existent sous MinGW : seule une compilation
  tranchera.
  Depuis le lot « Tests dans ctest », elle compile aussi `UnitTestCallbackLogger`.

### Ensuite

- **Un jeu de données pour `testWrapperCoreSA`** : JSON d'entrée et de référence, comme
  `InputSES_1.json` et `ResultSES_1.json`, puis sa ligne dans `BFECtests.csv`.
- **Refuser les valeurs vides dans le Core** là où elles écrivent au mauvais endroit :
  dossier de résultat des aires d'opération, nom de scénario des transformations, et à
  vérifier pour les autres dossiers de sortie. Préexistant pour une chaîne vide ; à
  décider.
- **En-tête de licence** : les fichiers du Core, sauf son `CMakeLists.txt` et les nouveaux du
  lot B, n'ont pas l'en-tête LiLiQ-R que `CodingStandards.md` exige. À généraliser dans un lot
  dédié, jamais au passage d'un autre changement.
- **Portabilité du Core** hors Windows : `WarningExceptionHandler::tryfileopener` lance
  Notepad++ par `WinExec` (`windows.h`), du comportement d'interface à isoler sous
  `#ifdef _WIN32` ou à rendre au wrapper. Sans objet tant que les deux chaînes de
  compilation tournent sous Windows.

## 5. Pièges connus

- **Encodage cp1252** -- depuis le nettoyage, tous les fichiers de `UI/` et de
  `FMTWrapperCore/` sont en UTF-8 sans BOM : les deux derniers tests en cp1252 sont devenus
  ASCII en passant à l'anglais. Des fichiers cp1252 subsistent ailleurs dans FMT (ex.
  `Source/FMTReplanningTask.cpp`, `Examples/C++/planningtest.cpp`) : les éditer avec un
  outil qui suppose l'UTF-8 corrompt les accents ; les réécrire entièrement, ou les éditer
  au niveau octet.
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
  - `Conversions::fromUtf8` (valeurs de retour : changelog, descriptions d'exceptions)
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
- **Filtrage des contraintes / outputs** -- unifié au lot 2 dans `Selection` (retrait des
  CR/LF puis trim, ordre du modèle, chaque élément retenu une fois). **L'ordre influence la
  matrice LP** : toute modification de `Selection` se valide par `testWrapperCoreSES`
  avant/après.
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
- **`FMT_WRAPPER_CORE_EXPORT`** sur toute classe du Core appelée depuis un autre binaire : le
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
  `FMTFormCache::m_Models` (aujourd'hui `ModelCache`). **Ne pas** « corriger » en incluant `FMTModel.h` dans
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
  sans rapport avec `ModelCache`. Hors périmètre, mais c'est pourquoi le Core n'utilise pas
  l'espace de noms `Wrapper`.
- **Pas d'espace de noms `Core` dans le Core** -- à l'intérieur de `FMTWrapper::Core`,
  `Core::FMTOutput` désignerait `FMTWrapper::Core::FMTOutput` et non la classe de FMTlib : la
  centaine d'usages de `Core::` du Core ne compilerait plus. C'est pourquoi la revue de la
  PR #345 a abouti à `FMTWrapper::Backend`. Même prudence avec les autres espaces de noms de
  FMTlib (`Models`, `Parser`, `Spatial`...).
- **Lambda dans une classe managée** -- `cl /clr` refuse une lambda écrite dans une fonction
  membre d'une `ref class` (C3923 : pas de classe locale dans une classe managée). Pour
  capturer `FMTForm` (par `gcroot<FMTForm^>`), construire la lambda dans une fonction libre.
  Vérifié le 2026-09-17, en préparant le lot C.

## 6. Tests

**Base et BFEC restent séparés**, chacun dans son CSV : `Examples/C++/tests/basetests.csv`
(modèles publics de `Examples/Models/`, suivi par git) et `Examples/C++/tests/BFECtests.csv`
(modèles privés sur `T:\`, local et ignoré par git).

- **ctest passe par ces CSV** (rectifié le 2026-09-16 : ce fichier disait les tests du Core
  « manuels »). `Examples/C++/CMakeLists.txt` enregistre chaque ligne dont la première
  colonne nomme une cible existante (`if (TARGET ...)`, sensible à la casse), avec les
  colonnes suivantes pour arguments. Les cibles du Core existent à ce moment-là
  (`FMTWrapperCore/CMakeLists.txt` est inclus avant) ; celles de `UI/tests`, pas encore :
  `UI/CMakeLists.txt` les enregistre lui-même.
- **Inscrits depuis le 2026-09-16** : tous les tests du Core sauf `testWrapperCoreSA`. Sans
  le préfixe `testWrapperCore` :
  - `basetests.csv` : `GetMaxAge`, `GetYield`, `GetAllMasks`, `WriteToProject`,
    `Rasterization`, `AreaVariability`, `Planning`, `Environment`, ainsi que
    `UnitTestCallbackLogger` ;
  - `BFECtests.csv` : `SES`, `GetYield`, `GetAllMasks`, `AggregateAllActions`,
    `SplitActions`, `BuildAction` et `OperatingArea`.
- **Hors ctest** : `testWrapperCoreSA`, faute de jeu de données (son repli est un gabarit
  `TODO`).
- **Une ligne mal nommée ne produit aucune erreur** : elle est ignorée. C'est ainsi que
  `testWrapperCoreGetYield` n'a jamais tourné de 2024 à 2026 (`testWrapperCoreGetyield`
  dans un CSV, `testWrapperCoreGetYield.cpp` dans l'autre). Après une modification,
  vérifier le nom dans `build/release/CTestTestfile.cmake`.
- Les tests `FMTWrapperCore/tests/*` suivent le patron de `testWrapperCoreSES.cpp` :
  paramètres via un JSON en `argv[1]`, valeurs de référence en `argv[2]`, repli codé en dur
  sur un modèle `T:\`.
- **Ne pas** ajouter d'`add_test()` dans `FMTWrapperCore/CMakeLists.txt` : un test du Core
  rejoint ctest par une ligne dans le CSV de son jeu de données.
- Chaque lot ajoute son `testWrapperCore<Domaine>.cpp` selon ce patron. Les tests
  appellent les entrées pures des services, jamais `Controller` : ils n'ont pas à peupler
  le singleton.
- Un test qui écrit dans un projet (planification) travaille sur une copie du jeu de
  données, jamais sur `Examples/Models` : voir `testWrapperCorePlanning`.
- Quand un jeu de données public existe, le test l'utilise par défaut et reprend la forme
  d'arguments de son cousin de `basetests.csv` : `testWrapperCoreRasterization` tourne sur
  `Examples/Models/TWD_land` avec les mêmes arguments que `maptoFMTforest`, et sa ligne
  de ce CSV reprend la sienne. Les tests SES, qui n'ont que des modèles privés, prennent
  un JSON.
- Sinon, le test tourne sur un modèle privé et reprend la forme d'arguments de son cousin
  de `BFECtests.csv` : `testWrapperCoreOperatingArea` a les mêmes arguments que
  `testOAschedulertask`, aucun modèle public n'ayant de yield `YOUVERT`.
