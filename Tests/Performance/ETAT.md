# État des benchmarks de performance et d'allocations (#349)

> Fichier de suivi du chantier de l'issue #349, « Add performance and allocation benchmarks »
> (jalon FMT2.0). Il est mis à jour **à la fin de chaque lot**. Une session qui démarre à froid
> doit pouvoir savoir d'ici quoi faire, sans relire l'historique.
>
> Le dépôt est public : ce fichier ne cite ni chemin privé ni modèle privé.
>
> **Ordre de lecture pour démarrer à froid** : la section 0 situe le chantier et ses liens avec
> #348 et #350 ; la section 1 donne les décisions et les règles ; la section 6 dit quoi faire
> ensuite ; la section 2 se lit avant de toucher au code, elle tient les pièges déjà repérés.
> Les sections 3 à 5 sont la conception, la feuille de route et le journal ; la section 7, les
> mesures. Le fonctionnement du banc lui-même (lancer, lire, ajouter un benchmark) est dans
> `Documentation/PerformanceTesting.md`.
>
> Les sections et sous-sections sont numérotées : « section 2.6 » renvoie à la sous-section 2.6
> de ce fichier. Les lots (lot 1, lot 2...) sont les étapes du chantier, sans lien avec ces
> numéros.

## 0. Vue d'ensemble

### 0.1 Ce que demande #349

Un banc de mesure séparé des tests fonctionnels, qui mesure :

- la durée d'exécution ;
- la mémoire : pic, et mémoire retenue après des exécutions répétées ;
- le nombre d'allocations et les octets alloués, jusqu'à exiger zéro allocation sur les
  chemins de calcul qui ne doivent pas allouer ;
- la mise à l'échelle avec le nombre de workers, et la mémoire par worker.

Chaque résultat part en JSON avec son environnement (version, commit, compilateur, OS,
processeur...) et se compare à une référence. Les benchmarks se lancent par groupe, depuis
CMake et ctest, en Release.

**Pas de seuil de temps au départ.** Seules les conditions déterministes font échouer : une
allocation sur un chemin qui ne doit pas allouer, un pic mémoire au-delà d'un maximum défini,
un benchmark qui ne termine pas, un résultat faux, un résultat manquant ou invalide.

Le premier jeu de benchmarks doit couvrir l'évaluation des yields complexes, la lecture des
yields, la lecture d'un projet, au moins un flux de modèle et au moins un flux multithread.

### 0.2 Pourquoi maintenant : #348

L'issue #348, « Refactor complex yields using preallocated strategies », refond l'évaluation
des yields complexes en une stratégie par opérateur, et exige que
`FMTComplexYieldHandler::get` n'alloue plus rien pendant le calcul. Sans mesure prise
**avant**, rien ne montrera le gain, ni une régression de temps masquée par une baisse des
allocations.

Le lot 1 mesure donc l'état actuel des yields complexes : temps et allocations par appel, sur
le code d'aujourd'hui. **La référence doit être prise sur un commit antérieur à tout changement
de #348** : celle de la section 7.1 a été prise le 2026-09-30, sur le build de Gabriel du commit
`9a5001df`, sous le tas du CRT ; la première, du 2026-09-29 (`47d4ea45`), est en section 7.4. Une
mesure « après » se prend sur la même machine, avec le même allocateur.

### 0.3 Lien avec #350 et le chantier des tests

- Le moniteur d'allocations du lot 1 est ce que demande le lot 9 de #350 (« absence
  d'allocation », section 6.3.6 de `Examples/C++/tests/ETAT.md`). Il vit dans une bibliothèque
  statique, `FMTBenchmarkHarness`, que la future cible `FMTCoreTests` pourra lier.
- Les tests (tests de chaîne dans `Examples/C++/`, tests unitaires dans `Tests/Core/` pour
  #350) protègent le comportement ; ce banc mesure. L'un ne remplace pas l'autre
  (`Documentation/Architecture.md`, « Measure before and after »).
- Les lignes de performance, en mode court, entrent dans la suite base : elles changent les
  nombres de la section 8 de `Examples/C++/tests/ETAT.md`, où une note le dit depuis le lot 1.

### 0.4 Où vivent les choses

| Quoi | Où |
|---|---|
| Suivi du chantier | ce fichier |
| Fonctionnement du banc : lancer, lire, comparer, ajouter un benchmark | `Documentation/PerformanceTesting.md` |
| Harnais : moniteurs, chronométrage, vérifications, JSON | `Tests/Performance/Harness/` (bibliothèque `FMTBenchmarkHarness`) |
| Benchmarks | `Tests/Performance/FMTPerformanceTests.cpp` et `Tests/Performance/*Benchmarks.cpp` |
| Lignes qui inscrivent les benchmarks, valeurs attendues et bornes | `Tests/Performance/performance.csv` |
| Données | `Examples/Models/TWD_land/Scenarios/perfyields` ; `Examples/Performance/` s'il faut un modèle plus gros (lot 5) |
| Benchmarks sur des modèles privés (lots 3 et 4) | un CSV local, ignoré par git comme `Examples/C++/tests/BFECtests.csv` ; aucun nom de modèle privé dans le dépôt |
| Résultats JSON | `build/release/tests/performance/` |
| Comparaison de deux passages | `Tests/Performance/CompareResults.cmake` |

### 0.5 Quel effort donner à l'agent

| Travail | Effort |
|---|---|
| Lancer les lignes de performance après un build, rapporter, mettre à jour la section 7 | `low` |
| Ajouter une ligne CSV à un benchmark existant, avec une valeur calculée à la main | `medium` |
| Écrire un benchmark | `high` |
| Le moniteur d'allocations, les benchmarks de threads | `xhigh` |
| Enquêter sur une mesure suspecte, démontrer un défaut de FMTlib | `xhigh` ou `max` |
| Mettre à jour ce fichier | `medium` |

Comme pour les tests : un effort trop bas est dangereux quand une valeur attendue se calcule à
la main ; trop haut ne coûte que du temps.

## 1. Objectif, décisions, règles et rôles

### 1.1 Objectif

Fermer #349. D'abord, donner à #348 sa référence « avant » ; ensuite, couvrir le premier jeu de
benchmarks que l'issue demande (section 4.1).

Ce que Gabriel attend du banc (2026-10-02) : voir une dérive avant qu'elle ne bloque la
production. Certains modèles roulent en replanification sur de très longues périodes, avec
beaucoup de réplicats : plusieurs jours de calcul sur 5 fils de ce poste, avec une mémoire déjà
près de la limite. Un développement qui ferait exploser la mémoire empêcherait ces passages ; un
autre qui multiplierait le temps de calcul les rendrait inutilisables. Les benchmarks sur TWD_land
gardent le coût des opérations de base ; la taille réelle passe par le groupe privé (décision 8).

### 1.2 Hors périmètre

- **Intégration continue** : le dépôt n'en a pas (`.github/` ne contient que des gabarits
  d'issues). À trancher au lot 5 ; l'issue dira pourquoi la publication automatique des
  rapports est reportée.
- **Seuils de temps** : aucun, comme le veut l'issue, tant qu'il n'y a pas assez de mesures de
  référence.
- **Benchmarks Python et R** (coût des wrappers) : l'issue les exclut du premier jet.
- **Compteur d'allocations hors Windows** : les lignes d'allocations y rendent 77. Les builds
  MSYS2 ne sont pas visés pour l'instant.
- **Toute correction de FMTlib**, refonte de #348 comprise. Un défaut trouvé donne une issue
  documentée, corrigée dans une autre session (règle 10).
- **Flux spatiaux** (SES, rastérisation, ordonnanceur d'aires d'opération) : hors du premier jeu ;
  à la demande après la fermeture. La replanification, qui en faisait partie, y est entrée le
  2026-10-02 (décision 13).

### 1.3 Décisions

Prises le 2026-09-25, sauf mention d'une autre date.

1. **Harnais maison, sans dépendance**, à la manière de `Examples/C++/tests/TestTools.h`.
   *Pourquoi* : l'issue veut un schéma JSON défini chez nous, le comptage des allocations et la
   mémoire, que Google Benchmark ne fournit pas. Et ce qui fait la force des tests de chaîne
   tient ici aussi : les valeurs attendues dans un CSV, un cas de plus sans recompiler, un vu
   rouge en faussant un argument.
2. **Allocations comptées par interception des imports de la bibliothèque C.** Le moniteur
   redirige `malloc`, `free` et compagnie dans la table d'imports des modules chargés.
   *Pourquoi* : sous MSVC, `operator new` est lié statiquement dans chaque module ; un
   `operator new` remplacé dans l'exécutable ne voit donc pas les allocations faites dans
   `FMTlib.dll`, alors que toutes finissent par le `malloc` importé. Rien ne change dans
   FMTlib, et on mesure le build livré. Replis écartés : Microsoft Detours (dépendance vcpkg)
   et un build instrumenté de FMTlib (on ne mesurerait plus le build livré). La preuve de
   faisabilité du lot 1 a validé ce choix (section 5.2).
3. **Chantier distinct**, suivi dans ce fichier. Le lot 1 fait le socle et les yields
   complexes, pour servir de référence à #348.
4. **Mode court dans la suite base, mesure complète sur demande.** *Pourquoi* : un benchmark
   qui ne tourne jamais pourrit sans que personne le voie. En mode court, chaque passage de la
   suite vérifie qu'il compile, qu'il calcule juste et qu'il respecte sa borne d'allocations.
   Le temps, lui, ne vaut rien sous `-j 8` : la mesure complète se lance à part, sans `-j`.
5. **Les JSON restent locaux** : ils dépendent de la machine. Les nombres de référence sont
   consignés en section 7.1, avec le commit et la machine.
6. **Le commit est lu à chaque build**, pas seulement à la configuration, pour qu'un résultat
   ne porte jamais un commit périmé.
7. **L'allocateur de chaque mesure est noté** (champ `allocator` de l'environnement).
   *Pourquoi* : un build peut tourner sous mimalloc (`-DWITH_MIMALLOC=ON`, décision 17). Actif,
   mimalloc change de 23 à 38 % les durées des yields complexes : deux mesures faites sous deux
   allocateurs ne se comparent pas. Le champ est entré avant le premier commit du banc : le schéma
   reste 1.
8. **Un groupe privé de benchmarks pour les flux, aux lots 3 et 4**, sur des modèles de
   `BFECtests.csv`. *Pourquoi* : TWD_land est trop petit pour un flux, ses coûts fixes
   dominent ; et un test BFEC tel quel ne donne que la durée de son processus, souvent sous
   `-j 8`, sans allocations ni mémoire. Règles :
   - un CSV local, ignoré par git, avec sa propre étiquette, jamais dans la suite base ;
   - un mode « flux » du harnais : 3 à 5 exécutions complètes, une passe comptée, le temps de
     chaque phase (lecture, graphe, matrice, résolution, outputs) pour séparer FMT de MOSEK ;
   - une empreinte SHA-256 des fichiers du modèle dans le JSON, puisque ces modèles ne sont pas
     versionnés : deux mesures ne se comparent que sur la même empreinte ;
   - des résultats locaux, et aucun chemin ni nom de modèle privé dans le dépôt ;
   - chaque flux garde une petite version publique sur TWD_land.
9. **mimalloc donne une issue** (section 2.6), selon la règle 10 : c'est #361.
10. **`hasFeature("ONNXRUNTIME")` donne une issue** (2026-09-29, section 2.5) : c'est #362.
11. **Suivi dans GitHub** (2026-09-29) : un commentaire sous #349 seulement quand ça en vaut la
    peine (un résultat qui sert à d'autres, comme la référence de #348, ou une décision à
    partager), pas un par lot. En français.
12. **La référence de #348 est reprise avec le harnais du lot 2** (2026-09-30), sur le commit du
    lot 2, toujours antérieur à #348. *Pourquoi* : la mesure note alors les cœurs du fil qui
    mesure et la mémoire retenue, et elle évite la pause au démarrage d'un processus
    (section 2.10), qui fausse la lecture d'un projet. La référence du lot 1 reste en section 7.4.
13. **La replanification entre dans le premier jeu** (2026-10-02, section 1.1) : un flux de
    replanification au lot 3, sur TWD_land et sur un modèle privé, puis à 5 fils au lot 4.
14. **Un flux borne le pic mémoire de son processus** (2026-10-02) : le plus haut pic de deux
    mesures dans chaque mode, plus 10 %. *Pourquoi* : en production, la mémoire est déjà près de
    la limite ; un changement qui la fait exploser doit faire échouer un benchmark.
15. **Le défaut du journal de FMT donne une issue** (section 2.12), selon la règle 10 : c'est
    #363, publiée le 2026-10-05.
16. **#361 passe avant le lot 4** (2026-10-05) : Gabriel met le chantier en pause pour faire #361
    (mimalloc) d'abord. Le banc devait y servir pour la première fois à juger un changement de FMT
    (abandonné, décision 17) ; le lot 4 reprend ensuite.
