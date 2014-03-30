@mkdir bin
cl /W1 /O2 /Gd /MD /D _WINDLL /EHsc /nologo 2048.cpp /Fo:bin\2048.obj /link /OUT:bin\2048.exe
cl /nologo bin\2048.obj /link /DLL /OUT:bin\2048.so
