#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <map>

#include "2048.h"

/* We can perform state lookups one row at a time by using arrays with 65536 entries. */

/* Move tables. Each row or compressed column is mapped to (oldrow^newrow) assuming row/col 0.
 *
 * Thus, the value is 0 if there is no move, and otherwise equals a value that can easily be
 * xor'ed into the current board state to update the board. */
static board_t row_left_table[65536];
static board_t row_right_table[65536];
static board_t col_up_table[65536];
static board_t col_down_table[65536];

void init_move_tables(void) {
    unsigned row;

    memset(row_left_table, 0, sizeof(row_left_table));
    memset(row_right_table, 0, sizeof(row_right_table));
    memset(col_up_table, 0, sizeof(col_up_table));
    memset(col_down_table, 0, sizeof(col_down_table));

    for(row = 0; row < 65536; row++) {
        unsigned int line[4] = {row & 0xf, (row >> 4) & 0xf, (row >> 8) & 0xf, (row >> 12) & 0xf};
        row_t result;
        int i, j;

        /* execute a move to the left */
        for(i=0; i<3; i++) {
            for(j=i+1; j<4; j++) {
                if(line[j] != 0)
                    break;
            }
            if(j == 4)
                break; // no more tiles to the right

            if(line[i] == 0) {
                line[i] = line[j];
                line[j] = 0;
                i--; // retry this entry
            } else if(line[i] == line[j] && line[i] != 0xf) {
                line[i]++;
                line[j] = 0;
            }
        }

        result = (line[0]) | (line[1] << 4) | (line[2] << 8) | (line[3] << 12);

        row_left_table[row] = row ^ result;
        row_right_table[reverse_row(row)] = reverse_row(row) ^ reverse_row(result);
        col_up_table[row] = unpack_col(row) ^ unpack_col(result);
        col_down_table[reverse_row(row)] = unpack_col(reverse_row(row)) ^ unpack_col(reverse_row(result));
    }
}

#define DO_LINE(tbl,i,lookup,xv) do { \
        tmp = tbl[lookup]; \
        ret ^= xv; \
    } while(0)

#define DO_ROW(tbl,i) DO_LINE(tbl,i, (board >> (16*i)) & ROW_MASK,          tmp << (16*i))
#define DO_COL(tbl,i) DO_LINE(tbl,i, pack_col((board >> (4*i)) & COL_MASK), tmp << (4*i))

static inline board_t execute_move_0(board_t board) {
    board_t tmp;
    board_t ret = board;

    DO_COL(col_up_table, 0);
    DO_COL(col_up_table, 1);
    DO_COL(col_up_table, 2);
    DO_COL(col_up_table, 3);

    return ret;
}

static inline board_t execute_move_1(board_t board) {
    board_t tmp;
    board_t ret = board;

    DO_COL(col_down_table, 0);
    DO_COL(col_down_table, 1);
    DO_COL(col_down_table, 2);
    DO_COL(col_down_table, 3);

    return ret;
}

static inline board_t execute_move_2(board_t board) {
    board_t tmp;
    board_t ret = board;

    DO_ROW(row_left_table, 0);
    DO_ROW(row_left_table, 1);
    DO_ROW(row_left_table, 2);
    DO_ROW(row_left_table, 3);

    return ret;
}

static inline board_t execute_move_3(board_t board) {
    board_t tmp;
    board_t ret = board;

    DO_ROW(row_right_table, 0);
    DO_ROW(row_right_table, 1);
    DO_ROW(row_right_table, 2);
    DO_ROW(row_right_table, 3);

    return ret;
}
#undef DO_ROW
#undef DO_COL
#undef DO_LINE

/* Execute a move. */
static inline board_t execute_move(int move, board_t board) {
    switch(move) {
    case 0: // up
        return execute_move_0(board);
    case 1: // down
        return execute_move_1(board);
    case 2: // left
        return execute_move_2(board);
    case 3: // right
        return execute_move_3(board);
    default:
        return ~0ULL;
    }
}

static inline int get_max_rank(board_t board) {
    int maxrank = 0;
    while(board) {
        int k = board & 0xf;
        if(k > maxrank) maxrank = k;
        board >>= 4;
    }
    return maxrank;
}