17. **mimalloc reste hors du build par défaut** (#361, 2026-10-06). Chargé tard, mimalloc ne peut
    pas remplacer le tas du CRT (section 2.6) : l'interface, Excel et Python, qui chargent FMTlib,
    restent sur ce tas. Les exécutables de CMake y restent aussi ; `-DWITH_MIMALLOC=ON` les lie à
    mimalloc en premier, pour mesurer ce qu'il apporterait. Les bornes ne changent pas, et la
    référence de #348 (section 7.1) reste valable. *Pourquoi* : Gabriel l'a d'abord voulu actif par
    défaut (2026-10-05), puis a jugé inutile un état que les utilisateurs n'auront pas ; les tests
    et le banc doivent voir ce que voient les utilisateurs. Les mesures avant et après prévues pour
    #361 sont abandonnées (2026-10-07) : le build par défaut ne change pas d'allocateur, et une
    mesure sous mimalloc vieillirait avec #348. Le banc servira pour la première fois à juger un
    changement de FMT avec #348.

Les choix de conception de chaque lot sont en section 3 ; ceux des lots 1, 2 et 3 sont validés
(2026-09-29, 2026-09-30 et 2026-10-05).

### 1.4 Règles

1. **Tout benchmark valide son résultat**, hors de la section mesurée. Un benchmark rapide qui
   calcule faux ne sert à rien. La valeur attendue se calcule à la main depuis les fichiers du
   modèle, jamais depuis ce que FMT affiche.
2. **Aucun seuil de temps.** Échouent seulement : un résultat faux, une borne d'allocations
   dépassée, un benchmark qui ne termine pas, un résultat manquant ou invalide.
3. **Une borne d'allocations porte sur un appel typique** : la médiane des appels comptés ne
   doit pas la dépasser, et une borne 0 exige en plus qu'aucun appel n'alloue. Elle est mesurée
   une fois, confirmée par un second passage, notée au journal et vue rouge : une borne sous la
   mesure fait échouer la ligne. *Pourquoi* : le cache des yields complexes dépend du temps
   (section 2.1). Un appel servi par le cache alloue moins qu'un calcul complet, et un calcul dont la
   valeur entre au cache alloue plus : ni le minimum ni le maximum ne sont stables, la médiane
   l'est. Quand une borne baisse, grâce à #348 par exemple, on la resserre et on le justifie au
   journal.
4. **Vu rouge sans recompiler** : valeurs attendues et bornes arrivent par le CSV, et les
   options `--expected` et `--max-allocations` les remplacent pour un passage.
5. **Release seulement.** Un résultat mesuré hors Release le dit dans son JSON et ne sert pas
   de référence.
6. **Aucune écriture sous `Examples/Models`** : les sorties vont dans
   `build/release/tests/performance/`, et la fixture `ExamplesModels` le vérifie.
7. **Déterminisme** : un seul thread hors des benchmarks de threads, graine fixe, et aucune
   journalisation dans la section mesurée.
8. **Solveur** : MOSEK si FMT est compilé avec, CLP sinon. La valeur attendue doit tenir avec
   les deux : un objectif plutôt qu'une solution.
9. **Fonctionnalité absente** (OSI, compteur d'allocations hors Windows) : la ligne rend 77.
10. **Aucune correction de FMTlib dans ce chantier.** Un comportement suspect se démontre
    d'abord : cause trouvée dans le code, scénario A/B minimal, exécutable du build, origine
    dans l'historique git. Il donne ensuite une issue.
11. **Toute valeur de référence modifiée se justifie** au journal.
12. **Une borne de mémoire retenue porte, elle aussi, sur un appel typique** : la médiane des
    octets que les appels comptés gardent alloués en rendant la main. Une borne 0 exige en plus
    qu'aucun appel ne garde rien.
13. **Une borne ne se pose que sur une mesure qui ne dépend ni du passage ni de la machine.** Le
    nombre d'allocations d'une lecture de projet grandit avec la longueur du chemin du projet
    (section 2.11) : il est mesuré et comparé, mais pas borné. La mémoire retenue d'un calcul de
    yields complexes dépend du passage (section 2.16) : sa borne est le plus que le cache des
    yields peut garder en un appel, plus 10 %.
14. **Le journal de FMT se règle une seule fois par processus, avant tout modèle**
    (`quietFmt`) : le remplacer pendant qu'un modèle existe laisse le solveur de ce modèle avec un
    gestionnaire de messages détruit (section 2.12).

### 1.5 Rôles

- **Gabriel** : compile, reconfigure CMake, commite.
- **Claude** : écrit le harnais, les benchmarks et ce fichier ; fait les contrôles statiques
  (compilation avec `cl` dans le scratchpad, projet CMake jetable) ; lance les lignes de
  performance et la suite base après le build de Gabriel, puis rapporte. Chaque lot s'arrête
  sur une livraison ; le suivant attend le feu vert.

`ctest.exe` n'est pas dans le `PATH`. Le mode court se lance ainsi :

```
& "C:/Program Files/Microsoft Visual Studio/2022/Professional/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/ctest.exe" --test-dir build/release -C Release -L performance --output-on-failure
```

et la mesure complète, sans `-j`, avec la même commande précédée de
`$env:FMT_BENCHMARK_MODE="full";`.

## 2. Constats et pièges

Relevés à la lecture du code au lot 0 (commit `a4a0d7bf`), puis mesurés au lot 1. « À
démontrer » : lu dans le code, sans démonstration.

### 2.1 Le cache des yields complexes

- **Il dépend du temps.** `FMTComplexYieldHandler::get`
  (`Source/FMTComplexYieldHandler.cpp:867-872`) ne met une valeur en cache que si son calcul a
  pris plus de 0,05 ms. Qu'un appel suivant soit servi par le cache dépend donc de la machine
  et de sa charge.
- **Il est global.** C'est un `boost::concurrent_flat_map` statique, commun à tout le
  processus et à tous les modèles (`Source/FMTYieldsCache.cpp:23`). Rien ne permet de le vider
  de l'extérieur, et `FMTYieldsCache` n'est pas exportée : un benchmark ne peut pas savoir si
  une valeur vient du cache, sinon par ses allocations.
- **Il se vide quand la mémoire manque** : tous les 10 000 éléments, `_clearIfTooBig` le vide
  s'il reste moins de 10 Go de mémoire disponible (`Source/FMTYieldsCache.cpp:80-94`). La
  mémoire disponible au départ est notée dans l'environnement de chaque résultat.
- **Chaque évaluation de premier niveau lit l'horloge deux fois** (`getClock`,
  `getDuration`), que la valeur entre au cache ou non. Ce coût fait partie de la référence.
- **Une même clé répétée bascule au cache pour de bon** (mesuré au lot 1). Les calculs mesurés
  durent de 0,3 à 2,5 µs, bien sous 0,05 ms ; mais sur des centaines de milliers d'appels, une
  préemption finit par en ralentir un, et sa clé reste ensuite au cache. En mesure complète,
  les 7 opérateurs répétés sur une seule clé ont tous basculé (2 allocations et ~100 ns par
  appel) ; en mode court, aucun. D'où les benchmarks sur 255 clés (section 3.1).
- **Sur 255 clés, quelques-unes entrent au cache pendant une mesure complète** : le minimum
  d'allocations par appel tombe à 2, la médiane ne bouge pas.
- **Il fait varier la mémoire retenue** de tout appel qui calcule des yields complexes
  (section 2.16).

### 2.2 Ce qui alloue à chaque appel (lu au lot 0, mesuré au lot 1)

- **La clé du cache**, construite à chaque recherche, même quand la valeur est trouvée
  (`FMTYieldsCache::_getKey`) : `FMTYieldDevelopment` copie le bitset du masque
  (`boost::dynamic_bitset<uint8_t>`) et le nom du yield (`std::string`). Le nom n'alloue qu'au
  delà de 15 caractères (petites chaînes de MSVC) : **la longueur des noms de yields change le
  nombre d'allocations**. Garder les mêmes noms entre la référence et le candidat.
- **La garde de récursion** : `m_lookat.insert(yld)` ajoute un nœud à un
  `mutable std::unordered_set<std::string>`, membre de la classe de base
  (`Include/FMTYieldHandler.h:374`).
- **Les calculs privés** construisent des vecteurs et une `std::map<std::string, double>` à
  chaque appel. Exemple, `_getShift` (`Source/FMTComplexYieldHandler.cpp:736-754`) : une copie
  du `FMTDevelopment`, deux appels à `getValues()` et un à `getSources()` qui rendent chacun
  un vecteur, puis `_getData` et `_toMap`.
- **Mesuré** (requête déjà préparée, sondes du lot 1 et section 7.1) :

  | Chemin | Allocations par appel | Durée |
  |---|---|---|
  | Calcul complet : `_SUM`, `_MULTIPLY` | 4 | ~0,3-0,4 µs |
  | Calcul complet : `_DIVIDE`, `_SUBTRACT` | 5 | ~0,35 µs |
  | Calcul complet : `_SHIFT` | 13 | ~0,6 µs |
  | Calcul complet : `_EQUATION(VOLUMETOTAL+VOLUMETOTAL)` | 31 | ~2,5 µs |
  | Calcul complet : chaîne de 5 `_SUM` | 20 | ~1,6 µs |
  | Valeur servie par le cache, tout opérateur | 2 (4 octets) | ~0,1 µs |
  | Valeur mise au cache | 2 de plus ; la première insertion alloue aussi la table (~2,4 Ko) | — |
  | Premier appel d'une requête neuve (`FMTYieldRequest::_updateData`) | 7 de plus | — |

  Même servie par le cache, une valeur coûte donc 2 allocations : la clé. #348 devra s'en
  occuper si le cache reste.
- **Mesuré au lot 2** (section 7.1) :

  | Chemin | Allocations par appel | Durée |
  |---|---|---|
  | Yield d'âge par une requête préparée (`Yield.Age`) | 0 | ~60 ns |
  | Même yield par une requête neuve (`Yield.Age.NewRequest`) | 7 (98 octets) | ~280 ns |
  | Appartenance d'un masque à une sélection (`Mask.IsSubsetOf`) | 0 | ~2 ns |
  | Masque construit depuis son texte (`Mask.FromString`) | 7 (197 octets) | ~390 ns |

  Les 7 allocations d'une requête neuve sont celles de `FMTYieldRequest::_updateData`
  (`filterMask`, puis `findSetsWithFiltered`), déjà vues au lot 1.

### 2.3 `m_lookat` après une exception (à démontrer)

Quand une opération lève une exception, le `m_lookat.erase(yld)` de la l. 864 est sauté : le
nom reste dans l'ensemble, et une évaluation suivante du même yield par le même gestionnaire
pourrait lever « Recursivity detected ». #348 le signale (« recursion cleanup may not occur
when an operation throws »). Rien n'est démontré : appliquer la règle 10 avant d'en parler
comme d'un défaut. Hors du lot 1.

### 2.4 La clé du cache tronque l'âge et la période (à démontrer)

`FMTYieldDevelopment` range l'âge et la période sur 8 bits (`static_cast<uint8_t>`,
`Source/FMTYieldDevelopment.cpp:6`) : les âges 7 et 263 d'un même développement auraient la
même clé. La portée est probablement nulle, un âge ou une période au-delà de 255 étant rare ;
rien n'est démontré. Le benchmark en tient compte : ses 255 clés sont les périodes 1 à 255.

### 2.5 `hasFeature("ONNXRUNTIME")` rend toujours faux (démontré au lot 1)

- **Cause** : `Version::FMTVersion::hasFeature` teste `#ifdef FMTWITHTONNXR`
  (`Source/FMTVersion.cpp:91`), alors que la définition du build est `FMTWITHONNXR`
  (`CMakeLists.txt:217-218`).
- **A/B dans le même build** : `hasFeature("GDAL")` rend vrai, `FMTWITHGDAL` étant défini ;
  `hasFeature("ONNXRUNTIME")` rend faux, alors que `FMTWITHONNXR` est défini et que
  `FMTlib.dll` importe `onnxruntime.dll`. Constaté avec le `FMTlib.dll` du build de `new_test`.
- **Origine** : `45230dc8` (2021-11-10, « cmake support for onnxruntime with FMT »), depuis
  l'ajout du test. Aucun appelant dans le dépôt.
- **Ici** : le champ `features` des résultats ne contient jamais `ONNXRUNTIME`.
- Pas corrigé (règle 10) : issue #362, publiée le 2026-09-29.

### 2.6 mimalloc est lié à FMTlib mais n'est jamais chargé (démontré au lot 1, réglé par #361)

- **Cause** : `CMakeLists.txt:141-148` lie FMTlib à mimalloc et affiche « mimalloc was found
  successfully! », mais aucun code de FMT n'appelle une fonction de mimalloc. L'éditeur de liens
  de MSVC ne garde alors pas la dépendance : `mimalloc.dll` n'est jamais chargé, et `malloc`
  reste servi par le tas du CRT. La note d'usage de mimalloc, que vcpkg affiche à chaque
  configuration (`build/release/vcpkg-manifest-install.log`), décrit ce cas.
- **A/B** : une DLL liée à `mimalloc.lib` sans appel à mimalloc ne dépend pas de
  `mimalloc.dll` ; la même, liée avec `/INCLUDE:mi_version`, en dépend.
- **Build de Gabriel** : aucune DLL ni aucun exécutable de `bin/Release` n'importe
  `mimalloc.dll`. À l'exécution (`MIMALLOC_VERBOSE=1`), le seul mimalloc qui démarre est une
  copie plus ancienne intégrée à `mosek64_10_1.dll`, qui ne sert que MOSEK. Selon ses
  statistiques, elle garde 12 Mio engagés, sur les 20 Mio environ de mémoire privée d'un
  processus du banc.
- **Origine** : `2c439c0d` (2026-08-13), sur `master` et dans `archive/v1.3.0`.
- **Effet mesuré** : l'exécutable du banc, compilé hors CMake, une fois tel quel (tas du CRT) et
  une fois lié à mimalloc en premier avec `/INCLUDE:mi_version` (mimalloc affiche alors
  « malloc is redirected ») ; même commit, une mesure complète de chaque côté.

  | Benchmark | Tas du CRT (ns) | mimalloc 2.1.2 (ns) | Écart |
  |---|---|---|---|
  | `ComplexYield.Sum` | 380,0 | 281,5 | -25,9 % |
  | `ComplexYield.Multiply` | 309,4 | 223,8 | -27,7 % |
  | `ComplexYield.Divide` | 337,7 | 249,5 | -26,1 % |
  | `ComplexYield.Subtract` | 336,5 | 245,6 | -27,0 % |
  | `ComplexYield.Shift` | 614,4 | 379,5 | -38,2 % |
  | `ComplexYield.Equation` | 2482,5 | 1913,6 | -22,9 % |
  | `ComplexYield.RecursiveChain` | 1519,9 | 1124,5 | -26,0 % |

  Nombre et octets des allocations inchangés : **le moniteur compte de la même façon sous
  mimalloc**. Pic de mémoire privée : 21,1 Mo sur le tas du CRT, 53,6 Mo sous mimalloc.
- **Conséquences** :
  - chaque résultat note son allocateur (décision 7), et `CompareResults.cmake` avertit quand
    les allocateurs diffèrent ;
  - pour #348, ne pas changer d'allocateur entre l'avant et l'après. Sous mimalloc, une
    allocation coûte moins cher : le gain en temps de #348 y serait plus petit que sur le tas
    du CRT.
