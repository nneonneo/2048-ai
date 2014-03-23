''' Help the user achieve a high score in a real game of threes by using a move searcher. '''

import ctypes
import time
import os
import socket
import json
import math
import re

# Enable multithreading?
MULTITHREAD = True

ailib = ctypes.CDLL('bin/2048.so')
ailib.init_move_tables()
ailib.init_score_tables()

ailib.find_best_move.argtypes = [ctypes.c_uint64]
ailib.score_toplevel_move.argtypes = [ctypes.c_uint64, ctypes.c_int]
ailib.score_toplevel_move.restype = ctypes.c_float

def to_c_board(m):
    board = 0
    i = 0
    for row in m:
        for c in row:            
            board |= c << (4*i)
            i += 1
    return board

def print_board(m):
    for row in m:
        for c in row:
            print '%8d' % c,
        print

def _to_val(c):
    if c == 0: return 0
    return 2**c

def to_val(m):
    return [[_to_val(c) for c in row] for row in m]

def _to_score(c):
    if c <= 1:
        return 0
    return (c-1) * (2**c)

def to_score(m):
    return [[_to_score(c) for c in row] for row in m]

if MULTITHREAD:
    from multiprocessing.pool import ThreadPool
    pool = ThreadPool(4)
    def score_toplevel_move(args):
        return ailib.score_toplevel_move(*args)

    def find_best_move(m):
        board = to_c_board(m)

        print_board(to_val(m))
        print "Current approx. score:", sum(_to_score(c) for row in m for c in row)

        scores = pool.map(score_toplevel_move, [(board, move) for move in xrange(4)])
        bestmove, bestscore = max(enumerate(scores), key=lambda x:x[1])
        if bestscore == 0:
            return -1
        return bestmove
else:
    def find_best_move(m):
        board = to_c_board(m)
        return ailib.find_best_move(board)

def movename(move):
    return ['up', 'down', 'left', 'right'][move]

class BrowserRemoteControl(object):
    def __init__(self, port):
        self.sock = socket.socket()
        self.sock.connect(('', port))

    def execute(self, cmd):
        self.sock.send(cmd + '\r\n')
        ret = []
        while True:
            chunk = self.sock.recv(4096)
            ret.append(chunk)
            if '\n' in chunk:
                break
        res = json.loads(''.join(ret))
        if 'error' in res:
            raise Exception(res['error'])
        else:
            return res['result']

def get_board(ctrl):
    res = ctrl.execute("var res = []; var tiles = tileContainer.children; for(var i=0; i<tiles.length; i++) res.push(tiles[i].className); res")
    board = [[0]*4 for _ in xrange(4)]
    for tile in res:
        tval = pos = None
        for k in tile.split():
            m = re.match(r'^tile-(\d+)$', k)
            if m:
                tval = int(m.group(1))
            m = re.match(r'^tile-position-(\d+)-(\d+)$', k)
            if m:
                pos = int(m.group(1)), int(m.group(2))
        board[pos[1]-1][pos[0]-1] = int(round(math.log(tval, 2)))

    return board

def keypress(ctrl, type, key):
    ctrl.execute(
'''var keyboardEvent = document.createEvent("KeyboardEvent");'''
'''var initMethod = typeof keyboardEvent.initKeyboardEvent !== 'undefined' ? "initKeyboardEvent" : "initKeyEvent";'''
'''keyboardEvent[initMethod]("%s", true, true, window, false, false, false, false, %d, 0);'''
'''document.dispatchEvent(keyboardEvent);''' % (type, key))

def check_end(ctrl):
    ''' Check if the game has ended. Continue the game automatically if it gets to the win screen. '''
    return ctrl.execute(
'var messageContainer = document.querySelector(".game-message");'
'if(messageContainer.className.search(/game-over/) !== -1) {"ended"}'
'else if(messageContainer.className.search(/game-won/) !== -1) {document.querySelector(".keep-playing-button").click(); "continued"}'
'else {"running"}')

def do_move(ctrl, move):
    key = [38, 40, 37, 39][move]
    keypress(ctrl, 'keydown', key)
    time.sleep(0.01)
    keypress(ctrl, 'keyup', key)

def rungame(args):
    if len(args) == 1:
        port = int(args[0])
    else:
        port = 32000

    ctrl = BrowserRemoteControl(port)
    ctrl.execute("var elems = document.getElementsByTagName('div'); for(var i in elems) if(elems[i].className == 'tile-container') {tileContainer = elems[i]; break;}")

    while 1:
        state = check_end(ctrl)
        if state == 'ended':
            print "Game over."
            break

        board = get_board(ctrl)
        move = find_best_move(board)
        if move < 0:
            break
        print "Recommended move:", movename(move)
        do_move(ctrl, move)
        time.sleep(0.05)

if __name__ == '__main__':
    import sys
    rungame(sys.argv[1:])
