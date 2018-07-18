#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <sstream>
#include <cstdlib>
#include "scanner.hpp"
#include "value.hpp"
#include "symtab.hpp"


struct History{
	std::string lexeme;
	Tokens token;
	Tokens operation;
	Symbol symbol;
	int lineno, charno;
	
	History(void){ 
		operation = Tokens::none;
		lineno = 1;
		charno = 1;
	}
};

std::string typespecs[] = { "num", "string", "char", "array", "function", "bool" };
std::string keywords [] = 
{ 
	"print", "println", "return", "typeof", "read", "if", 
	"else", "sizeof" 
};

class Parser{
	private:
		Scanner scanner;
		History current, previous;
	
	public:
		Parser(void){}
		Parser(std::string filename){
			scanner = Scanner(filename);
			linecount = 1;
			
			while(advance()){
				switch(current.token){
					case Tokens::cstring:{
						if(is_typespec(current.lexeme)){
							current.operation = Tokens::declaration;
							declaration();
						}
						else if(is_keyword(current.lexeme)){
							keyword();
						}
						else if(is_identifier(current.lexeme)){
							Symbol sym = environment.getsymbol(current.lexeme);
							if(sym.isfunction()){
								environment.run(current.lexeme);
							}
							current.symbol = sym;
							current.operation = Tokens::identifier;
							//std::cout << "Identifier: " << current.lexeme << std::endl;
						}
						else{
							if(previous.operation == Tokens::declaration ||
								previous.operation == Tokens::assignment ||
								previous.operation == Tokens::identifier
							){
								if(is_unknown(current.lexeme)){
									Symbol s = previous.symbol;
									Symbol sym(current.lexeme, s.gettype(), s.isfunction());
									environment.addsymbol(sym);
									current.symbol = sym;
									current.operation = Tokens::declaration;
								}
							}
							else{
								std::cout << "Undeclared name: '" << current.lexeme << "'" << std::endl;
							}
						}
					}break;
					
					case Tokens::assignment:{
						assignment();
					}break;
					
					default: break;
				}
			}
		}
		
	
	private:
		bool match(Tokens tok){
			if(current.token == tok)
				return true;
			return false;
		}
		
		
		void terminate(void){
			exit(EXIT_FAILURE);
		}
		
		
		bool advance(void){
			previous = current;
			current.token = scanner.scan();
			if(match(Tokens::done))
				return false;
			//else if(match(Tokens::newline)){
				current.lineno = linecount;
				current.charno = charcount;
				//advance();
			//}
			
			current.lexeme = scanner.getlexeme();
			return true;
		}
		
		
		void pushback(void){
			current = previous;
		}
		
		
		bool is_typespec(std::string lex){
			for(std::string s : typespecs)
				if(s == lex)
					return true;
			return false;
		}
		
		
		bool is_keyword(std::string lex){
			for(std::string s : keywords)
				if(s == lex)
					return true;
			return false;
		}
		
		
		bool is_identifier(std::string lex){
			Symbol s = environment.getsymbol(lex);
			//std::cout << "is_identifier(): " << s.getname() << std::endl;
			if(s.gettype() == Tokens::none)
					return false;
			return true;
		}
		
		
		bool is_unknown(std::string lex){
			//std::cout << "Is identifier: " << is_identifier(lex) << std::endl;
			if(!is_typespec(lex) && !is_keyword(lex) && !is_identifier(lex))
				return true;
			return false;
		}
		
		
		Tokens tellvaltype(std::string lex){
					if(lex == "num") 	return Tokens::number;
			else 	if(lex == "string") return Tokens::string;
			else 	if(lex == "char") 	return Tokens::character;
			else 	if(lex == "array") 	return Tokens::array;
			else 	if(lex == "bool") 	return Tokens::boolean;
			
			return Tokens::none;
		}
		
		
		Tokens tellkeyword(std::string lex){
				 if(lex == "print" || lex == "println") return Tokens::print;
			else if(lex == "return") return Tokens::ret;
			else if(lex == "break") return Tokens::brk;
			else if(lex == "typeof") return Tokens::typeof;
			else if(lex == "read") return Tokens::read;
			else if(lex == "if") return Tokens::if_cond;
			else if(lex == "else") return Tokens::else_cond;
			else if(lex == "sizeof") return Tokens::obsize;
			
			return Tokens::none;
		}
		
		
		
		
		
