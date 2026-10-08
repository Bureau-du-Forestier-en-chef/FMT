# Migration de l'interface : points à considérer

> Recueil des points à considérer le jour où l'interface sera retravaillée ou remplacée. Ce n'est
> pas un plan : rien n'y est décidé, sauf mention. Chaque point dit d'où il vient et comment il a été
> établi : démontré, lu dans le code, ou à vérifier. On peut ainsi le revérifier avant de s'en
> servir. À compléter au fil des chantiers.
>
> Les sections sont numérotées : « section 4.3 » renvoie à la sous-section 4.3 de ce fichier.

## 1. L'interface aujourd'hui (version 1.4.2)

Établi sur la 1.4.1, puis revérifié sur la 1.4.2 le 2026-10-06 : les deux versions ont la même
structure, sauf les DLL de mimalloc (section 4.3).

| Composant | Ce que c'est |
|---|---|
| `FMT.exe`, `FMT.dll`, `FMT.Interface.*.dll` | L'interface : une application de bureau .NET 8 en déploiement autonome, le runtime .NET 8 étant dans le dossier. `FMT.exe` est l'hôte que génère le SDK .NET ; il n'est pas signé. |
| `FMT.Interface.Wrapper.dll` | Un assemblage .NET Framework 4.8 qui référence `FMTWrapper`. |
| `FMTWrapper.dll` | Le wrapper C++/CLI de `UI/`, compilé pour le .NET Framework (`/clr`). Il importe `FMTWrapperCore.dll` et `FMTlib.dll`. |
| `FMTWrapperCore.dll` | Le cœur portable du wrapper, en C++ (`FMTWrapperCore/`). |
| `FMTExcel-AddIn64.xll`, `FMTExcel.dll`, `FMTExcelWrapper.dll` | Le complément Excel : Excel-DNA 1.9 sur .NET Framework 4.8, et son wrapper C++/CLI. Celui-ci appelle FMTlib directement, sans passer par `FMTWrapperCore`. |
| `FMT.pyd` | Le module Python, livré dans le même dossier. |

- **FMT tourne dans le processus de l'interface, replanification comprise.**
  - `FMT.deps.json` référence `FMTWrapper`, et la version ne livre aucun exécutable de FMT. Ses
    trois exécutables sont `FMT.exe`, l'outil `createdump.exe` du runtime .NET, et un utilitaire
    Python empaqueté par PyInstaller.
  - Un hôte .NET 10 charge ce `FMTWrapper.dll`, avec `FMTWrapperCore.dll` et `FMTlib.dll` (démontré
    le 2026-10-06).
- **Les assemblages de l'interface ne nomment que deux exécutables** : PowerShell, qui lance
  `FichierExcelOAS.ps1` pour ouvrir Excel avec le complément, et le programme de mise à jour.
  `FMTWrapper.dll` nomme aussi `notepad++.exe` (section 3).

## 2. Plateforme .NET

1. **.NET 8 n'est plus pris en charge après le 10 novembre 2026.** .NET 10, version à support long,
   l'est jusqu'en novembre 2028. Une nouvelle interface vise .NET 10 ou plus récent.
2. **Le wrapper C++/CLI est compilé pour le .NET Framework, et .NET 8 le charge quand même.**
   - `UI/CMakeLists.txt` et `Excel/CMakeLists.txt` posent `COMMON_LANGUAGE_RUNTIME ""`, c'est-à-dire
     `/clr`, et les deux DLL importent `mscoree.dll`.
   - Ça fonctionne aujourd'hui : un hôte .NET 10 charge `FMTWrapper.dll` par `coreclr.dll` seul, sans
     `clr.dll` (démontré le 2026-10-06).
   - Mais ce n'est pas le chemin que documente Microsoft pour .NET : `/clr:netcore`,
     `<TargetFramework>` et `ijwhost.dll` à côté de la DLL.
3. **Les limites du C++/CLI sous .NET**, selon Microsoft :
   - une DLL seulement, pas d'exécutable ;
   - Windows seulement ;
   - pas de `/clr:pure` ;
   - un projet par plateforme : le même projet ne cible pas à la fois .NET et .NET Framework.
