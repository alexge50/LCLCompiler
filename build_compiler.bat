@echo off

set include_directories=/I ../../sources/ /I ../../libs/
set disabled_warnings=/wd4458 /wd4201 /wd4189 /wd4100 /wd4505
set common_compiler_flags=/nologo /DEBUG:FULL /MT /MP /std:c++17 /Fe:lcl /Gm- /EHsc /EHa- /permissive- /Oi /Od /GR- /WX /W4 %disabled_warnings% /Z7
set libraries=../libs_bin/format.lib ../libs_bin/posix.lib ../libs_bin/utf8proc.lib

if not exist "./bin/compiler_bin" mkdir "./bin/compiler_bin"
pushd "./bin/compiler_bin"

cl %common_compiler_flags% %include_directories% ../../sources/*.cpp /link %libraries%

popd