#include <unordered_map>
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

/* The fundamental trick: the 4x4 board is represented as a 64-bit word,
 * with each board square packed into a single 4-bit nibble.
 *
 * The maximum possible board value that can be supported is 32768 (2^15), but
 * this is a minor limitation as achieving 65536 is highly unlikely under normal circumstances.
 *
 * The space and computation savings from using this representation should be significant.
 *
 * The nibble shift can be computed as (r,c) -> shift (4*r + c). That is, (0,0) is the LSB.
 */
typedef uint64_t board_t;
typedef uint16_t row_t;
static const board_t ROW_MASK = 0xFFFFULL;

static void print_board(board_t board)
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            putchar("0123456789abcdef"[board & 0xf]);
            board >>= 4;
        }
        putchar('\n');
    }
    putchar('\n');
}

// Transpose rows/columns in a board:
//   0123       048c
//   4567  -->  159d
//   89ab       26ae
//   cdef       37bf
static board_t transpose(board_t x)
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

// Reverse the rows in a board:
//   0123       3210
//   4567  -->  7654
//   89ab       ba98
//   cdef       fedc
static board_t reverse_rows(board_t board)
{
    return ((board & 0xF000F000F000F000ULL) >> 12) |
           ((board & 0x0F000F000F000F00ULL) >>  4) |
           ((board & 0x00F000F000F000F0ULL) <<  4) |
           ((board & 0x000F000F000F000FULL) << 12);
}

