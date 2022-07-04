#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define U32 unsigned long
#define m_set_bit(word, index) ((word) |= (1UL << (U32)(index)))
#define m_get_bit(word, index) ((word) & (1UL << (U32)(index)))
#define m_pop_bit(word, index) ((word) &= ~(1UL << (U32)(index)))

#define m_ne(index) ((index / 4 & 1) ? index - 4 : index - 3)
#define m_se(index) ((index / 4 & 1) ? index + 4 : index + 5)
#define m_nw(index) ((index / 4 & 1) ? index - 5 : index - 4)
#define m_sw(index) ((index / 4 & 1) ? index + 3 : index + 4)

#define m_2ne(index) ((index) - 7)
#define m_2se(index) ((index) + 9)
#define m_2nw(index) ((index) - 9)
#define m_2sw(index) ((index) + 7)

char* unicode_pieces[] = {"⛂ ", "⛃ ", "⛀ ", "⛁ "};

enum {white_pawn, white_king, black_pawn, black_king};
enum {black, white};

const U32 pawn_push_table[2][32] =
{
    {
        0x30UL, 0x60UL, 0xc0UL, 0x80UL,
        0x100UL, 0x300UL, 0x600UL, 0xc00UL,
        0x3000UL, 0x6000UL, 0xc000UL, 0x8000UL,
        0x10000UL, 0x30000UL, 0x60000UL, 0xc0000UL, 
        0x300000UL, 0x600000UL, 0xc00000UL, 0x800000UL, 
        0x1000000UL, 0x3000000UL, 0x6000000UL, 0xc000000UL, 
        0x30000000UL, 0x60000000UL, 0xc0000000UL, 0x80000000UL, 
        0x0UL, 0x0UL, 0x0UL, 0x0UL,
    },
    {
        0x0UL, 0x0UL, 0x0UL, 0x0UL,
        0x1UL, 0x3UL, 0x6UL, 0xcUL,
        0x30UL, 0x60UL, 0xc0UL, 0x80UL,
        0x100UL, 0x300UL, 0x600UL, 0xc00UL, 
        0x3000UL, 0x6000UL, 0xc000UL, 0x8000UL, 
        0x10000UL, 0x30000UL, 0x60000UL, 0xc0000UL, 
        0x300000UL, 0x600000UL, 0xc00000UL, 0x800000UL, 
        0x1000000UL, 0x3000000UL, 0x6000000UL, 0xc000000UL,
    }
};
const U32 pawn_attack_table[2][32] = 
{
    {
        0x200UL, 0x500UL, 0xa00UL, 0x400UL, 
        0x2000UL, 0x5000UL, 0xa000UL, 0x4000UL, 
        0x20000UL, 0x50000UL, 0xa0000UL, 0x40000UL, 
        0x200000UL, 0x500000UL, 0xa00000UL, 0x400000UL, 
        0x2000000UL, 0x5000000UL, 0xa000000UL, 0x4000000UL, 
        0x20000000UL, 0x50000000UL, 0xa0000000UL, 0x40000000UL, 
        0x0UL, 0x0UL, 0x0UL, 0x0UL, 
        0x0UL, 0x0UL, 0x0UL, 0x0UL,
    },
    {
        0x0UL, 0x0UL, 0x0UL, 0x0UL, 
        0x0UL, 0x0UL, 0x0UL, 0x0UL, 
        0x2UL, 0x5UL, 0xaUL, 0x4UL, 
        0x20UL, 0x50UL, 0xa0UL, 0x40UL, 
        0x200UL, 0x500UL, 0xa00UL, 0x400UL, 
        0x2000UL, 0x5000UL, 0xa000UL, 0x4000UL, 
        0x20000UL, 0x50000UL, 0xa0000UL, 0x40000UL, 
        0x200000UL, 0x500000UL, 0xa00000UL, 0x400000UL,
    }
};
const U32 pawn_occup_table[2][32] = 
{
    {
        0x20UL, 0x60UL, 0xc0UL, 0x80UL, 
        0x100UL, 0x300UL, 0x600UL, 0x400UL, 
        0x2000UL, 0x6000UL, 0xc000UL, 0x8000UL, 
        0x10000UL, 0x30000UL, 0x60000UL, 0x40000UL, 
        0x200000UL, 0x600000UL, 0xc00000UL, 0x800000UL, 
        0x1000000UL, 0x3000000UL, 0x6000000UL, 0x4000000UL, 
        0x0UL, 0x0UL, 0x0UL, 0x0UL, 
        0x0UL, 0x0UL, 0x0UL, 0x0UL, 
    },
    {
        0x0UL, 0x0UL, 0x0UL, 0x0UL, 
        0x0UL, 0x0UL, 0x0UL, 0x0UL, 
        0x20UL, 0x60UL, 0xc0UL, 0x80UL, 
        0x100UL, 0x300UL, 0x600UL, 0x400UL, 
        0x2000UL, 0x6000UL, 0xc000UL, 0x8000UL, 
        0x10000UL, 0x30000UL, 0x60000UL, 0x40000UL, 
        0x200000UL, 0x600000UL, 0xc00000UL, 0x800000UL, 
        0x1000000UL, 0x3000000UL, 0x6000000UL, 0x4000000UL, 
    }
};

