@echo off

set include_directories=/I ../../sources/ /I ../../libs/ /I ../../tests
set disabled_warnings=/wd4458 /wd4201 /wd4189 /wd4100 /wd4505
set common_compiler_flags=/nologo /DEBUG:FULL /MT /MP /std:c++17 /Fe:tokenizer_tests /Gm- /EHsc /EHa- /Z7 /permissive- /Oi /Od /GR- /WX /W4 %disabled_warnings%
set libraries=../libs_bin/format.lib ../libs_bin/posix.lib ../libs_bin/utf8proc.lib

if not exist "./bin/tests_bin" mkdir "./bin/tests_bin"
pushd "./bin/tests_bin"

cl %common_compiler_flags% %include_directories% ../../tests/test_tokenizer.cpp ../../sources/tokenizer.cpp /link %libraries%

popd