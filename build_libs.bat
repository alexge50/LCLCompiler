@echo off

if not exist external_bin mkdir external_bin
pushd external_bin

cl /nologo /c /MT /MP /Gm- /DUTF8PROC_STATIC /Z7 /I ../external/ ../external/utf8proc/utf8proc.c
cl /nologo /c /MT /MP /Gm- /Gm- /EHsc /EHa- /Z7 /I ../external/ ../external/fmt/posix.cc
cl /nologo /c /MT /MP /Gm- /Gm- /EHsc /EHa- /Z7 /I ../external/ ../external/fmt/format.cc

lib /nologo utf8proc.obj
lib /nologo posix.obj
lib /nologo format.obj 

del utf8proc.obj
del posix.obj
del format.obj

popd