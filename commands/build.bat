@echo off

set WarningEliminations=-wd4100 -wd4201 -wd4820 -wd4191 -wd5045 -wd4189
set CompilerFlags=-WL -nologo -Gm- -WX -Wall %WarningEliminations% -Od -Oi -Z7 -DDEBUG -DSYNCOP_UNIT_BUILD
REM set CompilerFlags=-WL -nologo -Gm- -WX -Wall %WarningEliminations% -O2 -DNDEBUG
set OutputPath=-Fe"build/bin/Syncop" -Fo"build/"
set Libraries=vulkan-1.lib user32.lib Winmm.lib
set Files=./src/main.c
set IncludeDirs=-I./src -IC:/VulkanSDK/1.3.283.0/Include 
set LibraryDirs=/LIBPATH:"C:/VulkanSDK/1.3.283.0/Lib" /LIBPATH:./build

IF NOT EXIST build/bin mkdir build\bin
cl %CompilerFlags% %OutputPath% %IncludeDirs% %Files% /link /SUBSYSTEM:windows %LibraryDirs% %Libraries%

IF NOT %errorlevel% neq 0 (echo Build Completed Successfully) ELSE (echo ERROR Build Stopped)
set HHHHHHH=1