- **Le banc sous mimalloc** (vérifié le 2026-10-05, en vue de #361), avec l'exécutable du scratchpad
  lié à mimalloc comme ci-dessus :
  - le nombre et les octets des allocations restent identiques sur les flux, les lectures, les
    masques et les yields ;
  - la mémoire retenue était fausse : sous mimalloc, `_msize` rend la taille de classe du bloc
    (16 octets pour 10 demandés, 32 pour 17, 1 024 pour 1 000), que le moniteur déduisait à la
    libération alors qu'il ajoutait la taille demandée à l'allocation. `Flow.Optimize` retenait
    -426 524 octets, la lecture d'un projet -127 237. Corrigé (section 2.8) : -992 et 0 ;
  - les bornes posées sous le tas du CRT ne tiennent plus : le pic d'un processus du banc passe
    de 22 à 29 Mo à 54 Mo, celui de `Flow.Replanning` de 62 à 87 Mo en mode court, et la
    replanification retient 5 242 144 octets, ses blocs comptant leur taille de classe, au-delà
    de sa borne de 5 Mo. Changer d'allocateur demande de mesurer de nouveau les bornes de pic et
    les bornes de mémoire retenue non nulles.
- **#361** (2026-10-05 au 2026-10-07, décision 17) :
  - **Chargé tard, mimalloc ne redirige pas.** `mimalloc-redirect.dll` refuse de basculer s'il
    démarre après `ucrtbase.dll` (« seems to be initialized after ucrtbase.dll », puis « standard
    malloc is _not_ redirected »). Le refus ne s'affiche qu'avec `MIMALLOC_VERBOSE=1`, et rien ne
    plante. Démontré dans Python 3.10.7 et dans un hôte .NET 10. L'interface (.NET 8), Excel et
    Python chargent FMTlib tard : ils restent sur le tas du CRT, quoi que fasse FMTlib.
  - **Le correctif d'abord proposé ne suffit pas.** Une FMTlib liée avec `/INCLUDE:mi_version` ne
    redirige pas non plus dans le banc, qui importe Boost avant FMTlib. Elle ne se chargerait plus
    dans la version 1.4.1, qui ne livre pas `mimalloc.dll`. La 1.4.2 livre `mimalloc.dll` et
    `mimalloc-redirect.dll`, mais aucun de ses binaires ne les importe.
  - **Ce qui marche** : lier mimalloc en premier à l'exécutable, avec `/INCLUDE:mi_version`
    (`mimalloc.dll` en tête de `dumpbin /DEPENDENTS`).
  - **Mécanisme retenu** : FMTlib ne lie plus mimalloc. Avec `-DWITH_MIMALLOC=ON`,
    `createexecutable` le lie en premier à chaque exécutable ; `MIMALLOC_DISABLE_REDIRECT=1` coupe
    alors la redirection pour un passage, et le test `FMTPerformanceTests.Mimalloc` vérifie que le
    banc tourne sous mimalloc. Par défaut, rien ne change. Vérifié le 2026-10-06 sur le build de
    Gabriel : aucun exécutable n'importe mimalloc, le banc affiche `CRT heap`, et la suite base
    passe (254 tests, 239 verts et 15 désactivés).
  - **Ordres de grandeur sous mimalloc**, pour une future interface. Mesurés le 2026-10-06 sur un
    build lié à mimalloc, compilé avant le commit. Les nombres d'allocations ne changent pas, et la
    suite base passe, sauf une borne de mémoire sur chacun des six flux publics. Pics en Mo : sur le
    tas du CRT, le plus haut des mesures complètes de la section 7.5 ; sous mimalloc, le plus haut
    de quatre passages seuls, deux courts et deux complets. Sous `-j 8`, les pics sous mimalloc
    montent jusqu'à 7 % plus haut.

    | Flux | Tas du CRT | mimalloc 2.1.2 |
    |---|---|---|
    | `Flow.Optimize` | 28,7 | 51,7 |
    | `Flow.Optimize.Long` | 62,5 | 79,7 |
    | `Flow.Replay` | 24,2 | 51,8 |
    | `Flow.Outputs` | 23,2 | 51,7 |
    | `Flow.Simulate` | 28,5 | 55,6 |
    | `Flow.Replanning` | 76,0 | 82,8 |
    | `Flow.Optimize.Bfec` | 582,4 | 705,0 |
    | `Flow.Replay.Bfec` | 942,7 | 961,4 |
    | `Flow.Outputs.Bfec` | 220,9 | 248,1 |
    | `Flow.Replanning.Bfec` | 348,5 | 416,1 |
    | `Yield.Model.Bfec` | 206,2 | 211,2 |

    Mémoire retenue sous mimalloc, en tailles de classe : 5 242 144 octets par replanification
    (4 737 039 sur le tas du CRT). Au pire cas du cache : 22 128 octets pour `Flow.Outputs` (19 987
    sur le tas du CRT), 7 476 848 pour `Flow.Optimize.Bfec`, 51 040 pour `Flow.Replay.Bfec`,
    25 468 176 pour `Flow.Outputs.Bfec` et 12 747 648 pour `Flow.Replanning.Bfec`.
  - **Le mimalloc de MOSEK brouille `MIMALLOC_VERBOSE=1`** : ses lignes « mimalloc: » s'affichent
    même sans le mimalloc de vcpkg. Seules les lignes « mimalloc-redirect: » parlent de ce dernier.
  - **Risque d'un build lié à mimalloc** : un logiciel de sécurité qui injecte une DLL avant
    mimalloc, et qui alloue avant la bascule, fait planter le démarrage. C'est l'issue 1221 de
    mimalloc, sur mimalloc 3.0.3 ; non démontré avec la 2.1.2, à revoir si vcpkg passe à mimalloc 3.
    `MIMALLOC_DISABLE_REDIRECT=1` le contourne.
  - **Pour l'interface**, les pistes et leurs risques sont dans
    `FMTWrapperCore/INTERFACE_MIGRATION.md`.
- **Issue** : #361, publiée le 2026-09-29, réglée par le commit de #361 (décision 17).

### 2.7 Threads (examiné au lot 4)

- `m_lookat` appartient au gestionnaire de yields, partagé par tous les threads, et il est écrit
  sans verrou. Le lien avec la course connue des caches de yields
  (section 7 de `Examples/C++/tests/ETAT.md`) n'est pas établi.
- **Aucune course visible sur la replanification** (lot 4) : sur 1, 2, 5 et 10 fils, chaque appel
  écrit exactement les lignes de la même replanification sur un fil, vérifiées par une empreinte des
  lignes triées. 24 passages de stress à 5 et 10 fils (72 replanifications) n'ont montré aucune
  différence, ni le modèle privé sur 5 fils comme sur un fil. La replanification copie ses modèles
  pour chaque réplicat et chaque période ; la course connue touche des modèles spatiaux, que le banc
  ne mesure pas.

### 2.8 Comptage des allocations

- **`operator new` est lié dans chaque module.** Selon `dumpbin /IMPORTS` sur
  `build/release/bin/Release/FMTlib.dll`, la DLL importe `malloc`, `free`, `calloc` et
  `_callnewh` de `api-ms-win-crt-heap-l1-1-0.dll`, et aucun `operator new`.
- **`FMTlib.dll` importe aussi `HeapAlloc` et `HeapFree`** de `KERNEL32`, que le compteur ne
  voit pas. Sur la lecture de TWD_land et l'évaluation des yields, la sonde du lot 1 n'en a vu
  aucun appel direct.
- **Plusieurs modules allouent.** La lecture de TWD_land alloue dans `FMTlib.dll` (13 766
  allocations), `boost_filesystem` (777) et `MSVCP140` (29) : d'où la redirection dans tous
  les modules chargés. Un module lié à une bibliothèque C statique échappe au compteur. MOSEK,
  lui, est compté (vérifié au lot 3) : les piles du diagnostic de la section 2.16 montrent ses
  blocs.
- **Le code en ligne compte pour l'exécutable.** Une fonction définie dans un en-tête de FMT
  (modèle, fonction `inline`, constructeur par défaut) est compilée dans l'exécutable du
  benchmark, pas dans `FMTlib.dll` ; ses allocations sont comptées aussi.
- **Les DLL chargées tard** (pilotes GDAL, par exemple) : la redirection est refaite avant
  chaque passage compté.
- **Point d'entrée depuis un exécutable** : `FMTYieldRequest` n'est pas exportée, mais
  `FMTYields::get(développement.getYieldRequest(), nom)` fonctionne, comme dans
  `Examples/C++/testScenarioReading.cpp:285-286`.
- **Les octets vivants comptent la taille que rend `_msize`**, à l'allocation comme à la
  libération (corrigé le 2026-10-05). Le moniteur ajoutait la taille demandée et déduisait celle de
  `_msize`, qui en diffère dans deux cas. La bibliothèque C donne un octet à `malloc(0)`,
  `calloc(0, n)` et `operator new(0)` : la mémoire retenue baissait d'un octet par bloc de taille 0
  alloué puis libéré, 7 octets sur les flux publics, 189 sur l'optimisation privée. Et mimalloc rend
  la taille de classe du bloc (section 2.6). La mémoire retenue est maintenant exacte sous tout
  allocateur ; sous le tas du CRT, seuls les blocs de taille 0 changent. Les octets alloués restent
  les tailles demandées, et se comparent toujours à la référence de la section 7.1.

### 2.9 Données

- **TWD_land est petit** (8 strates, 1814,76 ha) : le temps d'un flux de modèle y sera dominé
  par des coûts fixes. Pour une taille « moyenne », allonger d'abord l'horizon (lot 3) ; un
  modèle synthétique seulement si ça ne suffit pas (lot 5).
- **Les défauts #346 et #347 contraignent le choix des yields** : pas de `^` dans une
  équation, et pas d'équation numérique dans le bloc `*YC` des opérateurs mesurés
  (section 2.7 de `Examples/C++/tests/ETAT.md`). `perfyields` respecte ces deux conditions.
- **Ne jamais modifier les sections racine de TWD_land** : tous les tests de chaîne en
  dépendent. Un scénario par besoin, et `perfyields` ne se modifie plus : ce serait changer ce
  que mesurent ses benchmarks.

### 2.10 Mesure

- **Sous `ctest -j 8`, un temps ne vaut rien** : les tests se ralentissent entre eux. Le mode
  court ne fait que valider ; la mesure complète tourne sans `-j`.
- **Pause au démarrage d'un processus** (2026-09-30) : sur ce poste, un processus qui vient de lire
  son premier fichier est suspendu de 27 à 38 ms, de 45 à 70 ms plus tard, et tourne environ 20 %
  moins vite jusque-là. Le banc lit `performance.csv` en démarrant.
  - Démontré avec une petite sonde sans FMT qui lit un fichier puis calcule : même pause. Sans la
    lecture, aucune. Aucun autre fil du processus ne travaille pendant la pause : la cause est hors
    du processus. Microsoft Defender tourne sur le poste ; son rôle n'est pas démontré, et on ne le
    désactive pas.
  - Après une attente de 300 ms, plus de pause ni de ralentissement. Un inventaire des fils du
    processus (Toolhelp), fait juste avant la mesure, les fait disparaître aussi, sans qu'on sache
    pourquoi.
  - Effet sur une mesure complète, un processus par benchmark comme sous ctest (A/B du même
    exécutable, avec et sans attente, deux mesures de chaque) : un échantillon trop long d'environ
    30 ms dans chaque benchmark, l'écart type à 76-190 % de la médiane au lieu de 1,4-6,4 %, le
    plus lent échantillon à 5-8 fois la médiane au lieu de 1,2 au plus. Les médianes des appels
    courts bougent de -4,4 à +1,1 %, dans le bruit ; celle de `Parser.ReadProject` est trop haute
    d'un tiers (5,1 ms au lieu de 3,7 ms).
  - C'est ce que le lot 1 avait pris pour un « premier échantillon parfois lent » : sa référence
    (section 7.4) l'a subie dans chacun de ses benchmarks, sans effet notable sur ses médianes.
  - Depuis le 2026-09-30, une mesure complète attend 250 ms avant son premier benchmark
    (`BenchmarkSuite.cpp`). Le mode court n'attend pas : ses temps ne sont pas une mesure.
- **Deux mesures complètes du même commit** diffèrent de quelques pour cent (de -2,0 % à
  +0,8 % au lot 1, jusqu'à 8 % pour la référence de la section 7.1) : un écart de cet ordre n'est
  pas un changement. Une mesure entière peut aussi se décaler : le 2026-10-05, la deuxième de trois
  mesures publiques était plus lente de 6 à 22 % sur les micro-benchmarks, sans cause trouvée. Une
  boucle qui lance un processus chaque seconde n'en est pas la cause : elle ne change aucune
  médiane (A/B). D'un jour à l'autre, avec un harnais qui a changé entre-temps, les benchmarks des
  lots 1 et 2 s'écartent de -12 à +12 % de la référence, à allocations égales. Pour juger un
  changement, mesurer l'avant et l'après le même jour, au moins trois fois chacun, et écarter une
  mesure décalée tout entière.
- **L'en-tête du binaire note le commit au moment de la compilation.** Gabriel compile et teste
  avant de commiter, pour ne jamais pousser une erreur de compilation : le binaire porte donc le
  commit précédent, « modifié ». Pour une mesure de référence, recompiler après le commit : seul
  `BenchmarkEnvironment.cpp` inclut l'en-tête, et l'exécutable se relie en quelques secondes.
- **Windows ignore la casse des noms** : `Tests/Performance` et `tests/performance` sont le même
  dossier sous `build/release`. L'en-tête généré du commit va donc dans
  `generated/FMTBenchmarkHarness`, à part des résultats.
- **`PATH` sous Git Bash** : un chemin `C:/...` y est coupé au deux-points. Pour lancer un
  exécutable de la sonde, ajouter le `bin/Release` du build au `PATH` sous la forme `/c/...`,
  sans quoi il rend 127 (DLL introuvable).
- **Le vrai build compile en `/W1`** : les contrôles du scratchpad, en `/W4`, sont plus
  sévères ; un avertissement de niveau 2 à 4 n'apparaît pas dans la sortie du build de Gabriel.
- **Les messages du build ne sont écrits nulle part.** `CMakeFMTVS2022vcpkg_GL.bat` écrit dans
  la console seulement, et MSBuild ne laisse pas de journal par projet. Pour les garder, dans
  PowerShell : `.\CMakeFMTVS2022vcpkg_GL.bat 2>&1 | Tee-Object -FilePath build\release\build-bat.log`.
  Le journal va sous `build/` : un fichier non suivi à la racine rendrait `dirty` les
  résultats. Après coup, ils se reconstituent sans toucher au build :
  - chaque commande `cl` est dans `build/release/<cible>.dir/Release/*.tlog/CL.command.1.tlog`
    (UTF-16) ; la rejouer en envoyant `/Fo` et `/Fd` dans le scratchpad ;
  - la configuration se rejoue dans un autre dossier, avec les paquets du build :
    `-DVCPKG_INSTALLED_DIR=<build>/vcpkg_installed -DVCPKG_MANIFEST_INSTALL=OFF` (14 s).
- **Processeur hybride** : le i9-13900 a 16 processeurs logiques performants (0 à 15) et 16
  efficaces (16 à 31), qui font le même calcul jusqu'à 1,7 fois plus lentement.
  - Une mesure complète de tous les benchmarks a vu `ComplexYield.RecursiveChain` passer de 1 534
    à 2 311 ns, régulièrement (écart type de 20 ns) : Windows l'avait vraisemblablement gardé sur
    des cœurs efficaces.
  - Forcé sur les cœurs efficaces, il fait 2 620 ns ; sur les cœurs performants, 1 536 ns.
  - Le mode efficacité de Windows (EcoQoS), qu'il applique à un processus en arrière-plan,
    reproduit l'effet : 2 583 et 2 649 ns.
  - Depuis le lot 2, le banc sort de ce mode et garde le fil qui mesure sur les cœurs les plus
    rapides (`ProcessorPolicy`) : sous EcoQoS, 1 493 et 1 596 ns.
  - La référence du lot 1 (section 7.4) a été prise avant ; ses valeurs sont celles des cœurs
    performants.
- Les pièges communs avec les tests (encodage cp1252 des sources, macros de `windows.h`,
  chemins de plus de 260 caractères, `onnxruntime.dll` à côté des sondes, `LastTest.log` que
  tout appel de ctest réécrit) sont en section 7 de `Examples/C++/tests/ETAT.md`. Ils ne sont pas
  recopiés ici.

### 2.11 Lecture d'un projet (mesurée au lot 2)

- **Une lecture de la racine de TWD_land** fait 13 262 allocations (1,87 Mo) et dure environ
  3,7 ms. Avec le scénario `perfyields` dans le même appel : 19 930 allocations (2,88 Mo) et
  environ 4,8 ms, soit 30 % de temps de plus pour un second modèle.
- **Aucune mémoire retenue** : une fois ses modèles détruits, une lecture ne laisse rien, sur 20
  lectures de suite. Ni fuite ni cache qui grandit.
- **Le nombre d'allocations dépend du chemin du projet** : 13 262 avec un chemin de 78
  caractères, 13 288 avec un chemin de 161 caractères plus profond. Une borne exacte échouerait
  selon l'endroit où le dépôt est cloné (règle 13).
- **Le journal de FMT est commun au processus** (`FMTObject::_logger` est statique) : rendu
  silencieux dans la préparation, il fait taire les lectures mesurées (règle 7). Depuis le lot 3,
  `quietFmt()` le règle une seule fois par processus (règle 14).

### 2.12 Remplacer le journal de FMT fait échouer les copies de modèles (démontré au lot 3)

- **Constat** : la replanification de TWD_land échouait dans un processus sur quatre, au réplicat
  1 ou 2 : FMTexc 53, « Cannot copy solver », puis une violation d'accès (0xC0000005), sous MOSEK
  comme sous CLP, avec ou sans le moniteur d'allocations. Le benchmark appelait
  `FMTTaskHandler::setQuietLogger()` après avoir construit les modèles ; `replanningtest`, qui ne
  l'appelle pas, passe 8 fois sur 8.
- **Cause dans le code** :
  - `FMTObject::_logger` est un `std::unique_ptr` statique (`Source/FMTObject.cpp:80`) ;
    `setQuietLogger()` passe par `passInLogger`, qui le remplace par un clone et détruit l'ancien
    journal (ligne 281) ;
  - le constructeur de `FMTLpSolver` donne à l'interface OSI un pointeur nu vers le gestionnaire
    de messages du journal (`passInMessageHandler(*_logger)`, `Source/FMTLpSolver.cpp:152` et 541) ;
  - le constructeur de copie ne le renouvelle pas (`//passInMessageHandler(*_logger);`, ligne 110) :
    une copie hérite du pointeur, puis `copySolverInterface` clone le solveur et appelle
    `resolve()`, qui passe par ce gestionnaire détruit.
- **A/B minimal** (sonde du scratchpad, `FMTlib.dll` du build de Gabriel) : un modèle `NOT_MASK`
  optimisé, copié 2 000 fois. Sans remplacer le journal, aucune copie en échec sur 8 passages.
  Avec un `setQuietLogger()` entre l'optimisation et les copies, 8 passages sur 8 touchés : des
  copies en échec (1 et 1 827 sur 2 000), ou un arrêt net du processus, sans message (6 fois).
- **Origine** : le pointeur nu date de `bdd1997b` (2020-07-31), le journal global de `4c2111f5`
  (2021-12-07), la copie sans renouvellement de `4696d810` (2022-01-31). L'ancien journal est
  détruit depuis `e7737e41` (2024-05-24). Les deux exemples de replanification ont l'appel
  `handler.setQuietLogger()` en commentaire.
- **Dans le banc** : `quietFmt()` règle le journal une seule fois par processus, avant tout modèle
  (règle 14). La replanification passe alors dans 12 processus sur 12. Le défaut donne une issue
  (règle 10) : #363, publiée le 2026-10-05.

### 2.13 La replanification retient 4,7 Mo par fil (cause démontrée au lot 4)

- Chaque exécution de `FMTReplanningTask` lancée sur un fil par `conccurentRun` laisse de 4 735 432
  à 4 737 022 octets alloués après la destruction de la tâche et des modèles. C'est vrai avec 1, 2, 4
  ou 8 réplicats, sur 5 ou 10 périodes, sur TWD_land comme sur le modèle privé : une taille fixe, qui
  ne grandit pas d'un réplicat à l'autre. Le pic du processus monte d'autant à chaque exécution. Une
  fois corrigé le comptage des blocs de taille 0 (section 2.8), la mesure donne 4 737 039 octets, sur
  TWD_land comme sur le modèle privé.
- Le moniteur ne comptait que le fil courant, et le travail se fait dans un fil du gestionnaire de
  tâches : il ne le voyait pas. La replanification compte maintenant tous les fils.
- **Cause** (lot 4, section 2.17) : un bloc de 4 737 568 octets que MOSEK alloue à la première
  optimisation de chaque fil, et ne rend jamais. Un seul fil en garde un : la taille est fixe. Lancée
  comme dans l'interface, avec un fil par réplicat, la replanification en garde un par réplicat.
- La borne de mémoire retenue de `Flow.Replanning` est de 5 Mo : elle échouerait si la rétention se
  mettait à grandir avec les réplicats. La replanification privée calcule des yields complexes : sa
  borne couvre aussi le cache (section 2.16).

### 2.14 Coûts d'un modèle de production (mesurés au lot 3)

- **Calculer tous les outputs** d'un modèle optimisé de 1 526 outputs sur 5 périodes prend 62 s et
  723 millions d'allocations, contre 1,8 s pour le lire, 1,6 s pour le construire et 0,05 s pour
  le résoudre. Sur un modèle construit avec sa cédule, le calcul complet fait 17,6 millions
  d'allocations et dure 1,1 s, le premier 6,9 s.
- **La replanification privée** (1 réplicat de 5 périodes) alloue 375 Go en cumulé par exécution,
  en 42 millions d'allocations : 8,9 Ko en moyenne.
- **Le premier appel d'un solveur** coûte environ 60 ms de plus (MOSEK) : la mesure complète le
  laisse au réchauffement.
- **Le rejeu d'une cédule** passe par le présolve de `doPlanning` : sur TWD_land, 23 ms de
  construction, contre 1,4 ms sans présolve.

### 2.15 Données des flux

- **Les valeurs replanifiées dépendent du solveur** : pour les mêmes réplicats de TWD_land, le
  modèle local récolte 447 126 m³ sous MOSEK et 465 421 m³ sous CLP. Plusieurs solutions optimales
  existent : le benchmark vérifie le nombre de lignes écrites (règle 8).
- **La cédule de `NOT_MASK` n'est valide que jusqu'à la période 7** : à la période 8, elle demande
  une coupe totale d'un développement inopérable. Le rejeu prend celle de `LP`, valide sur ses
  10 périodes.
- **Sur le modèle privé, un output au moins ne rend pas de total** : `.at("Total")` y lève
  « invalid map<K, T> key ». La somme de `Flow.Outputs` saute ces outputs.
- **Un yield manquant d'un modèle de production** (FMTexc 42) est une erreur par défaut ;
  `doplanning` en fait un avertissement, et les flux font de même (`quietFmt`).

### 2.16 La mémoire retenue d'un calcul de yields complexes dépend du temps (démontré à la validation du lot 3)

- **Constat** : sur le build de Gabriel, `Flow.Optimize.Bfec` a échoué une fois sous ctest, en
  mode court : l'appel compté retenait 1 584 octets, pour une borne de 0. Six passages directs
  ont ensuite rendu -974 octets, la valeur du 2026-10-02.
- **Deux autres insertions le même jour**, sous les nouvelles bornes : `Flow.Optimize.Bfec` a
  retenu 1 773 octets à la première mesure complète (-785 + 2 558, la même insertion), et
  `Flow.Outputs` 2 418 octets en mode court, avec l'exécutable du scratchpad (2 400 + 16 + 2 : une
  valeur, dont le masque tient en 2 octets sur TWD_land). Avec une borne de 0, cette ligne publique
  aurait échoué dans la suite base.
- **Cause** : le cache des yields complexes (section 2.1). Une valeur n'y entre que si son calcul
  a duré plus de 0,05 ms ; la première entrée alloue la table du cache, et chaque entrée garde une
  copie du masque de sa clé. Quand aucun calcul de l'appel chronométré n'a dépassé le seuil, le
  cache est encore vide : un calcul ralenti pendant l'appel compté, par une préemption par
  exemple, y laisse la table et sa valeur, qui survivent à l'appel.
- **Démonstration**, par un moniteur de diagnostic gardé hors du dépôt, qui note la pile d'appel
  de chaque bloc encore vivant à la fin de l'appel compté :
  - les blocs retenus naissent dans la fonction que `FMTComplexYieldHandler::get` appelle après
    avoir comparé la durée du calcul au seuil. Dans `FMTlib.dll` désassemblée, cette comparaison
    lit la constante 0,05, et la fonction appelée appelle `FMTObject::getAvailableMemory` : c'est
    `FMTYieldsCache::set` et son `_clearIfTooBig`. Les blocs sont alloués pendant
    `FMTLpModel::build` ;
  - le compte tombe juste : 1 584 = -974 + 2 400 (la table) + 16 + 2 × 71 (deux entrées ; le
    masque du modèle compte 568 bits, soit 71 octets) ;
  - ralentir chaque allocation de 50 µs fait retenir 19 987 octets à `Flow.Outputs`, contre 0,
    et ne change rien aux flux qui ne calculent pas de yields complexes.
- **Benchmarks exposés** : `Flow.Outputs`, et les quatre flux privés `Flow.Optimize.Bfec`,
  `Flow.Replay.Bfec`, `Flow.Outputs.Bfec` et `Flow.Replanning.Bfec`. Les autres benchmarks dont
  la mémoire retenue est bornée n'appellent jamais `FMTComplexYieldHandler::get`, vérifié en
  ralentissant ses allocations : `Flow.Optimize`, `Flow.Optimize.Long`, `Flow.Replay`, `Flow.Simulate`,
  `Flow.Replanning`, `Yield.Age.NewRequest`, `Mask.FromString` et les deux lectures de projet ;
  `Yield.Model.Bfec` n'alloue rien. Leurs bornes tiennent.
- **Pire cas** : en ralentissant de 60 µs les allocations que fait `FMTComplexYieldHandler::get`
  pendant l'appel compté, chaque valeur calculée entre au cache, et la mémoire retenue devient le
  plus que le cache peut garder en un appel. `Flow.Outputs` : 19 987 octets, pour 190 valeurs ;
  optimisation privée : 6 957 138 octets ; rejeu privé : 49 199 ; outputs privés : 24 744 735 ;
  replanification privée : 11 833 143, dont les 4,7 Mo de la section 2.13. Les résultats restent
  justes.
- **Conséquence** : la borne de mémoire retenue d'un benchmark exposé est ce pire cas plus 10 %
  (règle 13, section 3.3). Elle ne décèle une fuite qu'au-delà ; le pic du processus reste la
  garde principale. Le pire cas des outputs privés (24,7 Mo) dépasse la marge de leur pic
  (22 Mo) : il faudrait que presque tous leurs calculs ralentissent pendant la même mesure.
- **Ce n'est pas un défaut de FMT** : le cache fait ce qu'il doit. Mais la vitesse de chaque
  calcul décide de son contenu, donc des allocations et de la mémoire retenue de tout calcul de
  yields complexes.
- **En passant** : la présolve de `doPlanning` copie un modèle à solveur, et la copie relance
  MOSEK (`copySolverInterface`), même pour la simulation. MOSEK y libère 785 octets alloués à
  l'appel précédent : d'où les -792 octets des flux publics, -785 une fois corrigé le comptage
  des blocs de taille 0 (section 2.8).

### 2.17 Un fil par réplicat : la mémoire de la replanification grandit avec les réplicats (démontré au lot 4)

- **Constat** : la replanification lancée comme dans l'interface retient environ 4,7 Mo de plus par
  réplicat, pour toute la vie du processus. Sur le modèle privé, `replanner.exe` du build, sur un fil,
  atteint 298 Mo de mémoire privée avec 1 réplicat, 327 Mo avec 5 et 354 Mo avec 10.
- **Cause dans le code** :
  - `FMTTaskHandler::onDemandRun` (`Source/FMTTaskHandler.cpp`) lance chaque tâche dans un nouveau
    fil (`FMTWorkerTask`, `Source/FMTWorkerTask.cpp`), et `FMTReplanningTask::spawn` donne une tâche
    par réplicat : un fil par réplicat, quel que soit le nombre de fils demandé ;
  - MOSEK alloue un bloc de 4 737 568 octets à la première optimisation d'un fil (`MSK_optimize`,
    appelée par `FMTLpSolver::initialSolve`), et ne le rend ni à la fin du fil, ni à la destruction
    des modèles. Le moniteur de diagnostic de la section 2.16 le montre : un tel bloc par fil, encore
    vivant à la fin.
