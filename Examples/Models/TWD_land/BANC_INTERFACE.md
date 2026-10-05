# Banc de scénarios pour tester l'interface

Les scénarios de ce modèle dont le nom porte `_pass`, `_fail` ou `_infaisable` ne servent pas à
l'aménagement : ils servent à éprouver l'interface, module par module. Chacun est réglé pour
qu'un essai se compte en secondes -- TWD_land n'a que huit peuplements et trois thèmes.

Trois familles :

- un **`_pass`** doit se terminer normalement et produire ses sorties ;
- un **`_fail`** doit faire remonter une **erreur** dans le journal, en rouge. C'est lui qui
  prouve que le chemin d'erreur fonctionne, quel que soit le message ;
- un **`_infaisable`** se termine en **succès** : le Core n'a pas de quoi refuser, et seul le
  rapport des contraintes change. Ne pas en conclure que le journal est muet.

Un tableau de suivi plus détaillé, avec ce qui a été vérifié et ce qui reste, vit dans
`FMTWrapperCore/ETAT.md`.

## Avant de commencer

- **Fichier primaire** : `TWD_land.pri` de ce dossier.
- **Liste des erreurs ignorées** : garder celle que l'interface propose par défaut. Sans elle,
  des avertissements du modèle de base -- `_DEATH` non défini, yield redéfini -- empêchent les
  scénarios de se lire.
- **Répertoire de sortie** : choisir un dossier hors du modèle. Trois modules écrivent dans le
  projet, voir la dernière section.

## Module par module

### Chargement d'un scénario

| | |
|---|---|
| Échec | `chargement_fail` |

Son fichier `._opt` contraint `oVolumeInexistant`, qui n'est pas une sortie du modèle. Le
scénario ne se lit pas, et la pile d'erreur pointe le fichier et la ligne :
`In ...\chargement_fail\TWD_land._opt at line 8 FMTsection(8) Optimize`. Notepad++ doit
s'ouvrir dessus. C'est, avec `transformation_fail`, le meilleur essai du chemin d'erreur
complet.

### Planification

| | |
|---|---|
| Succès | `planification_pass` |
| Échec | `planification_fail` |
| Cas muet | `planification_fail_solveur` |

Horizon 5, sortie `OVOLREC`, niveau TOTALE, pilote CSV. `planification_pass` porte une cédule
de dix périodes, donc il se planifie comme il se rejoue ; il se résout à 65 733.

**Laisser « Scénarios à interpréter » vide** pour les deux scénarios d'échec. Avec le rejeu, le
modèle est construit mais jamais résolu, donc jamais déclaré irréalisable.

Les deux échecs couvrent les deux régimes d'infaisabilité de FMT :

- `planification_fail` contraint `oSuppl >= 1` en période 1. La plantation n'étant opérable
  qu'entre 1 et 3 ans alors que l'inventaire commence à 7 ans, cette sortie n'a aucune variable
  à cette période. FMT s'en aperçoit **en posant la matrice** et lève :
  `FMTexc(61) Une contrainte rend le problème d'optimisation infaisable. OSUPPL >= 1 at period 1`.
- `planification_fail_solveur` contraint `oVolRec >= 1000000`. La matrice reste cohérente et
  **seul le solveur** peut rejeter : FMT journalise `infeasible scenario
  planification_fail_solveur` puis le traitement se termine en succès. C'est un défaut connu,
  le verdict ne sortant pas de `FMTPlanningTask`.

### Replanification

| | |
|---|---|
| Succès | `replanification_strategique` + `replanification_stochastique` + `replanification_tactique` |
| Échec | `replanification_tactique_fail`, à la place du tactique |

Les noms disent où les mettre. Horizon 10, horizon de replanification 3, variabilité 0,5,
deux réplicas au minimum comme au maximum, sortie `OVOLREC`, niveau TOTALE, CSV. Les deux
solveurs conviennent.

