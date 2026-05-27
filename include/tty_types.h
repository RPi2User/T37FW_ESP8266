/*
 * tty_types.h
 *
 *  Created on: Oct 9, 2025
 *      Author: RPi2User
 */

#include <stdint.h>

#ifndef INC_TTY_TYPES_H_
#define INC_TTY_TYPES_H_

typedef int8_t symbol_t;
typedef int8_t* sbf_t;

#define STR_MAIN_SIZE 256
#define SBF_MAIN_SIZE 300

extern const symbol_t SBF_TERMINATOR;	// shows end of symbol buffer (sbf)
extern char str_main[STR_MAIN_SIZE];
extern symbol_t sbf_main[SBF_MAIN_SIZE];
extern sbf_t currentLine;

typedef enum {
    TTY_LETTERS = 0,
    TTY_FIGURES = 1
} tty_mode_t;

typedef enum {
	LOWERCASE = 0,
	UPPERCASE = 1
} E_lettercase;

typedef struct { //TODO make this uint8_t pls
    int s1;
    int s2;
    int s3;
} Databit;


typedef enum {
	NOT_INIT = -1,
	TO_VER1_0 = 10
} E_InitState;

typedef struct {
	// PIN Definitions
	uint8_t TTY_SEND;
	uint8_t TTY_RECV;
	uint8_t TTY_READ_INHIBIT;

	//idea: uint8_t TTY_INVERT_TX; // This one inverts the OUTPUT 
	// uint8_t TTY_INVERT_RX; // This one inverts the INPUT

	// Serial properties
	float baudrate;
	float stopbit_cnt;
	uint8_t loopback;

	// Typing
	uint8_t linewidth;
	E_lettercase lettercase;
	uint8_t tabWidth;
	tty_mode_t tty_mode;

	// state vars
	uint32_t last_linefeed;		// why not 8bit? -> len(sbf_main) := 300 Byte
	uint32_t carriage_pos;

	// READ-COMPLETE Flag 
	E_InitState loadState;
} Teletype;

typedef enum {
	// Letters
	a = 3,
	b = 25,
	c = 14,
	d = 9,
	e = 1,
	f = 13,
	g = 26,
	h = 20,
	i = 6,
	j = 11,
	k = 15,
	l = 18,
	m = 28,
	n = 12,
	o = 24,
	p = 22,
	q = 23,
	r = 10,
	s = 5,
	t = 16,
	u = 7,
	v = 30,
	w = 19,
	x = 29,
	y = 21,
	z = 17,

	// Figures
	n0 = 22,
	n1 = 23,
	n2 = 19,
	n3 = 1,
	n4 = 10,
	n5 = 16,
	n6 = 21,
	n7 = 7,
	n8 = 6,
	n9 = 24,
	period = 28,
	comma = 12,
	plus = 17,
	minus = 3,
	lparen = 15,
	rparen = 18,
	slash = 29,
	colon = 14,
	quote = 5,
	bell = 11,
	equals = 30,
	question = 25,

	// Common
	space = 4,
	cr = 8,
	lf = 2,
	ltrs = 31,
	figs = 27,
	null = 0,

	// Special symbols, begins @ 0x50 === 80d
	enq	= 0x55,	// werda?
	tab = 0x59,
	ff	= 0x5C,	// form feed -> 10x new line
} E_symbols;



// Conversion LUTs: ------------------------------------------------

#ifdef __cplusplus
#include <array>

static constexpr std::array<char, 32> ltrs_to_char = []() {
	std::array<char, 32> table{};
	table[a] = 'a';
	table[b] = 'b';
	table[c] = 'c';
	table[d] = 'd';
	table[e] = 'e';
	table[f] = 'f';
	table[g] = 'g';
	table[h] = 'h';
	table[i] = 'i';
	table[j] = 'j';
	table[k] = 'k';
	table[l] = 'l';
	table[m] = 'm';
	table[n] = 'n';
	table[o] = 'o';
	table[p] = 'p';
	table[q] = 'q';
	table[r] = 'r';
	table[s] = 's';
	table[t] = 't';
	table[u] = 'u';
	table[v] = 'v';
	table[w] = 'w';
	table[x] = 'x';
	table[y] = 'y';
	table[z] = 'z';
	return table;
}();

static constexpr std::array<char, 32> figs_to_char = []() {
	std::array<char, 32> table{};
	table[n0] = '0';
	table[n1] = '1';
	table[n2] = '2';
	table[n3] = '3';
	table[n4] = '4';
	table[n5] = '5';
	table[n6] = '6';
	table[n7] = '7';
	table[n8] = '8';
	table[n9] = '9';
	table[period] = '.';
	table[comma] = ',';
	table[plus] = '+';
	table[minus] = '-';
	table[lparen] = '(';
	table[rparen] = ')';
	table[slash] = '/';
	table[colon] = ':';
	table[quote] = 0x27;
	table[bell] = 0x07;
	table[equals] = '=';
	table[question] = '?';
	return table;
}();

static constexpr std::array<symbol_t, 128> char_to_symCommon = []() {
	std::array<symbol_t, 128> table{};
	table[0x03] = null;
	table[0x04] = null;
	table[0x05] = enq;
	table[0x07] = bell;
	table[0x09] = tab;
	table[0x0A] = lf;
	table[0x0C] = ff;
	table[0x0D] = cr;
	table[0x20] = space;
	return table;
}();

