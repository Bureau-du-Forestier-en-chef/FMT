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
de #348** : celle de la section 7.1 a été prise le 2026-09-29, sur le build de Gabriel du commit
`47d4ea45`, sous le tas du CRT. Une mesure « après » se prend sur la même machine, avec le même
allocateur.

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
- **Flux spatiaux** (SES, rastérisation, ordonnanceur d'aires d'opération, replanification) :
  hors du premier jeu ; à la demande après la fermeture.

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
   *Pourquoi* : FMTlib est lié à mimalloc sans le charger (section 2.6). Actif, mimalloc change de 23 à
   38 % les durées des yields complexes : deux mesures faites sous deux allocateurs ne se
   comparent pas. Le champ est entré avant le premier commit du banc : le schéma reste 1.
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
    (section 2.10), qui fausse la lecture d'un projet. La référence du lot 1 reste consignée.

Les choix de conception de chaque lot sont en section 3 ; ceux des lots 1 et 2 sont validés
(2026-09-29 et 2026-09-30).

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
    (section 2.11) : il est mesuré et comparé, mais pas borné.

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
- **Mesuré au lot 2** (section 7.3) :

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

### 2.6 mimalloc est lié à FMTlib mais n'est jamais chargé (démontré au lot 1)

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
- **Issue** : #361, publiée le 2026-09-29.

### 2.7 Threads (à examiner au lot 4)

`m_lookat` appartient au gestionnaire de yields, partagé par tous les threads, et il est écrit
sans verrou. Le lien avec la course connue des caches de yields
(section 7 de `Examples/C++/tests/ETAT.md`) n'est pas établi.

### 2.8 Comptage des allocations

- **`operator new` est lié dans chaque module.** Selon `dumpbin /IMPORTS` sur
  `build/release/bin/Release/FMTlib.dll`, la DLL importe `malloc`, `free`, `calloc` et
  `_callnewh` de `api-ms-win-crt-heap-l1-1-0.dll`, et aucun `operator new`.
- **`FMTlib.dll` importe aussi `HeapAlloc` et `HeapFree`** de `KERNEL32`, que le compteur ne
  voit pas. Sur la lecture de TWD_land et l'évaluation des yields, la sonde du lot 1 n'en a vu
  aucun appel direct.
- **Plusieurs modules allouent.** La lecture de TWD_land alloue dans `FMTlib.dll` (13 766
  allocations), `boost_filesystem` (777) et `MSVCP140` (29) : d'où la redirection dans tous
  les modules chargés. Un module lié à une bibliothèque C statique échappe au compteur : à
  vérifier pour MOSEK au lot 3.
- **Le code en ligne compte pour l'exécutable.** Une fonction définie dans un en-tête de FMT
  (modèle, fonction `inline`, constructeur par défaut) est compilée dans l'exécutable du
  benchmark, pas dans `FMTlib.dll` ; ses allocations sont comptées aussi.
- **Les DLL chargées tard** (pilotes GDAL, par exemple) : la redirection est refaite avant
  chaque passage compté.
- **Point d'entrée depuis un exécutable** : `FMTYieldRequest` n'est pas exportée, mais
  `FMTYields::get(développement.getYieldRequest(), nom)` fonctionne, comme dans
  `Examples/C++/testScenarioReading.cpp:285-286`.

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
  - C'est ce que le lot 1 avait pris pour un « premier échantillon parfois lent » : la référence
    de la section 7.1 l'a subie dans chacun de ses benchmarks, sans effet notable sur ses médianes.
  - Depuis le 2026-09-30, une mesure complète attend 250 ms avant son premier benchmark
    (`BenchmarkSuite.cpp`). Le mode court n'attend pas : ses temps ne sont pas une mesure.