/* Optimizing the game */
static float row_heur_score_table[65536];
static float row_score_table[65536];

struct eval_state {
    std::map<board_t, float> trans_table; // transposition table, to cache previously-seen moves
    float cprob_thresh;
    int maxdepth;
    int curdepth;
    int cachehits;
    int moves_evaled;

    eval_state() : cprob_thresh(0), maxdepth(0), curdepth(0), cachehits(0), moves_evaled(0) {
    }
};

// score a single board heuristically
static float score_heur_board(board_t board);
// score a single board actually (adding in the score from spawned 4 tiles)
static float score_board(board_t board);
// score over all possible moves
static float score_move_node(eval_state &state, board_t board, float cprob);
// score over all possible tile choices and placements
static float score_tilechoose_node(eval_state &state, board_t board, float cprob);

void init_score_tables(void) {
    unsigned row;

    memset(row_heur_score_table, 0, sizeof(row_heur_score_table));
    memset(row_score_table, 0, sizeof(row_score_table));

    for(row = 0; row < 65536; row++) {
        int i;
        float heur_score = 0;
        float score = 0;

        for(i=0; i<4; i++) {
            int rank = (row >> (4*i)) & 0xf;

            if(rank == 0) {
                heur_score += 10000;
            } else if(rank >= 2) {
                // the score is the total sum of the tile and all intermediate merged tiles
                score += (rank-1) * powf(2, rank);
            }
        }
        row_score_table[row] = score;
        row_heur_score_table[row] = heur_score;
    }
}

#define SCORE_BOARD(board,tbl) ((tbl)[(board) & ROW_MASK] + \
    (tbl)[((board) >> 16) & ROW_MASK] + \
    (tbl)[((board) >> 32) & ROW_MASK] + \
    (tbl)[((board) >> 48) & ROW_MASK])

static float score_heur_board(board_t board) {
    return SCORE_BOARD(board, row_heur_score_table) + 100000;
}

static float score_board(board_t board) {
    return SCORE_BOARD(board, row_score_table);
}

static float score_tilechoose_node(eval_state &state, board_t board, float cprob) {
    float res = 0;
    int num_open = 0;

    for(int i=0; i<16; i++) {
        if(((board >> (4*i)) & 0xf) == 0)
            num_open++;
    }

    cprob /= num_open;

    for(int i=0; i<16; i++) {
        if(((board >> (4*i)) & 0xf) == 0) {
            res += score_move_node(state, board | (((board_t)1) << (4*i)), cprob * 0.9f) * 0.9f;
            res += score_move_node(state, board | (((board_t)2) << (4*i)), cprob * 0.1f) * 0.1f;
        }
    }

    return res / num_open;
}

/* Statistics and controls */
// cprob: cumulative probability
/* don't recurse into a node with a cprob less than this threshold */
#define CPROB_THRESH_BASE (0.001f)
#define CACHE_DEPTH_LIMIT 4

static float score_move_node(eval_state &state, board_t board, float cprob) {
    if(cprob < state.cprob_thresh || state.curdepth > 5) {
        if(state.curdepth > state.maxdepth)
            state.maxdepth = state.curdepth;
        return score_heur_board(board);
    }

    if(state.curdepth < CACHE_DEPTH_LIMIT) {
        const auto &i = state.trans_table.find(board);
        if(i != state.trans_table.end()) {
            state.cachehits++;
            return i->second;
        }
    }

    int move;
    float best = 0;

    state.curdepth++;
    for(move=0; move<4; move++) {
        board_t newboard = execute_move(move, board);
        state.moves_evaled++;
        if(board == newboard)
            continue;

        float res = score_tilechoose_node(state, newboard, cprob);
        if(res > best)
            best = res;
    }
    state.curdepth--;

    if(state.curdepth < CACHE_DEPTH_LIMIT) {
        state.trans_table[board] = best;
    }

    return best;
}

static float _score_toplevel_move(eval_state &state, board_t board, int move) {
    //int maxrank = get_max_rank(board);
    board_t newboard = execute_move(move, board);

    if(board == newboard)
        return 0;

    state.cprob_thresh = CPROB_THRESH_BASE;

    return score_tilechoose_node(state, newboard, 1.0f);
}

