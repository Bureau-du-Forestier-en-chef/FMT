# Inventaire des méthodes FMTlib à convertir en camelCase

> Généré automatiquement à partir de **deux sources** : (1) les marqueurs `// DocString: Classe::methode` dans `Include/FMT*.h/.hpp` (méthodes documentées, y compris signatures multi-lignes), et (2) un **scan direct des déclarations** qui récupère les méthodes **non documentées** (helpers privés `_xxx`, getters oubliés). Croisement avec `PYexport*.h`/`Rexport*.h` pour les méthodes exposées. **À réviser avant application.** La passe de renommage finale se fera classe par classe en relisant chaque en-tête.

## Règles de renommage appliquées

- Cible **lowerCamelCase** (1re lettre minuscule). Mots collés → majuscule à chaque mot suivant (`getarea`→`getArea`, `setparameter`→`setParameter`).
- snake_case FMT → camelCase (`push_aggregate`→`pushAggregate`).
- PascalCase FMT → 1re lettre en minuscule (`BuildAction`→`buildAction`, `_GetCount`→`_getCount`).
- Acronymes en Pascal, **sauf `FMT` et `GCBM` qui restent en MAJUSCULES** — et on **rétablit** les `Gcbm` déjà introduits (`getGcbmActionId`→`getGCBMActionId`).
- Méthodes d'un seul mot minuscule (`grow`, `hash`, `area`…) : **inchangées**.
- Méthodes std-mimétiques (`push_back`, `pop_back`, `insert`, `begin`, `end`…) : **inchangées**.

## Décisions actées

- **`Clone` → `clone`** (on convertit).
- **`Serialize` : gardé tel quel** (risque d'interférer avec l'API boost de sérialisation).
- **Acronymes `GDAL`/`MSK`** → camelCase (`gdalInitialization`, `_mskOptimize…`) ; seuls `FMT` et `GCBM` restent en majuscules.

## Colonne « Statut »

- **à convertir** : renommage FMT standard.
- **GCBM** : ré-uniformisation d'un `Gcbm` déjà introduit vers `GCBM`.
- **à confirmer** : découpage ambigu (typo, acronyme, mot simple PascalCase) — décision requise.

- La note **« exposé PY/R »** signale qu'il faut aussi mettre à jour le pointeur `&Classe::methode` dans `Include/PYexport*.h`/`Rexport*.h`, **en gardant la chaîne exposée gelée**.

- Note **« helper privé non documenté (scan direct) »** : méthode privée `_xxx` sans marqueur DocString, détectée par le scan direct — fiable.
- Note **« non documenté — vérifié en source (méthode réelle) »** : méthode publique sans DocString, détectée par le scan direct **et confirmée manuellement** dans l'en-tête.
- Les faux positifs du scan direct ont été écartés : types imbriqués (struct/enum) confondus avec leur constructeur (`BoundingBox`, `ParseState`, `EdgePeriodicFilter`, `ConstraintIndex`…) et variables locales (`tofind`, `targetedperiods` dans `FMTgraph.hpp`).

## Résumé

- Classes concernées : **55**
- À convertir : **211** · GCBM : **3** · À confirmer : **0**


---

### `FMTaction`  — `Include/FMTaction.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `push_aggregate` | `pushAggregate` | à convertir | exposé PY/R ("push_aggregate") → sync pointeur, chaîne gelée |
| `getGcbmActionDef` | `getGCBMActionDef` | GCBM | uniformiser GCBM |
| `getGcbmActionId` | `getGCBMActionId` | GCBM | uniformiser GCBM |
| `getGcbmActionName` | `getGCBMActionName` | GCBM | uniformiser GCBM |

### `FMTactionparser`  — `Include/FMTactionparser.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `_GetSerie` | `_getSerie` | à convertir |  |

### `FMTactualdevelopment`  — `Include/FMTactualdevelopment.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `getarea` | `getArea` | à convertir | exposé PY/R ("getarea") → sync pointeur, chaîne gelée |

### `FMTareaparser`  — `Include/FMTareaparser.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `_IsMapWithSameThemes` | `_isMapWithSameThemes` | à convertir | helper privé non documenté (scan direct) |
| `WriteForestExtended` | `writeForestExtended` | à convertir | non documenté — vérifié en source (méthode réelle) |

