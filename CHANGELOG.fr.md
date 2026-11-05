## [v1.5.0] - 2026-11-04 (84b8301b)

### Ajouté

- Exportation par `FMTlib` de 18 classes du Core qu’un programme lié à la DLL, comme un test unitaire, ne pouvait pas utiliser : les comparateurs `FMTActionComparator`, `FMTActualDevelopmentComparator`, `FMTMaskComparator`, `FMTOutputNodeValueComparator`, `FMTOutputNodeOriginComparator`, `FMTOutputSourceComparator`, `FMTThemeComparator` et `FMTTransitionComparator`, `FMTOutputNodeHasher`, `FMTYieldRequest`, `FMTExpression`, `FMTFunctionCall`, `FMTYieldsCache`, `FMTYieldDevelopment`, et les modèles de rendement `FMTYieldModel`, `FMTYieldModelRandom`, `FMTYieldModelTsla` et `FMTYieldModelUnitCoverage` (#350).

- Ajout d’une couverture de tests substantielle pour le Core, la sérialisation, les tâches parallèles et les scénarios de régression, incluant :
  - des tests unitaires et d’intégration pour les expressions, rendements, contraintes, actions, transitions, cédules, modèles, solveurs et types spatiaux ;
  - des tests d’intégration de sérialisation C++ et Python ;
  - des tests d’intégration des tâches parallèles ;
  - des mesures de couverture et des benchmarks d’allocations des chemins chauds ;
  - plusieurs scénarios de régression (`defaultdeath`, `spatialconstraints`, `harvestfromperiod3`, `decisiondeath`, etc.).

- Ajout d’informations diagnostiques supplémentaires lors des exceptions de type « vector too long » et de tests dédiés pour reproduire ces erreurs (#401).

### Modifié

- `FMTTaskHandler::onDemandRun` attend désormais jusqu’à 10 ms entre deux vérifications de ses tâches afin d’éviter l’occupation complète d’un cœur processeur.

- `FMTModel::preSolve` produit une erreur `FMTmissingdevelopment` plus explicite en nommant le modèle concerné lorsque sa superficie est vide.

- Les exceptions de FMT n’écrivent plus directement de messages sur la sortie standard, améliorant leur intégration dans les applications hôtes.

- Amélioration de la compatibilité avec Rtools en corrigeant l’itérateur `const` utilisé par `FMTYields`.

### Corrigé

- Réparation de l’action `_DEATH` lorsqu’elle est appliquée à des agrégats. Le comportement par défaut conserve correctement les attributs des peuplements morts (#415).

- Correction de `_ENDPOINT` afin d’éviter une violation d’accès lorsque la source sort de ses bornes (#392).

- Correction de `isPeriodBounded`, qui retournait à tort vrai pour toutes les périodes du graphe (#400).

- Correction de plusieurs problèmes liés à `_DISCOUNTFACTOR` :
  - calcul utilisant les paramètres du facteur plutôt que sa valeur réelle ;
  - contamination entre modèles causée par le cache statique des rendements complexes ;
  - réécriture incorrecte de certains modèles (#380, #390, #403).

- Correction de `FMTYields::operator==`, qui comparait les adresses des gestionnaires plutôt que leur contenu, entraînant des écarts entre copies d’un même modèle et des réécritures inutiles des fichiers de rendements (#383).

- Correction de `FMTTimeYieldHandler` pour que chaque rendement d’un bloc `*YT` utilise sa propre période de départ (#388).

- Correction du calcul `_MAX` pour les rendements complexes lorsque toutes les valeurs sont négatives (#389).

- Correction de l’écriture des contraintes spatiales `_SIZE`, `_ADJACENCY` et `_GROUP`, qui pouvait produire des modèles invalides et impossibles à relire (#397).

- Correction de `FMTConstantParser::write` afin de préserver correctement les constantes comportant plusieurs périodes lors d’un cycle écriture/relecture (#404).

- Correction de l’écriture des contraintes `_RANDOM(<output>)`, désormais relues fidèlement après export (#382).

- Correction de l’écriture des objectifs `_PENALTY` sans signe (#379).

- Correction de la lecture du paramètre `MAX_RET` des aires d’opération (#359).

- Correction de la gestion des multipolygones et des géométries avec coordonnées Z lors du calcul des voisinages spatiaux (#394, #396).

- Rétablissement du correctif de durée de vie du journal de `FMTLpSolver`, perdu lors d’une fusion précédente.

- Correction du complément Excel afin de retrouver correctement les méthodes renommées de `FMTExcelCache`.

- Correction des séries d’actions afin de préserver le comportement attendu lors de leur évaluation.

