# Refactorisation des noms de méthodes en camelCase

Compte rendu du passage de l'ensemble des méthodes du code FMT en **camelCase**
(ex. `getOperatingArea`), pour uniformiser les anciens noms tout-minuscule
(`setarea`, `clearcache`) et PascalCase (`BuildConstraint`).

## Portée

Renommage des méthodes dans `Include/`, `Source/`, `FMTWrapperCore/`, les
en-têtes template `*.hpp`, `Excel/`, `Examples/`, ainsi que la mise à jour des
sites d'appel dans `UI/`. Le renommage couvre à chaque fois **déclaration +
définition + tous les appels**, de façon cohérente dans tout le dépôt.

## Lots réalisés

| Lot | Description | Méthodes | Occurrences | Fichiers |
|-----|-------------|---------:|------------:|---------:|
| 1 | Noms tout-minuscule / underscore → camelCase (`setarea`→`setArea`, `clearcache`→`clearCache`, `push_data`→`pushData`) | 823 | 6181 | 213 |
| 2 | Noms PascalCase → camelCase (`BuildConstraint`→`buildConstraint`, `PostSolve`→`postSolve`, `MIPparameters`→`mipParameters`) | 144 | 725 | 82 |
| 3 | Méthodes des classes template `*.hpp` (`FMTspec`, `FMTbounds`, `FMTgraph`, `FMTlist`, `FMTlayer`) + `getYldsBounds` | 87 | 815 | 65 |
| 4 | Corrections orthographiques (voir plus bas) | 57 | 426 | ~60 |
| 5 | Dossiers consommateurs `Excel/` + `Examples/` (sites d'appel vers le cœur) | — | ~420 | 63 |
| — | Correctif d'encodage (voir plus bas) | — | — | 185 |

**Total : ~1 054 noms de méthodes distincts renommés.**

## Corrections orthographiques

Certaines coquilles présentes dans le code d'origine avaient d'abord été
préservées telles quelles lors du passage en camelCase. Elles ont ensuite été
corrigées (identifiants uniquement — méthodes, membres, paramètres ; aucune
classe ni interface touchée) :

| Coquille | Corrigé | Exemple |
|----------|---------|---------|
| neihgbor | neighbor | `getNeihgborsPerimeter` → `getNeighborsPerimeter` |
| binairies | binaries | `getCommonBinairies` → `getCommonBinaries` |
| spatialy | spatially | `isSpatialyBinding` → `isSpatiallyBinding` |
| feasability | feasibility | `verifySpatialFeasability` → `verifySpatialFeasibility` |
| refactortorization | refactorization | `needsRefactortorization` → `needsRefactorization` |
| verticies | vertices | `getVerticies` → `getVertices` |
| specificier | specifier | `specificierMask` → `specifierMask` |
| developement | development | membres + commentaires |
| dispertion | dispersion | `getDispertion` → `getDispersion` |

Note : les **noms d'interface R/Python** (chaînes `.method("getneihgborsperimeter", ...)`)
et quelques **libellés dans des messages d'erreur** conservent volontairement
l'orthographe d'origine, pour ne pas casser l'API R/Python exposée.

## Dossiers `Excel/` et `Examples/`

Ces deux dossiers consomment l'API du cœur mais avaient été oubliés au premier
passage. Leurs sites d'appel ont été mis à jour vers les nouveaux noms
(`getbuildexceptions`→`getBuildExceptions`, `writeforesttheme`→`writeForestTheme`,
`doplanning`→`doPlanning`, `getspschedule`→`getSpSchedule`, `GetXSize`→`getXSize`
via `FMTlayer`, etc.). Les appels de méthodes membres de conteneurs STL y sont
protégés (ex. `AllSeries.lower_bound()` sur un `std::set`).

### Add-in Excel (préservé, comme l'UI)

Le complément Excel C# (`Excel/FMTExcel/*.cs`) appelle les méthodes publiques de la
classe managée `FMTexcelcache` **par réflexion sur leur nom** (`GetMethod("...")`).
Les 6 méthodes de `FMTexcelcache` que le renommage avait touchées (`getvalue`,
`getyield`, `getattributes`, `getbuildexceptions`, `getgraphstats`, `getnochoice`)
ont donc été **rétablies à leur nom d'origine**, tout en gardant leurs **appels
internes** vers le cœur en camelCase (`mit->second.getValue()`). L'add-in continue
de fonctionner sans modification.

## Collisions avec des API externes (corrigées)

Le renommage global par token avait touché quelques méthodes **externes** homonymes
de méthodes FMT. Une détection par intersection avec les API STL / GDAL-OGR / boost /
OSI-Coin a permis de toutes les rétablir (7 collisions), en gardant les méthodes FMT
homonymes en camelCase :

| Nom | API externe | Méthode FMT homonyme conservée |
|-----|-------------|--------------------------------|
| `getline`, `lower_bound`, `upper_bound` | std (algo + membres `set`/`map`) | `FMTparser::getLine`, `FMTeventcontainer::lowerBound` |
| `setstate` | `std::ios` | `FMTpythonpickle::setState` |
| `GetXSize`, `GetYSize` | `GDALRasterBand` (`band->…`) | `FMTlayer::getXSize` (`layer.…`) |
| `IsEmpty` | `OGRGeometry` (`geom->…`) | `FMTGraphInfo::isEmpty`, `FMTSerie::isEmpty` |

## Volontairement NON modifié

Ces noms sont restés inchangés, par choix, pour ne rien casser :

- **Méthodes appelées par l'interface (`UI/`)** — l'interface managée (.NET/C++CLI)
  les appelle par nom ; les renommer briserait le lien. Ex. : `getarea`, `getmask`,
  `raise`, `raisefromcatch`, `printexceptions`, ainsi que tous les noms définis
  dans `UI/`.
- **Méthodes publiques de `FMTexcelcache`** — appelées par réflexion depuis le
  complément Excel C# (voir ci-dessus).
- **`serialize`** — requis par Boost.Serialization.
- **Noms conformes STL et opérateurs** — `empty`, `size`, `push_back`, `push_front`,
  `reserve`, `swap`, `operator==`, etc.
- **Interface R / Python** — seuls les pointeurs C++ (`&Classe::methode`) ont été
  renommés. Les **noms exposés** côté R/Python (chaînes dans `.method("...")` /
  `.def("...")`) sont **inchangés**, donc l'API R et Python reste identique.
- **Symboles de bibliothèques externes** — `std::`, `boost::`, GDAL/OGR
  (`OGRlayertoRaster`, `FMTCPLErrorHandler`), Rcpp, ONNX Runtime (`Ort::`),
  API C Python (`PyObject_GetIter`), variables locales boost.python
  (`CoreModule`, `SpatialModule`, `Exceptionclass`…).
- **Mots simples** (déjà du camelCase valide) — `presolve`, `postsolve`, `solve`,
  `build`, `read`, `write`, `compare`, `evaluate`, `simplify`, etc.
- **Déclarations commentées / code mort** — ex. `//void setstatus(...)`,
  `//size_t hashdata(...)` dans certains en-têtes ont été laissées telles quelles.

## Correctif d'encodage

Les fichiers du dépôt sont encodés en **Windows-1252 / Latin-1** (250 fichiers avec
`é` en 0xE9). Un premier passage de l'outil (lecture UTF-8) avait accidentellement
supprimé les octets accentués — « Québec » du copyright était devenu « Qubec ».
Ceci a été **entièrement corrigé** en re-dérivant les fichiers depuis l'original
avec une lecture/écriture **latin-1** (préservation octet par octet). Vérification :
les 326 fichiers ont un nombre d'octets non-ASCII identique à l'original (0 écart).
Les fins de ligne **CRLF** sont également préservées.

## Réserves / à réviser

- Les coquilles historiques ont été corrigées (voir « Corrections
  orthographiques »). Il peut subsister quelques découpages de mots imparfaits sur
  des noms très concaténés (le nom reste un camelCase valide, mais la coupure suit
  l'orthographe d'origine).
- La conversion a été appliquée par substitution de tokens sûre (hors chaînes de
  caractères et hors noms protégés). **La compilation reste à valider** de ton côté,
  notamment pour d'éventuelles surcharges de virtuelles issues de classes de base
  externes qui n'auraient pas été détectées.

## Vérification

- Un passage de balayage (recherche de toutes les définitions `Classe::methode`
  dans les `.cpp`) a confirmé qu'il ne reste plus de méthode FMT non-camelCase en
  dehors des exclusions volontaires listées ci-dessus.
- Une détection dédiée (intersection des noms renommés avec les API externes +
  analyse des récepteurs de type std/GDAL/OGR) a servi à corriger les collisions
  d'homonymes (voir « Collisions avec des API externes »).
- La compilation MSVC (`FMTlib` + `FMTExcelWrapper`) a servi de validation finale et
  a permis de cibler les derniers correctifs (dossiers `Excel/`/`Examples/` oubliés,
  homonymes STL/GDAL/OGR, add-in Excel).
