#include <stdlib.h>
#include "platdefs.h"

/* The fundamental trick: the 4x4 board is represented as a 64-bit word,
 * 기본적 요령 : 4X4 보드는 64비트 단어로 표현됩니다.
 * with each board square packed into a single 4-bit nibble.
 * 각각의 보드는 하나의 4비트 니블로 포장됩니다.
 *
 * The maximum possible board value that can be supported is 32768 (2^15), but
 * 지원 가능한 최대 가능 보드 값은 32768이지만,
 * this is a minor limitation as achieving 65536 is highly unlikely under normal circumstances.
 * 일반적인 상황에서 66536을 달성하는 것은 매우 희소하므로 이것은 매우 사소한 한계이다. (최대 가능 보드 값이 32768인 것.)

 * The space and computation savings from using this representation should be significant.
 * 이 표현을 사용함으로써 절약되는 공간과 계산은 의미있어야한다.
 * The nibble shift can be computed as (r,c) -> shift (4*r + c). That is, (0,0) is the LSB.
 * 니블 이동은 (r,c) -> shift (4*r + c)로 계산될 수 있다. 즉, (0,0)은 LSB이다.

 * LSB : 최하위 비트. 이진 정수에서 짝수인지 홀수인지를 결정하는 단위값이 되는 비트.
 */

typedef uint64_t board_t;
typedef uint16_t row_t;

//store the depth at which the heuristic was recorded as well as the actual heuristic
// 휴리스틱이 기록된 깊이는 물론 실제 휴리스틱 또한 저장한다.
struct trans_table_entry_t{
    uint8_t depth;
    float heuristic;
};

static const board_t ROW_MASK = 0xFFFFULL;
static const board_t COL_MASK = 0x000F000F000F000FULL;

static inline void print_board(board_t board) {
    int i,j;
    for(i=0; i<4; i++) {
        for(j=0; j<4; j++) {
            uint8_t powerVal = (board) & 0xf;
            printf("%6u", (powerVal == 0) ? 0 : 1 << powerVal);
            board >>= 4;
        }
        printf("\n");
    }
    printf("\n");
}

static inline board_t unpack_col(row_t row) {
    board_t tmp = row;
    return (tmp | (tmp << 12ULL) | (tmp << 24ULL) | (tmp << 36ULL)) & COL_MASK;
}

static inline row_t reverse_row(row_t row) {
    return (row >> 12) | ((row >> 4) & 0x00F0)  | ((row << 4) & 0x0F00) | (row << 12);
}

/* Functions 함수*/
#ifdef __cplusplus
extern "C" {
#endif

DLL_PUBLIC void init_tables();

typedef int (*get_move_func_t)(board_t);
DLL_PUBLIC float score_toplevel_move(board_t board, int move);
DLL_PUBLIC int find_best_move(board_t board);
DLL_PUBLIC int ask_for_move(board_t board);
DLL_PUBLIC void play_game(get_move_func_t get_move);

#ifdef __cplusplus
}
#endif
