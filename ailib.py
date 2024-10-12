import ctypes
import os

for suffix in ['so', 'dll', 'dylib']:
    dllfn = 'bin/2048.' + suffix
    if not os.path.isfile(dllfn):
        continue
    ailib = ctypes.CDLL(dllfn)
    break
else:
    print("Couldn't find 2048 library bin/2048.{so,dll,dylib}! Make sure to build it first.")
    exit()

ailib.init_tables()

ailib.find_best_move.argtypes = [ctypes.c_uint64]
ailib.score_toplevel_move.argtypes = [ctypes.c_uint64, ctypes.c_int]
ailib.score_toplevel_move.restype = ctypes.c_float
ailib.execute_move.argtypes = [ctypes.c_int, ctypes.c_uint64]
ailib.execute_move.restype = ctypes.c_uint64

def to_c_board(m):
    board = 0
    i = 0
    for row in m:
        for c in row:
            board |= int(c) << (4*i)
            i += 1
    return board

def from_c_board(n):
    board = []
    i = 0
    for ri in range(4):
        row = []
        for ci in range(4):
            row.append((n >> (4 * i)) & 0xf)
            i += 1
        board.append(row)
    return board

def to_c_index(n):
    return [0, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768].index(n)

def from_c_index(c):
    if c == 0: return 0
    return 2**c