### `FMTbasevertexproperties`  — `Include/FMTbasevertexproperties.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `_Save` | `_save` | à convertir | helper privé non documenté (scan direct) |

### `FMTconstantparser`  — `Include/FMTconstantparser.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `_FillConstants` | `_fillConstants` | à convertir |  |

### `FMTdebugexceptionhandler`  — `Include/FMTdebugexceptionhandler.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `Clone` | `clone` | à convertir |  |

### `FMTdebuglogger`  — `Include/FMTdebuglogger.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `Clone` | `clone` | à convertir |  |

### `FMTdefaultexceptionhandler`  — `Include/FMTdefaultexceptionhandler.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `Clone` | `clone` | à convertir |  |

### `FMTdefaultlogger`  — `Include/FMTdefaultlogger.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `Clone` | `clone` | à convertir |  |

### `FMTdevelopment`  — `Include/FMTdevelopment.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `Clone` | `clone` | à convertir |  |
| `getarea` | `getArea` | à convertir | exposé PY/R ("getarea") → sync pointeur, chaîne gelée |
| `getmask` | `getMask` | à convertir | exposé PY/R ("getmask") → sync pointeur, chaîne gelée |

### `FMTevent`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `GetOutsideBorders` | `getOutsideBorders` | à convertir | helper interne (PascalCase) |

### `FMTeventcontainer`  — `Include/FMTeventcontainer.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `_GetBoundingBoxes` | `_getBoundingBoxes` | à convertir | helper privé non documenté (scan direct) |
| `_GetSize` | `_getSize` | à convertir | helper privé non documenté (scan direct) |
| `_IsNull` | `_isNull` | à convertir | helper privé non documenté (scan direct) |

### `FMTexceptionhandler`  — `Include/FMTexceptionhandler.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `Clone` | `clone` | à convertir |  |
| `needtorethrow` | `needToRethrow` | à convertir |  |
| `passinlogger` | `passInLogger` | à convertir |  |
| `printexceptions` | `printExceptions` | à convertir |  |
| `raisefromcatch` | `raiseFromCatch` | à convertir |  |
| `throw_nested` | `throwNested` | à convertir |  |
| `updatestatus` | `updateStatus` | à convertir |  |

### `FMTforest`  — `Include/FMTforest.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `getthemes` | `getThemes` | à convertir | non documenté — vérifié en source (méthode réelle) |

### `FMTfork`  — `Include/FMTfork.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `_GetPath` | `_getPath` | à convertir | helper privé non documenté (scan direct) |

### `FMTfreeexceptionhandler`  — `Include/FMTfreeexceptionhandler.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `Clone` | `clone` | à convertir |  |

### `FMTfuturdevelopment`  — `Include/FMTfuturdevelopment.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `getarea` | `getArea` | à convertir | exposé PY/R ("getarea") → sync pointeur, chaîne gelée |

### `FMTgraph`  — `Include/FMTgraph.hpp`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `_GenerateDevelopments` | `_generateDevelopments` | à convertir | helper privé non documenté (scan direct) |
| `getstatsptr` | `getStatsPtr` | à convertir | non documenté — vérifié en source (méthode réelle) |

### `FMTgraphvertextoyield`  — `Include/FMTgraphvertextoyield.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `getmodel` | `getModel` | à convertir | non documenté — vérifié en source (méthode réelle) |

### `FMTlogger`  — `Include/FMTlogger.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `Clone` | `clone` | à convertir |  |
| `getlogstamp` | `getLogStamp` | à convertir |  |
| `logstamp` | `logStamp` | à convertir |  |
| `logtime` | `logTime` | à convertir |  |
| `logwithlevel` | `logWithLevel` | à convertir |  |
| `redirectofile` | `redirectToFile` | à convertir |  |
| `setlogginglevel` | `setLoggingLevel` | à convertir |  |
| `setstreamflush` | `setStreamFlush` | à convertir |  |

### `FMTlpmodel`  — `Include/FMTlpmodel.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `getmodelfromproportions` | `getModelFromProportions` | à convertir |  |
| `setstrictlypositivesoutputsmatrix` | `setStrictlyPositivesOutputsMatrix` | à convertir | exposé PY/R ("setstrictlypositivesoutputsmatrix") → sync pointeur, chaîne gelée |