- **Deux mesures complètes du même commit** diffèrent de quelques pour cent (de -2,0 % à
  +0,8 % au lot 1) : un écart de cet ordre n'est pas un changement.
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
  - La référence de la section 7.1 a été prise avant ; ses valeurs sont celles des cœurs
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
- **Le journal de FMT est commun au processus** (`FMTObject::_logger` est statique) : un seul
  `setQuietLogger()` dans la préparation fait taire les lectures mesurées (règle 7).

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
  premier benchmark (section 2.10). À relire.

## 4. Feuille de route et fermeture de #349

### 4.1 Ce que #349 attend, et quel lot s'en charge

| Attente de #349 | Lot | État |
|---|---|---|
| Une suite de performance séparée des tests fonctionnels | 1 | fait |
| Benchmarks en Release | 1 | fait (type de build et optimisation notés dans chaque résultat) |
| Jeux de données stables et versionnés | 1, 3, 5 | `perfyields` fait ; flux sur des modèles privés aux lots 3 et 4, hors dépôt (décision 8) ; modèle public moyen éventuel au lot 5 |
| Temps mesuré de façon constante | 1 | fait |
| Nombre d'allocations et octets alloués | 1 | fait (Windows) |
| Pic mémoire, là où c'est possible | 1, 2 | fait : pic du tas et du processus, mémoire retenue par appel (lot 2) |
| Mise à l'échelle multithread | 4 | à faire |
| Environnement dans chaque résultat | 1 | fait |
| Format lisible par une machine | 1, 2 | fait (JSON, schéma 2 depuis le lot 2) |
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
| Premier jeu : un flux de modèle | 3 | à faire |
| Premier jeu : un flux multithread | 4 | à faire |

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

- **Lot 1 : socle et yields complexes** : fait, validé le 2026-09-29 (section 5.2) ; référence en section 7.1.
- **Lot 2 : lecture des yields, masques, lecture d'un projet** : livré le 2026-09-29, validé sur le
  build de Gabriel le 2026-09-30 (section 5.3) ; à commiter, puis la référence de #348 est reprise
  (section 6.1).
  - `Yield.Age`, `Yield.Age.NewRequest`, `Mask.IsSubsetOf`, `Mask.FromString`,
    `Parser.ReadProject` et `Parser.ReadProject.TwoScenarios`.
  - Mémoire retenue par appel, réglages par benchmark, `ProcessorPolicy` (section 3.2).
- **Lot 3 : flux de modèle**, effort `high`.
  - Construction du graphe et de la matrice, optimisation (règle 8 ; 77 sans OSI).
  - Calcul de tous les outputs, rejeu d'une cédule, simulation NSS sans solveur.
  - Tailles « petite » et « moyenne » par la longueur de l'horizon de TWD_land.
  - Réglages : `RunSettings::forSlowCalls` existe depuis le lot 2 ; un flux de plusieurs
    secondes en demandera peut-être de plus légers.
  - Groupe privé (décision 8). Candidats, d'après les durées que ctest a notées :
    `FMTsetsolution` (lecture, construction avec une cédule imposée, simulation, un output
    vérifié, sans optimiser ; de 5 à 80 s), la lecture des mêmes modèles, un seul `doplanning`
    de taille moyenne. À écarter : les tests aléatoires, ceux de plus de 5 minutes et ceux que
    MOSEK domine.
  - La variante des yields sur un gros modèle privé, reportée du lot 2 : la recherche des blocs
    de yields d'un développement (`FMTYieldRequest::_updateData`, puis la boucle de
    `FMTYields::get`) grandit avec le nombre de sections, et TWD_land en a peu.
- **Lot 4 : threads**, effort `xhigh`.
  - `FMTTaskHandler` et `FMTPlanningTask` sur plusieurs scénarios, avec 1, 2, 4, 8 workers et
    le maximum (usage : `Examples/C++/planningtest.cpp:94-97`).
  - Accélération, efficacité, pic mémoire, allocations par worker : compteurs par thread à
    ajouter au moniteur.
  - Résultats validés ; la course connue (section 2.7) est à distinguer d'une régression.
  - Groupe privé : `planningtest` sur un modèle réel (de 18 à 25 s par ligne).
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
  médiane de -1,4 % à +2,2 %, allocations identiques (section 7.1). Les bornes du CSV sont les médianes
  mesurées : aucune ne change. JSON gardés localement, hors du dépôt et du dossier de build.