- **A/B minimal** (sonde du scratchpad, replanification de TWD_land, mémoire retenue de tous les fils) :

  | Solveur | Lancement | 1 réplicat | 5 réplicats | 10 réplicats |
  |---|---|---|---|---|
  | MOSEK | `onDemandRun`, 1 fil | 10,5 Mo | 33,6 Mo | 57,3 Mo |
  | MOSEK | `conccurentRun`, 1 fil | 10,5 Mo | — | 10,5 Mo |
  | CLP | `onDemandRun`, 1 fil | 0,9 Mo | 0,9 Mo | 0,9 Mo |
  | CLP | `conccurentRun`, 1 fil | — | — | 0,9 Mo |

  Détruire aussi les modèles pendant le comptage n'y change rien : 56,4 Mo pour 10 réplicats.
- **À quoi MOSEK rattache le bloc** : au fil, et à l'optimiseur au point intérieur, que
  `FMTLpSolver` choisit par défaut. Une sonde sans FMT ni OSI appelle l'API C de MOSEK 10.1.16 sur un
  LP de 200 colonnes et 100 lignes, avec les réglages de `FMTLpSolver` (un fil) :
  - chaque fil qui optimise garde 4 737 824 octets, le bloc et 256 octets ; un fil qui optimise deux
    fois ne les garde qu'une fois ;
  - ni `MSK_deletetask`, ni la fin du fil, ni `MSK_deleteenv`, ni `MSK_licensecleanup` ne les rendent,
    et un environnement par fil n'y change rien ;
  - la licence prend 0,9 Mo une fois, que `MSK_deleteenv` rend ;
  - au simplexe dual, rien ne reste.

  La taille ne dépend pas du problème : la même sur ce LP, sur TWD_land et sur le modèle privé. FMT et
  OSI rendent ce qui leur appartient : détruire un modèle détruit sa tâche (`~OsiMskSolverInterface`,
  `MSK_deletetask`), et la dernière instance d'OSI détruit l'environnement (`MSK_deleteenv`). Ce n'est
  pas une fuite de `initialSolve` : MOSEK garde le bloc du fil pour la vie du processus. Aucune des
  fonctions essayées ne le rend, et sa documentation n'en cite aucune pour le faire.

  MOSEK 9.2.36 se comporte de la même façon. Un rapport en anglais pour le support de MOSEK, avec un
  programme de reproduction autonome qui mesure la mémoire privée du processus, a été remis à Gabriel
  le 2026-10-07 : MOSEK n'a pas de dépôt public pour son optimiseur, ses rapports de bogue passent par
  `support@mosek.com`.
- **Origine** : `onDemandRun` lance un fil par tâche depuis son introduction, `bfdc9936`
  (2021-12-02) ; `cf54b8ff` (2025-10-08) l'a réécrit avec `FMTWorkerTask`, sans changer ce principe.
  L'interface replanifie par `onDemandRun` (`FMTWrapperCore/Source/Planning.cpp`, qui reprend son
  comportement), comme l'exemple `replanner`.
- **En production** : une replanification de R réplicats garde environ R × 4,7 Mo jusqu'à la fermeture
  de l'interface, et ces blocs s'additionnent d'une replanification à l'autre dans la même session :
  4,7 Go pour 1 000 réplicats. `conccurentRun`, lui, ne crée qu'un fil par worker.
- **Dans le banc** : la famille `Flow.Replanning.Threads<N>` reproduit ce lancement ; chaque appel
  garde 51,6 Mo (10 réplicats), et le pic du processus grandit avec le nombre d'appels.
- Défaut de conception de FMT, révélé par un comportement de MOSEK ; pas corrigé (règle 10) : issue
  #366, publiée par Gabriel le 2026-10-07.

### 2.18 `onDemandRun` attend en boucle active (démontré au lot 4)

- Le fil qui lance la replanification interroge ses workers en boucle (`isDone`, puis `checkSignals`),
  sans jamais attendre (`Source/FMTTaskHandler.cpp`, `onDemandRun`) : il occupe un cœur pendant toute
  la replanification.
- **Mesuré** : dans la sonde, sur 10 réplicats de TWD_land et un fil, le fil principal consomme 0,312 s
  de processeur pendant les 0,383 s de la replanification (81 %), contre rien avec `conccurentRun`,
  qui attend ses fils par `join`. Sur le modèle privé, `replanner.exe` avec un seul worker occupe
  1,54 cœur en moyenne (51,8 s de processeur en 33,6 s).
- **Origine** : la boucle date aussi de `bfdc9936` (2021-12-02).
- Même issue que la section 2.17, #366 : un ensemble fixe de fils qui prennent les réplicats à tour de
  rôle corrigerait les deux.

### 2.19 Mise à l'échelle de la replanification (mesurée au lot 4)

- **TWD_land**, 10 réplicats de 5 périodes, mesure complète (section 7.6) : un appel va de 1,57 à 1,61
  fois plus vite sur 2 fils, de 2,46 à 2,59 fois sur 5 et de 2,99 à 3,11 fois sur 10 ; la phase de
  replanification seule, de 1,86 à 1,91, de 3,91 à 4,01 et de 5,38 à 5,47 fois. La lecture et la
  planification globale de départ (environ 53 ms) ne se partagent pas entre les fils. Chaque fil de
  plus coûte de 14 à 24 Mo de pic.
- **Modèle privé**, 10 réplicats de 5 périodes : 60,4 s par appel sur un fil (mode court) ; sur 5 fils,
  de 17,6 à 21,1 s selon les passages, de 2,9 à 3,4 fois plus vite. Pic de 425,5 Mo sur un fil ; sur
  5 fils, il varie selon le chevauchement des réplicats, de 1 123 à 1 386 Mo en mode court : de 175 à
  240 Mo par worker de plus.
- **Les résultats ne dépendent pas des fils** (section 2.7).
- **Les fils de FMT tournent sur les cœurs performants** depuis le lot 4 (`ProcessorPolicy`,
  section 3.4) : un worker sur un cœur efficace ralentirait la mesure jusqu'à 1,7 fois (section 2.10).
  `Flow.Replanning`, dont le worker tourne pendant que le fil qui mesure attend, reste dans le bruit :
  de 121,8 à 123,1 ms, contre 124,9 et 128,7 ms en section 7.5.

### 2.20 Un fichier de sortie tenu un instant par un autre processus (validation du lot 4)

- **Constat** : à la troisième mesure complète du 2026-10-07, sur le build de Gabriel,
  `Flow.Replanning.Threads2` a échoué en vidant son dossier de sortie au début d'un appel :
  `remove_all` rend l'erreur Windows 32, « le processus ne peut pas accéder au fichier car ce fichier
  est utilisé par un autre processus ». L'appel précédent avait fermé ces fichiers une quarantaine de
  millisecondes plus tôt ; deux minutes plus tard, ils étaient libres.
- **Pas dans le processus** : `FMTReplanningTask::finalize` ferme les fichiers
  (`FMTParallelWriter::close`) avant que `onDemandRun` ou `conccurentRun` ne rende la main ; les deux
  joignent d'abord leurs fils, `onDemandRun` en détruisant chaque `FMTWorkerTask`. Le banc ferme ses
  lectures.
- **Rare** : un échec sur environ 500 suppressions faites ce jour-là sur le build. Aucun en relançant
  60 fois le même benchmark (360 suppressions), ni avec une sonde sans FMT qui écrit, ferme et supprime
  les mêmes fichiers 3 500 fois. Le processus qui tenait le fichier n'est pas identifié ; l'antivirus
  et l'indexeur de Windows tournent sur le poste.
- **Même message** quand un autre processus tient un fichier du dossier ouvert sans partage en
  suppression.
- **Correction, dans le banc** : chaque replanification écrit dans un dossier à elle, `call<N>`, et
  `prepare` vide le dossier du benchmark hors mesure, en réessayant pendant une seconde (section 3.4).
  Vérifié : un fichier tenu 0,8 s est attendu ; tenu 3 s, il fait échouer le benchmark après une
  seconde, avec le même message. `Flow.Replanning` (lot 3) et les replanifications privées en
  profitent aussi.

## 3. Conception du banc

Le fonctionnement est décrit dans `Documentation/PerformanceTesting.md` : options, phases d'un
benchmark, moniteur d'allocations, schéma JSON, comparaison, ajout d'un benchmark. Restent ici,
lot par lot, les choix qui ne se lisent pas dans le code. Chacun attend la relecture de Gabriel.

### 3.1 Choix du lot 1 (validés par Gabriel le 2026-09-29)

- **Un benchmark par opérateur, sur 255 clés** (`ComplexYield.<opérateur>`) : chaque appel
  demande le yield du développement suivant parmi 255 qui ne diffèrent que par la période. La
  variante qui répétait la même clé est retirée : elle bascule au cache à la première
  préemption (section 2.1), donc sa mesure dépend du moment où ça arrive.
- **La borne porte sur la médiane** des appels comptés (règle 3).
- **L'exécutable lit `performance.csv` à l'exécution** ; CMake le lit aussi, pour inscrire une
  ligne ctest par benchmark. Changer une valeur ne demande donc pas de reconfigurer, ajouter une
  ligne oui. Les tests de chaîne, eux, figent leurs arguments dans `CTestTestfile.cmake`.
- **Noms ctest** : `FMTPerformanceTests.<benchmark>`, avec des chemins absolus passés par
  CMake, pour ne pas dépendre de la profondeur du dossier de build.
- **Passe chronométrée sans le moniteur** (redirection retirée), **passe comptée avec** : le
  comptage ne coûte rien aux temps.
