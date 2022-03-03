---
title: Prérequis
linktitle: Prérequis
type: book
date: "2019-05-05T00:00:00+01:00"
# Prev/next pager order (if `docs_section_pager` enabled in `params.toml`)
weight: 2
---

1. Télécharger et installer [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/)

{{% callout warning %}}
Lors de l'installation de Visual Studio Community, n'oubliez pas d'indiquer pendant l'installation que vous voulez que l'outil de compilation `Cmake` soit installé.
{{% /callout %}}
{{% callout note %}}
Visual Studio Community est la version gratuite du logiciel Visual Studio de Microsoft. Nous l'utiliserons plus tard pour compiler FMT, car il est installé avec un compilateur compatible avec Windows.
{{% /callout %}}

2. Téléchargez et installez [Mosek](https://www.mosek.com/)

{{% callout note %}}
Mosek est un solveur pour les problèmes de programmation linéaire, qui est exactement le type de problèmes que traite FMT. Cependant, FMT peut fonctionner avec différents autres solveurs, libres ou non.
{{% /callout %}}

3. Créez un dossier `FMT-Installation` dans votre ordinateur. **Assurez-vous qu'il n'y a pas d'espaces ou d'accents dans le chemin menant au dossier.**

{{% callout note %}}
Les espaces et les accents dans le chemin peuvent causer des erreurs avec différents programmes et différentes étapes de la compilation. C'est embettant, mais c'est comme ça !
{{% /callout %}}

4. Ouvrez une invite de commande, et utilisez la commande `cd` pour naviguer dans le dossier `FMT-Installation`.

{{% callout note %}}
Pour ouvrir l'invite de commande sous Windows 10, appuyez simplement sur la touche {{< icon name="windows" pack="fab" >}}, tapez `cmd`, puis appuyez sur `Entrée`.

La commande `cd` est utilisée pour changer le dossier à l'intérieur duquel l'invite de commande est prête à agir.

Tapez `cd ..` pour aller dans le dossier parent de celui dans lequel vous êtes, et `cd dossier` pour aller dans un dossier.
{{% /callout %}}