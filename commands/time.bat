@echo off
@setlocal

set start=%time%

cmd /c %*

set end=%time%

set options="tokens=1-4 delims=:.,"
for /f %options% %%a in ("%start%") do set start_h=%%a&set /a start_m=100%%b %% 100&set /a start_s=100%%c %% 100&set /a start_ms=100%%d %% 100
for /f %options% %%a in ("%end%") do set end_h=%%a&set /a end_m=100%%b %% 100&set /a end_s=100%%c %% 100&set /a end_ms=100%%d %% 100

set /a hours=%end_h%-%start_h%
set /a mins=%end_m%-%start_m%
set /a secs=%end_s%-%start_s%
set /a cs=%end_ms%-%start_ms%
if %cs% lss 0 set /a secs = %secs% - 1 & set /a cs = 100%cs%
if %secs% lss 0 set /a mins = %mins% - 1 & set /a secs = 60%secs%
if %mins% lss 0 set /a hours = %hours% - 1 & set /a mins = 60%mins%
if %hours% lss 0 set /a hours = 24%hours%
if 1%cs% lss 100 set cs=0%cs%

set /a totalsecs = %hours%*3600 + %mins%*60 + %secs%
echo Build took %hours%h, %mins%m, %secs%.%cs%s (%totalsecs%.%cs%s total)
