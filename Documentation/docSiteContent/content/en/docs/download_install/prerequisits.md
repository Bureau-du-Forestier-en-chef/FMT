---
title: Prerequisits
linktitle: Prerequisits
type: book
date: "2019-05-05T00:00:00+01:00"
# Prev/next pager order (if `docs_section_pager` enabled in `params.toml`)
weight: 2
---

1. Download and install [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/)

{{% callout warning %}}
When installing Visual Studio Community, don't forget to indicate during the installation that you want the `Cmake build tool` installed.
{{% /callout %}}
{{% callout note %}}
Visual Studio Community is the free version of the Visual Studio Software by Microsoft. We will use it later to compile FMT, as it comes with a compiler compatible with Windows.
{{% /callout %}}

2. Download and install [Mosek](https://www.mosek.com/)

{{% callout note %}}
Mosek is a solver for linear programming problems, which is exactly the kind of problems that FMT deals with. However, FMT can work with different other solvers, both free or not free.
{{% /callout %}}

3. Create a `FMT-Installation` folder in your computer. **Make sure that there is no spaces or accents in the path leading to the folder.**

{{% callout note %}}
Spaces and accents in the path can cause errors with different programs and different steps of the compilations. It's annoying, but that's how it is !
{{% /callout %}}

4. Open a command prompt, and use the `cd` command to navigate to the `FMT-Installation` folder.

{{% callout note %}}
To open the command prompt on Windows 10, simply press the {{< icon name="windows" pack="fab" >}} key, type `cmd`, and then press `Enter`.

The `cd` command is used to change the folder inside which the command prompt is ready to act.

Type `cd ..` to go to the parent folder from the one you're in, and `cd folder` to go into a folder.
{{% /callout %}}