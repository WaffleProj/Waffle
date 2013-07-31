@echo	off
set	Project=Waffle
call	Common	StartUp	%1

::goto	nocommondll
call	Common	ChangeDirectory	%Project%.common.1.0.dll
call	Common	Compile	common.c
call	Common	Compile	src\cmdarg.c
call	Common	Compile	src\pemachine.c
call	Common	Compile	src\inject.c
call	Common	Compile	src\filesystem.c
call	Common	Compile	src\setting.c
call	Common	Compile	src\init.c
call	Common	Compile	src\hash.c
call	Common	Compile	src\exception.c
if	"%Machine%" == "I386"	(
ld	--subsystem windows --dll -L%MinGW%\lib -L%OUTPUT_PATH%\Component\Waffle\%Machine% --enable-stdcall-fixup -kill-at -e _DllMain -o %OUTPUT_PATH%\Component\%Project%\%Machine%\%Project%.common.1.0.dll common.o cmdarg.o pemachine.o inject.o filesystem.o setting.o init.o hash.o exception.o -lkernel32 -luser32 -ladvapi32 -lpsapi
	)
if	"%Machine%" == "AMD64"	(
ld	--subsystem windows --dll -L%MinGW%\lib -L%OUTPUT_PATH%\Component\Waffle\%Machine% --enable-stdcall-fixup -kill-at -e  DllMain -o %OUTPUT_PATH%\Component\%Project%\%Machine%\%Project%.common.1.0.dll common.o cmdarg.o pemachine.o inject.o filesystem.o setting.o init.o hash.o exception.o -lkernel32 -luser32 -ladvapi32 -lpsapi
	)
call	Common	CleanUp
:nocommondll

::goto nowaffle
call	Common	ChangeDirectory	%Project%.exe
call	Common	Compile	waffle.c
if	"%Machine%" == "I386"	(
ld	--subsystem windows -L%MinGW%\lib -L%OUTPUT_PATH%\Component\Waffle\%Machine% --enable-stdcall-fixup -kill-at -e _Main -o %OUTPUT_PATH%\Component\%Project%\%Machine%\%Project%.exe waffle.o -lWaffle.common.1.0 -lkernel32 -luser32 -lcomdlg32
	)
if	"%Machine%" == "AMD64"	(
ld	--subsystem windows -L%MinGW%\lib -L%OUTPUT_PATH%\Component\Waffle\%Machine% --enable-stdcall-fixup -kill-at -e  Main -o %OUTPUT_PATH%\Component\%Project%\%Machine%\%Project%.exe waffle.o -lWaffle.common.1.0 -lkernel32 -luser32 -lcomdlg32
	)
call	Common	CleanUp
:nowaffleexe

popd