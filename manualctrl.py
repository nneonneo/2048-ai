from __future__ import print_function

from ailib import ailib, to_c_board, from_c_board, to_c_index, from_c_index
from gamectrl import Generic2048Control

try:
    input = raw_input
except NameError:
    pass

def print_board(m):
    for row in m:
        for c in row:
            print('%8d' % from_c_index(c), end=' ')
        print()

class ManualControl(Generic2048Control):
    def __init__(self):
        print("Enter board one row at a time, with entries separated by spaces")
        board = []
        for ri in range(4):
            board.append([to_c_index(int(c)) for c in input("Row %d: " % (ri + 1)).split()])
        self.cur_board = board

    def get_status(self):
        return "running"

    def restart_game(self):
        print("Game over - time to restart!")

    def continue_game(self):
        pass

    def get_score(self):
        # don't care
        return 0

    def get_board(self):
        print("Current board:")
        print_board(self.cur_board)

        updates = input("Enter updates in the form r,c,n (1-indexed row/column), separated by spaces: ")
        for item in updates.split():
            r, c, n = map(int, item.split(","))
            self.cur_board[r-1][c-1] = to_c_index(n)

        return self.cur_board

    def execute_move(self, move):
        print("EXECUTE MOVE:", ["up", "down", "left", "right"][move])
        self.cur_board = from_c_board(ailib.execute_move(move, to_c_board(self.cur_board)))
