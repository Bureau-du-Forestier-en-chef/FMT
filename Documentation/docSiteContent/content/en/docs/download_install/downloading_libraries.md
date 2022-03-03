---
title: Downloading the libraries
linktitle: Downloading the libraries
type: book
date: "2019-05-05T00:00:00+01:00"
# Prev/next pager order (if `docs_section_pager` enabled in `params.toml`)
weight: 3
---

1. If you don't have git installed on your computer, install it from [here](https://git-scm.com/).

{{% callout note %}}
Git is a free and open-source software used to help organise the way that people create source code for programs and tool, and how they collaborate to do it.

Here, git will help you download the code used to make the `vcpkg` software function, which will download all of the librairies that FMT needs for us.
{{% /callout %}}

2. Use `git clone https://github.com/microsoft/vcpkg.git` in the command prompt to download [vcpkg](https://github.com/microsoft/vcpkg). 

{{% callout note %}}
If you have closed the command prompt, or if you don't remember how to open it, see [the previous page](/docs/download_install/prerequisits).
{{% /callout %}}

3. Use `cd ./vcpkg` in the command prompt to navigate into the vcpkg folder.

{{% callout note %}}
If you don't remember how to use `cd`, see [the previous page](/docs/download_install/prerequisits).
{{% /callout %}}

4. Copy/paste the following command in the command prompt to install the dependencies necessary to compile FMT : 

``` bash
vcpkg install boost:x64-windows boost-dll:x64-windows boost-icl:x64-windows boost-python:x64-windows gdal:x64-windows geos:x64-windows pthreads:x64-windows clp:x64-windows
```

{{% callout warning %}}
This will take time, and will use some space on your computer (around 10Gb).

Just leave the command prompt open so that `vcpkg` does its work.
{{% /callout %}}

* * *

Once `vcpkg` is done, go to the next page.