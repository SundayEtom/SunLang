#ifndef SYMTAB_HPP
#define SYMTAB_HPP

#include <string>
#include <iostream>
#include <vector>
#include "defs.hpp"
#include "value.hpp"
#include "runtime.hpp"

using namespace std;


union SymOptions{
	std::vector<Value> values;
	Value value;
};


class Symbol{
	private:
		string name;
		Tokens type;
		bool isfunc;
		Value value;
		vector<Value> values;
	
	public:
		Symbol(string n, Value val, Tokens t, bool func) :
			name{n}, type{t}, isfunc{func}{
				if(type == Tokens::array)
					values.push_back(val);
				else value = val;
		}
		
		Symbol(string n, Tokens t, bool func) :
			name{n}, type{t}, isfunc{func}{}
		
		Symbol(void){ 
			name = "";
			type = Tokens::none;
			isfunc = false;
		}
		
		string getname(void){
			return name;
		}
		
		Tokens gettype(void){
			return type;
		}
		
		
		Tokens getvaluetype(int index){
			if(type == Tokens::array && index < values.size()){
				return values.at(index).gettype();
			}
			return value.gettype();
		}
		
		
		void setvalue(Value val){
			if(type == Tokens::array)
				values.push_back(val);
			else value = val;
		}
		
		string getvalue(void){
			return value.get();
		}
		
		std::string getvalue(int index){
			if(type == Tokens::array && index < values.size())
				return values.at(index).get();
			//std::cout << "Symbol.getvalue(): index=" << index << std::endl;
			return "";
		}
		
		Value getobjvalue(void){
			return value;
		}
		
		Value getobjvalue(int index){
			if(type == Tokens::array && index < values.size())
				return values.at(index);
			return value;
		}
		
		int countels(void){
			if(type == Tokens::array){
				return values.size();
			}
			return 0;
		}
		
		bool isfunction(void){
			return isfunc;
		}
		
		bool isarray(void){
			if(type == Tokens::array)
				return true;
			return false;
		}
};


class Environment{
	private:
		vector<Symbol> symtable;
	
	public:
		Environment(void){}
		
		void addsymbol(Symbol sym){
			symtable.push_back(sym);
			//std::cout << "New symbol, '" << sym.getname() << "' added. Stack size: " << symtable.size() << std::endl;
		}
		
		Symbol getsymbol(string name){
			Symbol sym;
			for(Symbol s : symtable){
				if(s.getname() == name){
					sym = s;
					break;
				}
			}
			return sym;
		}
		
		bool updatesymbol(Symbol& sym){
			for(Symbol& s : symtable){
				if(sym.getname() == s.getname()){
					s = sym;
					return true;
				}
			}
			return false;
		}
		
		Value run(std::string obj){
			Symbol s = getsymbol(obj);
			Value val;
			std::cout << "environment.run(): Function called...\n";
			std::cout << s.getvalue() << std::endl;
			
			//Parser parse(s.getvalue());
			
			return val;
		}
};

Environment environment;

#endif
