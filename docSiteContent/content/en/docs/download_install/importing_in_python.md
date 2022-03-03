---
title: Importing FMT in Python
linktitle: Importing FMT in Python
type: book
date: "2019-05-05T00:00:00+01:00"
# Prev/next pager order (if `docs_section_pager` enabled in `params.toml`)
weight: 5
---

To import FMT that you just compiled into Python, you need to have [Python](https://www.python.org/) and [pip](https://packaging.python.org/en/latest/tutorials/installing-packages/) installed on your computer.

{{% callout note %}}
If you don't have Python installed, we recommand to you install [Miniconda](https://docs.conda.io/en/latest/miniconda.html), the "light" version of the popular [Anaconda](https://www.anaconda.com/) Python distribution. Miniconda contains Python, pip, and many other packages that are usefull when using FMT.
{{% /callout %}}

There are two ways to import FMT into Python :

1. **By using `sys.path.append`**

`sys.path.append` allows you to tell Python where to find the FMT library.

If you use it, the beginning of your Python scripts will look like this :

```python
import sys
sys.path.append(r"<Path to FMT folder>/")
sys.path.append(r"<Path to FMT folder>/Release/")
from FMT import Models
from FMT import Parser
from FMT import Core
from FMT import Version
```

in which `Path to FMT folder` will be replaced with the location of the folder where the FMT files that you've downloaded from GitHub are.

It can get redondant however, which make the second option - installing with pip - more interesting.

2. **By using pip and the Python wheel that you're compiled**

Python wheels are files that contain everything necessary for a Python module to be installed in your Python environment with the `pip` command. When installed in such a way, the module can be activated quicker in your python scripts.

To install the Python wheel that you've compiled, use the following command in a command prompt (if you have installed Python and pip independantly of Anaconda or Miniconda), or in an Anaconda prompt :

```bash
pip install "<path to FMT folder>\build\release\bin\Release/FMT-0.9.1-py3-none-any.whl"
```

Once this is done, you'll be able to import FMT in your scripts more quickly :

```python
from FMT import Models
from FMT import Parser
from FMT import Core
from FMT import Version
```

* * *

And **that's it, you're done 🎊🎉!** Now, you can start using FMT !

{{% callout note %}}
If you had any issues during the process of downloading and installing FMT, please [post an issue on the issue page of the repository for FMT](https://github.com/gcyr/FMT/issues).
{{% /callout %}}