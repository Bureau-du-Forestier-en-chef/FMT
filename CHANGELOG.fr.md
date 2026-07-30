# Journal des modifications

Tous les changements notables de ce projet sont consignés dans ce fichier.

Le format s'appuie sur [Keep a Changelog](https://keepachangelog.com/fr/1.1.0/),
et ce projet suit le [versionnage sémantique](https://semver.org/lang/fr/).

> English version: [CHANGELOG.md](CHANGELOG.md)

## [v1.1.0] - 2026-07-30 (0231c55e)

> **Sources de version désynchronisées :** dernier tag `v1.0.2`, `CMakeLists.txt` déclare `1.0.3`,
> `vcpkg.json` déclare `1.0.0`. Le renommage cassant en camelCase justifierait `v2.0.0` selon un
> SemVer strict. À réconcilier au moment de couper la version.

### Ajouté
- Exposition des thèmes statiques et du nombre maximal d'avertissements à l'interface R, avec des règles de patch supplémentaires.
- Utilitaire de fermeture du journal (log) appelable depuis le wrapper (`FMTWrapperCore`).
- Rétablissement du module FMTExcel.
- Nouvel exemple Python associant les actions aux superficies, ainsi que des fichiers d'exemple et Excel additionnels.
- Couverture de tests additionnelle pour `writeProject` et `testWrapperCore` (dont le recuit simulé).

### Modifié
- **Cassant :** Migration des noms de méthodes publiques vers le camelCase dans toute l'API (auparavant PascalCase/minuscule), y compris les couches UI et Excel.
- Amélioration de l'optimiseur par recuit simulé : taux de recuit configurable, gestion par mini-lots et critères d'arrêt basés sur la température.
- Reprise des docstrings Doxygen et correction de fautes d'orthographe dans la documentation des méthodes.

### Corrigé
- Résolution de l'instabilité du journal (log) : plantages silencieux, journal bloqué et problèmes d'horodatage.
- Correction des contraintes d'adjacence et de voisinage pour l'ordonnanceur de secteurs d'aménagement (`OAscheduler`).
- Correction de l'optimisation du Replanner.
- Correction de `writeProject` et de la sortie raster GDAL.
- Correction des régressions de sensibilité à la casse introduites par le renommage camelCase.
- Correction des avertissements et de la compilation R (dont le problème de `git fetch` pour R).
- Résolution des tickets #317 et #320.

### Supprimé
- Suppression de l'en-tête obsolète `Include/FMTareaparser.h`.

[v1.1.0]: https://github.com/Bureau-du-Forestier-en-chef/FMT/compare/v1.0.2...HEAD
