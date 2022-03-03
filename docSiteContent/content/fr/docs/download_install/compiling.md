---
title: Compiler
linktitle: Compiler
type: book
date: "2019-05-05T00:00:00+01:00"
# Prev/next pager order (if `docs_section_pager` enabled in `params.toml`)
weight: 4
---

Maintenant que tout est prêt, il est temps de passer à l'étape de compilation proprement dite.

1. Utilisez `cd ..` pour retourner dans le dossier `FMT-Installation`.

2. Clonez le *repository* de FMT en utilisant `git clone https://github.com/gcyr/FMT.git`.

3. Utilisez `cd ./FMT` pour naviguer dans le dossier FMT téléchargé depuis le *repository* de FMT.

4. Copiez/collez les commandes suivantes **dans le bloc-notes de Windows**, puis remplacez le paramètre `-DMOSEK_DIR` par l'emplacement de Mosek sur votre ordinateur :

```bash
cmake CMakeLists.txt -B build/release -G "Visual Studio 16 2019" -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_TOOLCHAIN_FILE="../vcpkg/scripts/buildsystems/vcpkg.cmake" -DBOOST_DIR="../vcpkg/installed/x64-windows/" -DCMAKE_BUILD_TYPE=Release -DGDAL_DIR=". ./vcpkg/installed/x64-windows/" -DGEOS_DIR="../vcpkg/installed/x64-windows/" -DOSI_DIR="../vcpkg/installed/x64-windows/" -DPYTHON_DIR="../vcpkg/packages/python3_x64-windows/" -DMOSEK_DIR="C:/PROGRA~1/Mosek/"
cmake --build build/release --config Release
cmake --install build/release --config Release
```

{{% callout note %}}
Par défaut, Mosek est installé dans votre dossier `C:/Program Files` sous Windows. Ceci peut être exprimé par le chemin `C:/PROGRA~1` pour éviter l'espace entre `Program` et `Files`, qui peut créer des erreurs.

Ainsi, si vous avez installé Mosek dans `C:\Program Files\Mosek`, vous pouvez conserver `C:/PROGRA~1/Mosek/` dans ces commandes.
{{% /callout %}}

5. Copiez/collez toutes les commandes qui sont prêtes dans le bloc-notes vers l'invite de commande pour lancer la compilation. Appuyez sur la touche Entrée pour les activer.

{{% callout warning %}}
Cela prendra du temps, et vous pourriez voir des avertissements dans l'invite de commande pendant le processus. Ne vous en inquiétez pas.
{{% /callout %}}

{{% callout warning %}}
Si la compilation se termine par des messages d'erreur en rouge, vous pouvez [poster un problème sur la page des problèmes du *repository* de FMT](https://github.com/gcyr/FMT/issues).
{{% /callout %}}

* * *

Si tout a fonctionné, félicitations ! Il ne nous reste plus qu'à importer FMT dans Python et à vérifier que tout fonctionne.