4. **Excel reste sur le .NET Framework 4.8**, puisque le complément Excel-DNA cible net48. Si le
   wrapper de l'interface passe à `/clr:netcore`, celui d'Excel garde sa propre compilation, comme
   aujourd'hui.
5. **Le choix du déploiement compte** : autonome ou dépendant du framework. La version 1.4.2 embarque
   le runtime (.NET 8.0.22). Ce choix décide de la taille, de l'application des correctifs de
   sécurité de .NET, et de la piste de la section 4.4 : les API d'hébergement documentées visent les
   applications dépendantes du framework.

## 3. Le contrat entre l'interface et FMT

Défini dans `FMTWrapperCore/ETAT.md`, sections 1 et 5 :

1. **L'interface ne parle à FMT que par `FMTWrapper::Backend::Controller`.**
   - Le contrôleur a 38 méthodes statiques, qui ne font que déléguer.
   - Les données passent par des objets de transfert en types `std` (`<Domaine>Types.h`).
   - La progression et les erreurs passent par des événements typés (`Events.h`, `EventPublisher`).
   - Aucun type de FMTlib ne traverse la frontière.
   - L'état de la session (scénarios, journal, gestionnaire d'exceptions) vit dans `ModelCache`.
2. **Les signatures de `FMTForm` sont figées**, parce que l'interface .NET actuelle en dépend. Une
   nouvelle interface est l'occasion de les revoir, avec le côté C#.
3. **Un refus n'est pas une exception.** Quatre opérations rendent `success` et `errorMessage` au
   lieu de lever : la simulation et l'optimisation spatiales, les aires d'opération, et la variabilité
   de l'aire initiale. L'interface doit lire le drapeau et afficher le message.
4. **Défauts et décisions ouverts**, à reprendre ou à trancher (`FMTWrapperCore/ETAT.md`,
   section 4) :
   - la planification ne fait pas remonter un scénario irréalisable : `Controller::plan` est `void`,
     et le correctif tient dans `FMTPlanningTask` ;
   - en replanification, un modèle tactique sans solution n'est qu'un avertissement, par
     construction ;
   - `Selection::selectOutputs` rend une liste vide, sans rien dire, pour un nom de sortie inconnu ;
   - `Transformation::aggregateAllActions` ignore l'ordre que choisit l'utilisateur : il faut soit
     retirer le paramètre, soit l'honorer ;
   - l'interface actuelle valide plus que le Core : l'écran de la simulation spatiale exige
     `STANLOCK.tif` même sans verrou, et celui du calendrier de COS exige les colonnes `NPE` et
     `GUP`, que le Core n'exige pas.
5. **Du comportement d'interface est resté dans le Core** : `WarningExceptionHandler::tryfileopener`
   ouvre le fichier fautif dans Notepad++ par `WinExec`. Il faut le rendre à l'interface si elle
   change d'éditeur ou de plateforme.
6. **La planification et les transformations écrivent dans le projet** : elles réécrivent les
   `._seq`, ajoutent un scénario, réécrivent le `.pri`. Une interface qui travaille sur une copie doit
   en tenir compte.

## 4. Processus, mémoire et allocateur

### 4.1 Ce que vit la production