### `FMTlpsolver`  — `Include/FMTlpsolver.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `_MSKOptimizeWithDefaultParameters` | `_mskOptimizeWithDefaultParameters` | à convertir |  |
| `_MSKOptimizeWithParameters` | `_mskOptimizeWithParameters` | à convertir |  |
| `passinlogger` | `passInLogger` | à convertir |  |

### `FMTmodel`  — `Include/FMTmodel.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `_GetAggregatesThemes` | `_getAggregatesThemes` | à convertir |  |
| `_GetAggregatesWrap` | `_getAggregatesWrap` | à convertir |  |
| `_GetYieldAttribute` | `_getYieldAttribute` | à convertir |  |
| `_GetYieldsStraticAggregates` | `_getYieldsStraticAggregates` | à convertir | helper interne (PascalCase) |
| `BuildAction` | `buildAction` | à convertir | helper interne (PascalCase) |
| `getoutputs` | `getOutputs` | à convertir | exposé PY/R ("getoutputs") → sync pointeur, chaîne gelée |
| `getparameter` | `getParameter` | à convertir |  |
| `getyields` | `getYields` | à convertir | exposé PY/R ("getyields") → sync pointeur, chaîne gelée |
| `setactions` | `setActions` | à convertir | exposé PY/R ("setactions") → sync pointeur, chaîne gelée |
| `setconstraints` | `setConstraints` | à convertir | exposé PY/R ("setconstraints") → sync pointeur, chaîne gelée |
| `setparameter` | `setParameter` | à convertir |  |

### `FMTmodelparser`  — `Include/FMTmodelparser.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `writeresults` | `writeResults` | à convertir | exposé PY/R ("writeresults") → sync pointeur, chaîne gelée |
| `writetoproject` | `writeToProject` | à convertir |  |

### `FMTnssmodel`  — `Include/FMTnssmodel.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `setparameter` | `setParameter` | à convertir |  |

### `FMTobject`  — `Include/FMTobject.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `GetExceptionHandler` | `getExceptionHandler` | à convertir |  |
| `getruntimelocation` | `getRuntimeLocation` | à convertir |  |
| `passinexceptionhandler` | `passInExceptionHandler` | à convertir |  |
| `passinlogger` | `passInLogger` | à convertir |  |
| `seterrorstowarnings` | `setErrorsToWarnings` | à convertir | exposé PY/R ("seterrorstowarnings") → sync pointeur, chaîne gelée |
| `setmaxwarningsbeforesilenced` | `setMaxWarningsBeforeSilenced` | à convertir | exposé PY/R ("setmaxwarningsbeforesilenced") → sync pointeur, chaîne gelée |

### `FMTopareaschedulertaskk`  — `Include/FMTopareaschedulertask.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `passinlogger` | `passInLogger` | à convertir |  |

### `FMToperatingarea`  — `Include/FMToperatingarea.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `getarea` | `getArea` | à convertir | exposé PY/R ("getarea") → sync pointeur, chaîne gelée |
| `getmask` | `getMask` | à convertir | exposé PY/R ("getmask") → sync pointeur, chaîne gelée |

### `FMToperatingareascheme`  — `Include/FMToperatingareascheme.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `setconstraints` | `setConstraints` | à convertir | exposé PY/R ("setconstraints") → sync pointeur, chaîne gelée |

### `FMTparser`  — `Include/FMTparser.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `_ProcessConstants` | `_processConstants` | à convertir | helper privé non documenté (scan direct) |
| `_QueryDatabase` | `_queryDatabase` | à convertir | helper interne (PascalCase) |
| `_SetForLoopLines` | `_setForLoopLines` | à convertir |  |
| `GDALInitialization` | `gdalInitialization` | à convertir | helper interne (PascalCase) |