		Value print(void){
			Value val;
			current.operation = Tokens::print;
			std::string lex = current.lexeme;
			advance();
			
			if(match(Tokens::semicolon) /*&& lex == "println"*/){
				std::cout << "\n";
				return val;
			}
			else{
				while(!match(Tokens::semicolon)){
					val = expression();
					if(lex == "println")
						std::cout << val.get() << std::endl;
					else if(lex == "print")
						std::cout << val.get() << " ";
					//advance();
				}
			}
			return val;
		}
		
		
		
		Value typeof(void){
			Value val;
		
			advance();
			val = expression();
			Tokens type = val.gettype();
			val.settype(Tokens::string);
						
			switch(type){
				case Tokens::string: 	val.set("string"); break;
				case Tokens::number: 	val.set("num"); break;
				case Tokens::character: val.set("char"); break;
				case Tokens::array: 	val.set("array"); break;
				default: 				val.set("Unknown type"); break;
			}
			return val;
		}
		
		
		
		Value if_condition(void){
			current.operation = Tokens::condition;
			advance();
			Value val;
			
			while(!match(Tokens::semicolon))
				val = expression();
			if(match(Tokens::semicolon))
				advance();
			
			if(val.get() != "" || val.tonumber() != 0){
				if(match(Tokens::o_brace)){
					advance();
					while(!match(Tokens::c_brace)){
						val = expression();
						if(match(Tokens::semicolon))
							advance();
					}
					advance();
					advance();
					
					if(current.lexeme == "else"){
						while(current.lexeme == "else"){
							advance();
							if(match(Tokens::o_brace))
								while(!match(Tokens::c_brace))
									advance();
							else if(match(Tokens::if_cond)){
								return if_condition();
							}
						}
					}
					return val;
				}
			}
			else{
				if(match(Tokens::o_brace)){
					while(!match(Tokens::c_brace))
						advance();
				}
				else{
					while(!match(Tokens::semicolon))
						advance();
				}
			}
			return val;
		}
		
		
		
