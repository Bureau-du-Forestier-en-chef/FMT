::At first the shared library needs to be put in FMT\inst\libs\x64
::Then For building the R package
R CMD build FMT
::and then For installing the R package
R CMD INSTALL --no-multiarch FMT_0.9.tar.gz