- **Relecture** (2026-09-29) : Gabriel valide les choix de conception (section 3.1) ; issue
  #362 pour `hasFeature("ONNXRUNTIME")`.

### 5.3 Lot 2 : lecture des yields, masques, lecture d'un projet (2026-09-29)

**Statut** : livré le 2026-09-29 ; compilé par Gabriel et validé le 2026-09-30, choix relus
(section 3.2). Pas encore commité.

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

## 6. Prochain lot

Les lots 1 et 2 sont validés (sections 5.2 et 5.3), et leurs choix de conception aussi
(sections 3.1 et 3.2), sauf l'attente de 250 ms, ajoutée à la validation du lot 2.

### 6.1 D'abord : commiter le lot 2 et reprendre la référence de #348

1. Gabriel commite le lot 2 (avec l'attente, les deux ETAT et la documentation), puis compile. Une
   seule compilation suffit, sans reconfiguration : elle apporte l'attente, et l'en-tête du commit
   porte le nouveau commit avec `dirty` à 0.
2. Claude lance `-L performance` et la suite base (`-E "T:/" -j 8`) sur ce build.
3. Claude prend la référence (décision 12) : deux mesures complètes à la suite, sans `-j`, la
   machine au repos. Elle remplace la section 7.1 ; celle du lot 1 passe en section 7.4. JSON
   gardés localement, hors du dépôt (décision 5).
4. Gabriel relit l'attente de 250 ms (section 3.2).

### 6.2 Ensuite : lot 3

Flux de modèle (section 4.2), avec le groupe privé (décision 8), effort `high`.

## 7. Mesures

### 7.1 Référence « avant #348 »

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

Les JSON de ces deux mesures restent locaux, hors du dépôt (décision 5) : ils servent à comparer
la mesure « après » de #348 avec `CompareResults.cmake`. Sans eux, la comparaison se fait à la
main depuis ce tableau.

Chacun de ces benchmarks a subi la pause au démarrage d'un processus (section 2.10) : un
échantillon trop long, sans effet notable sur les médianes. Cette référence sera remplacée par
celle du harnais du lot 2 (décision 12), et gardée en section 7.4.

### 7.2 La suite de performance

| Date | Lignes | Dont allocation | Mode court sous `-j 8` (temps réel) | Mesure complète (temps réel) |
|---|---|---|---|---|
| 2026-09-25 (lot 1, projet jetable) | 7 | 7 | 0,12 à 0,21 s | 1,86 s |
| 2026-09-25 (build de Gabriel, passage complet de la suite sous `-j 8`) | 7 | 7 | 0,93 s-processeur en tout | — |
| 2026-09-29 (commit `47d4ea45`, build de Gabriel) | 7 | 7 | 0,95 s, sans `-j` | 2,0 s |
| 2026-09-29 (lot 2, projet jetable) | 13 | 11 | 0,34 s | 2,7 à 2,9 s, en un seul processus |
| 2026-09-30 (lot 2, build de Gabriel, non commité) | 13 | 11 | 1,6 s, sans `-j` | 3,7 s, sans l'attente |
| 2026-09-30 (lot 2 avec l'attente, exécutable du scratchpad) | 13 | 11 | — | 6,9 s, un processus par benchmark |

### 7.3 Lot 2, avec l'attente

Deux mesures complètes du 2026-09-30, un processus par benchmark comme sous ctest, avec
l'exécutable du scratchpad : les sources du lot 2 avec l'attente de 250 ms, compilées hors CMake
contre le `FMTlib.dll` du build de Gabriel. Fil qui mesure sur les cœurs performants, tas du CRT.
La référence de la décision 12 les remplacera.

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
