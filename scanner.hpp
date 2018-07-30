#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <cctype>
#include "defs.hpp"

int linecount, charcount;

class Scanner{
	private:
		std::string text, lexeme;
		int index;
		char curtok, prevtok, nexttok;
	
	public:
		Scanner(void){
			text = "";
			index = 3;
			linecount = 1;
			charcount = 1;
		}
		
		Scanner(std::string filename){
			std::ifstream is(filename);
			if(is){
				std::string line;
				text = "";
				linecount = 1;
				charcount = 1;
				while(!is.eof()){
					getline(is, line);
					text += line + "\n";
				}
				index = 3;
				is.close();
				advance();
				//curtok = text.at(index++);
			}
			else{
				text = filename;
				index = 0;
				linecount = 1;
				charcount = 1;
				advance();
				//std::cout << "Scan failed. Source file could not be opened." << std::endl;
			}
		}
		
		
		bool advance(void){
			if(index >= text.size())
				return false;
			prevtok = curtok;
			curtok = text.at(index++);
			
			if(index < text.size())
				nexttok = text.at(index);
			/*
			if(curtok == '\n'){
				linecount++;
				charcount = 1;
			}
			else charcount++;
			*/
			
			return true;
		}
		
		
		void pushback(void){
			index--;
			curtok = text.at(index-1);
			prevtok = text.at(index-2);
			nexttok = text.at(index);
		}
		
		
		std::string getlexeme(void){
			return lexeme;
		}
		
		
		int getpos(void){
			return index;
		}
		
		
		void setpos(int idx){
			index = idx;
		}
		
		
		int getfilesize(void){
			return text.length();
		}
		
		
		Tokens scan(void){
			if(index >= text.size())
				return Tokens::done;
			
			if(isdigit(curtok) || curtok == '.'){
				int ndots = 0;
				if(curtok == '.') ndots++;
				
				lexeme = "";
				while(isdigit(curtok) || curtok == '.'){
					if(curtok == '.'){
						ndots++;
						if(ndots > 1){
							pushback();
							break;
						}
					}
					lexeme += curtok;
					advance();
				}
				return Tokens::number;
			}
			else if(curtok == '"'){
				advance();
				lexeme = "";
				while(true){
					if(curtok == '\\' && nexttok == '"'){
						advance();
						lexeme += curtok;
						advance();
						continue;
					}
					else if(curtok == '"')
						break;
					lexeme += curtok;
					advance();
				}
				advance();
				return Tokens::string;
			}
			else if(curtok == '\''){
				advance();
				if(curtok == '\\' && nexttok == '\''){
					advance();
					if(nexttok != '\''){
						lexeme = "";
						advance();
						return Tokens::none;
					}
					else{
						advance();
						lexeme = curtok;
						advance();
						return Tokens::character;
					}
				}
				else if(curtok != '\'' && nexttok == '\''){
					lexeme = curtok;
					advance();	// move on to closing single quote
					advance();	// and advance beyond it to next token
					return Tokens::character;
				}
				else if(curtok != '\''){
					lexeme = "";
					advance();
					return Tokens::none;
				}
			}
			else if(isalpha(curtok) || curtok == '_'){
				lexeme = "";
				while(isalnum(curtok) || curtok == '_'){
					lexeme += curtok;
					advance();
				}
				return Tokens::cstring;
			}
			else if(curtok == '\n'){
				advance();
				lexeme = "";
				return Tokens::newline;
			}
			else if(curtok == '/'){
				lexeme = "";
				if(nexttok == '/'){
					while(curtok != '\n')
						advance();
					linecount++;
					charcount = 1;
					advance();
					return scan();
				}
				else if(nexttok == '*'){
					while(advance()){
						if(curtok == '*' && nexttok == '/'){
							advance();
							advance();
							break;
						}
						
						else if(curtok == '\n'){
							linecount++;
							charcount = 1;
							//advance();
						}
					}
					
					return scan();
				}
				else{
					lexeme = "/";
					advance();
					return Tokens::mathop;
				}
			}
			else if(curtok == '='){
				lexeme = "=";
				if(nexttok == '='){
					advance(); // next '='
					advance(); // point after '='
					lexeme += "=";
					return Tokens::comparison;
				}
				advance();
				return Tokens::assignment;
			}
			else if(curtok == ';'){
				lexeme = ";";
				advance();
				return Tokens::semicolon;
			}
			else if(isspace(curtok)){
				lexeme = "";
				advance();
				return scan();
			}
			else if(curtok == '>'){
				if(nexttok == '='){
					advance();
					lexeme = ">=";
				}
				else{
					lexeme = ">";
				}
				advance();
				return Tokens::comparison;
			}
			else if(curtok == '<'){
				if(nexttok == '='){
					advance();
					lexeme = "<=";
				}
				else{
					lexeme = "<";
				}
				advance();
				return Tokens::comparison;
			}
			else if(curtok == '!'){
				if(nexttok == '='){
					advance();
					lexeme = "!=";
				}
				else{
					lexeme = "!";
					advance();
					return Tokens::invert;
				}
				advance();
				return Tokens::comparison;
			}
			else if(curtok == '('){
				lexeme = '(';
				advance();
				return Tokens::o_bracket;
			}
			else if(curtok == '$'){
				advance();
				lexeme = "$";
				return Tokens::seq_end;
			}
			else if(curtok == '@'){
				advance();
				lexeme = "@";
				return Tokens::subfunc;
			}
			else if(curtok == ')'){
				lexeme = ')';
				advance();
				return Tokens::c_bracket;
			}
			else if(curtok == ','){
				lexeme = ',';
				advance();
				return Tokens::comma;
			}
			else if(curtok == '&' && nexttok == '&'){
				lexeme = "&&";
				advance();
				advance();
				return Tokens::comparison;
			}
			else if(curtok == '|' && nexttok == '|'){
				lexeme = "||";
				advance();
				advance();
				return Tokens::comparison;
			}
			else if(curtok == '{'){
				lexeme = '{';
				advance();
				return Tokens::o_brace;
			}
			else if(curtok == '}'){
				lexeme = '}';
				advance();
				return Tokens::c_brace;
			}
			else if(curtok == '+' || curtok == '-' || curtok == '*' || curtok == '%'){
				lexeme = curtok;
				advance();
				return Tokens::mathop;
			}
			return Tokens::done;
		}
};

#endif