L'échec journalise, une fois par réplicat : `FMTexc(77) ... Ignoring: infeasible model named
replanification_tactique_fail on replicate N at replanning period 1`. **C'est un avertissement,
pas une erreur rouge**, et c'est voulu côté FMT : `FMTReplanningWarning` dérive de `FMTWarning`,
donc elle n'est jamais levée. Un réplicat peut échouer pendant que les autres aboutissent.

### Rastérisation

| | |
|---|---|
| Succès | `rasterisation_pass` |
| Échec | `rasterisation_fail` |

Carte `Carte/TWD_land.shp`, champ d'âge `AGE`, champ de superficie `SUPERFICIE`.

**Vider le champ Stanlock**, que l'interface remplit d'office : aucune des deux cartes du
modèle ne porte ce champ, et FMT lève `FMTexc(28) A required field is missing from the dataset`
dès qu'un nom est fourni et introuvable. Vide, il est ignoré.

La résolution se saisit en **hectares par pixel**, pas en mètres : `0.04` devient
« Résolution FMT : 20 ». Saisir `201.64` pour retrouver les 1420 mètres des tests automatisés.

`rasterisation_fail` nomme `UNITE4` un attribut que la carte appelle `UNITE3`, d'où
`FMTexc(19) A referenced attribute is missing or undefined. UNITE3 at theme 1 at line 5`.

### Transformation

| | |
|---|---|
| Succès | `transformation_pass` |
| Échec | `transformation_fail` |

Traitement « Agrégat ». Dans « Agrégats et actions », cocher **les trois agrégats et eux
seuls** :

| Agrégat | Actions couvertes |
|---|---|
| `ARECOLTE` | COUPETOTALE, ARECUP |
| `ASYLVICULTURE` | PLANTATION |
| `APERTURBATION` | AFIRE, ACARIBOU |

La liste mêle les agrégats et les actions, et la sélection doit **partitionner** les actions du
modèle : chacune exactement une fois, nommée directement ou par un agrégat.

- Trop peu : `Missing aggregate for actions AFIRE,ARECUP,COUPETOTALE,ACARIBOU,PLANTATION`. Le
  message énumère toutes les actions, pas seulement les manquantes.
- Trop : `Action AFIRE already in aggregate`, un nom étant reconnu aussi bien comme action que
  comme agrégat.
- Une partition mixte fonctionne, par exemple `ARECOLTE`, `AFIRE`, `ACARIBOU`, `PLANTATION`.
- `_DEATH` est sans effet, l'agrégation l'ajoutant d'elle-même.
- **Ne pas cocher que des actions** : les cinq actions seules font mourir le traitement sans
  message.

La priorisation des actions n'a aucun effet : le paramètre est reçu puis ignoré, l'ordre venant
des cédules.

`transformation_fail` porte une cédule qui nomme une action `COUPEPARTIELLE` absente du modèle.
La pile pointe `transformation_fail\TWD_land._seq` à la ligne 6, et Notepad++ doit s'ouvrir
dessus.

### Simulation spatialement explicite

| | |
|---|---|
| Succès | `sse_pass` |
| Échec | `spatial_fail_rasters` |
| Cas infaisable | `sse_infaisable` |

Dossier de rasters `rasters/`, 5 périodes, recherche gloutonne 5, sortie `OVOLREC`, niveau
TOTALE, périodes 1 à 5, CSV, mode carbone décoché.

`spatial_fail_rasters` nomme `UNITE4` un attribut que les rasters appellent `UNITE3` : la
lecture des rasters lève `FMTexc(19)`, dans `FMTsection(3) Area`. Sa cédule suit le modèle,
pour que l'échec tombe sur les rasters et non sur sa lecture.

`sse_infaisable` contraint `oVolRec >= 1000000`. Le traitement **réussit** ; ce qui change est
le rapport des contraintes, à lire dans le journal : `OVOLREC >= 1000000.000000 1.._LENGTH(...)`
et `Percentage of infeasible constraints 20 %`, contre `0 %` pour `sse_pass`.

### Optimisation spatialement explicite

| | |
|---|---|
| Succès | `ose_pass` |
| Échec | `spatial_fail_rasters`, le même scénario |
| Cas infaisable | `ose_infaisable` |

Mêmes rasters, 5 ou 10 périodes, et les valeurs de recuit de `sasolve` : 500 000 mouvements,
3 000 acceptés, 5 000 par cycle.

**Aucun réglage ne fera « échouer » `ose_infaisable`** : l'optimisation n'a pas de sortie de
refus, elle lève ou elle réussit. Son rapport affiche `Percentage of infeasible constraints
50 %` et nomme la contrainte brisée, contre `0 %` pour `ose_pass`, qui récolte réellement.

### Calendrier de COS

| | |
|---|---|
| Succès | `cos_pass` |
| Échec | `cos_fail` |

Carte `Carte/TWD_land.shp`, champs `AGE` et `SUPERFICIE`, champ Stanlock vide, fichier de
paramètres `Scenarios/cos_pass/parametres_cos.csv`. Son en-tête porte les huit colonnes,
`OA;OPT;RET;MAXRET;REP;OPR;NPE;GUP` : le Core n'exige que les cinq premières, mais l'interface
refuse le fichier sans `NPE` ni `GUP`. Les deux valent zéro exprès -- au-dessus, FMT va lire les
blocs voisins dans la carte, ce que ce modèle n'a jamais exercé.

Le numéro de thème découpe les aires d'opération sur le thème 3, celui des UTR. Le Core le passe
tel quel à une fonction qui compte à partir de zéro : si les aires ne sont pas reconnues, essayer
2 plutôt que 3.

`cos_fail` définit le yield `youvert` mais aucune de ses actions ne le lit, d'où le refus :
« Aucune action dans le modèle n'a de yield youvert ». La fonction rend faux, sans exception.

## Ce que les essais écrivent dans le modèle

Trois modules modifient le projet. Après une séance d'essais, `git status` sur ce dossier dit ce
qui a bougé, et `git checkout -- .` le remet en état.

- **La planification** réécrit la cédule des scénarios qu'elle optimise, `planification_pass`
  compris : un essai à l'horizon 1 ramène sa cédule de dix périodes à une seule, et le banc perd
  son cas de rejeu.
- **La transformation** ajoute un dossier de scénario et réécrit le `.pri`.
- **La rastérisation** écrit ses rasters dans le dossier qu'on lui donne. Le répertoire proposé
  par défaut est `rasters/`, celui-là même où la simulation et l'optimisation spatiales vont
  lire.

## Inventaire

| Scénario | Rôle |
|---|---|
| `chargement_fail` | Contrainte sur une sortie inexistante : le scénario ne se lit pas |
| `cos_pass` | Calendrier de COS : une action lit le yield `youvert` |
| `cos_fail` | Le yield `youvert` existe, aucune action ne le lit |
| `ose_pass` | Optimisation spatiale : objectif `_MAX`, contrainte d'adjacence, récolte réelle |
| `ose_infaisable` | Le même, plus une contrainte que la forêt ne peut pas atteindre |
| `planification_pass` | Planification : cédule de dix périodes, se résout à 65 733 |
| `planification_fail` | Contrainte impossible dès la pose de la matrice |
| `planification_fail_solveur` | Contrainte que seul le solveur rejette : aucune erreur ne remonte |
| `rasterisation_pass` | Thèmes du modèle conformes à la carte |
| `rasterisation_fail` | Un attribut du modèle absent de la carte |
| `replanification_strategique` | Modèle stratégique du trio |
| `replanification_stochastique` | Modèle stochastique du trio, avec ses feux |
| `replanification_tactique` | Modèle tactique du trio |
| `replanification_tactique_fail` | Le tactique, rendu insoluble |
| `spatial_fail_rasters` | Un attribut du modèle absent des rasters, pour les deux modules spatiaux |
| `sse_pass` | Simulation spatiale : rejoue une cédule, rapport à 0 % |
| `sse_infaisable` | Le même, plus une contrainte que la forêt ne peut pas atteindre |
| `transformation_pass` | Trois agrégats d'actions couvrant les cinq actions |
| `transformation_fail` | Cédule nommant une action absente du modèle |
