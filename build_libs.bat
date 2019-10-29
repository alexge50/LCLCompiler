@echo off

if not exist "./bin/libs_bin" mkdir "./bin/libs_bin"
pushd "bin/libs_bin"

::Clean
del *.lib > NUL

::Build utf8proc
cl /nologo /c /MT /MP /Gm- /DUTF8PROC_STATIC /Z7 /I ../../libs/ ../../libs/utf8proc/utf8proc.c

::Build fmt
cl /nologo /c /MT /MP /Gm- /Gm- /EHsc /EHa-  /Z7 /I ../../libs/ ../../libs/fmt/posix.cc
cl /nologo /c /MT /MP /Gm- /Gm- /EHsc /EHa-  /Z7 /I ../../libs/ ../../libs/fmt/format.cc

::Gen utf8proc lib
lib /nologo utf8proc.obj

::Gen fmt libs
lib /nologo posix.obj
lib /nologo format.obj 

del *.obj

popd