static constexpr std::array<symbol_t, 128> char_to_symFIGS = []() {
	std::array<symbol_t, 128> table{};
	table[0x07] = bell;
	table[0x27] = quote;
	table['{'] = lparen;
	table['}'] = rparen;
	table['<'] = quote;
	table['>'] = quote;
	table['['] = lparen;
	table[']'] = rparen;
	table['('] = lparen;
	table[')'] = rparen;
	table['+'] = plus;
	table[','] = comma;
	table['-'] = minus;
	table['.'] = period;
	table['/'] = slash;
	table['0'] = n0;
	table['1'] = n1;
	table['2'] = n2;
	table['3'] = n3;
	table['4'] = n4;
	table['5'] = n5;
	table['6'] = n6;
	table['7'] = n7;
	table['8'] = n8;
	table['9'] = n9;
	table[':'] = colon;
	table['='] = equals;
	table['?'] = question;
	return table;
}();

static constexpr std::array<symbol_t, 128> char_to_symLTRS = []() {
	std::array<symbol_t, 128> table{};
	table['A'] = a;
	table['B'] = b;
	table['C'] = c;
	table['D'] = d;
	table['E'] = e;
	table['F'] = f;
	table['G'] = g;
	table['H'] = h;
	table['I'] = i;
	table['J'] = j;
	table['K'] = k;
	table['L'] = l;
	table['M'] = m;
	table['N'] = n;
	table['O'] = o;
	table['P'] = p;
	table['Q'] = q;
	table['R'] = r;
	table['S'] = s;
	table['T'] = t;
	table['U'] = u;
	table['V'] = v;
	table['W'] = w;
	table['X'] = x;
	table['Y'] = y;
	table['Z'] = z;
	return table;
}();

#else

static const char ltrs_to_char[32] = {
	[a] = 'a',
	[b] = 'b',
	[c] = 'c',
	[d] = 'd',
	[e] = 'e',
	[f] = 'f',
	[g] = 'g',
	[h] = 'h',
	[i] = 'i',
	[j] = 'j',
	[k] = 'k',
	[l] = 'l',
	[m] = 'm',
	[n] = 'n',
	[o] = 'o',
	[p] = 'p',
	[q] = 'q',
	[r] = 'r',
	[s] = 's',
	[t] = 't',
	[u] = 'u',
	[v] = 'v',
	[w] = 'w',
	[x] = 'x',
	[y] = 'y',
	[z] = 'z'};

static const char figs_to_char[32] = {
	[n0] = '0',
	[n1] = '1',
	[n2] = '2',
	[n3] = '3',
	[n4] = '4',
	[n5] = '5',
	[n6] = '6',
	[n7] = '7',
	[n8] = '8',
	[n9] = '9',
	[period] = '.',
	[comma] = ',',
	[plus] = '+',
	[minus] = '-',
	[lparen] = '(',
	[rparen] = ')',
	[slash] = '/',
	[colon] = ':',
	[quote] = 0x27,
	[bell] = 0x07,
	[equals] = '=',
	[question] = '?'};

static const symbol_t char_to_symCommon[128] = {
	[0x03] =	null, // strg+c
	[0x04] 	=	null, // strg+d
	[0x05]	=	enq,
	[0x07]	=	bell,
	[0x09]	=	tab,
	[0x0A]	=	lf,
	[0x0C]	=	ff,
	[0x0D]	= 	cr,
	[0x20]	= 	space};

static const symbol_t char_to_symFIGS[128] = {
	[0x07]	=	bell,
	[0x27]	=	quote,
	['{']	=	lparen,
	['}']	=	rparen,
	['<']	=	quote,
	['>']	=	quote,
	['[']	=	lparen,
	[']']	=	rparen,
	['(']	=	lparen,
	[')']	=	rparen,
	['+']	=	plus,
	[',']	=	comma,
	['-']	=	minus,
	['.']	=	period,
	['/']	=	slash,
	['0']	=	n0,
	['1']	=	n1,
	['2']	=	n2,
	['3']	=	n3,
	['4']	=	n4,
	['5']	=	n5,
	['6']	=	n6,
	['7']	=	n7,
	['8']	=	n8,
	['9']	=	n9,
	[':']	=	colon,
	['=']	=	equals,
	['?']	=	question};

static const symbol_t char_to_symLTRS[128] = {
	['A']	=	a,
	['B']	=	b,
	['C']	=	c,
	['D']	=	d,
	['E']	=	e,
	['F']	=	f,
	['G']	=	g,
	['H']	=	h,
	['I']	=	i,
	['J']	=	j,
	['K']	=	k,
	['L']	=	l,
	['M']	=	m,
	['N']	=	n,
	['O']	=	o,
	['P']	=	p,
	['Q']	=	q,
	['R']	=	r,
	['S']	=	s,
	['T']	=	t,
	['U']	=	u,
	['V']	=	v,
	['W']	=	w,
	['X']	=	x,
	['Y']	=	y,
	['Z']	=	z};

#endif

#endif /* INC_TTY_TYPES_H_ */
