# Inventaire des classes FMT à convertir en CamelCase

> Généré par `.rename_tools/propose_classes.py` : découpage par dictionnaire **atomique** +
> plus longue correspondance avec **échec dur** (aucun repli silencieux — un splitter glouton
> produit `model|par|ameters`). **À réviser avant les lots 2 à 5.**

## Règles appliquées

- Cible **FMTCamelCase** : préfixe `FMT` en majuscules, puis majuscule à chaque mot.
- Acronymes en Pascal, **sauf `FMT` et `GCBM`** — règle déjà actée dans `camelCase_inventory.md:11`
  et appliquée à ~1054 méthodes. Donc `FMTLpModel`, `FMTSaModel`, `FMTSesModel`.
- **Noms exposés R/Python gelés** : le C++ est renommé, la chaîne `class_<>("FMTaction")` ne bouge pas.
  Le token `@DocString(X)`, lui, est renommé (clé d appariement doc, pas une API).
- Coquilles **conservées** dans les classes (le lot orthographique ne visait que les méthodes).

## Colonne « Statut »

- **acté** : décision non mécanique, validée par Gabriel le 2026-07-16.
- mécanique : découpage non ambigu.

## Classes (137)

| Ancien | Nouveau | Statut | Note |
|---|---|---|---|
| `FMTGCBMtransition` | `FMTGCBMTransition` | **acté** | GCBM reste en majuscules (règle actée) |
| `FMTaction` | `FMTAction` | mécanique |  |
| `FMTactionparser` | `FMTActionParser` | mécanique |  |
| `FMTactualdevelopment` | `FMTActualDevelopment` | mécanique |  |
| `FMTactualdevelopmentcomparator` | `FMTActualDevelopmentComparator` | mécanique |  |
| `FMTagebounds` | `FMTAgeBounds` | mécanique |  |
| `FMTageyieldhandler` | `FMTAgeYieldHandler` | mécanique |  |
| `FMTareaparser` | `FMTAreaParser` | mécanique |  |
| `FMTbaseedgeproperties` | `FMTBaseEdgeProperties` | mécanique |  |
| `FMTbasevertexproperties` | `FMTBaseVertexProperties` | mécanique |  |
| `FMTbindingspatialaction` | `FMTBindingSpatialAction` | mécanique |  |
| `FMTbounds` | `FMTBounds` | mécanique |  |
| `FMTcomplexyieldhandler` | `FMTComplexYieldHandler` | mécanique |  |
| `FMTconstantparser` | `FMTConstantParser` | mécanique |  |
| `FMTconstants` | `FMTConstants` | mécanique |  |
| `FMTconstraint` | `FMTConstraint` | mécanique |  |
| `FMTcoordinate` | `FMTCoordinate` | mécanique |  |
| `FMTdata` | `FMTData` | mécanique |  |
| `FMTdebugexceptionhandler` | `FMTDebugExceptionHandler` | mécanique |  |
| `FMTdebuglogger` | `FMTDebugLogger` | mécanique |  |
| `FMTdefaultexceptionhandler` | `FMTDefaultExceptionHandler` | mécanique |  |
| `FMTdefaultlogger` | `FMTDefaultLogger` | mécanique |  |
| `FMTdevelopment` | `FMTDevelopment` | mécanique |  |
| `FMTdevelopmentpath` | `FMTDevelopmentPath` | mécanique |  |
| `FMTedgeproperties` | `FMTEdgeProperties` | mécanique |  |
| `FMTerror` | `FMTError` | mécanique |  |
| `FMTevent` | `FMTEvent` | mécanique |  |
| `FMTeventcontainer` | `FMTEventContainer` | mécanique |  |
| `FMTeventiteratorsorter` | `FMTEventIteratorSorter` | mécanique |  |
| `FMTeventrelation` | `FMTEventRelation` | mécanique |  |
| `FMTexception` | `FMTException` | mécanique |  |
| `FMTexceptionhandler` | `FMTExceptionHandler` | mécanique |  |
| `FMTexponentialschedule` | `FMTExponentialSchedule` | mécanique |  |
| `FMTexpression` | `FMTExpression` | mécanique |  |
| `FMTforest` | `FMTForest` | mécanique |  |
| `FMTfork` | `FMTFork` | mécanique |  |
| `FMTfreeexceptionhandler` | `FMTFreeExceptionHandler` | mécanique |  |
| `FMTfunctioncall` | `FMTFunctionCall` | mécanique |  |
| `FMTfuturdevelopment` | `FMTFuturDevelopment` | **acté** | **coquille conservée** (« futur »). Les corrections orthographiques excluaient explicitement les classes. Futur→Future = ticket séparé |
| `FMTgraph` | `FMTGraph` | mécanique |  |
| `FMTgraphstats` | `FMTGraphStats` | mécanique |  |
| `FMTgraphvertextoyield` | `FMTGraphVertexToYield` | mécanique |  |
| `FMTlandscapeparser` | `FMTLandscapeParser` | mécanique |  |
| `FMTlayer` | `FMTLayer` | mécanique |  |
| `FMTlifespanparser` | `FMTLifespanParser` | mécanique |  |
| `FMTlifespans` | `FMTLifespans` | mécanique |  |
| `FMTlinegraph` | `FMTLineGraph` | mécanique |  |
| `FMTlist` | `FMTList` | mécanique |  |
| `FMTlockbounds` | `FMTLockBounds` | mécanique |  |
| `FMTlogger` | `FMTLogger` | mécanique |  |
| `FMTlookup` | `FMTLookup` | mécanique |  |
| `FMTlpheuristic` | `FMTLpHeuristic` | mécanique |  |
| `FMTlpheuristicmthandler` | `FMTLpHeuristicMtHandler` | **acté** | mt = multi-thread |
| `FMTlpmodel` | `FMTLpModel` | **acté** | acronyme en Pascal (règle actée) → Lp, et non LP |
| `FMTlpsolver` | `FMTLpSolver` | mécanique |  |
| `FMTmask` | `FMTMask` | mécanique |  |
| `FMTmaskcomparator` | `FMTMaskComparator` | mécanique |  |
| `FMTmaskfilter` | `FMTMaskFilter` | mécanique |  |
| `FMTmatrixbuild` | `FMTMatrixBuild` | mécanique |  |
| `FMTmodel` | `FMTModel` | mécanique |  |
| `FMTmodelcomparator` | `FMTModelComparator` | mécanique |  |
| `FMTmodelparameters` | `FMTModelParameters` | mécanique |  |
| `FMTmodelparser` | `FMTModelParser` | mécanique |  |
| `FMTmodelstats` | `FMTModelStats` | mécanique |  |
| `FMTmodelyieldhandler` | `FMTModelYieldHandler` | mécanique |  |
| `FMTmovestats` | `FMTMoveStats` | mécanique |  |
| `FMTnssmodel` | `FMTNssModel` | **acté** | nss = non spatial simulation |
| `FMTobject` | `FMTObject` | mécanique |  |
| `FMTopareaschedulertask` | `FMTOpAreaSchedulerTask` | **acté** | « oparea » = *operating area* → `Op` + `Area`. Tranché par Gabriel le 2026-07-16. |
| `FMToperatingarea` | `FMTOperatingArea` | mécanique |  |
| `FMToperatingareacluster` | `FMTOperatingAreaCluster` | mécanique |  |
| `FMToperatingareaclusterbinary` | `FMTOperatingAreaClusterBinary` | mécanique |  |
| `FMToperatingareaclusterer` | `FMTOperatingAreaClusterer` | mécanique |  |
| `FMToperatingareacomparator` | `FMTOperatingAreaComparator` | mécanique |  |
| `FMToperatingareascheduler` | `FMTOperatingAreaScheduler` | mécanique |  |
| `FMToperatingareascheme` | `FMTOperatingAreaScheme` | mécanique |  |
| `FMToperatingareaschemecomparator` | `FMTOperatingAreaSchemeComparator` | mécanique |  |
| `FMToperator` | `FMTOperator` | mécanique |  |
| `FMToptimizationparser` | `FMTOptimizationParser` | mécanique |  |
| `FMToutput` | `FMTOutput` | mécanique |  |
| `FMToutputcomparator` | `FMTOutputComparator` | mécanique |  |
| `FMToutputnode` | `FMTOutputNode` | mécanique |  |
| `FMToutputnodecache` | `FMTOutputNodeCache` | mécanique |  |
| `FMToutputnodehasher` | `FMTOutputNodeHasher` | mécanique |  |
| `FMToutputnodeorigincomparator` | `FMTOutputNodeOriginComparator` | mécanique |  |
| `FMToutputnodevaluecomparator` | `FMTOutputNodeValueComparator` | mécanique |  |
| `FMToutputparser` | `FMTOutputParser` | mécanique |  |
| `FMToutputsource` | `FMTOutputSource` | mécanique |  |
| `FMToutputsourcecomparator` | `FMTOutputSourceComparator` | mécanique |  |
| `FMTparallelwriter` | `FMTParallelWriter` | mécanique |  |
| `FMTparser` | `FMTParser` | mécanique |  |
| `FMTperbounds` | `FMTPerBounds` | mécanique |  |
| `FMTplanningtask` | `FMTPlanningTask` | mécanique |  |
| `FMTpredictor` | `FMTPredictor` | mécanique |  |
| `FMTquietexceptionhandler` | `FMTQuietExceptionHandler` | mécanique |  |
| `FMTquietlogger` | `FMTQuietLogger` | mécanique |  |
| `FMTreplanningtask` | `FMTReplanningTask` | mécanique |  |
| `FMTsamodel` | `FMTSaModel` | **acté** | sa = simulated annealing |
| `FMTsaschedule` | `FMTSaSchedule` | mécanique |  |
| `FMTsasolution` | `FMTSaSolution` | mécanique |  |
| `FMTschedule` | `FMTSchedule` | mécanique |  |
| `FMTscheduleparser` | `FMTScheduleParser` | mécanique |  |
| `FMTseException` | `FMTSeException` | **acté** | le « se » est *Structured Exception* (SEH, <eh.h>), pas le « se » de FMTsemodel ; cohérent avec FMTScopedSeTranslator / getSeNumber |
| `FMTsemodel` | `FMTSeModel` | **acté** | se = spatially explicit |
| `FMTserializablematrix` | `FMTSerializableMatrix` | mécanique |  |
| `FMTsesmodel` | `FMTSesModel` | **acté** | ses = spatially explicit simulation |
| `FMTsolverlogger` | `FMTSolverLogger` | mécanique |  |
| `FMTspatialnodescache` | `FMTSpatialNodesCache` | mécanique |  |
| `FMTspec` | `FMTSpec` | mécanique |  |
| `FMTsrmodel` | `FMTSrModel` | **acté** | sr = spatially referenced |
| `FMTtask` | `FMTTask` | mécanique |  |
| `FMTtaskhandler` | `FMTTaskHandler` | mécanique |  |
| `FMTtasklogger` | `FMTTaskLogger` | mécanique |  |
| `FMTtheme` | `FMTTheme` | mécanique |  |
| `FMTthemecomparator` | `FMTThemeComparator` | mécanique |  |
| `FMTtimeyieldhandler` | `FMTTimeYieldHandler` | mécanique |  |
| `FMTtransition` | `FMTTransition` | mécanique |  |
| `FMTtransitioncomparator` | `FMTTransitionComparator` | mécanique |  |
| `FMTtransitionmask` | `FMTTransitionMask` | mécanique |  |
| `FMTtransitionparser` | `FMTTransitionParser` | mécanique |  |
| `FMTversion` | `FMTVersion` | mécanique |  |
| `FMTvertexlookup` | `FMTVertexLookup` | mécanique |  |
| `FMTvertexproperties` | `FMTVertexProperties` | mécanique |  |
| `FMTwarning` | `FMTWarning` | mécanique |  |
| `FMTyieldhandler` | `FMTYieldHandler` | mécanique |  |
| `FMTyieldmodel` | `FMTYieldModel` | mécanique |  |
| `FMTyieldmodelRandom` | `FMTYieldModelRandom` | mécanique |  |
| `FMTyieldmodelTSLA` | `FMTYieldModelTsla` | **acté** | applique la règle : acronymes en Pascal, seuls `FMT` et `GCBM` restent en majuscules. Tranché par Gabriel le 2026-07-16. |
| `FMTyieldmodelUnitCoverage` | `FMTYieldModelUnitCoverage` | mécanique |  |
| `FMTyieldmodeldecisiontree` | `FMTYieldModelDecisionTree` | mécanique |  |
| `FMTyieldmodelnep` | `FMTYieldModelNep` | **acté** | nep = net ecosystem production |
| `FMTyieldmodelnn` | `FMTYieldModelNn` | **acté** | nn = neural network |
| `FMTyieldmodelpools` | `FMTYieldModelPools` | mécanique |  |
| `FMTyieldparser` | `FMTYieldParser` | mécanique |  |
| `FMTyieldrequest` | `FMTYieldRequest` | mécanique |  |
| `FMTyields` | `FMTYields` | mécanique |  |
| `FMTyldbounds` | `FMTYldBounds` | **acté** | « Yld » conservé : précédent getYldsBounds (lot 3 des méthodes) ; fratrie FMTAgeBounds / FMTPerBounds / FMTLockBounds |

## Hors périmètre

| Nom | Raison |
|---|---|
| `FMTexcelcache`, `FMTmodelcache` | déclarées dans `Excel/` ; `FMTexcelcache` est résolue par réflexion C# (`FMTcache.cs:88`) |
| `FMTFormCache`, `FMTFormLogger`, `FMTexceptionhandlerwarning` | déclarées dans `UI/` |
| `FMTspatialaction` | **fantôme** : plus aucune déclaration, seulement du code commenté |
| `FMT_pickle_suite` | idiome boost.python |
| `Exception::FMTLogger` | forward-decl **morte** (mauvais namespace *et* mauvaise casse) — supprimée de `UI/Include/FmtFormCache.h` ; c était la seule collision de casse du dépôt |

## Déjà CamelCase (non touchées)

`FMTActionComparator` `FMTExcelExceptionHandler` `FMTExcelLogger` `FMTGraphInfo` `FMTGraphValues`
`FMTLineInfo` `FMTPatchRules` `FMTScopedSeTranslator` `FMTSerie` `FMTSolutionTracker`
`FMTSpatialGraphs` `FMTSpatialSchedule` `FMTVirtualLineGraph` `FMTWorkerTask`
`FMTYieldDevelopment` `FMTYieldsCache`
