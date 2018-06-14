* ASCII text, with very long lines
�ΰ������� ���� [2048 ����](http://gabrielecirulli.github.io/2048/). �̰��� �ֱ� �ϵ������ �ʴ� õ���� �̻��� �������� �����ϱ� ���� ��ȿ�� ��Ʈ���� ǥ�ÿ� ��� ����ȭ�� ����մϴ�. ���Ǵ� �޸���ƽ�� �� ���簢���� ���� ���ʽ��� �����ڸ� �� �𼭸� ��ó�� ���� ��ġ�ϴ� ���ʽ��� �����մϴ�. �˰��� ���� �ڼ��� ������ StackOverflow�� �����ϼ���. [StackOverflow answer](https://stackoverflow.com/a/22498940/1204143).

## ����

### Unix/Linux/OS X

����

    ./configure
    make

�͹̳ο���. � ���� �ֱ��� C++ �����Ϸ���� �̰��� ����� ����� �� �־�� �մϴ�.
����� `make install`;�� ���� ������ �� ���α׷��� �� ���丮���� ����˴ϴ�

### Windows

��ſ��� ����� ������ �ν��� �ߴ����� ������ �� ���� �ɼ��� �ֽ��ϴ�.
-Pure Cygwin : ���� Unix/Linux/OS X ��ħ�� �����ϴ�. �� ����� DLL�� *Cygwin ���α׷�������* ����� �� �����Ƿ� ������ ���� ������ �����Ϸ��� Cygwin Python (python.org�� python�� �ƴ�)�� ����ؾ��մϴ�. 
�ܰ躰 ��ħ�� �� ������ �����ϼ���.
 courtesy Tamas Szell (@matukaa), [�� ������ �������.](https://github.com/nneonneo/2048-ai/wiki/CygwinStepByStep.pdf).


- Cygwin with MinGW: ����

        CXX=x86_64-w64-mingw32-g++ CXXFLAGS='-static-libstdc++ -static-libgcc -D_WINDLL -D_GNU_SOURCE=1' ./configure ; make

MinGW�� Cygwin������ ������ ��. ��� DLL�� Cygwin�� �ƴ� ���α׷��� �Բ� ����� �� �ֽ��ϴ�. 
-���־�Ʃ��� : �����Ʃ��� ���������Ʈ�� ���� ���� ���丮`cd` �� 2048-ai ��η� ������ �� ��� ������Ʈ�� `make-msv.bat` ������ ����մϴ�.

## Ŀ�ǵ���� ���� ���� 
AI�� ������ �����ϴ� ���� ���� �ʹٸ� `bin/2048`�� �����Ű����.

## ������ ��Ʈ�� ���� ����

###���̾�����

Install [ Firefox�� ���� ������� ��ġ�ϼ���.](https://github.com/nneonneo/FF-Remote-Control/raw/V_1.2/remote_control-1.2-fx.xpi).
2048 game�̳� [2048 game](http://gabrielecirulli.github.io/2048/) ȣȯ�Ǵ� �������� ���� ������� ���� �ϼ��� 
 `2048.py -b firefox` �� �����Ű�� ������ ���Ѻ�����!

### ũ��

 `remote-debugging-port` �� ���� ũ�� �������� ������� Ȱ��ȭ ��Ű�� command-line switch (e.g. `google-chrome --remote-debugging-port=9222`).
[2048 game](http://gabrielecirulli.github.io/2048/)�� ����, `2048.py -b chrome`�� �����Ų��, ������ ���Ѻ�����. The `-p` �ɼ��� �ַ� ����� ��Ʈ�� �����մϴ�.

