@echo off

set WarningEliminations=-wd4100 -wd4201 -wd4820 -wd4191 -wd5045 -wd4189
set CompilerFlags=-WL -nologo -Gm- -WX -Wall %WarningEliminations% -Od -Oi -Z7 -DDEBUG -DSYNCOP_UNIT_BUILD
REM set CompilerFlags=-WL -nologo -Gm- -WX -Wall %WarningEliminations% -O2 -DNDEBUG
set OutputPath=-Fo"build/"
set OutputFile=-Fe"build/bin/Syncop" 
set Libraries=vulkan-1.lib user32.lib Winmm.lib
set Files=./src/main.c
set IncludeDirs=-I./src -IC:/VulkanSDK/1.3.283.0/Include 
set LibraryDirs=/LIBPATH:"C:/VulkanSDK/1.3.283.0/Lib" /LIBPATH:./build

IF NOT EXIST build/bin mkdir build\bin

del .\build\bin\*.pdb > NUL 2> NUL
cl %CompilerFlags% %OutputPath% -Fe"build/bin/game" %IncludeDirs% ./src/game.c /LD /link %LibraryDirs% vulkan-1.lib /EXPORT:game_update_and_render

cl %CompilerFlags% %OutputFile% %OutputPath% %IncludeDirs% %Files% /link /SUBSYSTEM:windows %LibraryDirs% %Libraries%

REM copy .\build\bin\game.dll .\build\bin\game_load.dll

IF NOT %errorlevel% neq 0 (echo Build Completed Successfully) ELSE (echo ERROR Build Stopped)
set HHHHHHH=1
