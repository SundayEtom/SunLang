#ifndef DEFS_HPP
#define DEFS_HPP

enum class Tokens{
	none=-1, done, boolean, character, array, number, string, cstring,
	comment, comma, mathop, space, o_brace, c_brace, o_bracket, c_bracket,
	declaration, assignment, invert, comparison, print, if_cond, else_cond, 
	read, ret, semicolon, identifier, newline, typeof, seq_end, 
	condition, loop, obsize, until, brk_loop, cont_loop, return_value,
	subfunc
};

#endif