### `FMTPatchRules`  — `Include/FMTPatchRules.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `_BuildPatchRules` | `_buildPatchRules` | à convertir | helper privé non documenté (scan direct) |
| `_EvaluateAdjacency` | `_evaluateAdjacency` | à convertir | helper privé non documenté (scan direct) |
| `_EvaluateGroup` | `_evaluateGroup` | à convertir | helper privé non documenté (scan direct) |
| `_EvaluateSize` | `_evaluateSize` | à convertir | helper privé non documenté (scan direct) |
| `_GetBounds` | `_getBounds` | à convertir | helper privé non documenté (scan direct) |
| `_GetOrderedRules` | `_getOrderedRules` | à convertir | helper privé non documenté (scan direct) |
| `_HasMaximalAdjacency` | `_hasMaximalAdjacency` | à convertir | helper privé non documenté (scan direct) |
| `_HasMinimalAdjacency` | `_hasMinimalAdjacency` | à convertir | helper privé non documenté (scan direct) |
| `_IsAdjacencyUsed` | `_isAdjacencyUsed` | à convertir | helper privé non documenté (scan direct) |
| `_IsGroupUsed` | `_isGroupUsed` | à convertir | helper privé non documenté (scan direct) |
| `_IsSizeUsed` | `_isSizeUsed` | à convertir | helper privé non documenté (scan direct) |
| `_IsTooClose` | `_isTooClose` | à convertir | helper privé non documenté (scan direct) |
| `_TooBig` | `_tooBig` | à convertir | helper privé non documenté (scan direct) |
| `_TooSmall` | `_tooSmall` | à convertir | helper privé non documenté (scan direct) |

### `FMTplanningtask`  — `Include/FMTplanningtask.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `passinlogger` | `passInLogger` | à convertir |  |

### `FMTquietexceptionhandler`  — `Include/FMTquietexceptionhandler.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `Clone` | `clone` | à convertir |  |

### `FMTquietlogger`  — `Include/FMTquietlogger.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `Clone` | `clone` | à convertir |  |

### `FMTreplanningtask`  — `Include/FMTreplanningtask.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `passinlogger` | `passInLogger` | à convertir |  |
| `setreplicates` | `setReplicates` | à convertir |  |
| `writeresults` | `writeResults` | à convertir | exposé PY/R ("writeresults") → sync pointeur, chaîne gelée |

