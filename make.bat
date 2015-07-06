call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"

cl mandelbrotOriginal.c
cl mandelbrotMMX.c
cl mandelbrotMMXmelhorado.c
cl MandelbrotAVX.c

echo Em execucao...

mandelbrotOriginal
mandelbrotMMX
mandelbrotMMXmelhorado
MandelbrotAVX

echo Execucao concluida.

pause
