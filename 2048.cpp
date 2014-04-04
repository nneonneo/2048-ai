#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unordered_map>

#include "2048.h"

// Transpose rows/columns in a board:
//   0123       048c
//   4567  -->  159d
//   89ab       26ae
//   cdef       37bf
static inline board_t transpose(board_t x)
{
	board_t a1 = x & 0xF0F00F0FF0F00F0FULL;
	board_t a2 = x & 0x0000F0F00000F0F0ULL;
	board_t a3 = x & 0x0F0F00000F0F0000ULL;
	board_t a = a1 | (a2 << 12) | (a3 >> 12);
	board_t b1 = a & 0xFF00FF0000FF00FFULL;
	board_t b2 = a & 0x00FF00FF00000000ULL;
	board_t b3 = a & 0x00000000FF00FF00ULL;
	return b1 | (b2 >> 24) | (b3 << 24);
}

// Count the number of empty positions (= zero nibbles) in a board.
// Precondition: the board cannot be fully empty.
static int count_empty(uint64_t x)
{
	x |= (x >> 2) & 0x3333333333333333ULL;
	x |= (x >> 1);
	x = ~x & 0x1111111111111111ULL;
	// At this point each nibble is:
	//  0 if the original nibble was non-zero
	//  1 if the original nibble was zero
	// Next sum them all
	x += x >> 32;
	x += x >> 16;
	x += x >>  8;
	x += x >>  4; // this can overflow to the next nibble if there were 16 empty positions
	return x & 0xf;
}

/* We can perform state lookups one row at a time by using arrays with 65536 entries. */

/* Move tables. Each row or compressed column is mapped to (oldrow^newrow) assuming row/col 0.
 *
 * Thus, the value is 0 if there is no move, and otherwise equals a value that can easily be
 * xor'ed into the current board state to update the board. */
static row_t row_left_table [65536];
static row_t row_right_table[65536];
static board_t col_up_table[65536];
static board_t col_down_table[65536];

