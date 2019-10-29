@echo off

set include_directories=/I ../sources/ /I ../external/ /I ../tests
set disabled_warnings=/wd4458 /wd4201 /wd4189 /wd4100 /wd4505
set common_compiler_flag=/nologo /DEBUG:FULL /MT /MP /std:c++17 /Fe:tokenizer_tests /Gm- /EHsc /EHa- /Z7 /permissive- /Oi /Od /GR- /WX /W4 %disabled_warnings%

if not exist tests_bin mkdir tests_bin
pushd tests_bin

cl %common_compiler_flag% %include_directories% ../tests/test_tokenizer.cpp ../sources/tokenizer.cpp /link ../external_bin/format.lib ../external_bin/posix.lib ../external_bin/utf8proc.lib

popd

.\tests_bin\tokenizer_tests.exe