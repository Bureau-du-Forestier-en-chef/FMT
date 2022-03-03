---
title: Importer FMT dans Python
linktitle: Importer FMT dans Python
type: book
date: "2019-05-05T00:00:00+01:00"
# Prev/next pager order (if `docs_section_pager` enabled in `params.toml`)
weight: 5
---

Pour importer FMT que vous venez de compiler dans Python, vous devez avoir installé [Python](https://www.python.org/) et [pip](https://packaging.python.org/en/latest/tutorials/installing-packages/) sur votre ordinateur.

{{% callout note %}}
Si Python n'est pas installé sur votre ordinateur, nous vous recommandons d'installer [Miniconda](https://docs.conda.io/en/latest/miniconda.html), la version "légère" de la célèbre distribution Python [Anaconda](https://www.anaconda.com/). Miniconda contient Python, pip et de nombreux autres modules qui sont utiles pour utiliser FMT.
{{% /callout %}}

Il existe deux façons d'importer FMT dans Python :

1. **En utilisant `sys.path.append`**.

`sys.path.append` vous permet d'indiquer à Python où trouver les bibliothèques de fonction de FMT.

Si vous l'utilisez, le début de vos scripts Python ressemblera à ceci :

```python
import sys
sys.path.append(r"<Chemin vers le dossier FMT>/")
sys.path.append(r"<Chemin vers le dossier FMT>/Release/")
from FMT import Models
from FMT import Parser
from FMT import Core
from FMT import Version
```

dans lequel `<Chemin vers le dossier FMT>` sera remplacé par l'emplacement du dossier où se trouvent les fichiers FMT que vous avez téléchargés depuis GitHub (et où se trouvent aussi les bibliothèque de fonction compilées de FMT, à présent).

Cela peut cependant devenir redondant, ce qui rend la deuxième option - l'installation avec pip - plus intéressante.

2. **En utilisant pip et la *Python wheel* que vous avez compilée**.

Les *Python wheels* sont des fichiers qui contiennent tout ce qui est nécessaire pour qu'un module Python soit installé dans votre environnement Python avec la commande `pip`. Une fois installé de cette manière, le module peut être activé plus rapidement dans vos scripts Python.

Pour installer la *Python wheel* que vous avez compilée, utilisez la commande suivante dans une invite de commande (si vous avez installé Python et pip indépendamment d'Anaconda ou Miniconda), ou dans une invite de commande Anaconda :

```bash
pip install "<chemin vers le dossier FMT>\build\release\bin\Release/FMT-0.9.1-py3-none-any.whl"
```

Une fois cette opération effectuée, vous pourrez importer FMT dans vos scripts plus rapidement :

```python
from FMT import Models
from FMT import Parser
from FMT import Core
from FMT import Version
```

* * *

Et **c'est tout, vous avez terminé 🎊🎉!** Maintenant, vous pouvez commencer à utiliser FMT !

{{% callout note %}}
Si vous avez eu des problèmes pendant le processus de téléchargement et d'installation de FMT, veuillez [poster un problème sur la page de problèmes du *repository* de FMT](https://github.com/gcyr/FMT/issues).
{{% /callout %}}