void init_move_tables() {
    for (unsigned row = 0; row < 65536; ++row) {
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

static inline board_t execute_move_0(board_t board) {
    board_t ret = board;
    board_t t = transpose(board);
    ret ^= col_up_table[(t >>  0) & ROW_MASK] <<  0;
    ret ^= col_up_table[(t >> 16) & ROW_MASK] <<  4;
    ret ^= col_up_table[(t >> 32) & ROW_MASK] <<  8;
    ret ^= col_up_table[(t >> 48) & ROW_MASK] << 12;
    return ret;
}

static inline board_t execute_move_1(board_t board) {
    board_t ret = board;
    board_t t = transpose(board);
    ret ^= col_down_table[(t >>  0) & ROW_MASK] <<  0;
    ret ^= col_down_table[(t >> 16) & ROW_MASK] <<  4;
    ret ^= col_down_table[(t >> 32) & ROW_MASK] <<  8;
    ret ^= col_down_table[(t >> 48) & ROW_MASK] << 12;
    return ret;
}

static inline board_t execute_move_2(board_t board) {
    board_t ret = board;
    ret ^= board_t(row_left_table[(board >>  0) & ROW_MASK]) <<  0;
    ret ^= board_t(row_left_table[(board >> 16) & ROW_MASK]) << 16;
    ret ^= board_t(row_left_table[(board >> 32) & ROW_MASK]) << 32;
    ret ^= board_t(row_left_table[(board >> 48) & ROW_MASK]) << 48;
    return ret;
}

static inline board_t execute_move_3(board_t board) {
    board_t ret = board;
    ret ^= board_t(row_right_table[(board >>  0) & ROW_MASK]) <<  0;
    ret ^= board_t(row_right_table[(board >> 16) & ROW_MASK]) << 16;
    ret ^= board_t(row_right_table[(board >> 32) & ROW_MASK]) << 32;
    ret ^= board_t(row_right_table[(board >> 48) & ROW_MASK]) << 48;
    return ret;
}

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
static float line_heur_score_table[65536];
static float row_score_table[65536];

struct eval_state {
    std::unordered_map<board_t, float> trans_table; // transposition table, to cache previously-seen moves
    float cprob_thresh;
    int curdepth;

    eval_state() : cprob_thresh(0), curdepth(0) {}
};

// score a single board heuristically
static float score_heur_board(board_t board);
// score a single board actually (adding in the score from spawned 4 tiles)
static float score_board(board_t board);
// score over all possible moves
static float score_move_node(eval_state &state, board_t board, float cprob);
// score over all possible tile choices and placements
static float score_tilechoose_node(eval_state &state, board_t board, float cprob);

void init_score_tables() {
    for (unsigned row = 0; row < 65536; ++row) {
        unsigned int line[4] = {row & 0xf, (row >> 4) & 0xf, (row >> 8) & 0xf, (row >> 12) & 0xf};
        int i;
        float heur_score = 0;
        float score = 0;

        for(i=0; i<4; i++) {
            int rank = line[i];

            if(rank == 0) {
                heur_score += 10000;
            } else if(rank >= 2) {
                // the score is the total sum of the tile and all intermediate merged tiles
                score += (rank - 1) * (1 << rank);
            }
        }

        int maxi = 0;
        int maxrank = 0;
        for(i=0; i<4; i++) {
            int rank = line[i];

            if(rank > maxrank) {
                maxrank = rank;
                maxi = i;
            }
        }

        if(maxi == 0 || maxi == 3)
            heur_score += 20000;

        // Check if maxis are close to eachother, and of diff ranks (eg 128 256)
        for(i=1; i<4; i++) {
            if ((line[i] == line[i-1] + 1) || (line[i] == line[i-1] - 1)) {
                heur_score += 1000;
            }
        }

        // Check if the values are ordered:
        if ((line[0] < line[1]) && (line[1] < line[2]) && (line[2] < line[3])) heur_score += 10000;
        if ((line[0] > line[1]) && (line[1] > line[2]) && (line[2] > line[3])) heur_score += 10000;

        row_score_table[row] = score;
        line_heur_score_table[row] = heur_score;
    }
}

static float score_helper(board_t board, const float* table) {
    return table[(board >>  0) & ROW_MASK] +
           table[(board >> 16) & ROW_MASK] +
           table[(board >> 32) & ROW_MASK] +
           table[(board >> 48) & ROW_MASK];
}

static float score_heur_board(board_t board) {
    return score_helper(          board , line_heur_score_table) +
           score_helper(transpose(board), line_heur_score_table) +
           100000.0f;
}

static float score_board(board_t board) {
    return score_helper(board, row_score_table);
}

static float score_tilechoose_node(eval_state &state, board_t board, float cprob) {
    int num_open = count_empty(board);
    cprob /= num_open;

    float res = 0.0f;
    for(int i=0; i<16; i++) {
        if(((board >> (4*i)) & 0xf) == 0) {
            res += score_move_node(state, board | (((board_t)1) << (4*i)), cprob * 0.9f) * 0.9f;
            res += score_move_node(state, board | (((board_t)2) << (4*i)), cprob * 0.1f) * 0.1f;
        }
    }
    return res / num_open;
}

// Statistics and controls
// cprob: cumulative probability
// don't recurse into a node with a cprob less than this threshold
static const float CPROB_THRESH_BASE = 0.0001f;
static const int CACHE_DEPTH_LIMIT  = 6;
static const int SEARCH_DEPTH_LIMIT = 8;

static float score_move_node(eval_state &state, board_t board, float cprob) {
    if(cprob < state.cprob_thresh || state.curdepth >= SEARCH_DEPTH_LIMIT) {
        return score_heur_board(board);
    }

    if (state.curdepth < CACHE_DEPTH_LIMIT) {
        auto it = state.trans_table.find(board);
        if (it != state.trans_table.end()) return it->second;
    }

    int move;
    float best = 0;

    state.curdepth++;
    for(move=0; move<4; move++) {
        board_t newboard = execute_move(move, board);
        if (board == newboard) continue;

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

float score_toplevel_move(board_t board, int move) {
    board_t newboard = execute_move(move, board);
    if (board == newboard) return 0.0f;

    eval_state state;
    state.cprob_thresh = CPROB_THRESH_BASE;

    return score_tilechoose_node(state, newboard, 1.0f) + 1e-6f;
}

/* Find the best move for a given board. */
int find_best_move(board_t board) {
    int move;
    float best = 0;
    int bestmove = -1;

    print_board(board);

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

    print_board(board);

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

static unsigned unif_random(unsigned n)
{
    return rand() % n;
}

/* Playing the game */
static board_t draw_tile() {
    return (unif_random(10) < 9) ? 1 : 2;
}

static board_t insert_tile_rand(board_t board, board_t tile) {
    int index = unif_random(count_empty(board));
    board_t tmp = board;
    while (true) {
        while ((tmp & 0xf) != 0) {
            tmp >>= 4;
            tile <<= 4;
        }
        if (index == 0) break;
        --index;
        tmp >>= 4;
        tile <<= 4;
    }
    return board | tile;
}

static board_t initial_board() {
    board_t board = draw_tile() << (4 * unif_random(16));
    return insert_tile_rand(board, draw_tile());
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

        board_t tile = draw_tile();
        if (tile == 2) scorepenalty += 4;
        board = insert_tile_rand(newboard, tile);
    }

    print_board(board);
    printf("\nGame over. Your score is %.0f. The highest rank you achieved was %d.\n", score_board(board) - scorepenalty, get_max_rank(board));
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    srand(123); // fixed seed, for benchmarking we want the same sequence
    init_move_tables();
    init_score_tables();

    play_game(find_best_move);
    return 0;
}
