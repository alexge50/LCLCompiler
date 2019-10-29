@echo off

set include_directories=/I ../sources/ /I ../external/
set disabled_warnings=/wd4458 /wd4201 /wd4189 /wd4100 /wd4505
set common_compiler_flag=/nologo /DEBUG:FULL /MT /MP /std:c++17 /Fe:lcl /Gm- /EHsc /EHa- /permissive- /Oi /Od /GR- /WX /W4 %disabled_warnings% /Z7

if not exist bin mkdir bin
pushd bin

cl %common_compiler_flag% %include_directories% ../sources/*.cpp /link ../external_bin/format.lib ../external_bin/posix.lib ../external_bin/utf8proc.lib

popd