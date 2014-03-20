''' Help the user achieve a high score in a real game of threes by using a move searcher. '''

import ctypes
import time
import os
import socket
import json
import numpy as np
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
    for i,v in enumerate(m.flatten()):
        board |= v << (4*i)
    return board

def to_val(m):
    vals = 2**m
    vals[m == 0] = 0
    return vals

def to_score(m):
    vals = 2**m
    vals *= m-1
    vals[m <= 1] = 0
    return vals

if MULTITHREAD:
    from multiprocessing.pool import ThreadPool
    pool = ThreadPool(4)
    def score_toplevel_move(args):
        return ailib.score_toplevel_move(*args)

    def find_best_move(m):
        board = to_c_board(m)

        print to_val(m)
        print "Current approx. score:", to_score(m).sum()

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
    board = np.zeros((4,4), dtype=int)
    for tile in res:
        tval = pos = None
        for k in tile.split():
            m = re.match(r'^tile-(\d+)$', k)
            if m:
                tval = int(m.group(1))
            m = re.match(r'^tile-position-(\d+)-(\d+)$', k)
            if m:
                pos = int(m.group(1)), int(m.group(2))
        board[pos[1]-1, pos[0]-1] = np.round(np.log2(tval))

    return board

def keypress(ctrl, type, key):
    ctrl.execute(
'''var keyboardEvent = document.createEvent("KeyboardEvent");'''
'''var initMethod = typeof keyboardEvent.initKeyboardEvent !== 'undefined' ? "initKeyboardEvent" : "initKeyEvent";'''
'''keyboardEvent[initMethod]("%s", true, true, window, false, false, false, false, %d, 0);'''
'''document.dispatchEvent(keyboardEvent);''' % (type, key))

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
    ctrl.execute("var elems = document.getElementsByTagName('div'); for(var i in elems) if(elems[i].className == 'tile-container') tileContainer = elems[i];")

    while 1:
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
