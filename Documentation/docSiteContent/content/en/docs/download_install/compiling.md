---
title: Compiling
linktitle: Compiling
type: book
date: "2019-05-05T00:00:00+01:00"
# Prev/next pager order (if `docs_section_pager` enabled in `params.toml`)
weight: 4
---

Now that everything is ready, it is time to get into the compiling step proper.

1. Use `cd ..` to go back to the `FMT-Installation` folder.

2. Clone the repository of FMT by using `git clone https://github.com/gcyr/FMT.git`

3. Use `cd ./FMT` to navigate into the FMT folder downloaded from the repository of FMT.

4. Copy/paste the following commands **in the Windows notepad**, and then replace the `-DMOSEK_DIR` parameter with the location of Mosek on your computer:

```bash
cmake CMakeLists.txt -B build/release -G "Visual Studio 16 2019" -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_TOOLCHAIN_FILE="../vcpkg/scripts/buildsystems/vcpkg.cmake" -DBOOST_DIR="../vcpkg/installed/x64-windows/" -DCMAKE_BUILD_TYPE=Release -DGDAL_DIR="../vcpkg/installed/x64-windows/" -DGEOS_DIR="../vcpkg/installed/x64-windows/" -DOSI_DIR="../vcpkg/installed/x64-windows/" -DPYTHON_DIR="../vcpkg/packages/python3_x64-windows/" -DMOSEK_DIR="C:/PROGRA~1/Mosek/"
cmake --build build/release --config Release
cmake --install build/release --config Release
```

{{% callout note %}}
By default, Mosek was installed in your `C:/Program Files` folder on windows. This can be expressed by the path `C:/PROGRA~1` to avoid the space between `Program` and `Files`, which can create errors.

Thus, if you installed Mosek in `C:\Program Files\Mosek`, you can keep `C:/PROGRA~1/Mosek/` in the commands.
{{% /callout %}}

5. Copy/paste all of the commands that are ready in the notepad into the command prompt to start the compilation. Press enter to activate them.

{{% callout warning %}}
This will take time, and you might see warning in the command prompt during the process. Don't worry about them.
{{% /callout %}}

{{% callout warning %}}
If the compilation ends up with error messages that are red, you can come and [post an issue on the issue page of the repository for FMT](https://github.com/gcyr/FMT/issues).
{{% /callout %}}

* * *

If everything worked, congratulations ! Now, it leaves us with importing FMT in Python, and seeing that everything works.