typedef struct Position{
    U32 bits[4];
    U32 count_move;
} Position;
typedef struct Move_tab{
    Position positions[64];
    U32 count;
} Move_tab;

static inline U32 get_white(Position* position){
    return position->bits[white_pawn] | position->bits[white_king];
}

static inline U32 get_black(Position* position){
    return position->bits[black_pawn] | position->bits[black_king];
}

static inline U32 get_occup(Position* position){
    return get_white(position) | get_black(position);
}
/*
U32 pawn_occup_table[2][32];
void calculate_pawn_occup_table(){
    for(int i = 0; i < 24; i++){
        if(i % 8 != 3 && i % 8 != 7) m_set_bit(pawn_occup_table[black][i], m_se(i));
        if(i % 8 != 4 && i % 8 != 0) m_set_bit(pawn_occup_table[black][i], m_sw(i));
    }
    for(int i = 8; i < 32; i++){
        if(i % 8 != 4 && i % 8 != 0) m_set_bit(pawn_occup_table[white][i], m_nw(i));
        if(i % 8 != 3 && i % 8 != 7) m_set_bit(pawn_occup_table[white][i], m_ne(i));
    }
}
void print_pawn_occup_table(){
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < 32; j++){
            printf("0x%lxUL, ", pawn_occup_table[i][j]);
        }
        printf("\n");
    }
}
*/
/*
U32 pawn_push_table[2][32];
void calculate_pawn_push_table(){
    for(int i = 0; i < 28; i++){
        if(i % 8 != 3) m_set_bit(pawn_push_table[black][i], m_se(i));
        if(i % 8 != 4) m_set_bit(pawn_push_table[black][i], m_sw(i));
    }
    for(int i = 4; i < 32; i++){
        if(i % 8 != 4) m_set_bit(pawn_push_table[white][i], m_nw(i));
        if(i % 8 != 3) m_set_bit(pawn_push_table[white][i], m_ne(i));
    }
}
void print_pawn_push_table(){
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < 32; j++){
            printf("0x%lxUL, ", pawn_push_table[i][j]);
        }
        printf("\n");
    }
}
*/
/*
U32 pawn_attack_table[2][32];
void calculate_pawn_attack_table(){
    for(int i = 0; i < 24; i++){
        if(i % 8 != 3 && i % 8 != 7) m_set_bit(pawn_attack_table[black][i], m_2se(i));
        if(i % 8 != 4 && i % 8 != 0) m_set_bit(pawn_attack_table[black][i], m_2sw(i));
    }
    for(int i = 8; i < 32; i++){
        if(i % 8 != 4 && i % 8 != 0) m_set_bit(pawn_attack_table[white][i], m_2nw(i));
        if(i % 8 != 3 && i % 8 != 7) m_set_bit(pawn_attack_table[white][i], m_2ne(i));
    }
}
void print_pawn_attack_table(){
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < 32; j++){
            printf("0x%lxUL, ", pawn_attack_table[i][j]);
        }
        printf("\n");
    }
}
*/
void print_bitboard(U32 bits){
    for(int i = 0; i < 8; i++){
        printf("\n %2d  ", i * 4);
        if(!(i & 1)) printf("  ");
        for(int j = 0; j < 4; j++){
            printf("%s", m_get_bit(bits, i*4+j) ? "X " : ". ");
            printf("  ");
        }
    }
    printf("\n\n");
}
void print_position(Position* position){
    for(int i = 0; i < 8; i++){
        printf("\n %2d  ", i * 4);
        if(!(i & 1)) printf("  ");
        for(int j = 0; j < 4; j++){
            printf("%s", m_get_bit(get_occup(position), i*4+j) ? "" : ". ");
            for(int piece = white_pawn; piece <= black_king; piece++)
            {
                printf("%s", m_get_bit(position->bits[piece], i*4+j) ? unicode_pieces[piece] : "");
            }
            printf("  ");
        }
    }
    printf("\n\n Turn: %s\n Move: %lu\n", position->count_move % 2 ? "white" : "black", position->count_move);
}
static inline int get_population_count(U32 bitboard)
{
    int count = 0;
    while (bitboard)
    {
        count++;
        bitboard &= bitboard - 1;
    }
    return count;
}