### `FMTsamodel`  — `Include/FMTsamodel.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `_AllowAdjacencyDestruction` | `_allowAdjacencyDestruction` | à convertir |  |
| `_AllowAnyMove` | `_allowAnyMove` | à convertir | helper privé non documenté (scan direct) |
| `_AllowAreaDestruction` | `_allowAreaDestruction` | à convertir | helper privé non documenté (scan direct) |
| `_AllowEventsSpread` | `_allowEventsSpread` | à convertir |  |
| `_AllowGroupDestruction` | `_allowGroupDestruction` | à convertir |  |
| `_AllowMove` | `_allowMove` | à convertir | helper privé non documenté (scan direct) |
| `_CoolDown` | `_coolDown` | à convertir | helper privé non documenté (scan direct) |
| `_DoConflictDestruction` | `_doConflictDestruction` | à convertir | helper privé non documenté (scan direct) |
| `_DoEventsAdjacencyConflictDestrutorMove` | `_doEventsAdjacencyConflictDestrutorMove` | à convertir | helper privé non documenté (scan direct) |
| `_DoEventsAreaConflictDestrutorMove` | `_doEventsAreaConflictDestrutorMove` | à convertir | helper privé non documenté (scan direct) |
| `_DoEventsSpread` | `_doEventsSpread` | à convertir |  |
| `_DoFactorization` | `_doFactorization` | à convertir | helper privé non documenté (scan direct) |
| `_DoGroupsConflictDestrutorMove` | `_doGroupsConflictDestrutorMove` | à convertir | helper privé non documenté (scan direct) |
| `_DoLocalMove` | `_doLocalMove` | à convertir | helper privé non documenté (scan direct) |
| `_DoWriteDisturbances` | `_doWriteDisturbances` | à convertir |  |
| `_GetAcceptedCycleMoves` | `_getAcceptedCycleMoves` | à convertir | helper privé non documenté (scan direct) |
| `_GetAMove` | `_getAMove` | à convertir | helper privé non documenté (scan direct) |
| `_GetConstraintsStats` | `_getConstraintsStats` | à convertir | helper privé non documenté (scan direct) |
| `_GetCycleMoves` | `_getCycleMoves` | à convertir | helper privé non documenté (scan direct) |
| `_GetFromBindings` | `_getFromBindings` | à convertir | helper privé non documenté (scan direct) |
| `_GetLocalMoveSize` | `_getLocalMoveSize` | à convertir | helper privé non documenté (scan direct) |
| `_GetMaximalMoveSize` | `_getMaximalMoveSize` | à convertir | helper privé non documenté (scan direct) |
| `_GetNonTabouMoves` | `_getNonTabouMoves` | à convertir | helper privé non documenté (scan direct) |
| `_GetRandomLpModel` | `_getRandomLpModel` | à convertir | helper privé non documenté (scan direct) |
| `_GetRandomMoveSize` | `_getRandomMoveSize` | à convertir | helper privé non documenté (scan direct) |
| `_GetRebuild` | `_getRebuild` | à convertir | helper privé non documenté (scan direct) |
| `_InitialGrow` | `_initialGrow` | à convertir | helper privé non documenté (scan direct) |
| `_IsBetter` | `_isBetter` | à convertir | helper privé non documenté (scan direct) |
| `_LogCycleStatus` | `_logCycleStatus` | à convertir | helper privé non documenté (scan direct) |
| `_LogSolutionStatus` | `_logSolutionStatus` | à convertir | helper privé non documenté (scan direct) |
| `_Move` | `_move` | à convertir | helper privé non documenté (scan direct) |
| `_RandomBuild` | `_randomBuild` | à convertir | helper privé non documenté (scan direct) |
| `_ResetTabouMoves` | `_resetTabouMoves` | à convertir | helper privé non documenté (scan direct) |
| `_SchedulesBuild` | `_schedulesBuild` | à convertir | helper privé non documenté (scan direct) |
| `_SetBestSolutionTo` | `_setBestSolutionTo` | à convertir | helper privé non documenté (scan direct) |
| `_Warmup` | `_warmup` | à convertir | helper privé non documenté (scan direct) |
| `_WriteDisrturbances` | `_writeDisrturbances` | à convertir |  |
| `AllowAnyMove` | `allowAnyMove` | à convertir | helper interne (PascalCase) |
| `AllowDestruction` | `allowDestruction` | à convertir | helper interne (PascalCase) |
| `AllowMove` | `allowMove` | à convertir | helper interne (PascalCase) |
| `CoolDown` | `coolDown` | à convertir | helper interne (PascalCase) |
| `CoolingSchedule` | `coolingSchedule` | à convertir | helper interne (PascalCase) |
| `CycleMoves` | `cycleMoves` | à convertir | helper interne (PascalCase) |
| `DoConflictDestruction` | `doConflictDestruction` | à convertir | helper interne (PascalCase) |
| `DoEventsAdjacencyConflictDestrutorMove` | `doEventsAdjacencyConflictDestrutorMove` | à convertir | helper interne (PascalCase) |
| `DoEventsAreaConflictDestrutorMove` | `doEventsAreaConflictDestrutorMove` | à convertir | helper interne (PascalCase) |
| `Dofactorization` | `dofactorization` | à convertir | helper interne (PascalCase) |
| `DoGroupsConflictDestrutorMove` | `doGroupsConflictDestrutorMove` | à convertir | helper interne (PascalCase) |
| `DoLocalMove` | `doLocalMove` | à convertir | helper interne (PascalCase) |
| `GetAcceptedCycleMoves` | `getAcceptedCycleMoves` | à convertir | helper interne (PascalCase) |
| `GetAMove` | `getAMove` | à convertir | helper interne (PascalCase) |
| `GetCycleMoves` | `getCycleMoves` | à convertir | helper interne (PascalCase) |
| `GetFromBindings` | `getFromBindings` | à convertir | helper interne (PascalCase) |
| `GetRebuild` | `getRebuild` | à convertir | helper interne (PascalCase) |
| `LogSolutionStatus` | `logSolutionStatus` | à convertir | helper interne (PascalCase) |
| `LogTemperatureStatus` | `logTemperatureStatus` | à convertir | helper interne (PascalCase) |
| `NotAcceptedMovesCount` | `notAcceptedMovesCount` | à convertir | helper interne (PascalCase) |
| `TotalMoves` | `totalMoves` | à convertir | helper interne (PascalCase) |
| `UpdateFailedMoveCount` | `updateFailedMoveCount` | à convertir | helper interne (PascalCase) |

