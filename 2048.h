#include <stdlib.h>
#include "platdefs.h"

/* The fundamental trick: the 4x4 board is represented as a 64-bit word,
 * �⺻�� ��� : 4X4 ����� 64��Ʈ �ܾ�� ǥ���˴ϴ�.
 * with each board square packed into a single 4-bit nibble.
 * ������ ����� �ϳ��� 4��Ʈ �Ϻ�� ����˴ϴ�.
 *
 * The maximum possible board value that can be supported is 32768 (2^15), but
 * ���� ������ �ִ� ���� ���� ���� 32768������,
 * this is a minor limitation as achieving 65536 is highly unlikely under normal circumstances.
 * �Ϲ����� ��Ȳ���� 66536�� �޼��ϴ� ���� �ſ� ����ϹǷ� �̰��� �ſ� ����� �Ѱ��̴�. (�ִ� ���� ���� ���� 32768�� ��.)

 * The space and computation savings from using this representation should be significant.
 * �� ǥ���� ��������ν� ����Ǵ� ������ ����� �ǹ��־���Ѵ�.
 * The nibble shift can be computed as (r,c) -> shift (4*r + c). That is, (0,0) is the LSB.
 * �Ϻ� �̵��� (r,c) -> shift (4*r + c)�� ���� �� �ִ�. ��, (0,0)�� LSB�̴�.

 * LSB : ������ ��Ʈ. ���� �������� ¦������ Ȧ�������� �����ϴ� �������� �Ǵ� ��Ʈ.
 */

typedef uint64_t board_t;
typedef uint16_t row_t;

//store the depth at which the heuristic was recorded as well as the actual heuristic
// �޸���ƽ�� ��ϵ� ���̴� ���� ���� �޸���ƽ ���� �����Ѵ�.
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

/* Functions �Լ�*/
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