static inline int get_least_bit_index(U32 bitboard)
{
    if (bitboard)
    {
        return get_population_count((bitboard & -bitboard) - 1);
    }
    else
        return -1;
}
void move_gen(Position* position, Move_tab* move_tab, int side){
    int our_color = (side == white) ? white : black;
    int enemy_color = (side == black) ? white : black;
    int our_pawns = (side == white) ? white_pawn : black_pawn;
    int our_kings = (side == white) ? white_king : black_king;
    int enemy_pawns = (side == black) ? white_pawn : black_pawn;
    int enemy_kings = (side == black) ? white_king : black_king;
    //pawn captures
    U32 population = (side == white) ? position->bits[white_pawn] : position->bits[black_pawn];
    int population_count = get_population_count(population);
    for(int i = 0; i < population_count; i++){
        int source = get_least_bit_index(population);
        m_pop_bit(population, source);
        U32 candidate_moves = pawn_attack_table[our_color][source];
        U32 condition_square = pawn_occup_table[our_color][source];
        int candidate_moves_count = get_population_count(candidate_moves);
        for(int j = 0; j < candidate_moves_count; j++){
            int dest = get_least_bit_index(candidate_moves);
            int occup = get_least_bit_index(condition_square);
            int captured_piece = -1;
            m_pop_bit(candidate_moves, dest);
            m_pop_bit(condition_square, occup);
            if(m_get_bit(get_occup(position), dest)) continue;
            if(m_get_bit(position->bits[enemy_pawns], occup)){
                captured_piece = enemy_pawns;
            }
            else if(m_get_bit(position->bits[enemy_kings], occup)){
                captured_piece = enemy_kings;
            }
            if(captured_piece == -1) continue;
            memcpy(&(move_tab->positions[move_tab->count]), position, sizeof(Position));
            m_pop_bit(move_tab->positions[move_tab->count].bits[our_pawns], source);
            m_pop_bit(move_tab->positions[move_tab->count].bits[captured_piece], occup);
            if(dest < 4 || dest >=28) m_set_bit(move_tab->positions[move_tab->count].bits[our_kings], dest);
            else m_set_bit(move_tab->positions[move_tab->count].bits[our_pawns], dest);
            move_tab->count++;
            move_tab->positions[move_tab->count].count_move++;
            printf("%lu: %dx%d next: %s\n", move_tab->count - 1, source, dest, move_tab->positions[move_tab->count-1].count_move  % 2 ? "white" : "black" );
        }
    }
    if(move_tab->count > 0) return;


    //pawn pushes
    population = (side == white) ? position->bits[white_pawn] : position->bits[black_pawn];
    population_count = get_population_count(population);
    for(int i = 0; i < population_count; i++){
        int source = get_least_bit_index(population);
        m_pop_bit(population, source);
        U32 candidate_moves = pawn_push_table[our_color][source];
        int candidate_moves_count = get_population_count(candidate_moves);
        for(int j = 0; j < candidate_moves_count; j++){
            int dest = get_least_bit_index(candidate_moves);
            if(get_occup(position) & (1UL << dest)) continue;
            m_pop_bit(candidate_moves, dest);
            memcpy(&(move_tab->positions[move_tab->count]), position, sizeof(Position));
            move_tab->positions[move_tab->count].count_move++;
            m_pop_bit(move_tab->positions[move_tab->count].bits[our_pawns], source);
            if(dest < 4 || dest >=28) m_set_bit(move_tab->positions[move_tab->count].bits[our_kings], dest);
            else m_set_bit(move_tab->positions[move_tab->count].bits[our_pawns], dest);
            move_tab->count++;
            printf("%lu: %d-%d next: %s\n", move_tab->count - 1, source, dest, move_tab->positions[move_tab->count-1].count_move  % 2 ? "white" : "black" );
        }
    }
    //king pushes
    population = (side == white) ? position->bits[white_king] : position->bits[black_king];
    population_count = get_population_count(population);
    for(int i = 0; i < population_count; i++){
        int source = get_least_bit_index(population);
        m_pop_bit(population, source);
        U32 candidate_moves = pawn_push_table[our_color][source] | pawn_push_table[enemy_color][source];
        int candidate_moves_count = get_population_count(candidate_moves);
        for(int j = 0; j < candidate_moves_count; j++){
            int dest = get_least_bit_index(candidate_moves);
            if(get_occup(position) & (1UL << dest)) continue;
            m_pop_bit(candidate_moves, dest);
            memcpy(&(move_tab->positions[move_tab->count]), position, sizeof(Position));
            move_tab->positions[move_tab->count].count_move++;
            m_pop_bit(move_tab->positions[move_tab->count].bits[our_kings], source);
            m_set_bit(move_tab->positions[move_tab->count].bits[our_kings], dest);
            move_tab->count++;
            printf("%lu: %d-%d next: %s\n", move_tab->count - 1, source, dest, move_tab->positions[move_tab->count-1].count_move  % 2 ? "white" : "black" );
        }
    }
}
int main(){
    Position position;
    memset(&position, 0, sizeof(position));
    for(int i = 0; i < 12; i++){
        if(i > 3 && i < 8) continue;
        m_set_bit(position.bits[black_pawn], i);
        m_set_bit(position.bits[white_pawn], 31 - i);
    }
    //m_set_bit(position.bits[black_pawn], 13);
    //m_set_bit(position.bits[white_pawn], 14);
    Move_tab moves;
    memset(&moves, 0, sizeof(moves));
    while(1){
        int num = 0;
        moves.count = 0;
        print_position(&position);
        move_gen(&position, &moves, (position.count_move & 1UL));
        /*
        printf("\nXXXXXXXXXXXXXXXXX\n");
        for(U32 i = 0; i < moves.count; i++){
            printf("----%lu----", i);
            print_position(&moves.positions[i]);
        }
        printf("\nXXXXXXXXXXXXXXXXX\n");
        */
        scanf("%d", &num);
        memcpy(&position, &moves.positions[num], sizeof(Position));
    }
    return 0;
}