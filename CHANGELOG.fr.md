# Journal des modifications

Tous les changements notables de ce projet sont consignés dans ce fichier.

Le format s'appuie sur [Keep a Changelog](https://keepachangelog.com/fr/1.1.0/),
et ce projet suit le [versionnage sémantique](https://semver.org/lang/fr/).

> English version: [CHANGELOG.md](CHANGELOG.md)

## [Non publié]

### Modifié
- Amélioration des outils de génération et de publication du changelog.
- Le script `Release-Changelog.ps1` prend désormais en charge un mode d'accumulation basé sur le titre `## [Non publié]` / `## [Unreleased]`, permettant de valider, committer et pousser les mises à jour du changelog sans créer de tag, de branche d'archivage ni effectuer de mise à jour de version.

## [v1.1.0] - 2026-07-30 (ac3bddd1)

### Ajouté
- Exposition des thèmes statiques dans l’interface R, avec ajout de nouvelles règles de correctifs associées.
- Réintégration du support FMTExcel et ajout d’exemples d’utilisation (dont un exemple Python « Map to Area »).
- Ajout d’un utilitaire permettant au wrapper de fermer correctement les journaux (logs).
- Ajout d’options et de fonctionnalités dans le wrapper R liées à la nouvelle syntaxe et aux mécanismes de remplacement (`_replace`, `rxreplace`).

### Modifié
- Refonte importante de l’API et du wrapper R : harmonisation des noms de méthodes vers le format `camelCase`, corrections de casse et renommages cohérents.
- Introduction et maturation d’une nouvelle infrastructure de gestion par lots (« batch » / mini-batch) pour les méthodes d’optimisation.
- Refonte de la gestion des blocs et réorganisation de plusieurs composants internes afin d’améliorer la maintenabilité.
- Amélioration de l’algorithme de recuit simulé (Simulated Annealing), notamment des paramètres de température et du comportement d’optimisation.
- Améliorations de la documentation Doxygen, des exemples et du système de génération du changelog.
- Mise à jour du système de compilation (CMake, build MAM) et ajustements de compatibilité pour l’interface R.

### Corrigé
- Correction de l’optimisation spatialement explicite lorsqu’aucune cache n’est disponible (#317 Optimisation spatialement Explicite non Fonctionnelle sans la cache).
- Résolution de plusieurs problèmes liés au système de journalisation et au gestionnaire d’exceptions du wrapper, incluant des blocages, incohérences et plantages silencieux (#313 Adaptation Log et Exception Handler dans FMTWrapper).
- Correction de la gestion des voisinages et des contraintes d’adjacence dans les algorithmes d’ordonnancement et d’optimisation spatiale.
- Correction de la rasterisation lorsque des thèmes invalides sont rencontrés (#326 Invalid Theme Missing Mask sous rastérisation CC).
- Correction de la lecture d’une transition GCBM vide (#311 Lecture d'une transition GCBM vide).
- Correction de plusieurs problèmes de compilation et d’intégration de l’interface R (avertissements, sensibilité à la casse, appels surchargés, export GDAL, compilation multi-plateforme).
- Correction de l’écriture de projets et stabilisation des tests associés.
- Ajout d’un mécanisme d’écriture de la cache depuis l’interface (#320 Créer un bouton pour écrire la cache dans l'interface).
- Correctifs liés à la nouvelle syntaxe et aux expressions régulières SQL (#323 Nouvelle syntaxe, #322 rxreplace, #321 _replace, #316 Adaptation regex pour constante SQL).
- Réduction de plusieurs causes de plantage et amélioration générale de la robustesse du wrapper et des outils d’optimisation.

### Supprimé
- Retrait de certains fichiers d’en-tête et inclusions devenus obsolètes dans le cadre du nettoyage et de la réorganisation du code.