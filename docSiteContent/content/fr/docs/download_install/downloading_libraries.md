---
title: Télécharger les librairies
linktitle: Télécharger les librairies
type: book
date: "2019-05-05T00:00:00+01:00"
# Prev/next pager order (if `docs_section_pager` enabled in `params.toml`)
weight: 3
---

1. Si git n'est pas installé sur votre ordinateur, installez-le depuis [ce lien](https://git-scm.com/).

{{% callout note %}}
Git est un logiciel libre et gratuit utilisé pour aider à organiser la façon dont les gens créent le code source de programmes ou d'outils, et comment ils collaborent pour le faire.

Ici, git vous aidera à télécharger le code utilisé pour faire fonctionner le logiciel `vcpkg`, qui téléchargera toutes les librairies dont FMT a besoin pour le compiler.
{{% /callout %}}

2. Utilisez `git clone https://github.com/microsoft/vcpkg.git` dans l'invite de commande pour télécharger [vcpkg](https://github.com/microsoft/vcpkg). 

{{% callout note %}}
Si vous avez fermé l'invite de commande, ou si vous ne vous souvenez pas comment l'ouvrir, consultez [la page précédente](/docs/download_install/prerequisits).
{{% /callout %}}

3. Utilisez `cd ./vcpkg` dans l'invite de commande pour naviguer dans le dossier `vcpkg`.

{{% callout note %}}
Si vous ne vous souvenez pas comment utiliser `cd`, consultez [la page précédente](/docs/download_install/prerequisits).
{{% /callout %}}

4. Copiez/collez la commande suivante dans l'invite de commande pour installer les dépendances et librairies nécessaires à la compilation de FMT grace à `vcpkg` : 

``` bash
vcpkg install boost:x64-windows boost-dll:x64-windows boost-icl:x64-windows boost-python:x64-windows gdal:x64-windows geos:x64-windows pthreads:x64-windows clp:x64-windows
```

{{% callout warning %}}
Cela prendra du temps, et utilisera un certain espace sur votre ordinateur (environ 10 Go).

Laissez juste l'invite de commande ouverte pour que `vcpkg` fasse son travail.
{{% /callout %}}

* * *

Une fois que `vcpkg` est terminé, passez à la page suivante.