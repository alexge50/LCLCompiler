@echo off

set include_directories=/I ../sources/ /I ../external/ /I ../tests
set disabled_warnings=/wd4458 /wd4201 /wd4189 /wd4100 /wd4505
set common_compiler_flag=/nologo /MT /MP /std:c++17 /Gm- /EHsc /EHa- /permissive- /Oi /Od /GR- /WX /W4 %disabled_warnings% /Z7

if not exist tests_bin mkdir tests_bin
pushd tests_bin

cl %common_compiler_flag% /Fetokenizer_tests %include_directories% ../sources/tokenizer.cpp ../tests/test_tokenizer.cpp

popd

.\tests_bin\tokenizer_tests.exe