- **Le harnais est une bibliothèque statique** (`FMTBenchmarkHarness`), sans dépendance aux
  benchmarks, pour servir aussi à `FMTCoreTests` (#350, lot 9).
- **L'état « modifié » compte les fichiers non suivis** : un benchmark pas encore commité ne
  doit pas faire croire qu'un résultat mesure exactement le commit.
- **Le `.cpp` principal s'appelle `FMTPerformanceTests.cpp`** : `createexecutable` nomme la
  cible d'après le fichier, et le banc se construit ainsi comme les exemples.

### 3.2 Choix du lot 2 (validés par Gabriel le 2026-09-30)

- **Réglages par benchmark** : `Benchmark::getSettings` rend par défaut ceux du lot 1. Une
  opération de quelques millisecondes, comme la lecture d'un projet, rend
  `RunSettings::forSlowCalls` : un appel par échantillon, 10 échantillons et 20 appels comptés en
  mesure complète, 2 et 3 en mode court.
- **Mémoire retenue par appel** : la différence des octets vivants avant et après chaque appel
  compté, que le moniteur du lot 1 suivait déjà. Borne facultative en cinquième colonne de
  `performance.csv` (règle 12), étiquette ctest `memory`, option `--max-retained-bytes`.
- **Schéma JSON 2** : champs `processors`, `retainedBytesPerCallMedian`,
  `retainedBytesPerCallMax`, `retainedBytes` et `maxRetainedBytesPerCall`. `CompareResults.cmake`
  lit encore le schéma 1.
- **La « mémoire retenue après des exécutions répétées » de #349** se vérifie par la mémoire que
  garde chaque lecture, sur 20 lectures en mesure complète, et non par la mémoire du processus
  après 100 lectures, comme le prévoyait le plan. La mesure du tas est exacte et déterministe ;
  celle du processus mêle ce que l'allocateur garde en réserve.
- **Les lectures de projet ne bornent pas leurs allocations** (règle 13, section 2.11) : seule leur
  mémoire retenue est bornée, à 0.
- **Un nouveau parseur à chaque lecture**, comme une application, et le journal de FMT rendu
  silencieux pour tout le processus dans la préparation.
- **`PerfYields` a ses propres fichiers**, partagés par les groupes `ComplexYield` et `Yield`, avec
  une instance par groupe. `Yield.Age` alterne entre les 255 requêtes, comme les yields
  complexes ; `Yield.Age.NewRequest` reprend toujours le même développement.
- **Masques** : la sélection `UC PROD ?` mêle deux agrégats et un `?`. `Mask.FromString` rend le
  nombre de bits du masque construit (8), qui se calcule à la main depuis `TWD_land.lan`.
- **`ProcessorPolicy`** : au démarrage, le processus sort du mode efficacité de Windows, et le fil
  qui mesure est restreint aux cœurs de la classe d'efficacité la plus haute, sans effet sur un
  processeur non hybride (section 2.10). L'environnement le note (`processors`), et la
  comparaison avertit quand les deux passages le connaissent et qu'il diffère.
- **Reportée au lot 3** : la variante des yields sur un gros modèle privé, qui a besoin de
  l'infrastructure du groupe privé (décision 8).
- **Ajoutée à la validation, après la relecture** : une mesure complète attend 250 ms avant son
  premier benchmark (section 2.10). Validée par Gabriel le 2026-10-02 : bénigne, et sa cause est
  connue.

### 3.3 Choix du lot 3 (validés par Gabriel le 2026-10-05)

- **Une classe par flux, la même pour le public et le privé** : `Flow.Optimize`, `Flow.Replay`,
  `Flow.Outputs`, `Flow.Simulate` et `Flow.Replanning`. Une ligne privée donne son modèle en
  arguments, dans une septième colonne.
- **Chaque appel relit le modèle**, comme une application. Seul `Flow.Outputs` prépare le sien une
  fois et ne mesure que le calcul des outputs, trop lourd sur un modèle de production pour rester
  une phase de l'optimisation (section 2.14).
- **Phases** déclarées par le benchmark et chronométrées dans les mêmes appels. Graphe et matrice
  ne se séparent pas sans toucher à FMTlib : `FMTLpModel::build` les construit période par période.
- **L'optimisation appelle `build()` puis `solve()`**, sans le présolve de `doPlanning`, pour
  séparer FMT du solveur ; l'objectif est le même (`presolvetest`). Le rejeu garde `doPlanning`,
  comme `FMTsetsolution`.
- **Réglages des flux** : 1 réchauffement, 3 échantillons d'un appel et 1 appel compté en mesure
  complète ; 1 échantillon et 1 appel compté en mode court. Un échantillon d'un seul appel n'est
  plus calibré, ce qui épargne aussi un appel aux lectures de projet.
- **Borne de pic mémoire** (décision 14) : sixième colonne `MAX_PEAK_MB`, option
  `--max-peak-memory`, étiquette `memory`. Elle n'est vérifiée que pour le premier benchmark d'un
  processus, puisque le pic est celui du processus.
- **Bornes de mémoire retenue** (corrigées à la validation, section 2.16) : 0 pour les flux qui ne
  calculent pas de yields complexes ; 5 Mo pour la replanification publique (section 2.13) ; pour
  `Flow.Outputs` et les flux privés exposés, le plus que le cache des yields peut garder en un
  appel, plus 10 %, arrondi au millier supérieur. La console ne propose plus d'abaisser une borne
  de mémoire retenue : un appel typique n'en montre pas le pire cas.
- **Groupe privé** (décision 8) : `performance-private.csv`, ignoré par git, étiquette `bfec-perf`
  seule. Le modèle doit être sur `T:\` : le nom du test, qui porte son chemin, l'écarte de la
  suite base. Le JSON note l'empreinte SHA-256 des fichiers du modèle (racine et scénarios lus),
  et la comparaison avertit quand elles diffèrent.
- **La replanification compte tous les fils** et vérifie le nombre de lignes écrites
  (section 2.15).
- **Les flux lisent en avertissements** les erreurs que `doplanning` change en avertissements.
- **Schéma JSON 3** : `datasetFingerprint`, `phases` et `maxPeakMemoryMB`.
- **Console** : durées en ns, ms ou s selon leur taille ; une ligne par phase, une pour la mémoire
  et une pour l'empreinte.
- **Reportées au lot 4** : la replanification à 5 fils et la mémoire par worker.

### 3.4 Choix du lot 4 (validés par Gabriel le 2026-10-07)

- **Lancement comme l'interface** : la famille `Flow.Replanning.Threads<N>` replanifie par
  `onDemandRun`, que l'interface utilise (`FMTWrapperCore/Source/Planning.cpp`), et non par
  `conccurentRun`, celui de `replanningtest`. `Flow.Replanning` (lot 3) garde `conccurentRun` sur un
  fil, pour ne pas changer ce qu'il mesure.
- **1, 2, 5 et 10 fils, 10 réplicats** : 5 comme en production, 10 au-delà des 8 cœurs performants
  physiques. Le scénario de feu de TWD_land donne des valeurs pour 11 réplicats au plus
  (`BURNEDAREA.csv`) ; 10 se partagent en tours complets sur 1, 2, 5 et 10 fils.
- **Résultats vérifiés** : une empreinte FNV-1a sur 64 bits des lignes triées de chaque fichier écrit,
  `resultFingerprint` dans le JSON. Une ligne publique la compare à celle de la même replanification
  sur un fil par `conccurentRun`, faite par `prepare` ; une ligne privée, à son premier appel,
  puisqu'un passage sur un fil y coûterait une minute. Le benchmark échoue sur une différence.
- **Mémoire par worker** : la pente du pic du processus entre les membres d'une famille, que donne
  `ScalingReport.cmake`. Le moniteur n'a pas de compteurs par fil : #349 demande la mémoire par
  worker, pas les allocations, et le total par appel montre déjà que les allocations ne dépendent
  pas du nombre de fils.
- **Tous les fils sur les cœurs performants** : `ProcessorPolicy::apply` fixe aussi les ensembles de
  processeurs par défaut du processus, que suivent les fils lancés par FMT. Le champ `processors`
  devient « fastest cores for every thread », et la console dit « Processors: ».
- **Bornes** : 100 lignes écrites ; mémoire retenue de 56 759 000 octets, la mesure de 51 598 927 plus
  10 %, presque toute dans MOSEK (section 2.17) ; pic par ligne, le plus haut de deux mesures dans
  chaque mode plus 10 %. La replanification de TWD_land ne calcule pas de yields complexes : sa
  mémoire retenue ne dépend pas du passage.
- **Groupe privé** : un neuvième argument facultatif, le nombre de fils, lance la replanification par
  `onDemandRun`. `Flow.Replanning.Threads5.Bfec` : 10 réplicats de 5 périodes sur 5 fils, 50 lignes.
- **Schéma JSON 4** : `resultFingerprint`, et `threads` qui prend sa valeur.
- **`ScalingReport.cmake`**, et `ResultsFormat.cmake` qui garde les fonctions communes avec
  `CompareResults.cmake`.
- **Écarts au plan** : pas de planification parallèle (`FMTPlanningTask`), puisque les calculs qui
  durent des jours sont des replanifications ; 5 et 10 fils au lieu de 4 et 8 ; pas de compteurs par
  fil dans le moniteur.
- **Un dossier par appel** (ajouté à la validation, à relire ; section 2.20) : chaque replanification
  écrit dans `call<N>`, sous le dossier du benchmark, que `prepare` vide hors mesure en réessayant
  pendant une seconde. Aucun appel mesuré ne supprime plus de fichiers : la suppression comptait dans
  la phase de préparation des replanifications, `Flow.Replanning` compris.

## 4. Feuille de route et fermeture de #349

### 4.1 Ce que #349 attend, et quel lot s'en charge

| Attente de #349 | Lot | État |
|---|---|---|
| Une suite de performance séparée des tests fonctionnels | 1 | fait |
| Benchmarks en Release | 1 | fait (type de build et optimisation notés dans chaque résultat) |
| Jeux de données stables et versionnés | 1, 3, 5 | `perfyields` fait ; groupe privé fait au lot 3, hors dépôt, avec l'empreinte SHA-256 des modèles (décision 8) ; modèle public moyen éventuel au lot 5 |
| Temps mesuré de façon constante | 1 | fait |
| Nombre d'allocations et octets alloués | 1 | fait (Windows) |
| Pic mémoire, là où c'est possible | 1, 2, 3 | fait : pic du tas et du processus, mémoire retenue par appel (lot 2), borne de pic des flux (lot 3) |
| Mise à l'échelle multithread | 4 | fait : famille `Flow.Replanning.Threads<N>` et `ScalingReport.cmake` (accélération, efficacité, mémoire par worker) |
| Environnement dans chaque résultat | 1 | fait |
| Format lisible par une machine | 1 à 4 | fait (JSON, schéma 4 depuis le lot 4) |
| Comparaison avec une référence | 1, 5 | script fait ; rapport à finaliser au lot 5 |
| Chemins sans allocation vérifiés à zéro | 1, 2 | fait : `Yield.Age` et `Mask.IsSubsetOf`, bornés à 0 (lot 2) |
| Chaque benchmark valide son calcul | 1 | fait |
| Cibles CMake et ctest | 1 | fait |
| Groupes lancés séparément | 1 | fait (`--filter`, étiquettes) |
| Lancement local par les développeurs | 1 | fait (documentation) |
| Rapports publiés par l'intégration continue | 5 | hors périmètre, décision au lot 5 |
| Documentation pour ajouter et lancer un benchmark | 1, 5 | faite, à compléter au lot 5 |
| Premier jeu : yields complexes | 1 | fait |
| Premier jeu : lecture des yields | 2 | fait |
| Premier jeu : lecture d'un projet | 2 | fait |
| Premier jeu : un flux de modèle | 3 | fait : optimisation, rejeu, outputs, simulation et replanification (lot 3) |
| Premier jeu : un flux multithread | 4 | fait : la replanification sur 1, 2, 5 et 10 fils, et sur 5 fils dans le groupe privé |

Trois demandes de #349 sur les yields complexes dépendent de #348 :

- « évaluation avec un contexte de worker préalloué » : le contexte n'existe pas encore ; la
  variante s'ajoutera quand #348 le créera ;
- « cache hit » : on ne peut pas mettre une valeur au cache à coup sûr depuis l'extérieur de
  FMT, puisque ça dépend de la durée du calcul. Son coût est mesuré par les sondes du lot 1
  (section 2.2) ; un benchmark s'ajoutera si #348 rend le cache déterministe ;
- « pas de garde de récursion retenue après une exception » : c'est un comportement, pas une
  mesure. Il se démontre d'abord (section 2.3), puis relève d'un test de chaîne ou d'un test
  unitaire, pas de ce banc.

### 4.2 Lots

- **Lot 1 : socle et yields complexes** : fait, validé le 2026-09-29 (section 5.2) ; première
  référence en section 7.4.
- **Lot 2 : lecture des yields, masques, lecture d'un projet** : fait, validé et commité le
  2026-09-30 (`9a5001df`, section 5.3) ; référence de #348 reprise en section 7.1.
  - `Yield.Age`, `Yield.Age.NewRequest`, `Mask.IsSubsetOf`, `Mask.FromString`,
    `Parser.ReadProject` et `Parser.ReadProject.TwoScenarios`.
  - Mémoire retenue par appel, réglages par benchmark, `ProcessorPolicy` (section 3.2).
- **Lot 3 : flux de modèle** : fait, validé et commité le 2026-10-05 (`88daf175` et `f9061c93`,
  section 5.4) ; mesure complète en section 7.5.
  - Six flux publics sur TWD_land et cinq benchmarks privés, dont la variante des yields sur un
    gros modèle (section 3.3).
  - Phases, borne de pic mémoire, groupe privé, empreinte des modèles, schéma JSON 3.
- **Lot 4 : threads** : livré le 2026-10-07 ; validé le 2026-10-08 sur le build de Gabriel, avec la
  correction trouvée à sa validation (section 2.20) ; à commiter (section 6.1). Mesure complète en
  section 7.6.
  - `Flow.Replanning.Threads1`, `2`, `5` et `10` sur TWD_land, `Flow.Replanning.Threads5.Bfec` dans le
    groupe privé : la replanification lancée comme l'interface (section 3.4).
  - Accélération, efficacité et mémoire par worker par `ScalingReport.cmake` ; résultats vérifiés
    contre un fil.
  - Constats : un fil par réplicat et 4,7 Mo par fil dans MOSEK (section 2.17), attente active
    (section 2.18) : issue #366.
- **Lot 5 : jeux de données et clôture**, effort `high`.
  - Modèle synthétique « moyen » sous `Examples/Performance/`, si le lot 3 le montre utile.
  - Rapport de comparaison finalisé, documentation complète.
  - Décision sur l'intégration continue, écrite dans l'issue.

### 4.3 Décisions encore ouvertes

| Décision | Quand | Remarque |
|---|---|---|
| Intégration continue | lot 5 | le dépôt n'en a pas ; #356 (builds de version automatisés) pourrait la porter |
| Modèle synthétique « moyen » | lot 5 | pour la suite publique seulement, si l'horizon de TWD_land ne suffit pas (lot 3) ; la taille réelle passe par le groupe privé (décision 8) |
| Compteur d'allocations hors Windows | lot 5 | lié à la demande de portabilité de #350 |
| Mesurer et comparer en un geste | lot 5 | un `.bat` ou une cible du projet Visual Studio qui lance la mesure complète, garde ses JSON et les compare à la référence de la machine |
| Reprendre la référence « avant #348 » | lot 5 | depuis le lot 4, tous les fils tournent sur les cœurs performants : `CompareResults.cmake` avertit que les durées ne se comparent plus avec la référence du 2026-09-30 (section 7.1), même si les benchmarks à un fil restent dans le bruit (section 7.6). La reprendre sur le commit du lot 4, comme la décision 12 l'a fait au lot 2 |
| Mesurer le pire cas du cache depuis le banc | lot 5 | une option du harnais qui ralentit les allocations de `FMTComplexYieldHandler::get` pendant les appels comptés ; aujourd'hui, un moniteur de diagnostic hors du dépôt (section 2.16) |

### 4.4 Fermer #349

L'issue peut être fermée quand, ensemble :

- le premier jeu de benchmarks (yields complexes, lecture des yields, lecture d'un projet, un
  flux de modèle, un flux multithread) tourne et valide ses résultats ;
- chaque attente du tableau ci-dessus est faite, ou écartée dans l'issue en disant pourquoi ;
- la référence « avant #348 » est consignée en section 7.1 ;
- `Documentation/PerformanceTesting.md` explique comment lancer et ajouter un benchmark.

## 5. Journal des lots

### 5.1 Lot 0 : lecture et ce fichier (2026-09-25)

**Statut** : livré le 2026-09-25. Documentation seulement, aucun code modifié.

- **Lu** :
  - les issues #349, #348 et #350 ;
  - `Documentation/Architecture.md` (« Performance and Memory Efficiency ») et
    `Documentation/CodingStandards.md` (« Testing ») ;
  - l'infrastructure CMake et ctest (`CMakeLists.txt`, `Examples/C++/CMakeLists.txt`,
    `cmake/ConfigFunctions.cmake`) ;
  - l'évaluation et le cache des yields complexes, `FMTVersion`, `FMTTaskHandler`.
- **Vérifié** : `dumpbin /IMPORTS` sur le `FMTlib.dll` du build de `new_test` (section 2.8).
- **Décisions de Gabriel** : section 1.3.
- **Constats** : section 2.

### 5.2 Lot 1 : socle et yields complexes (2026-09-25)

**Statut** : livré le 2026-09-25, commité par Gabriel le 2026-09-29 (`47d4ea45`) et validé le
même jour. Le `CMakeLists.txt` racine inclut maintenant `Tests/Performance/CMakeLists.txt`.

- **Preuve de faisabilité du moniteur** (étape 1), dans le scratchpad, contre le `FMTlib.dll`
  du build de `new_test`. Chaque allocation est attribuée à un module par son adresse de retour :
  - 3 allocations faites dans l'exécutable : 3 comptées, attribuées à l'exécutable ;
  - lecture de TWD_land (scénario `yieldoperatorsalone`) : 14 575 allocations, dont 13 766
    dans `FMTlib.dll`, 777 dans `boost_filesystem` et 29 dans `MSVCP140` ;
  - `FMTDevelopment::getAge` : 0 ;
  - aucun appel direct à `HeapAlloc` sur ces chemins ;
  - vu rouge : 2 allocations déclarées au lieu de 3, la sonde rend 1.

  Une seconde sonde a mesuré les chemins du cache en forçant une insertion (pause de 2 ms dans
  la dernière allocation d'un calcul) : les coûts de la section 2.2.
- **Harnais** `Tests/Performance/Harness/`, bibliothèque statique `FMTBenchmarkHarness`, espace
  de noms `Performance` : `AllocationMonitor` (Windows, et repli portable qui ne compte rien),
  `MemoryMonitor`, `Benchmark`, `BenchmarkOptions`, `BenchmarkRunner`, `BenchmarkResult`,
  `BenchmarkEnvironment`, `BenchmarkSuite`, `JsonWriter`, et `CommitInfo.cmake`, qui écrit
  l'en-tête du commit à chaque build.
- **Benchmarks** : l'exécutable `FMTPerformanceTests` (`FMTPerformanceTests.cpp`,
  `ComplexYieldBenchmarks.h/.cpp`), 7 benchmarks `ComplexYield.<opérateur>` et leurs 7 lignes
  dans `Tests/Performance/performance.csv`.
- **Données** : le scénario `perfyields` de TWD_land, copie de `yieldoperatorsalone` plus un
  `_EQUATION` sans nombre (`EQUATIONSUM`) et une chaîne de 5 `_SUM` (`CHAIN1` à `CHAIN5`),
  chacun dans son bloc `*YC`. Valeurs calculées à la main depuis la table d'âge de
  `peuplement1` (`volumetotal` = 120 à 7 ans, 130 à 8 ans) : 240, 240, 60, 100, 130, 240, 720.
- **Bornes d'allocations** : les médianes mesurées (4, 4, 5, 5, 13, 31, 20), identiques en mode
  court et sur deux mesures complètes.
- **Inscription** : `Tests/Performance/CMakeLists.txt`, inclus par le `CMakeLists.txt` racine
  juste après `Examples/C++/CMakeLists.txt`.
- **Comparaison** : `Tests/Performance/CompareResults.cmake`, en CMake seul (`string(JSON)`).
- **Documentation** : `Documentation/PerformanceTesting.md` (nouveau) ; `AGENTS.md` (table des
  documents, carte du dépôt, section « Benchmarks ») ; une note en section 8 de
  `Examples/C++/tests/ETAT.md`.
- **Écarts au plan** : les choix de la section 3.1, en particulier le retrait de la variante à clé unique
  et la borne sur la médiane ; les compteurs par thread du moniteur, reportés au lot 4 qui s'en
  servira ; le défaut de `hasFeature("ONNXRUNTIME")`, trouvé en écrivant l'environnement.

Vérifications (hors build, 2026-09-25) :

- **Compilation** avec `cl` en `/W4`, options du `.vcxproj` d'un exemple, contre le
  `FMTlib.dll` du build. Ce build date du 2026-09-17 ; le seul commit postérieur qui touche
  FMTlib, `267cd12c`, ne change que deux modèles de rendement ONNX. Aucun avertissement dans les
  nouveaux fichiers : ceux qui restent viennent de `FMTException.h`, `FMTList.hpp` et
  `FMTModel.h`.
- **Projet CMake jetable** (Ninja de VS 2022), qui inclut le vrai `Tests/Performance/CMakeLists.txt`
  avec les réglages de la racine : configuration, compilation et lien de `FMTPerformanceTests`
  par CMake. `ctest -N` : 7 tests. `--show-only=json-v1` : étiquettes `performance` et
  `allocation`, `SKIP_RETURN_CODE 77`, `FIXTURES_REQUIRED ExamplesModels`, chemins absolus. Sur
  une copie du CSV : l'alerte pour une cible inexistante, et l'étiquette `performance` seule pour
  une ligne sans borne.
- **Passages par ctest** : mode court sous `-j 8`, deux fois, 7 verts en 0,12 et 0,21 s réelles ;
  mesure complète sans `-j`, 7 verts en 1,86 s.
- **Vu rouge**, sans recompiler : résultat faux (`--expected 241`), borne sous la mesure
  (`--max-allocations 12` sur `Shift`), borne 0, benchmark inconnu, surcharge sans
  `--benchmark`, ligne absente du CSV : tous rendent 1 ; les bonnes valeurs rendent 0.
- **Comparaison** de deux mesures complètes du même commit : écarts de médiane de -2,0 % à
  +0,8 %, allocations identiques. Avertissement quand les modes diffèrent ; un dossier accepté
  comme un fichier.
- **JSON** relu par Python : valide, 7 résultats.
- **Encodages** : sources `.h` et `.cpp` en cp1252 (octet `E9` de « Québec » dans l'en-tête),
  fichiers CMake, Markdown, CSV et scénario en UTF-8 ou ASCII, tout en CRLF, aucun U+FFFD.

Validation sur le build de Gabriel (2026-09-25) :

- **Build** : configuration à 15 h 55, puis 62 unités recompilées, dont 5 de FMTlib (commits
  postérieurs au build du 2026-09-17) : `FMTlib.dll` est neuf.
- **Messages** : aucun ne vient du lot 1. Relus en rejouant la configuration et les 62
  compilations (section 2.10) :
  - l'avertissement CMP0167 sur FindBoost ;
  - deux lignes de `BFECtests.csv` sans cible ;
  - C4244 dans `FMTYieldModelNn.cpp:295`, une conversion voulue depuis 2023 ;
  - une note de Boost.Optional sur la fin de C++03, dans deux tests C++/CLI ;
  - la note de vcpkg sur mimalloc, qui a mené au constat de la section 2.6.
- **ctest** : un passage complet de Gabriel à 16 h 02, 333 tests sur 333 réussis, dont les 7
  lignes de performance et 91 lignes privées. Aucune alerte `performance.csv: no target named`.
- **`BFECtests.csv` de ce worktree** (fichier local, copie du 2026-07-27) :
  - `UnitTestFMTFormLogger` retirée : ce test est devenu `UnitTestCallbackLogger` (`f4e5a11c`),
    déjà inscrit par `basetests.csv` ;
  - `testWrapperCoreGetYield.cpp` renommée `testWrapperCoreGetYield` ; lancée à la main avec
    ses arguments, la ligne rend la valeur attendue en 5 s ;
  - la copie du worktree `dev` a en plus une ligne `testWrapperCoreOperatingArea`, ajoutée par
    la migration ; elle n'est pas reprise ici.
- **Amendement `allocator`** (décision 7) : champ de l'environnement, ligne d'environnement de
  la console, avertissement de `CompareResults.cmake`, documentation. Compilé en `/W4` hors
  CMake sans avertissement nouveau, vérifié sous les deux allocateurs (section 2.6). Les résultats
  écrits avant lui se comparent encore : leur allocateur s'affiche « allocator unknown ».

Validation finale (2026-09-29), sur le commit `47d4ea45` (le lot, 31 fichiers), reconfiguré puis
recompilé par Gabriel après le commit :

- **En-tête du commit** : `47d4ea45`, `dirty` à 0.
- **`-L performance`** : 7 verts ; l'environnement affiche le commit, sans « modified », et
  `CRT heap`.
- **Suite base** (`-E "T:/" -j 8`) : 242 tests, 227 verts, 15 désactivés (`knownbugs.csv`),
  aucun échec, 16 s réelles.
- **Inscription** : 334 tests en tout, dont 242 publics. La ligne privée
  `testWrapperCoreGetYield` est inscrite et verte ; `UnitTestFMTFormLogger` a disparu.
- **Issue mimalloc** : #361.
- **Référence** : deux mesures complètes à la suite, sans `-j`, la machine au repos ; écarts de
  médiane de -1,4 % à +2,2 %, allocations identiques (section 7.4). Les bornes du CSV sont les
  médianes mesurées : aucune ne change. JSON gardés localement, hors du dépôt et du dossier de
  build.
- **Relecture** (2026-09-29) : Gabriel valide les choix de conception (section 3.1) ; issue
  #362 pour `hasFeature("ONNXRUNTIME")`.

### 5.3 Lot 2 : lecture des yields, masques, lecture d'un projet (2026-09-29)

**Statut** : livré le 2026-09-29 ; validé, relu et commité par Gabriel le 2026-09-30 (`b789dd76`
pour `AGENTS.md`, `9a5001df` pour le reste). Référence de #348 reprise (section 7.1).

- **Benchmarks**, 6 lignes de plus, 13 en tout :
  - `YieldBenchmarks.h/.cpp` : `Yield.Age`, `Yield.Age.NewRequest` ;
  - `MaskBenchmarks.h/.cpp` : `Mask.IsSubsetOf`, `Mask.FromString` ;
  - `ParserBenchmarks.h/.cpp` : `Parser.ReadProject`, `Parser.ReadProject.TwoScenarios` ;
  - `PerfYields.h/.cpp`, sorti de `ComplexYieldBenchmarks.cpp`.
- **Harnais** :
  - `RunSettings.h/.cpp` : les réglages, sortis de `BenchmarkRunner`, plus `forSlowCalls` ;
    `Benchmark::getSettings` ;
  - la mémoire retenue par appel : `AllocationCounts::liveBytes`, statistiques, vérification,
    option, cinquième colonne du CSV, étiquette `memory` ;
  - `ProcessorPolicy.h/.cpp` ;
  - le schéma JSON 2, et `CompareResults.cmake` (mémoire retenue, processeurs).
- **Valeurs attendues**, calculées à la main :
  - `VOLUMETOTAL` de `peuplement1` à 7 ans : 100 + (7 - 5) × (150 - 100) / 5 = 120
    (`TWD_land.yld`) ;
  - `UC PROD ?` : 2 + 3 + 3 = 8 bits (`TWD_land.lan`) ;
  - aire initiale de la racine : 403,28 + 7 × 201,64 = 1 814,76 ha (`TWD_land.are`), deux fois
    pour la racine et `perfyields` (3 629,52 ha).
- **Bornes** : allocations 0, 7, 0 et 7 pour les yields et les masques ; mémoire retenue 0 pour
  `Yield.Age.NewRequest`, `Mask.FromString` et les deux lectures. Mêmes médianes en mode court et
  sur quatre mesures complètes.
- **Documentation** : `Documentation/PerformanceTesting.md` (réglages, cœurs, mémoire retenue,
  schéma 2, nouveaux benchmarks), `AGENTS.md` (étiquette `memory`), note de la section 8 de
  `Examples/C++/tests/ETAT.md`.

Vérifications (hors build, 2026-09-29) :

- **Compilation** avec `cl` en `/W4` contre le build de Gabriel : aucun avertissement dans les
  fichiers du banc.
- **Projet CMake jetable** : 13 lignes inscrites, étiquettes `allocation` (11) et `memory` (4),
  code 77 ; par ctest sous `-j 8`, 14 tests verts en 0,34 s.
- **Passages** : les 13 benchmarks verts en mode court et sur quatre mesures complètes.
- **Vu rouge**, sans recompiler : résultat faux, borne d'allocations sous la mesure, borne 0 sur un
  appel qui alloue, `--max-retained-bytes` sans `--benchmark`, cinquième colonne illisible : tous
  rendent 1.
- **Mémoire retenue**, vue rouge avec une sonde jetable qui garde 64 octets par appel :
  - borne 0 : échec, « a counted call retains 64 bytes » ;
  - borne 100 : succès, « the bound can be lowered to 64 » ;
  - la même sonde, qui libère ses octets, passe la borne 0.
- **Cœurs** : A/B sous le mode efficacité de Windows, avant et après `ProcessorPolicy`
  (section 2.10).
- **Encodages** : sources en cp1252 (un octet `E9`), le reste en UTF-8, tout en CRLF.
- **À surveiller** : deux lancements sur une quinzaine, sortie masquée, n'ont pas écrit leur
  JSON. Aucun plantage au journal de Windows, et aucun autre lancement n'a échoué : non reproduit.

Validation sur le build de Gabriel (2026-09-30), reconfiguré et compilé vers 11 h, fichiers du lot 2
non commités :

- **Inscription** : 340 tests, dont 248 publics. `-L performance`, `-L allocation` et `-L memory`
  retiennent 13, 11 et 4 lignes, plus les 2 tests de la fixture `ExamplesModels`.
- **Mode court** : les trois étiquettes vertes (15, 13 et 6 tests ; 1,6, 1,3 et 0,7 s). Le JSON
  est au schéma 2, avec `CRT heap` et `fastest cores, 16 of 32 logical processors, not throttled`.
- **Suite base** (`-E "T:/" -j 8`) : 248 tests, 233 verts, 15 désactivés (`knownbugs.csv`), aucun
  échec, 13,8 s réelles.
- **Deux mesures complètes par ctest** : 13 lignes vertes, allocations et mémoire retenue
  identiques à celles du scratchpad. Elles ont révélé la pause au démarrage (section 2.10) : un
  échantillon trop long d'environ 30 ms dans chaque benchmark, et `Parser.ReadProject` à 5,4 ms.
- **Correction** : l'attente de 250 ms dans `BenchmarkSuite.cpp`, documentée dans
  `Documentation/PerformanceTesting.md`. Compilée en `/W4` hors du build, sans avertissement, et
  vérifiée par un A/B du même exécutable (section 2.10) ; mesures en section 7.3. Elle ne demande
  aucune reconfiguration.
- **JSON manquants** (« À surveiller » ci-dessus) : aucun sur environ 200 lancements.
- **Relecture** : Gabriel valide les choix de la section 3.2 et la reprise de la référence de
  #348 (décision 12).

Validation finale (2026-09-30), sur le commit `9a5001df`, compilé par Gabriel après le commit :

- **Commits** : `b789dd76` ne contenait que `AGENTS.md` ; les 31 autres fichiers sont venus dans
  `9a5001df`. L'en-tête du commit porte `9a5001df`, `dirty` à 0.
- **`-L performance`** : 13 lignes vertes, plus les 2 tests de la fixture.
- **Suite base** (`-E "T:/" -j 8`) : 248 tests, 233 verts, 15 désactivés, aucun échec, 14,0 s
  réelles.
- **Référence** (décision 12) : trois mesures complètes à la suite, la machine au repos
  (section 7.1). Comparée à celle du lot 1, elle donne des médianes de -1,2 à +3,5 % sur les
  yields complexes, avec les mêmes allocations.
- **#349** : texte d'un commentaire remis à Gabriel (décision 11).

### 5.4 Lot 3 : flux de modèle (2026-10-02)

**Statut** : livré le 2026-10-02, commité par Gabriel le 2026-10-05 (`852e1be0`, puis la
correction `88daf175`) et validé le même jour ; choix validés (section 3.3).

- **Flux** (`FlowBenchmarks.h/.cpp`), 6 lignes publiques de plus, 19 en tout :
  - `Flow.Optimize` et `Flow.Optimize.Long` : `NOT_MASK` sur 5 et 20 périodes, objectifs 90 738 et
    362 952, les mêmes sous MOSEK et CLP ;
  - `Flow.Replay` : la cédule de `LP` sur 10 périodes, `OVOLREC` à la période 2 : 48 008,953705 ;
  - `Flow.Outputs` : tous les outputs du même modèle, 3 079 696,8362052 ;
  - `Flow.Simulate` : `DECISION`, `UNIT_REC` à la période 5 : 60, comme `FMTNsstest` ;
  - `Flow.Replanning` : les scénarios de `replanningtest`, 2 réplicats de 5 périodes, un fil :
    20 lignes écrites.
- **Groupe privé** : `DefinedBenchmarks.h/.cpp` crée les benchmarks des lignes qui donnent des
  arguments ; `performance-private.csv`, local, compte 5 lignes : `Flow.Optimize.Bfec`,
  `Flow.Replay.Bfec`, `Flow.Outputs.Bfec`, `Flow.Replanning.Bfec` et `Yield.Model.Bfec`, sur trois
  modèles de `BFECtests.csv`. Leurs valeurs attendues sont celles de `doplanning` et de
  `FMTsetsolution`, ou mesurées une fois.
- **Harnais** : phases (`Benchmark::definePhase`, `beginPhases`, `endPhase`),
  `RunSettings::forFlows`, portée des fils comptés (`Benchmark::getThreadScope`), benchmark
  indisponible (77 sans OSI), borne de pic mémoire, `DatasetFingerprint` (SHA-256 par l'API de
  chiffrement de Windows), schéma JSON 3, `CompareResults.cmake` (phases, empreintes, unités).
- **`QuietFmt.h/.cpp`** : le journal et les avertissements de FMT réglés une fois par processus ;
  les lectures de projet et les yields privés l'utilisent aussi.
- **Bornes** : mémoire retenue 0 pour cinq flux, 5 Mo pour la replanification (section 2.13) ; pic
  de 26 à 84 Mo selon le flux, le plus haut pic de deux mesures dans chaque mode plus 10 %. Les
  lignes privées ont leurs bornes, posées de la même façon, dans le fichier local.
- **Documentation** : `Documentation/PerformanceTesting.md` (flux, phases, borne de pic, groupe
  privé, schéma 3, comparaison), `AGENTS.md` (étiquette `bfec-perf`), note de la section 8 de
  `Examples/C++/tests/ETAT.md`.
- **Écarts au plan** : le calcul des outputs devient son propre benchmark (section 3.3) ; graphe
  et matrice restent une seule phase ; la replanification à 5 fils passe au lot 4.

Vérifications (hors build, 2026-10-02) :

- **Compilation** avec `cl` en `/W4` contre le build de Gabriel : aucun avertissement dans le banc ;
  ceux qui restent viennent de `FMTException.h`, `FMTList.hpp`, `FMTModel.h` et, depuis que les
  flux incluent les modèles, `FMTGraph.hpp`.
- **Projet CMake jetable** : 25 tests inscrits (19 lignes publiques, la fixture, 5 privées).
  `-L performance` en retient 20, `-L bfec-perf` 5, `-E "T:/"` 20 : aucune ligne privée dans la
  suite base. Une ligne privée hors de `T:/`, ou sans arguments, est refusée avec un avertissement.
- **Passages par ctest** : les 20 tests publics sous `-j 8` en 0,70 s ; les 5 privés en mode court
  en 45 s.
- **Mesures** : deux mesures complètes de chaque flux, seul dans son processus, et de chaque
  benchmark privé (section 7.5) ; écarts de 2 % au plus, allocations identiques.
- **Vu rouge**, sans recompiler : borne de pic sous la mesure, borne de mémoire retenue 0 sur la
  replanification, résultat faux, taille non positive, surcharge sans `--benchmark`, ligne privée
  aux arguments trop peu nombreux, d'un genre inconnu ou de longueur non entière : tous rendent 1.
  Le pic d'un benchmark qui n'est pas le premier de son processus est signalé comme non vérifié.
- **Comparaison** : phases, unités et avertissement d'empreinte vérifiés ; les résultats des
  schémas 1 et 2 se lisent encore.
- **Constats** : sections 2.12 à 2.15.
- **Encodages** : sources en cp1252 (un octet `E9`), le reste en UTF-8, tout en CRLF.

Validation sur le build de Gabriel (2026-10-05) :

- **Build** : compilé à 08:55, avant le commit `852e1be0` de 09:16 ; aucun fichier suivi n'a
  changé entre les deux. L'en-tête du binaire indique donc `9a5001df`, modifié.
- **Inscriptions** : 351 tests, dont 254 dans la suite base (6 de plus) ; `-L performance` en
  retient 21 (les 19 lignes et les 2 tests de la fixture), `-L memory` 12, `-L bfec-perf` 5.
- **Mode court** : `-L performance` 21 sur 21 en 3,2 s, `-L memory` 12 sur 12 ; `-L bfec-perf`
  4 sur 5 : `Flow.Optimize.Bfec` retient 1 584 octets pour une borne de 0 (section 2.16).
- **Suite base** (`-E "T:/" -j 8`) : 254 tests, 239 verts et 15 désactivés, aucun échec, en 16 s.
- **Diagnostic** : la mémoire retenue des calculs de yields complexes (section 2.16), le comptage
  des blocs de taille 0 (section 2.8).

Correction (2026-10-05), à compiler :

- **Bornes** : `Flow.Outputs` passe de 0 à 22 000 octets ; dans le fichier local, les quatre flux
  privés exposés prennent le pire cas du cache plus 10 % : 7 653 000 octets pour l'optimisation,
  55 000 pour le rejeu, 27 220 000 pour les outputs et 13 017 000 pour la replanification.
- **`AllocationMonitorWindows.cpp`** : un bloc de taille 0 compte pour un octet vivant.
- **`BenchmarkRunner.cpp`** : la console ne propose plus d'abaisser une borne de mémoire retenue.
- **Documentation** : la borne d'un calcul de yields complexes, l'exactitude des octets retenus,
  les résultats des flux qui ne dépendent pas du solveur.

Validation de la correction (2026-10-05, commit `88daf175`) :

- **Build** : compilé avant le commit, comme le premier : l'en-tête du binaire indique `852e1be0`,
  modifié, pour les sources de `88daf175`.
- **Mode court** : `-L performance` 21 sur 21 en 3,1 s, `-L memory` 12 sur 12, `-L bfec-perf`
  5 sur 5 en 46 s ; suite base : 254 tests, 239 verts et 15 désactivés, en 16 s.
- **Mesure complète** : section 7.5. La nouvelle borne de `Flow.Optimize.Bfec` y a absorbé une
  insertion au cache (section 2.16).
- **Sous mimalloc**, en vue de #361 : la mémoire retenue était fausse. Corrigé dans
  `AllocationMonitorWindows.cpp` (sections 2.6 et 2.8).

Validation de la correction sous mimalloc (2026-10-05, compilée par Gabriel, pas encore commitée) :
`-L performance` 21 sur 21, `-L memory` 12 sur 12, `-L bfec-perf` 5 sur 5 ; suite base : 254 tests,
239 verts et 15 désactivés, en 17 s. Sous le tas du CRT, les flux publics gardent leurs valeurs
(-785, 0 et 4 737 039 octets retenus) ; seuls les flux privés exposés au cache varient
(section 2.16).

### 5.5 Lot 4 : threads (2026-10-07)

**Statut** : livré le 2026-10-07, compilé par Gabriel le même jour après la reconfiguration (quatre
lignes nouvelles dans `performance.csv`) ; choix de la section 3.4 validés ; issue #366 publiée. La
validation a trouvé une suppression de fichiers qui pouvait échouer : corrigée dans le banc
(section 2.20), recompilée et validée le 2026-10-08. Reste le commit.

- **Harnais** : `Benchmark::getThreads` et `Benchmark::getResultFingerprint`, écrits dans le JSON
  (schéma 4) et à la console (« threads », « output fingerprint ») ; `ProcessorPolicy::apply` pour
  tous les fils.
- **Flux** : `ReplanningFlow` replanifie aussi par `onDemandRun` sur N fils, et vérifie l'empreinte de
  ce qu'il écrit. Quatre lignes publiques (résultat 100, mémoire retenue 56 759 000 octets, pics de
  347, 374, 422 et 491 Mo) ; une ligne privée locale (résultat 50, mémoire retenue 60 143 000 octets,
  pic de 1 725 Mo).
- **Scripts** : `ScalingReport.cmake` ; `ResultsFormat.cmake` reprend trois fonctions de
  `CompareResults.cmake`, qui lit les schémas 1 à 4.
- **Documentation** : `Documentation/PerformanceTesting.md` (fichiers, cœurs, schéma 4, section
  « Scaling with threads », famille de replanification, argument privé).
- **Constats** : sections 2.7, 2.13 (cause trouvée), 2.17 à 2.19.
- **Issue** : #366, publiée par Gabriel, pour `onDemandRun` (sections 2.17 et 2.18). Précisée le même
  jour par une sonde sans FMT ni OSI : le bloc appartient au fil et à l'optimiseur au point intérieur
  de MOSEK (section 2.17).

Vérifications (hors build, 2026-10-07) :

- **Compilation** avec `cl` en `/W4` contre le build de Gabriel : aucun avertissement dans le banc.
- **Passages** : les quatre lignes publiques et `Flow.Replanning` passent avec leurs bornes ; la ligne
  privée écrit ses 50 lignes à chacun de ses quatre passages, avec la même empreinte.
- **Vu rouge**, sans recompiler : résultat faux, borne de mémoire retenue et borne de pic sous la
  mesure : tous rendent 1. Avec une référence faussée (variante du scratchpad), le benchmark échoue :
  « the replanning on 5 threads wrote other rows than the same replicates on one thread ».
- **Stress** : 24 passages à 5 et 10 fils, aucune différence (section 2.7).
- **Pire cas du cache** de la ligne privée, qui calcule des yields complexes (règle 13) :
  54 675 227 octets.
- **Rapport** : `ScalingReport.cmake` sur la mesure complète (section 7.6).
- **Encodages** : sources en cp1252 (un octet `E9`), scripts et texte en UTF-8, tout en CRLF.

Validation sur le build de Gabriel (2026-10-07, en-tête `cdf6a63f`, modifié) :

- **Gabriel** : ctest sans filtre, 356 tests (les 258 de la suite base et 98 privés), aucun échec.
- **Étiquettes** : `-L performance` (23 lignes et les 2 tests des données), `-L memory` (14 lignes) et
  `-L bfec-perf` (6 lignes, 102 s) passent. Suite base sous `-j 8` : 258 tests, 243 verts et
  15 désactivés, en 17 s.
- **Mode court** : les quatre lignes publiques écrivent les mêmes lignes (`2714db3feb3b0737`) et
  retiennent 51 598 927 octets ; 3 417 652 allocations par appel, une de plus une fois. La ligne
  privée écrit ses 50 lignes et retient 47 380 984 octets ; pic de 1 125,4 Mo.
- **Mesure complète** : trois passages des lignes publiques. Les deux premiers passent ; au troisième,
  `Flow.Replanning.Threads2` échoue en vidant son dossier de sortie (section 2.20). Accélération d'un
  appel : 1,58 et 1,63 sur 2 fils, 2,49 et 2,76 sur 5, 3,02 et 3,06 sur 10 (1,60, 2,47 et 2,86 dans
  les mesures provisoires de la section 7.6).
- **Correction** (section 2.20), dans `FlowBenchmarks.cpp` et la documentation : compilée hors CMake en
  `/W4`, aucun avertissement dans le banc ; les cinq replanifications publiques passent, avec un
  dossier par appel ; un fichier tenu 0,8 s est attendu, un fichier tenu 3 s fait échouer le benchmark
  après une seconde.

Validation après la correction (2026-10-08, build de Gabriel recompilé, en-tête `cdf6a63f`, modifié) :

- **Étiquettes** : `-L performance` (25 tests) et `-L bfec-perf` (6 lignes, 101 s) passent. Suite base
  sous `-j 8` : 258 tests, 243 verts et 15 désactivés, en 17 s. Chaque replanification écrit dans
  `call1`, `call2`...
- **Mesure complète** : trois passages publics et deux privés, tous verts (section 7.6). Les
  replanifications font 10 ou 11 allocations de moins par appel (799 167 et 3 417 641) : la
  suppression du dossier est sortie de l'appel.
- **`ScalingReport.cmake`** ne fait plus que nommer une famille d'un seul membre. La ligne privée
  `Flow.Replanning.Threads5.Bfec` y formait une famille « Flow.Replanning.Bfec », qu'on pouvait
  confondre avec le benchmark de ce nom. Script et documentation seulement, rien à compiler.

## 6. Prochain lot

Les lots 1 à 3 sont validés et commités (sections 5.2 à 5.4). #361 est fait, sans changer l'allocateur
par défaut ni les bornes (décision 17). Le lot 4 est validé ; reste son commit (section 5.5).

### 6.1 D'abord : commiter le lot 4

Validé le 2026-10-08 sur le build de Gabriel, avec la correction de la section 2.20 (section 5.5) ;
mesure complète en section 7.6.

1. Gabriel relit le choix « un dossier par appel » (section 3.4) et le changement de
   `ScalingReport.cmake` (section 5.5).
2. Gabriel commite le lot 4. Rien n'est à remesurer : les sources mesurées sont celles du commit.

### 6.2 Ensuite : lot 5

Clôture (section 4.2) : mesurer et comparer en une commande, documenter le déroulement avant et après
un changement, reprendre la référence « avant #348 » si Gabriel le décide (section 4.3), fusionner
`new_test` dans `master`, écrire dans #349 les attentes écartées (section 4.3) et un commentaire de
clôture.

## 7. Mesures

### 7.1 Référence « avant #348 »

Reprise le 2026-09-30 avec le harnais du lot 2 (décision 12). Mesures complètes par ctest, sans
`-j`, un processus par benchmark, sur le build de Gabriel du commit `9a5001df`, sans modification
locale : FMT 1.3.0, Release, MSVC 19.44.35228, tas du CRT, fil qui mesure sur les cœurs
performants, attente de 250 ms au démarrage. Machine : Intel Core i9-13900, 32 cœurs logiques,
Windows 10.0.22631, 40 Gio disponibles, au repos.

Trois mesures à la suite. La troisième a été prise parce que la deuxième avait un échantillon lent
dans `ComplexYield.Equation` (1,99 fois la médiane) ; elle en a un aussi (1,91 fois). C'est le
bruit ordinaire de ce benchmark, pas une perturbation : les trois mesures forment la référence.

| Benchmark | Médiane, mesure 1 | Mesure 2 | Mesure 3 | Allocations par appel (min / médiane / max) | Octets par appel | Mémoire retenue | Pic du tas (octets) |
|---|---|---|---|---|---|---|---|
| `ComplexYield.Sum` | 389,6 ns | 377,9 ns | 385,4 ns | 2 / 4 / 4 | 82 | 0 | 80 |
| `ComplexYield.Multiply` | 315,6 ns | 328,2 ns | 312,2 ns | 2 / 4 / 4 | 66 | 0 | 64 |
| `ComplexYield.Divide` | 360,0 ns | 349,2 ns | 352,3 ns | 2 / 5 / 5 | 82 | 0 | 80 |
| `ComplexYield.Subtract` | 351,5 ns | 346,7 ns | 349,7 ns | 2 / 5 / 5 | 82 | 0 | 80 |
| `ComplexYield.Shift` | 626,9 ns | 658,8 ns | 634,2 ns | 2 / 13 / 13 | 366 | 0 | 348 |
| `ComplexYield.Equation` | 2 557,8 ns | 2 596,0 ns | 2 605,2 ns | 2 / 31 / 31 | 2 858 | 0 | 872 |
| `ComplexYield.RecursiveChain` | 1 585,0 ns | 1 648,3 ns | 1 593,2 ns | 2 / 20 / 20 | 410 | 0 | 400 |
| `Yield.Age` | 65,0 ns | 62,1 ns | 60,7 ns | 0 / 0 / 0 | 0 | 0 | 0 |
| `Yield.Age.NewRequest` | 288,8 ns | 289,4 ns | 284,9 ns | 7 / 7 / 7 | 98 | 0 | 90 |
| `Mask.IsSubsetOf` | 2,4 ns | 2,4 ns | 2,4 ns | 0 / 0 / 0 | 0 | 0 | 0 |
| `Mask.FromString` | 402,1 ns | 398,0 ns | 398,2 ns | 7 / 7 / 7 | 197 | 0 | 160 |
| `Parser.ReadProject` | 3,73 ms | 4,01 ms | 3,78 ms | 13 262 / 13 262 / 13 262 | 1 870 683 | 0 | 149 044 |
| `Parser.ReadProject.TwoScenarios` | 4,90 ms | 4,91 ms | 4,88 ms | 19 930 / 19 930 / 19 930 | 2 883 888 | 0 | 311 884 |

Les médianes d'un même benchmark diffèrent de 8 % au plus entre les trois mesures : un écart plus
petit entre cette référence et une mesure « après » n'est pas un changement. Les allocations, les
octets et la mémoire retenue sont identiques dans les trois ; le minimum des yields complexes tombe
à 2 quand une clé entre au cache (section 2.1). Pic de mémoire privée du processus : 20,8 à
21,1 Mo.

Comparée à la référence du lot 1 (section 7.4) par `CompareResults.cmake`, la mesure 1 donne des
médianes de -1,2 à +3,5 % sur les yields complexes, avec les mêmes allocations : les deux
références concordent.

Les JSON des trois mesures restent locaux, hors du dépôt (décision 5). Pour juger #348, comparer la
mesure « après » à chacune avec `CompareResults.cmake`.

### 7.2 La suite de performance

| Date | Lignes | Dont allocation | Mode court sous `-j 8` (temps réel) | Mesure complète (temps réel) |
|---|---|---|---|---|
| 2026-09-25 (lot 1, projet jetable) | 7 | 7 | 0,12 à 0,21 s | 1,86 s |
| 2026-09-25 (build de Gabriel, passage complet de la suite sous `-j 8`) | 7 | 7 | 0,93 s-processeur en tout | — |
| 2026-09-29 (commit `47d4ea45`, build de Gabriel) | 7 | 7 | 0,95 s, sans `-j` | 2,0 s |
| 2026-09-29 (lot 2, projet jetable) | 13 | 11 | 0,34 s | 2,7 à 2,9 s, en un seul processus |
| 2026-09-30 (lot 2, build de Gabriel, non commité) | 13 | 11 | 1,6 s, sans `-j` | 3,7 s, sans l'attente |
| 2026-09-30 (lot 2 avec l'attente, exécutable du scratchpad) | 13 | 11 | — | 6,9 s, un processus par benchmark |
| 2026-09-30 (commit `9a5001df`, build de Gabriel) | 13 | 11 | 3,1 s, sans `-j` | 7,0 s, attente comprise |
| 2026-10-02 (lot 3, projet jetable) | 19 | 11 | 0,70 s | — |
| 2026-10-05 (commit `852e1be0`, build de Gabriel) | 19 | 11 | 3,2 s, sans `-j` ; suite base de 254 tests en 16 s sous `-j 8` | — |
| 2026-10-05 (commit `88daf175`, build de Gabriel) | 19 | 11 | 3,1 s, sans `-j` ; suite base de 254 tests en 16 s sous `-j 8` | 11 à 12 s pour les lignes publiques, 89 s pour les 5 privées |
| 2026-10-08 (lot 4, build de Gabriel, non commité) | 23 | 11 | 9,4 s, sans `-j` ; suite base de 258 tests en 17 s sous `-j 8` | 18 à 20 s pour les lignes publiques, 202 et 208 s pour les 6 privées |

### 7.3 Lot 2, avec l'attente

Deux mesures complètes du 2026-09-30, un processus par benchmark comme sous ctest, avec
l'exécutable du scratchpad : les sources du lot 2 avec l'attente de 250 ms, compilées hors CMake
contre le `FMTlib.dll` du build de Gabriel. Fil qui mesure sur les cœurs performants, tas du CRT.
Remplacées depuis par la référence de la section 7.1.

| Benchmark | Médiane, mesure 1 | Médiane, mesure 2 | Allocations par appel (min / médiane / max) | Octets par appel | Mémoire retenue | Pic du tas (octets) |
|---|---|---|---|---|---|---|
| `ComplexYield.Sum` | 352,6 ns | 357,6 ns | 2 / 4 / 4 | 82 | 0 | 80 |
| `ComplexYield.Multiply` | 320,7 ns | 299,1 ns | 4 / 4 / 4 | 66 | 0 | 64 |
| `ComplexYield.Divide` | 337,5 ns | 337,1 ns | 5 / 5 / 5 | 82 | 0 | 80 |
| `ComplexYield.Subtract` | 328,2 ns | 324,2 ns | 5 / 5 / 5 | 82 | 0 | 80 |
| `ComplexYield.Shift` | 600,4 ns | 607,3 ns | 13 / 13 / 13 | 366 | 0 | 348 |
| `ComplexYield.Equation` | 2 480,4 ns | 2 505,8 ns | 31 / 31 / 31 | 2 858 | 0 | 872 |
| `ComplexYield.RecursiveChain` | 1 503,3 ns | 1 521,0 ns | 20 / 20 / 20 | 410 | 0 | 400 |
| `Yield.Age` | 57,6 ns | 55,7 ns | 0 / 0 / 0 | 0 | 0 | 0 |
| `Yield.Age.NewRequest` | 275,2 ns | 277,3 ns | 7 / 7 / 7 | 98 | 0 | 90 |
| `Mask.IsSubsetOf` | 2,0 ns | 2,0 ns | 0 / 0 / 0 | 0 | 0 | 0 |
| `Mask.FromString` | 389,2 ns | 391,0 ns | 7 / 7 / 7 | 197 | 0 | 160 |
| `Parser.ReadProject` | 3,78 ms | 3,71 ms | 13 262 / 13 262 / 13 262 | 1 870 683 | 0 | 149 044 |
| `Parser.ReadProject.TwoScenarios` | 4,72 ms | 4,71 ms | 19 930 / 19 930 / 19 930 | 2 883 888 | 0 | 311 884 |

Le minimum des yields complexes varie d'une mesure à l'autre : 2 quand une clé entre au cache
(section 2.1). Pic de mémoire privée du processus : 20,8 à 21,1 Mo. Sans l'attente, sur le build
de Gabriel, `Parser.ReadProject` donnait 5,42 et 5,44 ms (section 2.10).

### 7.4 Première référence « avant #348 » (lot 1, 2026-09-29)

Mesure complète du 2026-09-29, sans `-j`, sur le build de Gabriel (générateur Visual Studio)
du commit `47d4ea45`, sans modification locale : FMT 1.3.0, Release, MSVC 19.44.35228, tas du
CRT. Machine : Intel Core i9-13900, 32 cœurs logiques, Windows 10.0.22631, 40 Gio disponibles.
Deux mesures, à la suite :

| Benchmark | Médiane (ns par appel), mesure 1 | Médiane, mesure 2 | Allocations par appel (min / médiane / max) | Octets par appel | Pic du tas (octets) |
|---|---|---|---|---|---|
| `ComplexYield.Sum` | 376,1 | 377,9 | 2 / 4 / 4 | 82 | 80 |
| `ComplexYield.Multiply` | 310,4 | 310,4 | 2 / 4 / 4 | 66 | 64 |
| `ComplexYield.Divide` | 354,3 | 349,2 | 2 / 5 / 5 | 82 | 80 |
| `ComplexYield.Subtract` | 342,1 | 349,6 | 2 / 5 / 5 | 82 | 80 |
| `ComplexYield.Shift` | 621,1 | 622,8 | 2 / 13 / 13 | 366 | 348 |
| `ComplexYield.Equation` | 2590,9 | 2588,6 | 2 / 31 / 31 | 2858 | 872 |
| `ComplexYield.RecursiveChain` | 1567,7 | 1547,8 | 2 / 20 / 20 | 410 | 400 |

Pic de mémoire privée du processus : 21,5 à 21,7 Mo, lecture de TWD_land comprise. Le minimum
de 2 allocations vient des clés entrées au cache pendant la mesure (section 2.1) ; à la seconde mesure,
un appel de `ComplexYield.Equation` en a fait 33 : une valeur mise au cache, 2 allocations de
plus.

La référence provisoire du lot 1 (exécutable compilé avec `cl` hors CMake, `FMTlib.dll` du
2026-09-17) tombait à 4 % au plus de ces médianes.

Les JSON de ces deux mesures restent locaux, hors du dépôt (décision 5).

Chacun de ces benchmarks a subi la pause au démarrage d'un processus (section 2.10) : un
échantillon trop long, sans effet notable sur les médianes. Cette référence est remplacée par
celle de la section 7.1, avec laquelle elle concorde.

### 7.5 Lot 3

Mesures complètes du 2026-10-05, par ctest, sans `-j`, un processus par benchmark, sur le build de
Gabriel du commit `88daf175`, compilé avant le commit : l'en-tête des JSON indique `852e1be0`,
modifié, pour les mêmes sources. Tas du CRT, MOSEK, fil qui mesure sur les cœurs performants,
attente de 250 ms. Machine : Intel Core i9-13900, 32 cœurs logiques, Windows 10.0.22631, 40 Gio
disponibles. Trois mesures des benchmarks publics, deux des privés ; la deuxième mesure publique
est décalée (section 2.10) et reste hors du tableau, sauf pour les pics.

| Benchmark | Médiane, mesure 1 | Mesure 3 | Phases, mesure 1 (ms) | Allocations par appel | Mémoire retenue (octets) | Pic (Mo), trois mesures |
|---|---|---|---|---|---|---|
| `Flow.Optimize` | 30,4 ms | 29,3 ms | lecture 5,0 ; construction 2,0 ; résolution 21,5 | 47 167 | -785 | 28,6 à 28,7 |
| `Flow.Optimize.Long` | 63,1 ms | 63,9 ms | lecture 4,5 ; construction 26,2 ; résolution 29,2 | 537 987 | -785 | 62,0 à 62,5 |
| `Flow.Replay` | 32,0 ms | 31,6 ms | lecture 7,3 ; construction 22,5 | 54 147 | -785 | 23,7 à 24,2 |
| `Flow.Outputs` | 3,7 ms | 3,7 ms | — | 54 227 | 0 | 23,1 à 23,2 |
| `Flow.Simulate` | 32,5 ms | 32,7 ms | lecture 6,2 ; simulation 24,1 | 51 191 | -785 | 28,3 à 28,5 |
| `Flow.Replanning` | 124,9 ms | 128,7 ms | lecture 21,3 ; préparation 31,5 ; replanification 68,7 ; résultat 1,1 | 799 177 | 4 737 039 | 75,5 à 76,0 |

Les valeurs provisoires du 2026-10-02, prises avec l'exécutable du scratchpad, tombaient à 5 % au
plus de ces médianes, avec les mêmes allocations. La mémoire retenue passe de -792 à -785 octets
depuis la correction du comptage des blocs de taille 0 (section 2.8).

Groupe privé, sur trois modèles de production, désignés ici sans leur nom, deux mesures :

| Benchmark | Ce qu'il mesure | Médiane, mesure 1 | Mesure 2 | Phases, mesure 1 | Allocations par appel | Mémoire retenue (octets) | Pic (Mo) |
|---|---|---|---|---|---|---|---|
| `Flow.Optimize.Bfec` | optimisation sur 5 périodes ; 381 sections de yields, 1 526 outputs | 3,48 s | 3,56 s | lecture 1,80 s ; construction 1,60 s ; résolution 53 ms | 38 825 307 à 38 825 442 | 1 773 puis -785 (section 2.16) | 581,8 à 582,4 |
| `Flow.Replay.Bfec` | cédule rejouée sur 30 périodes, comme `FMTsetsolution` | 1,65 s | 1,59 s | lecture 0,96 s ; construction 0,66 s | 19 390 388 | -785 | 942,3 à 942,7 |
| `Flow.Outputs.Bfec` | tous les outputs du même modèle sur 5 périodes | 1,22 s | 1,18 s | — | 17 573 027 à 17 573 213 | 80 puis 0 | 220,1 à 220,9 |
| `Flow.Replanning.Bfec` | 1 réplicat de 5 périodes, un fil | 7,07 s | 7,12 s | lecture 0,46 s ; préparation 1,19 s ; replanification 5,41 s | 42 292 463 à 42 292 489 | 4 737 303 | 347,3 à 348,5 |
| `Yield.Model.Bfec` | `YV_S` pour 1 000 développements du premier modèle | 73,6 µs | 69,9 µs | — | 0 | 0 | 206,2 |

Le nombre d'allocations des flux privés qui calculent des yields complexes varie de quelques
dizaines d'une mesure à l'autre : les valeurs servies par le cache (section 2.1). Les médianes
privées sont de 1 à 10 % plus hautes que les valeurs provisoires du 2026-10-02, prises avec un
autre exécutable, un autre jour : à mesurer de nouveau avant d'y voir un changement (section 2.10).

Les benchmarks des lots 1 et 2, dans les mêmes mesures, gardent les allocations et les octets de la
référence de la section 7.1 ; leurs médianes s'en écartent de -12 à +12 % (`Yield.Age` : +18 % à la
troisième mesure). Les JSON restent locaux, hors du dépôt (décision 5).

### 7.6 Lot 4

Mesures complètes du 2026-10-08, par ctest, sans `-j`, un processus par benchmark, sur le build de
Gabriel compilé avec la correction de la section 2.20, avant le commit du lot 4 (en-tête `cdf6a63f`,
modifié). Tas du CRT, MOSEK, tous les fils sur les cœurs performants, attente de 250 ms. Machine :
Intel Core i9-13900, 32 cœurs logiques, Windows 10.0.22631, 43 Gio disponibles. Trois mesures des
benchmarks publics, deux des privés ; les pics du mode court viennent de `-L performance` et de
`-L bfec-perf` du même jour.

| Benchmark | Médiane, mesures 1, 2 et 3 | Replanification, mesure 1 | Allocations par appel | Mémoire retenue (octets) | Pic (Mo), court / complet |
|---|---|---|---|---|---|
| `Flow.Replanning` | 123,1 ; 121,8 ; 122,1 ms | 68,4 ms | 799 167 | 4 737 039 | 62,2 / 75,7 à 75,8 |
| `Flow.Replanning.Threads1` | 368,2 ; 367,9 ; 371,8 ms | 311,0 ms | 3 417 641 | 51 598 927 | 159,4 / 314,4 à 314,6 |
| `Flow.Replanning.Threads2` | 229,5 ; 227,7 ; 235,7 ms | 166,8 ms | 3 417 641 | 51 598 927 | 174,0 / 334,6 à 338,7 |
| `Flow.Replanning.Threads5` | 149,3 ; 142,6 ; 143,3 ms | 77,5 ms | 3 417 641 | 51 598 927 | 225,6 / 382,6 à 387,0 |
| `Flow.Replanning.Threads10` | 122,6 ; 118,1 ; 124,2 ms | 57,8 ms | 3 417 641 et 3 417 642 | 51 598 927 | 287,3 / 444,1 à 448,8 |

Rapport de `ScalingReport.cmake` sur chaque mesure ; les quatre lignes ont écrit les mêmes lignes à
chaque mesure (empreinte `2714db3feb3b0737`) :

| Fils | Accélération d'un appel, mesures 1, 2 et 3 | Efficacité | Accélération de la replanification | Pic de plus par fil |
|---|---|---|---|---|
| 2 | 1,60 ; 1,61 ; 1,57 | 78 à 80 % | 1,86 ; 1,91 ; 1,86 | 20,2 à 24,1 Mo |
| 5 | 2,46 ; 2,57 ; 2,59 | 49 à 51 % | 4,01 ; 3,91 ; 4,00 | 17,0 à 18,1 Mo |
| 10 | 3,00 ; 3,11 ; 2,99 | 29 à 31 % | 5,38 ; 5,47 ; 5,45 | 14,4 à 14,9 Mo |

- Le pic de plus par fil compte aussi les blocs que MOSEK garde d'un appel à l'autre (section 2.17).
- La phase de résultat, où le banc relit ce que la replanification a écrit, dure moins de 2 ms sur un
  fil et jusqu'à 9 ms sur plusieurs, sans cause cherchée : elle pèse sur l'accélération d'un appel,
  pas sur celle de la replanification.
- Comparées à la section 7.5 par `CompareResults.cmake`, les médianes des benchmarks des lots 1 à 3
  vont de -6 à +8 % (`Yield.Age`), avec les mêmes allocations, sauf `Flow.Replanning` : 10 de moins,
  la suppression du dossier de sortie étant sortie de l'appel (section 2.20). Le script avertit que
  les durées ne se comparent pas, puisque le champ `processors` a changé (section 4.3).
- Les mesures provisoires du 2026-10-07, prises avec l'exécutable du scratchpad avant la correction,
  donnaient des appels de 12 à 18 % plus longs (`Flow.Replanning.Threads1` : 413,2 et 426,5 ms) et des
  accélérations de 1,60, 2,47 et 2,86 : un autre exécutable, un autre jour (section 2.10).

Groupe privé, `Flow.Replanning.Threads5.Bfec` (10 réplicats de 5 périodes sur 5 fils), la même
empreinte à chaque passage (`ba037ca987cc9f37`) :

| Mode | Médiane | Replanification | Allocations par appel | Mémoire retenue (octets) | Pic (Mo) |
|---|---|---|---|---|---|
| court | 17,59 s | 15,89 s | 330 679 296 | 47 381 389 | 1 386,3 |
| complet, mesure 1 | 19,26 s | 17,36 s | 330 675 893 | 47 383 402 | 1 291,2 |
| complet, mesure 2 | 19,47 s | 17,41 s | 330 675 300 | 47 386 082 | 1 298,1 |

- Sur un fil, les mêmes 10 réplicats prenaient 60,41 s par appel en mode court, avec un pic de
  425,5 Mo (2026-10-07, exécutable du scratchpad).
- Le pic sur 5 fils varie selon le chevauchement des réplicats : de 1 123 à 1 386 Mo en mode court,
  de 1 291 à 1 567 Mo en mesure complète, sur quatre passages de chaque mode depuis le 2026-10-07. Sa
  borne, dans le fichier local, est de 1 725 Mo.
- Les autres benchmarks privés, en mesure complète : `Flow.Optimize.Bfec` 3,53 et 3,49 s,
  `Flow.Replay.Bfec` 1,61 et 1,58 s, `Flow.Outputs.Bfec` 1,14 et 1,13 s, `Flow.Replanning.Bfec` 6,67
  et 7,00 s, `Yield.Model.Bfec` 71,0 et 71,8 µs : de -6 à +2 % de la section 7.5.

Les JSON restent locaux, hors du dépôt (décision 5).