### `FMTsemodel`  — `Include/FMTsemodel.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `_BuildArea` | `_buildArea` | à convertir | helper privé non documenté (scan direct) |
| `_BuildGraphs` | `_buildGraphs` | à convertir | helper privé non documenté (scan direct) |
| `_BuildSolution` | `_buildSolution` | à convertir | helper privé non documenté (scan direct) |
| `_CopyGraphs` | `_copyGraphs` | à convertir | helper privé non documenté (scan direct) |
| `_CopySolution` | `_copySolution` | à convertir | helper privé non documenté (scan direct) |
| `_GetConstraintNumerator` | `_getConstraintNumerator` | à convertir | helper privé non documenté (scan direct) |

### `FMTSolutionTracker`  — `Include/FMTSolutionTracker.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `_BuildConstraintsFamilies` | `_buildConstraintsFamilies` | à convertir | helper privé non documenté (scan direct) |
| `_ModifySolution` | `_modifySolution` | à convertir | helper privé non documenté (scan direct) |

### `FMTSpatialGraphs`  — `Include/FMTSpatialGraphs.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `_BuildConstraintsLocator` | `_buildConstraintsLocator` | à convertir | helper privé non documenté (scan direct) |
| `_BuildConstraintsValues` | `_buildConstraintsValues` | à convertir | helper privé non documenté (scan direct) |
| `_BuildTacker` | `_buildTacker` | à convertir | helper privé non documenté (scan direct) |
| `_FillConstraintResults` | `_fillConstraintResults` | à convertir | helper privé non documenté (scan direct) |
| `_FillValuesFromLastPeriod` | `_fillValuesFromLastPeriod` | à convertir | helper privé non documenté (scan direct) |
| `_GetConstraintsValueFromSolution` | `_getConstraintsValueFromSolution` | à convertir | helper privé non documenté (scan direct) |
| `_GetConstraintsValueFromTracker` | `_getConstraintsValueFromTracker` | à convertir | helper privé non documenté (scan direct) |
| `_GetConstraintsValues` | `_getConstraintsValues` | à convertir | helper privé non documenté (scan direct) |
| `_GetFamily` | `_getFamily` | à convertir | helper privé non documenté (scan direct) |
| `_GetGraphCells` | `_getGraphCells` | à convertir | helper privé non documenté (scan direct) |
| `_GetMaxGraphLength` | `_getMaxGraphLength` | à convertir | helper privé non documenté (scan direct) |
| `_GetMinGraphLength` | `_getMinGraphLength` | à convertir | helper privé non documenté (scan direct) |
| `_GetNaturalGrowthIterator` | `_getNaturalGrowthIterator` | à convertir | helper privé non documenté (scan direct) |
| `_GetNonSpatialId` | `_getNonSpatialId` | à convertir | helper privé non documenté (scan direct) |
| `_GetUseFullBits` | `_getUseFullBits` | à convertir | helper privé non documenté (scan direct) |
| `_RemoveGraphsShorterThan` | `_removeGraphsShorterThan` | à convertir | helper privé non documenté (scan direct) |

### `FMTSpatialSchedule`  — `Include/FMTSpatialSchedule.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `_EvaluateSpatialAdjacency` | `_evaluateSpatialAdjacency` | à convertir |  |
| `_EvaluateSpatialGroups` | `_evaluateSpatialGroups` | à convertir | helper privé non documenté (scan direct) |
| `_GetAdjacencyConflict` | `_getAdjacencyConflict` | à convertir | helper privé non documenté (scan direct) |
| `_GetConstraintFloorValue` | `_getConstraintFloorValue` | à convertir | helper privé non documenté (scan direct) |
| `_GetExponentialFactorization` | `_getExponentialFactorization` | à convertir | helper privé non documenté (scan direct) |
| `_GetNonSpatialCellsCount` | `_getNonSpatialCellsCount` | à convertir | helper privé non documenté (scan direct) |
| `_GetRulesEvaluation` | `_getRulesEvaluation` | à convertir | helper privé non documenté (scan direct) |
| `_GetSpatialGroupsConflict` | `_getSpatialGroupsConflict` | à convertir | helper privé non documenté (scan direct) |
| `_PutTabouInCache` | `_putTabouInCache` | à convertir | helper privé non documenté (scan direct) |
| `_SetTabouOutOfCache` | `_setTabouOutOfCache` | à convertir | helper privé non documenté (scan direct) |