// Reverse the columns in a board:
//   0123       cdef
//   4567  -->  89ab
//   89ab       4567
//   cdef       0123
static board_t reverse_cols(board_t board)
{
    return ((board                        ) >> 48) |
           ((board & 0x0000FFFF00000000ULL) >> 16) |
           ((board & 0x00000000FFFF0000ULL) << 16) |
           ((board                        ) << 48);
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

// Move left table. Each row is mapped to (oldrow ^ newrow). Thus, the value is
// 0 if there is no move, and otherwise equals a value that can easily be
// xor'ed into the current board state to update the board.
static row_t move_left_table[65536];

// Table to speedup heuristic-score calculation.
//   This lookup table contains 65536 entries and we want a 'float' result, so
//   the obvious choice would be 'float table[65536]'. Though it turns out
//   there are only very few unique values in this table. So we can save quite
//   some memory by storing an (8-bit) index into a 2nd table that contains the
//   actual float value.
static const int MAX_HEUR_IDX = 100; // actually ATM 23 would be enough
static uint8_t heur_idx[65536]; // points to a value in heur_scores[]
static float heur_scores[MAX_HEUR_IDX];

// Table to speedup score calculation.
//   Similar to above: we cut the memory requirements (almost) in half by
//   storing a (16-bit) index to a 2nd table.
static const int MAX_SCORE_IDX = 3000; // actually ATM 2875 would be enough
static uint16_t score_idx[65536]; // points to a value in score_table[]
static int score_table[MAX_SCORE_IDX];

static void init_tables()
{
    int score_cnt = 0;
    int heur_cnt = 0;
    for (unsigned row = 0; row < 65536; ++row) {
        unsigned line[4] = {
                (row >>  0) & 0xf,
                (row >>  4) & 0xf,
                (row >>  8) & 0xf,
                (row >> 12) & 0xf
        };

        float heur_score = 100000.0f / (4 + 4); // constant term spread over 4+4 rows/columns
        int score = 0;
        for (int i = 0; i < 4; ++i) {
            int rank = line[i];
            if (rank == 0) {
                heur_score += 10000.0f;
            } else if (rank >= 2) {
                // the score is the total sum of the tile and all intermediate merged tiles
                score += (rank - 1) * (1 << rank);
            }
        }
        // store 'score' into 'score_idx' (and possibly update 'score_table')
        int sidx = std::find(score_table, score_table + score_cnt, score) - score_table;
        if (sidx == score_cnt) {
            score_table[sidx] = score;
            ++score_cnt;
        }
        assert(sidx < MAX_SCORE_IDX);
        score_idx[row] = sidx;

        int maxi = 0;
        for (int i = 1; i < 4; ++i) {
            if (line[i] > line[maxi]) maxi = i;
        }

        if (maxi == 0 || maxi == 3) heur_score += 20000.0f;

        // Check if maxi's are close to each other, and of diff ranks (eg 128 256)
        for (int i = 1; i < 4; ++i) {
            if ((line[i] == line[i - 1] + 1) || (line[i] == line[i - 1] - 1)) heur_score += 1000.0f;
        }

        // Check if the values are ordered:
        if ((line[0] < line[1]) && (line[1] < line[2]) && (line[2] < line[3])) heur_score += 10000.0f;
        if ((line[0] > line[1]) && (line[1] > line[2]) && (line[2] > line[3])) heur_score += 10000.0f;

        // store 'heur_score' into 'heur_idx' (and possibly update 'heur_scores')
        int hidx = std::find(heur_scores, heur_scores + heur_cnt, heur_score) - heur_scores;
        if (hidx == heur_cnt) {
            heur_scores[hidx] = heur_score;
            ++heur_cnt;
        }
        assert(hidx < MAX_HEUR_IDX);
        heur_idx[row] = hidx;

        // execute a move to the left
        for (int i = 0; i < 3; ++i) {
            int j;
            for (j = i + 1; j < 4; ++j) {
                if (line[j] != 0) break;
            }
            if (j == 4) break; // no more tiles to the right

            if (line[i] == 0) {
                line[i] = line[j];
                line[j] = 0;
                i--; // retry this entry
            } else if (line[i] == line[j] && line[i] != 0xf) {
                line[i]++;
                line[j] = 0;
            }
        }
        row_t result = (line[0] <<  0) |
                       (line[1] <<  4) |
                       (line[2] <<  8) |
                       (line[3] << 12);
        move_left_table[row] = row ^ result;
    }
}

static board_t execute_move_left(board_t board)
{
    board_t ret = board;
    ret ^= board_t(move_left_table[(board >>  0) & ROW_MASK]) <<  0;
    ret ^= board_t(move_left_table[(board >> 16) & ROW_MASK]) << 16;
    ret ^= board_t(move_left_table[(board >> 32) & ROW_MASK]) << 32;
    ret ^= board_t(move_left_table[(board >> 48) & ROW_MASK]) << 48;
    return ret;
}
static board_t execute_move_right(board_t board)
{
    return reverse_rows(execute_move_left(reverse_rows(board)));
}
static board_t execute_move_up(board_t board)
{
    return transpose(execute_move_left(transpose(board)));
}
static board_t execute_move_down(board_t board)
{
    // transpose(reverse_rows(..)) is equivalent to reverse_cols(transpose(..))
    // but the latter generates faster code. And similarly
    // reverse_rows(transpose(..)) is equivalent to transpose(reverse_cols(..))
    //return transpose(execute_move_right(transpose(board)));
    //return transpose(reverse_rows(execute_move_left(reverse_rows(transpose(board)))));
    return reverse_cols(transpose(execute_move_left(transpose(reverse_cols(board)))));
}

static int get_max_rank(board_t board)
{
    int maxrank = 0;
    while (board) {
        maxrank = std::max(maxrank, int(board & 0xf));
        board >>= 4;
    }
    return maxrank;
}

static float score_helper(board_t board)
{
    return heur_scores[heur_idx[(board >>  0) & ROW_MASK]] +
           heur_scores[heur_idx[(board >> 16) & ROW_MASK]] +
           heur_scores[heur_idx[(board >> 32) & ROW_MASK]] +
           heur_scores[heur_idx[(board >> 48) & ROW_MASK]];
}

// score a single board heuristically
static float score_heur_board(board_t board)
{
    return score_helper(board) + score_helper(transpose(board));
}

// score a single board actually (adding in the score from spawned 4 tiles)
static int score_board(board_t board)
{
    return score_table[score_idx[(board >>  0) & ROW_MASK]] +
           score_table[score_idx[(board >> 16) & ROW_MASK]] +
           score_table[score_idx[(board >> 32) & ROW_MASK]] +
           score_table[score_idx[(board >> 48) & ROW_MASK]];
}

struct eval_state
{
    std::unordered_map<board_t, float> trans_table; // transposition table, to cache previously-seen moves
    float cprob_thresh;
    int curdepth;
};

static float score_move_node(eval_state& state, board_t board, float cprob);

// score over all possible tile choices and placements
static float score_tilechoose_node(eval_state& state, board_t board, float cprob)
{
    int num_open = count_empty(board);
    cprob /= num_open;

    float res = 0.0f;
    board_t tmp = board;
    board_t tile_2 = 1;
    while (tile_2) {
        if ((tmp & 0xf) == 0) {
            res += score_move_node(state, board |  tile_2      , cprob * 0.9f) * 0.9f;
            res += score_move_node(state, board | (tile_2 << 1), cprob * 0.1f) * 0.1f;
        }
        tmp >>= 4;
        tile_2 <<= 4;
    }
    return res / num_open;
}

// Statistics and controls
// cprob: cumulative probability
// don't recurse into a node with a cprob less than this threshold
static const float CPROB_THRESH_BASE = 0.0001f;
static const int CACHE_DEPTH_LIMIT  = 6;
static const int SEARCH_DEPTH_LIMIT = 8;

// score over all possible moves
static float score_move_node(eval_state& state, board_t board, float cprob)
{
    if (cprob < state.cprob_thresh || state.curdepth >= SEARCH_DEPTH_LIMIT) {
        return score_heur_board(board);
    }

    if (state.curdepth < CACHE_DEPTH_LIMIT) {
        auto it = state.trans_table.find(board);
        if (it != state.trans_table.end()) return it->second;
    }

    float best = 0.0f;

    state.curdepth++;
    board_t up    = execute_move_up   (board);
    if (board != up   ) best = std::max(best, score_tilechoose_node(state, up,    cprob));
    board_t down  = execute_move_down (board);
    if (board != down ) best = std::max(best, score_tilechoose_node(state, down,  cprob));
    board_t left  = execute_move_left (board);
    if (board != left ) best = std::max(best, score_tilechoose_node(state, left,  cprob));
    board_t right = execute_move_right(board);
    if (board != right) best = std::max(best, score_tilechoose_node(state, right, cprob));
    state.curdepth--;

    if (state.curdepth < CACHE_DEPTH_LIMIT) {
        state.trans_table[board] = best;
    }
    return best;
}

static float score_toplevel_move(board_t board)
{
    eval_state state;
    state.cprob_thresh = CPROB_THRESH_BASE;
    state.curdepth = 0;
    return score_tilechoose_node(state, board, 1.0f);
}

// Execute the best move for a given board.
static board_t do_best_move(board_t board)
{
    float bestScore = -1.0f;
    board_t bestmove = board;

    board_t up = execute_move_up(board);
    if (board != up) {
        float scoreUp = score_toplevel_move(up);
        bestScore = scoreUp;
        bestmove = up;
    }
    board_t down = execute_move_down(board);
    if (board != down) {
        float scoreDown = score_toplevel_move(down);
        if (scoreDown > bestScore) {
            bestScore = scoreDown;
            bestmove = down;
        }
    }
    board_t left = execute_move_left(board);
    if (board != left) {
        float scoreLeft = score_toplevel_move(left);
        if (scoreLeft > bestScore) {
            bestScore = scoreLeft;
            bestmove = left;
        }
    }
    board_t right = execute_move_right(board);
    if (board != right) {
        float scoreRight = score_toplevel_move(right);
        if (scoreRight > bestScore) {
            bestScore = scoreRight;
            bestmove = right;
        }
    }

    return bestmove;
}

static unsigned unif_random(unsigned n)
{
    return rand() % n;
}

static board_t draw_tile()
{
    return (unif_random(10) < 9) ? 1 : 2;
}

static board_t insert_tile_rand(board_t board, board_t tile)
{
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

static board_t initial_board()
{
    board_t board = draw_tile() << (4 * unif_random(16));
    return insert_tile_rand(board, draw_tile());
}

static void play_game()
{
    board_t board = initial_board();
    int moveno = 0;
    int scorepenalty = 0; // "penalty" for obtaining free 4 tiles

    while (true) {
        printf("Move #%d, current score=%d\n", ++moveno, score_board(board) - scorepenalty);
        print_board(board);

        board_t newboard = do_best_move(board);
        if (newboard == board) break; // no more legal moves

        board_t tile = draw_tile();
        if (tile == 2) scorepenalty += 4;
        board = insert_tile_rand(newboard, tile);
    }

    printf("Game over. The highest rank you achieved was %d.\n", get_max_rank(board));
}

int main()
{
    srand(123); // fixed seed, for benchmarking we want the same sequence
    init_tables();
    play_game();
}