float score_toplevel_move(board_t board, int move) {
    float res;
    struct timeval start, finish;
    double elapsed;
    eval_state state;

    gettimeofday(&start, NULL);
    res = _score_toplevel_move(state, board, move);
    gettimeofday(&finish, NULL);

    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_usec - start.tv_usec) / 1000000.0;

    printf("Move %d: result %f: eval'd %d moves (%d cache hits, %zd cache size) in %.2f seconds (maxdepth=%d)\n", move, res,
        state.moves_evaled, state.cachehits, state.trans_table.size(), elapsed, state.maxdepth);

    return res;
}

/* Find the best move for a given board. */
int find_best_move(board_t board) {
    int move;
    float best = 0;
    int bestmove = -1;

    printf("%s\n", BOARDSTR(board, '\n'));
    printf("Current scores: heur %.0f, actual %.0f\n", score_heur_board(board), score_board(board));

    for(move=0; move<4; move++) {
        float res = score_toplevel_move(board, move);

        if(res > best) {
            best = res;
            bestmove = move;
        }
    }

    return bestmove;
}

int ask_for_move(board_t board) {
    int move;
    char validstr[5];
    char *validpos = validstr;

    printf("%s\n", BOARDSTR(board, '\n'));

    for(move=0; move<4; move++) {
        if(execute_move(move, board) != board)
            *validpos++ = "UDLR"[move];
    }
    *validpos = 0;
    if(validpos == validstr)
        return -1;

    while(1) {
        char movestr[64];
        const char *allmoves = "UDLR";

        printf("Move [%s]? ", validstr);

        if(!fgets(movestr, sizeof(movestr)-1, stdin))
            return -1;

        if(!strchr(validstr, toupper(movestr[0]))) {
            printf("Invalid move.\n");
            continue;
        }

        return strchr(allmoves, toupper(movestr[0])) - allmoves;
    }
}

/* Playing the game */
static int draw_tile() {
    return (UNIF_RANDOM(10) < 9) ? 1 : 2;
}

static board_t insert_tile_rand(board_t board, int tile) {
    int num_open = 0;
    for(int i=0; i<16; i++) {
        if(((board >> (4*i)) & 0xf) == 0)
            num_open++;
    }

    if(num_open == 0) {
        printf("insert_tile_rand: no open spots!\n");
        return board;
    }

    int index = UNIF_RANDOM(num_open);
    for(int i=0; i<16; i++) {
        if(((board >> (4*i)) & 0xf) != 0)
            continue;
        if(index == 0) {
            board |= ((board_t)tile) << (4*i);
            break;
        }
        index--;
    }

    return board;
}
        
static board_t initial_board() {
    board_t board = 0;

    /* Draw initial values */
    for(int i=0; i<2; i++) {
        board = insert_tile_rand(board, draw_tile());
    }

    return board;
}


void play_game(get_move_func_t get_move) {
    board_t board = initial_board();
    int moveno = 0;
    int scorepenalty = 0; // "penalty" for obtaining free 4 tiles

    while(1) {
        int move;
        board_t newboard;

        for(move = 0; move < 4; move++) {
            if(execute_move(move, board) != board)
                break;
        }
        if(move == 4)
            break; // no legal moves

        printf("\nMove #%d, current score=%.0f\n", ++moveno, score_board(board) - scorepenalty);

        move = get_move(board);
        if(move < 0)
            break;

        newboard = execute_move(move, board);
        if(newboard == board) {
            printf("Illegal move!\n");
            moveno--;
            continue;
        }

        int tile = draw_tile();
        if(tile == 2)
            scorepenalty += 4;
        board = insert_tile_rand(newboard, tile);
    }

    printf("%s\n", BOARDSTR(board, '\n'));
    printf("\nGame over. Your score is %.0f. The highest rank you achieved was %d.\n", score_board(board) - scorepenalty, get_max_rank(board));
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    init_move_tables();
    init_score_tables();

    play_game(find_best_move);
    return 0;
}