### `FMTsrmodel`  — `Include/FMTsrmodel.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `buildperiod` | `buildPeriod` | à convertir | exposé PY/R ("buildperiod") → sync pointeur, chaîne gelée |
| `getavailablesolverinterface` | `getAvailableSolverInterface` | à convertir | exposé PY/R ("getavailablesolverinterface") → sync pointeur, chaîne gelée |
| `passinlogger` | `passInLogger` | à convertir |  |
| `setparameter` | `setParameter` | à convertir |  |

### `FMTtaskhandler`  — `Include/FMTtaskhandler.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `conccurentrun` | `conccurentRun` | à convertir | exposé PY/R ("conccurentrun") → sync pointeur, chaîne gelée |
| `ondemandrun` | `onDemandRun` | à convertir | exposé PY/R ("ondemandrun") → sync pointeur, chaîne gelée |
| `passinlogger` | `passInLogger` | à convertir |  |

### `FMTtasklogger`  — `Include/FMTtasklogger.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `Clone` | `clone` | à convertir |  |

### `FMTtheme`  — `Include/FMTtheme.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `_FindFirst` | `_findFirst` | à convertir |  |
| `_FindFirstFlip` | `_findFirstFlip` | à convertir |  |
| `_GetCount` | `_getCount` | à convertir |  |
| `_GetFlipCount` | `_getFlipCount` | à convertir |  |
| `_IsEqual` | `_isEqual` | à convertir |  |
| `_IsFlipEqual` | `_isFlipEqual` | à convertir |  |
| `getbaseattributes` | `getBaseAttributes` | à convertir |  |
| `push_aggregate` | `pushAggregate` | à convertir | exposé PY/R ("push_aggregate") → sync pointeur, chaîne gelée |

### `FMTyieldmodel`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `Clone` | `clone` | à convertir | helper interne (PascalCase) |

### `FMTyieldmodeldecisiontree`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `Clone` | `clone` | à convertir | helper interne (PascalCase) |

### `FMTyieldmodelnep`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `Clone` | `clone` | à convertir | helper interne (PascalCase) |

### `FMTyieldmodelpools`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `Clone` | `clone` | à convertir | helper interne (PascalCase) |

### `FMTyieldmodelRandom`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `Clone` | `clone` | à convertir | helper interne (PascalCase) |

### `FMTyieldmodelTSLA`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `Clone` | `clone` | à convertir | helper interne (PascalCase) |

### `FMTyieldmodelUnitCoverage`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `Clone` | `clone` | à convertir | helper interne (PascalCase) |

### `FMTyields`  — `Include/FMTyields.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `getallyieldnames` | `getAllYieldNames` | à convertir | exposé PY/R ("getallyieldnames") → sync pointeur, chaîne gelée |

### `FMTYieldsCache`  — `Include/FMTYieldsCache.h`

| Méthode actuelle | Renommage proposé | Statut | Note |
|---|---|---|---|
| `_ClearIfTooBig` | `_clearIfTooBig` | à convertir | helper privé non documenté (scan direct) |


---
## Exceptions à NE PAS renommer (rappel)

- **Chaînes exposées Python/R** : 1er argument de `.def("…")` / `.method("…")` dans `Include/PYexport*.h` / `Include/Rexport*.h` — API publique, jamais renommée.
- **API du wrapper** : méthodes propres à `FMTWrapperCore/` et `UI/` (hors FMTlib).
- **Méthodes non-FMT** (héritées/mimant des libs tierces) : `FMTlpsolver` (OSI/Coin : `isProvenOptimal`, `getObjValue`, `writeLP`, `writeMPS`…), `FMTsolverlogger` (CoinMessageHandler), `FMTexception`/`FMTseException` (std), `FMT_pickle_suite` (boost), `FMTlist::push_back` (STL).


## Points en suspens repérés

- `Source/FMTsrmodel.cpp` : 2 appels `getname()` non convertis (~L1589, ~L1604).
- `Source/planningtest.cpp` : encore en ancien style, non touché sur la branche.