Des replanifications de plusieurs jours sur 5 fils, avec une mémoire déjà près de la limite
(section 1.1 de `Tests/Performance/ETAT.md`, l'ETAT du banc). Un changement qui fait grossir la
mémoire ou les durées d'un processus d'interface se voit d'abord là.

### 4.2 L'état global de FMT dans un processus

Ce point compte pour une interface qui garde un processus ouvert longtemps, ou qui y lance plusieurs
calculs :

- **Un seul journal par processus.** Il se règle une fois, avant tout modèle. Le remplacer pendant
  qu'un modèle existe fait échouer la copie de ses solveurs (issue #363 ; démontré, section 2.12 de
  l'ETAT du banc).
- **Un seul cache des yields complexes par processus**, partagé par tous les modèles et tous les
  fils. Il ne garde qu'un calcul de plus de 0,05 ms, et il se vide quand il reste moins de 10 Go de
  mémoire disponible (sections 2.1 et 2.16 de l'ETAT du banc).
- **Une course est connue** sur les caches de yields partagés entre fils (section 7 de
  `Examples/C++/tests/ETAT.md`).
- **Chaque replanification garde environ 4,7 Mo**, une taille fixe, quel que soit le nombre de
  réplicats. La cause n'est pas trouvée (section 2.13 de l'ETAT du banc). Un processus qui en enchaîne
  beaucoup les cumule.
- **FMTlib est une DLL partagée** : ses statiques (journal, gestionnaire d'exceptions) sont communes à
  `FMTWrapper.dll` et à `FMTWrapperCore.dll`.

### 4.3 L'allocateur : pourquoi mimalloc ne sert pas l'interface actuelle

mimalloc a réduit de 23 à 38 % les durées des yields complexes, mais le pic d'un processus du banc
est passé de 21 à 54 Mo (section 2.6 de l'ETAT du banc). Depuis #361, un build configuré avec
`-DWITH_MIMALLOC=ON` fait tourner les exécutables C++ de FMT sous mimalloc (exemples, tests,
banc). Par défaut, ils restent sur le tas du CRT, comme l'interface, pour que les tests et le banc
voient ce que voient les utilisateurs (décision de l'équipe, 2026-10-06). L'interface, elle, ne peut
pas passer sous mimalloc. Pourquoi :

1. Toutes les DLL compilées en `/MD` appellent `malloc` et `free` dans `ucrtbase.dll` : FMTlib, Boost,
   GDAL, les wrappers et le runtime .NET.
2. `mimalloc-redirect.dll`, une dépendance de `mimalloc.dll`, modifie en mémoire les points d'entrée
   de `ucrtbase.dll` pour qu'ils sautent dans mimalloc. Tout le processus bascule d'un coup.
3. Un bloc alloué avant la bascule et libéré après serait remis à mimalloc, qui ne le connaît pas :
   plantage ou corruption du tas. C'est le cas que rapporte l'issue 1221 de mimalloc.
4. Pour l'éviter, `mimalloc-redirect.dll` refuse de basculer si `ucrtbase.dll` est déjà initialisée.
   Le refus ne s'affiche qu'avec `MIMALLOC_VERBOSE=1`, et rien ne plante :

   ```text
   mimalloc-redirect: error: mimalloc-redirect.dll seems to be initialized after ucrtbase.dll
   mimalloc-redirect: warning: standard malloc is _not_ redirected! -- using regular malloc/free. (v1.1c)
   ```

5. mimalloc ne sert donc que si `mimalloc.dll` est le premier import de l'exécutable que Windows
   démarre. `FMT.exe` importe lui-même `ucrtbase.dll`, et l'interface charge FMTlib bien plus tard.
   Excel et Python sont dans le même cas.

Démontré les 5 et 6 octobre 2026, avec le mimalloc 2.1.2 de vcpkg :

- refus, sans plantage, dans Python 3.10.7 et dans un hôte .NET 10 ;
- refus aussi dans le banc officiel, avec une FMTlib qui importe mimalloc, parce que l'exécutable
  importe Boost avant FMTlib. C'était le correctif d'abord proposé dans #361 ;
- redirection réussie dans le banc relié avec `mimalloc.lib` en premier.

La version 1.4.2 livre `mimalloc.dll` et `mimalloc-redirect.dll`, mais aucun de ses 407 binaires ne
les importe : elles ne se chargent jamais (vérifié le 2026-10-06). La 1.4.1 ne les livrait pas : une
FMTlib qui les aurait importées ne s'y serait plus chargée.

Le mécanisme de #361 :

- avec `-DWITH_MIMALLOC=ON`, `createexecutable` lie mimalloc en premier à chaque exécutable ;
- dans un tel build, `MIMALLOC_DISABLE_REDIRECT=1` le coupe pour un passage, ce que le page heap de
  gflags exige ;
- dans un tel build aussi, le test `FMTPerformanceTests.Mimalloc` garde le mécanisme ;
- un tel build sert à mesurer ce que mimalloc apporterait, pas à tester : la plupart des flux du banc
  y dépassent leurs bornes de mémoire, mesurées sur le tas du CRT.

Pour vérifier un processus, au choix :

- lancer avec `MIMALLOC_VERBOSE=1`, et lire les lignes `mimalloc-redirect:`. Les autres lignes
  `mimalloc:` peuvent venir de MOSEK : `mosek64_10_1.dll`, livrée aussi avec l'interface, embarque sa
  propre copie de mimalloc. Celle-ci s'initialise au chargement de MOSEK, sans rediriger le tas du CRT
  (vu le 2026-10-06 dans le banc, où le champ `allocator` reste `CRT heap`) ;
- lire le champ `allocator` des résultats du banc ;
- lancer `dumpbin /DEPENDENTS` sur l'exécutable, qui doit montrer `mimalloc.dll` en tête.

### 4.4 Ce que permettrait une interface retravaillée

Il faut que le processus démarre d'un exécutable que l'équipe construit, lié à mimalloc en premier.
La technologie de l'interface compte moins que la question de savoir qui fournit l'`.exe`.

- **Garder le C#, avec un lanceur natif à nous.**
  - Aujourd'hui, `FMT.exe` ne fait que charger le `hostfxr.dll` du dossier et lui passer la main.
  - Un petit lanceur C++ peut faire de même par les API d'hébergement de .NET (`nethost`,
    `hostfxr`). Lié à mimalloc en premier, il fait passer tout le processus sous mimalloc : le runtime
    .NET, l'interface, `FMTWrapper` et FMTlib. Le code C# ne change pas.
  - Premier risque : .NET et ses bibliothèques de bureau sous mimalloc ne forment pas une combinaison
    que Microsoft teste. `coreclr.dll` alloue lui aussi par `ucrtbase.dll`, donc il passerait sous
    mimalloc. Un prototype doit valider la combinaison.
  - Second risque : un logiciel de sécurité qui injecte sa DLL dans le processus avant mimalloc, et
    qui alloue avant la bascule.
    - L'issue 1221 de mimalloc rapporte un plantage au démarrage dans ce cas, sur mimalloc 3.0.3,
      avec une DLL d'Admin By Request.
    - Avec le mimalloc 2.1.2 de vcpkg, on sait seulement que la redirection refuse quand
      `ucrtbase.dll` est déjà initialisée. Le cas d'une DLL injectée n'est pas démontré.
    - Le risque vaut pour tout exécutable lié à mimalloc en premier : ce lanceur, un `FMT.exe` modifié
      (section 4.5), et les exécutables C++ de FMT dans un build configuré avec `-DWITH_MIMALLOC=ON`.
    - Le prototype doit donc tourner sur les postes des utilisateurs.
- **Une interface native en C++** : Qt, WinUI 3 en C++, ou une page web affichée par WebView2 dans un
  exécutable C++.
  - Liée comme les exécutables de `createexecutable`, elle a mimalloc dès le départ, sans runtime .NET
    dans le processus.
  - Elle s'appuie directement sur `FMTWrapperCore`, dont les tests passent sous mimalloc (vérifié le
    2026-10-06, dans un build lié à mimalloc).
  - C'est le plus gros changement de technologie.
- **Ce qui n'aiderait pas** : une nouvelle interface .NET avec l'hôte du SDK, ou une interface en
  Python, Electron ou Java. FMT y serait toujours chargé tard.

### 4.5 Les pistes déjà examinées

| Piste | Verdict |
|---|---|
| Un processus natif à part pour les calculs longs (`replanner.exe` lancé par l'interface) | Écartée par l'équipe (2026-10-06). |
| Le Segment Heap, par le manifeste de l'interface (`<heapType>SegmentHeap</heapType>`) | Déjà essayée par l'équipe. Résultat : à consigner. |
| Des allocateurs explicites dans FMTlib (`mi_stl_allocator`, `std::pmr`) | Reportée après #348, à guider par le banc (voir plus bas). |
| Modifier `FMT.exe` après coup (`minject`), ou `AppInit_DLLs` | Écartées. `minject` n'est pas dans le paquet vcpkg, et il faudrait le repasser sur chaque `FMT.exe`, que le SDK régénère à chaque publication. Le lanceur de la section 4.4 donne le même ordre de démarrage sans modifier de binaire, mais il garde les mêmes risques, dont celui de l'issue 1221. `AppInit_DLLs` est obsolète. |
| Un `operator new` propre à FMTlib | Écartée : sous MSVC, chaque DLL a son `operator new`. Un bloc alloué par FMTlib et libéré par le code en ligne d'un wrapper irait au mauvais allocateur, et le processus planterait. |
| Les réglages du ramasse-miettes .NET | Sans effet : FMT alloue en natif. |

**`mi_stl_allocator`** donne à un conteneur l'allocateur de mimalloc sans redirection. C'est la seule
façon d'avoir mimalloc dans l'interface sans changer d'exécutable, mais ce n'est pas un gain rapide :

- **Les types changent** : `std::vector<T, mi_stl_allocator<T>>` n'est pas `std::vector<T>`. Les
  liaisons Python, R et .NET devraient convertir ou copier à la frontière, si bien que seuls des
  conteneurs internes s'y prêtent.
- **Le build R n'a pas mimalloc** (il passe par GCC et MSYS2) : il faudrait un repli, donc deux
  comportements à tester.
- **FMTlib dépendrait de deux DLL de plus**, `mimalloc.dll` et `mimalloc-redirect.dll`. La 1.4.2
  les livre déjà, sans s'en servir (section 4.3).
- **Deux tas coexisteraient dans le processus**, alors que la mémoire de production est déjà près de
  la limite.
- **Le gain serait partiel** : les gains mesurés viennent de toutes les petites allocations.
- **La piste recouvre #348**, qui doit supprimer les allocations du chemin le plus chaud,
  `FMTComplexYieldHandler::get`.

L'ordre conseillé :

1. préallouer et réutiliser (`Documentation/Architecture.md`, et #348) ;
2. chercher avec le banc les allocations qui restent chaudes ;
3. s'il en reste, passer par un type alias propre à FMT, pour pouvoir changer d'implémentation et la
   mesurer.

Pour mesurer un allocateur explicite avec le banc :

- **Le mesurer sur le tas du CRT**, celui de l'interface et du banc par défaut. Dans un build
  configuré avec `-DWITH_MIMALLOC=ON`, tout passe déjà par mimalloc : un conteneur
  `mi_stl_allocator` n'y changerait presque rien.
- **Étendre d'abord le moniteur d'allocations.** Il ne compte que les appels au tas du CRT, `malloc`,
  `free` et leurs variantes dans `ucrtbase.dll` (lu dans
  `Tests/Performance/Harness/AllocationMonitorWindows.cpp`). Une allocation passée à `mi_malloc`
  sortirait des comptes sans avoir disparu.

### 4.6 Avant de décider

1. **Mesurer le plafond du gain, le moment venu.** Un build configuré avec `-DWITH_MIMALLOC=ON`,
   mesuré avec et sans `MIMALLOC_DISABLE_REDIRECT=1`, en alternance, compare le tas du CRT et
   mimalloc sur les mêmes binaires (section « The allocator » de
   `Documentation/PerformanceTesting.md`). Ces mesures n'ont pas été faites avec #361 : elles
   vieilliraient avec #348, qui retire des allocations. Les ordres de grandeur connus sont dans la
   section 2.6 de l'ETAT du banc. Si le gain est faible sur les flux de taille réelle, l'allocateur
   ne justifie pas à lui seul de refaire l'interface.
2. **Si le gain est réel, prototyper le lanceur de la section 4.4** avec l'interface actuelle, sur un
   poste d'utilisateur (second risque de la section 4.4). On saurait si elle fonctionne sous
   mimalloc, et ce que gagne une vraie replanification, avec et sans `MIMALLOC_DISABLE_REDIRECT=1`.
3. **Juger le coût mémoire** sur la replanification à 5 fils (lot 4 du banc).

## 5. Déploiement

- **Les DLL natives vont à côté de l'exécutable.** Windows cherche dans le dossier de l'exécutable
  avant `System32`, où une `onnxruntime.dll` plus ancienne peut se trouver : c'est vu sur le poste de
  développement, où il faut copier `onnxruntime.dll` à côté de tout exécutable lancé hors de
  `bin/Release`. La version 1.4.2 livre la sienne.
- **`GDAL_DATA`, `proj` et `YieldPredModels` vont à côté de `FMTlib.dll`**, que FMTlib cherche dans
  son propre dossier. Démontré le 2026-10-06 : une `FMTlib.dll` seule dans un autre dossier lève
  `FMTexc(13)` « Can not find GDAL_DATA at <dossier de FMTlib.dll>\GDAL_DATA ».
- **Si un exécutable lie mimalloc**, `mimalloc.dll` et `mimalloc-redirect.dll` vont toutes deux à
  côté de lui.
- **`hasFeature("ONNXRUNTIME")` rend toujours faux** (issue #362) : une interface qui affiche ou
  teste les fonctionnalités de FMT ne peut pas s'y fier pour ONNX Runtime.
- **Le module Python est livré dans le même dossier** : il doit venir du même build que FMTlib.

## 6. Messages et chemins

- **L'encodage des messages est à vérifier.**
  - MSVC compile sans `/utf-8`, et deux chemins décodent les chaînes différemment.
    `Conversions::fromUtf8` décode en UTF-8. `_toFeedback`, par où passent les messages du journal,
    fait `gcnew System::String(const char*)`, qui décode dans la page ANSI sous .NET Framework
    (section 5 de `FMTWrapperCore/ETAT.md`).
  - L'interface tourne maintenant sous .NET 8 : le décodage de ce second chemin est à vérifier là.
    Les accents des messages de progression restent à valider dans l'interface.
- **Les messages vus par l'utilisateur sont en français** (`AGENTS.md`, section Language).
- **Les chemins de plus de 260 caractères ne sont pas lus.** Un fichier de scénario au-delà de cette
  limite (dossier courant et chemin relatif mis bout à bout) est ignoré sans message, et FMT lit la
  section de la racine à la place (section 7 de `Examples/C++/tests/ETAT.md`). Une interface qui
  laisse choisir des dossiers profonds doit le tester ou l'empêcher.

## 7. Valider une nouvelle interface

- **Le banc de scénarios** : `Examples/Models/TWD_land/BANC_INTERFACE.md`. Il prévoit un scénario qui
  passe et un qui échoue par module, avec les réglages à saisir, les messages attendus, et ce que les
  essais réécrivent dans le projet.
- **Les validations encore en attente** dans l'interface actuelle (section 4 de
  `FMTWrapperCore/ETAT.md`) :
  - la simulation spatiale ;
  - le calendrier de COS ;
  - `chargement_fail` ;
  - l'ouverture du fichier fautif et `RecoverFromCrash` ;
  - le niveau du journal après une replanification ;
  - les accents.
- **Les tests du Core** (`testWrapperCore*`) sont dans ctest, sur le tas du CRT comme l'interface. Ils
  passent aussi sous mimalloc (vérifié le 2026-10-06).
- **Le banc de performance** (#349, `Documentation/PerformanceTesting.md`) mesure les durées et la
  mémoire.

## 8. Références

- Dans le dépôt :
  - `FMTWrapperCore/ETAT.md` : le contrat, les pièges, les validations ;
  - `Tests/Performance/ETAT.md` : l'ETAT du banc, sections 1.1, 2.1, 2.6, 2.12, 2.13 et 2.16 ;
  - `Examples/C++/tests/ETAT.md`, section 7 ;
  - `Examples/Models/TWD_land/BANC_INTERFACE.md` ;
  - `Documentation/PerformanceTesting.md`, section « The allocator » ;
  - `AGENTS.md`, section Building.
- Issues : #361 (mimalloc), #362 (`hasFeature`), #363 (journal de FMT), #348 (yields complexes),
  #349 (banc).
- mimalloc : `readme.md`, section « Dynamic Override on Windows », et ses issues 582 (pas de
  redirection après un `LoadLibrary`) et 1221 (plantage au démarrage quand une DLL injectée par un
  logiciel de sécurité alloue avant la redirection).
- Microsoft :
  - le cycle de vie de .NET (fin de .NET 8 le 2026-11-10) ;
  - « Migrate C++/CLI projects to .NET » ;
  - « Write a custom .NET runtime host » ;
  - « Application manifests » (élément `heapType`).
