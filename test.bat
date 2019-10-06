@echo off

set name=lcl_tests
set include_directories=/I ../sources/ /I ../external/ /I ../tests
set disabled_warnings=/wd4458 /wd4201 /wd4189 /wd4100 /wd4505
set common_compiler_flag=/nologo /MT /MP /std:c++17 /Gm- /Fe%name% /EHsc /EHa- /permissive- /Oi /Od /GR- /WX /W4 %disabled_warnings% /Z7
set common_linker_flags=-opt:ref user32.lib Gdi32.lib

if not exist tests_bin mkdir tests_bin
pushd tests_bin

REM 32 bit build
REM cl %common_compiler_flag% ../source/Win32Handmade.cpp /link /subsystem:windows,5.1 %common_linker_flags%

REM 64 bit build
REM We should just exclude main.cpp from sources 
cl %common_compiler_flag% %include_directories% ../sources/tokenizer.cpp ../tests/*.cpp /link %common_linker_flags%

popd

.\tests_bin\lcl_tests.exe