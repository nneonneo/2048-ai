AI for the [2048 game](http://gabrielecirulli.github.io/2048/). This uses *expectimax optimization*, along with a highly-efficient bitboard representation to search upwards of 10 million moves per second on recent hardware. Heuristics used include bonuses for empty squares and bonuses for placing large values near edges and corners. Read more about the algorithm on the [StackOverflow answer](https://stackoverflow.com/a/22498940/1204143).

## Building

### Unix/Linux/OS X

Execute

    ./configure
    make

in a terminal. Any relatively recent C++ compiler should be able to build the output.

Note that you don't do `make install`; this program is meant to be run from this directory.

### Windows

You have a few options, depending on what you have installed.

- Pure Cygwin: follow the Unix/Linux/OS X instructions above. The resulting DLL can *only* be used with Cygwin programs, so
to run the browser control version, you must use the Cygwin Python (not the python.org Python). For step-by-step instructions, courtesy Tamas Szell (@matukaa), see [this document](https://github.com/nneonneo/2048-ai/wiki/CygwinStepByStep.pdf).
- Cygwin with MinGW: run

        CXX=x86_64-w64-mingw32-g++ CXXFLAGS='-static-libstdc++ -static-libgcc -D_WINDLL -D_GNU_SOURCE=1' ./configure ; make

    in a MinGW or Cygwin shell to build. The resultant DLL can be used with non-Cygwin programs.
- Visual Studio: open a Visual Studio command prompt, `cd` to the 2048-ai directory, and run `make-msvc.bat`.

## Running the command-line version

Run `bin/2048` if you want to see the AI by itself in action.

## Running the browser-control version

You can use this 2048 AI to control the 2048 browser game. The browser control capability is meant as a proof of concept to show the performance of the AI; it will only work on the [original 2048 browser game](http://gabrielecirulli.github.io/2048/) or any *compatible* clone, not all 2048 games.

### Firefox

Enable Firefox remote debugging by setting the about:config options "devtools.debugger.remote-enabled" and "devtools.chrome.enabled" to true, then quit Firefox and restart it with the `--start-debugger-server 32000` command-line option.

Open the game in a new tab, then run `2048.py -b firefox` and watch the game! The `-p` option can be used to set the port to connect to.

### Chrome

Enable Chrome remote debugging by quitting it and then restarting it with the `remote-debugging-port` command-line switch (e.g. `google-chrome --remote-debugging-port=9222`).

Open the game in a new tab, then run `2048.py -b chrome` and watch the game! The `-p` option can be used to set the port to connect to.