		Value obsize(void){
			Value val;
			advance();
			if(is_identifier(current.lexeme)){
				Symbol sym = environment.getsymbol(current.lexeme);
				if(sym.gettype() == Tokens::array){
					advance();
					if(match(Tokens::o_bracket)){
						advance();
						Value v = expression();
						if(!match(Tokens::c_bracket))
							std::cout << "Incomplete array indexing.\n";
						else advance();
								
						int index = (int)v.tonumber();
						val = sym.getobjvalue(index);
								
						switch(val.gettype()){
							case Tokens::string:{
								val.settype(Tokens::number);
								val.set(val.tostring(val.get().length()));
							}break;
									
							case Tokens::number:{
								val.set(val.tostring(sizeof(val.tonumber())));
							}break;
									
							case Tokens::character:{
								val.settype(Tokens::number);
								val.set(val.tostring(sizeof(val.get().at(0))));
							}break;
									
							default: break;
						}
					}
					else{
						val.settype(Tokens::number);
						val.set(val.tostring(sym.countels()));
					}
							
					return val;
				}
				else{
					if(sym.gettype() == Tokens::string){
						val.settype(Tokens::number);
						val.set(val.tostring(sym.getvalue().length()));
					}
					else if(sym.gettype() == Tokens::number){
						val.settype(Tokens::number);
						val.set(val.tostring(sizeof(val.tonumber())));
					}
					else if(sym.gettype() == Tokens::character){
						val.settype(Tokens::number);
						val.set(val.tostring(sizeof(val.get().at(0))));
					}
				}
				advance();
				return val;
			}
			return val;
		}
		
		
		
		
		Value declaration(void){
			Value val;
			if(advance()){
				bool isfunc = false;
				std::string prevlex = previous.lexeme;
				
				if(current.lexeme == "_f"){
					isfunc = true;
					//std::cout << "Function declaration.\n";
					advance();
				}
				if(match(Tokens::cstring) && is_unknown(current.lexeme)){
					Tokens type = tellvaltype(prevlex);
					
					Symbol sym(current.lexeme, type, isfunc);
					environment.addsymbol(sym);
					current.symbol = sym;
					current.operation = Tokens::declaration;
					advance();
					if(match(Tokens::assignment))
						val = assignment();
				}
				else{
					std::cout << "Redeclaration of '" << current.lexeme << "' on line " << current.lineno << "." << std::endl;
				}
			}
			return val;
		}
		
		
		Value assignment(void){
			Value val;
			
			if(previous.symbol.getname() != ""){
				Symbol s = environment.getsymbol(previous.symbol.getname());
				Tokens type = s.gettype();
				
				advance();
				if(s.isarray()){
					while(current.token != Tokens::semicolon){
						val = expression();
						s.setvalue(val);
					}
				}
				else if(s.isfunction()){
					std::string fbody = "";
					if(match(Tokens::o_brace)){
						advance();
						while(!match(Tokens::c_brace)){
							if(match(Tokens::string))
								fbody += "\"" + current.lexeme + "\" ";
							else if(match(Tokens::character))
								fbody += "'" + current.lexeme + "' ";
							else
								fbody += current.lexeme + " ";
							advance();
						}
					}
					val.set(fbody);
					s.setvalue(val);
					std::cout << "assignment(): " << s.getvalue() << "\n";
					// function assignment
				}
				else{
					val = expression();
					s.setvalue(val);
				}
				environment.updatesymbol(s);
				current.operation = Tokens::assignment;
				current.symbol = s;
							
				return val;
				//std::cout << "Assignment to '" << previous.symbol.getname() << "'" << std::endl;
			}
			else{
				std::cout << "No symbol to assign to.\n";
			}
			return val;
		}
		
		
		Value keyword(void){
			Tokens kwd = tellkeyword(current.lexeme);
			Value val;
			
			switch(kwd){
				case Tokens::print:{
					return print();
				}break;
				
				case Tokens::typeof:{
					return typeof();
				}break;
				
				case Tokens::obsize:{
					return obsize();
				}break;
				
				case Tokens::if_cond:{
					return if_condition();
				}break;
				
				case Tokens::read:{
					advance();
					// current.lexeme should now be 'std' or 'file'
					std::string src = current.lexeme;
					
					advance();
					// current.lexeme should contain a type specifier
					Tokens inptype;
					if(is_typespec(current.lexeme)){
						inptype = tellvaltype(current.lexeme);
						advance();
					}
					// or a string holding a prompt message or file name
					else inptype = Tokens::string;
					
					std::string msg = "";
					if(match(Tokens::string)){
						msg = current.lexeme;
					}
					
					std::string str = "";
					if(src == "std"){
						std::cout << msg << " ";
						getline(cin, str);
					}
					else if(src == "file"){
						ifstream ifs(msg);
						if(ifs){
							std::string l;
							while(!ifs.eof()){
								getline(ifs, l);
								str += l + "\n";
							}
							ifs.close();
						}
					}
					val.set(str);
					
					advance();
				}break;
				
				default: break;
			}
					
			return val;
		}
		
		
		Value factor(void){
			Value val;
			
			if(match(Tokens::number) || match(Tokens::string) || match(Tokens::character)){
				val.set(current.lexeme);
				val.settype(current.token);
			}
			else if(match(Tokens::cstring) && is_identifier(current.lexeme)){
				Symbol sym = environment.getsymbol(current.lexeme);
				if(sym.gettype() == Tokens::array){
					advance();
					if(match(Tokens::o_bracket)){
						advance();
						Value v = expression();
						int index = (int)v.tonumber();
				
						if(!match(Tokens::c_bracket)){
							std::cout << "Incomplete array indexing\n";
							return val;
						}
						else{
							val = sym.getobjvalue(index);
							advance();
							return val;
						}
					}
					else{
						val.settype(Tokens::array);
						val.set("array");
						return val;
					}
				}
				else{
					if(sym.isfunction()){
						environment.run(current.lexeme);
					}
					val = sym.getobjvalue();
					current.symbol = sym;
					current.operation = Tokens::identifier;
					advance();
				
					return val;
				}
			}
			else if(match(Tokens::o_bracket)){
				advance();
				val = expression();
				if(!match(Tokens::c_bracket) && !match(Tokens::semicolon)){
					std::cout << "Closing bracket expected but found '" << current.lexeme << "' onine " << current.lineno << "." << std::endl;
				}
				else advance();
			}
			else if(match(Tokens::semicolon)){
				advance();
			}
			else if(is_keyword(current.lexeme)){
				return keyword();
			}
			else if(is_typespec(current.lexeme)){
				current.operation = Tokens::declaration;
				return declaration();
			}
			else if(match(Tokens::assignment)){
				return assignment();
			}
			else if(match(Tokens::seq_end)){
				//advance();
				std::cout << "End of sequence\n";
			}
			else{
				std::cout << "Error: Unexpected token '" << current.lexeme << "' on line " << current.lineno << std::endl;
				terminate();
			}
			
			advance();
			return val;
		}
		
		
		Value term(void){
			Value val = factor();
			
			if(match(Tokens::mathop)){
				while(match(Tokens::mathop)){
					if(current.lexeme == "/"){
						//std::cout << "Division (/)\n";
						advance();
						val /= factor();
					}
					else if(current.lexeme == "*"){
						//std::cout << "Multiplication (*)\n";
						advance();
						val *= factor();
					}
					else break;
				}
			}
			/*
			else if(match(Tokens::comparison)){
				while(match(Tokens::comparison)){
					if(current.lexeme == ">"){
						advance();
						val = (val > factor());
					}
					else if(current.lexeme == "<"){
						advance();
						val = (val < factor());
					}
					else if(current.lexeme == "=="){
						advance();
						val = (val == factor());
					}
					else if(current.lexeme == ">="){
						advance();
						val = (val >= factor());
					}
					else if(current.lexeme == "<="){
						advance();
						val = (val <= factor());
					}
					else if(current.lexeme == "!="){
						advance();
						val = (val != factor());
					}
					else break;
				}
			}*/
			
			return val;
		}
		
		
		Value expression(void){
			Value val = term();
			
			if(match(Tokens::mathop)){
				while(match(Tokens::mathop)){
					if(current.lexeme == "+"){
						//std::cout << "Addition (+)\n";
						advance();
						val += term();
					}
					else if(current.lexeme == "-"){
						//std::cout << "Subtraction (-)\n";
						advance();
						val -= term();
					}
					else break;
				}
			}
			/*
			else if(match(Tokens::comparison)){
				while(match(Tokens::comparison)){
					if(current.lexeme == "&&"){
						advance();
						val = (val && term());
					}
					else if(current.lexeme == "||"){
						advance();
						val = (val || term());
					}
					else break;
				}
			}*/
			
			if(match(Tokens::comparison)){
				while(match(Tokens::comparison)){
					if(current.lexeme == "&&"){
						advance();
						val = (val && expression());
					}
					else if(current.lexeme == "||"){
						advance();
						val = (val || expression());
					}
					else if(current.lexeme == ">"){
						advance();
						val = (val > term());
					}
					else if(current.lexeme == "<"){
						advance();
						val = (val < term());
					}
					else if(current.lexeme == "=="){
						advance();
						val = (val == term());
					}
					else if(current.lexeme == ">="){
						advance();
						val = (val >= term());
					}
					else if(current.lexeme == "<="){
						advance();
						val = (val <= term());
					}
					else if(current.lexeme == "!="){
						advance();
						val = (val != term());
					}
					else break;
				}
			}
			
			return val;
		}
};

#endif
