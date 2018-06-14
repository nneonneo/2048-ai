* ASCII text, with very long lines
인공지능을 위한 [2048 게임](http://gabrielecirulli.github.io/2048/). 이것은 최근 하드웨어의 초당 천만번 이상의 움직임을 감지하기 위해 고효율 비트보드 표시와 기댓값 최적화를 사용합니다. 사용되는 휴리스틱은 빈 정사각형에 대한 보너스와 가장자리 및 모서리 근처에 값을 배치하는 보너스를 포함합니다. 알고리즘에 대한 자세한 내용은 StackOverflow를 참조하세요. [StackOverflow answer](https://stackoverflow.com/a/22498940/1204143).

## 빌드

### Unix/Linux/OS X

실행

    ./configure
    make

터미널에서. 어떤 비교적 최근의 C++ 컴파일러라면 이것의 결과를 출력할 수 있어야 합니다.
당신이 `make install`;을 하지 않으면 이 프로그램은 이 디렉토리에서 실행됩니다

### Windows

당신에겐 당신이 무엇을 인스톨 했는지에 따르는 몇 개의 옵션이 있습니다.
-Pure Cygwin : 위의 Unix/Linux/OS X 지침을 따릅니다. 그 결과로 DLL은 *Cygwin 프로그램에서만* 사용할 수 있으므로 브라우저 제어 버전을 실행하려면 Cygwin Python (python.org의 python이 아님)을 사용해야합니다. 
단계별 지침은 이 문서를 참조하세요.
 courtesy Tamas Szell (@matukaa), [이 문서를 열어보세요.](https://github.com/nneonneo/2048-ai/wiki/CygwinStepByStep.pdf).


- Cygwin with MinGW: 실행

        CXX=x86_64-w64-mingw32-g++ CXXFLAGS='-static-libstdc++ -static-libgcc -D_WINDLL -D_GNU_SOURCE=1' ./configure ; make

MinGW나 Cygwin쉘에서 빌드할 때. 결과 DLL은 Cygwin이 아닌 프로그램과 함께 사용할 수 있습니다. 
-비주얼스튜디오 : 비쥬얼스튜디오 명령프롬프트를 열고 현재 디렉토리`cd` 를 2048-ai 경로로 설정한 후 명령 프롬프트에 `make-msv.bat` 실행을 명령합니다.

## 커맨드라인 버전 실행 
AI가 스스로 동작하는 것을 보고 싶다면 `bin/2048`을 실행시키세요.

## 브라우저 컨트롤 버전 실행

###파이어폭스

Install [ Firefox를 위한 원격제어를 설치하세요.](https://github.com/nneonneo/FF-Remote-Control/raw/V_1.2/remote_control-1.2-fx.xpi).
2048 game이나 [2048 game](http://gabrielecirulli.github.io/2048/) 호환되는 복제본을 열고 원격제어를 시작 하세요 
 `2048.py -b firefox` 를 실행시키고 게임을 지켜보세요!

### 크롬

 `remote-debugging-port` 을 통해 크롬 원격제어 디버깅을 활성화 시키고 command-line switch (e.g. `google-chrome --remote-debugging-port=9222`).
[2048 game](http://gabrielecirulli.github.io/2048/)을 열고, `2048.py -b chrome`을 실행시킨뒤, 게임을 지켜보세요. The `-p` 옵션은 주로 연결될 포트를 설정합니다.

