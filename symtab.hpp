#ifndef SYMTAB_HPP
#define SYMTAB_HPP

#include <string>
#include <iostream>
#include <vector>
#include "defs.hpp"
#include "value.hpp"
#include "runtime.hpp"

using namespace std;



class Symbol{
	private:
		string name;
		Tokens type;
		bool isfunc;
		int startindex, stackindex;
		Value value;
		vector<Value> values;
	
	public:
		Symbol(string n, Value val, Tokens t, bool func) :
			name{n}, type{t}, isfunc{func}{
				if(type == Tokens::array)
					values.push_back(val);
				else value = val;
				
				startindex = 0;
				stackindex = 0;
		}
		
		Symbol(string n, Tokens t, bool func) :
			name{n}, type{t}, isfunc{func}{
				startindex = 0;
				stackindex = 0;
			}
		
		Symbol(void){ 
			name = "";
			type = Tokens::none;
			isfunc = false;
			startindex = 0;
			stackindex = 0;
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
		
		
		void setindices(int start, int stack){
			startindex = start;
			stackindex = stack;
		}
		
		
		void setstackindex(int index){
			stackindex = index;
		}
		
		
		int getstartindex(void){
			return startindex;
		}
		
		
		int getstackindex(void){
			return stackindex;
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


class Stack{
	private:
		vector<Symbol> symtable;
		std::string belongs_to;
	
	public:
	
		Stack(std::string owner="none") 
			: belongs_to{owner} {}
			
		
		void addsymbol(Symbol sym){
			symtable.push_back(sym);
			//std::cout << "New symbol, '" << sym.getname() << "' added. Stack size: " << symtable.size() << std::endl;
		}
		
		
		bool dropsymbol(std::string symname){
			bool dropped = false;
			for(int i=0; i<symtable.size(); i++){
				if(symtable.at(i).getname() == symname){
					symtable.erase(symtable.begin()+i);
					dropped = true;
					break;
				}
			}
			return dropped;
		}
		
		
		bool symbolexists(std::string name){
			for(Symbol s : symtable)
				if(s.getname() == name)
					return true;
			return false;
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
		
		
		Symbol getsymbol(int index){
			Symbol sym;
			if(symtable.size() > index){
				sym = symtable.at(index);
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
		
		
		int getstacksize(void){
			return symtable.size();
		}
		
		
		std::string getstackowner(void){
			return belongs_to;
		}
		
		
		bool setvalue(Symbol sym, int index){
			if(index < symtable.size()){
				symtable[index] = sym;
				return true;
			}
			return false;
		}
};



/*
	Let's comment this out for now
	
class Environment{
	private:
		vector<Stack> stack, paramstack;
		int curstack;
	
	public:
		Environment(void){
			curstack = newstack();	// use default argument: true
		}
		
		void addsymbol(Symbol sym, bool to_general=true){
			if(to_general){
				//sym.setstackindex(curstack);
				stack.at(curstack).addsymbol(sym);
			}
			else{
				int stackindex = paramstack.size()-1;
				if(stackindex >= 0)
					paramstack.at(stackindex).addsymbol(sym);
			}
		}
		
		
		bool dropsymbol(std::string symname, bool general_stack=true){
			if(general_stack && symbolexists(symname))
				return stack.at(curstack).dropsymbol(symname);
			else if(symbolexists(symname, false)){
				int curstack = paramstack.size()-1;
				return paramstack.at(curstack).dropsymbol(symname);
			}
			return false;
		}
		
		
		bool symbolexists(std::string name, bool general_stack=true){
			if(general_stack)
				return stack.at(curstack).symbolexists(name);
			else{
				int curstack = paramstack.size()-1;
				return paramstack.at(curstack).symbolexists(name);
			}
			return false;
		}
		
		
		
		Symbol getsymbol(string name, bool from_general=true){
			Symbol sym;
			if(from_general){
				for(int i=curstack; i>=0; i--){
					sym = stack.at(i).getsymbol(name);
					if(sym.getname() == name)
						break;
				}
			}
			else{
				int stackindex = paramstack.size()-1;
				for(int i=stackindex; i>=0; i--){
					sym = paramstack.at(i).getsymbol(name);
					if(sym.getname() == name)
						break;
				}
			}
			return sym;
		}
		
		
		
		//	This method returns a symbol at index 'symindex' 
		//		in the current stack frame
		
		Symbol getsymbol(int symindex, bool from_general=true){
			Symbol sym;
			if(from_general){
				sym = stack.at(curstack).getsymbol(symindex);
			}
			else{
				int stackindex = paramstack.size()-1;
				sym = paramstack.at(stackindex).getsymbol(symindex);
			}
			return sym;
		}
		
		
		
		bool updatesymbol(Symbol& sym, bool in_general=true){
			if(in_general){
				for(int i=curstack; i>=0; i--){
					if(stack.at(i).updatesymbol(sym))
						return true;
				}
			}
			else{
				int stackindex = paramstack.size()-1;
				for(int i=stackindex; i>=0; i--){
					if(stack.at(i).updatesymbol(sym))
						return true;
				}
			}
			return false;
		}
		
		
		int getstacksize(bool general_stack = true){
			if(general_stack)
				return stack.at(curstack).getstacksize();
			else
				return paramstack.at(paramstack.size()-1).getstacksize();
			
			return 0;
		}
		
		
		int getcurstack(void){
			return curstack;
		}
		
		
		
		int newstack(std::string belongs_to="none", bool general_stack=true){
			Stack table(belongs_to);
			if(general_stack){
				stack.push_back(table);
				return stack.size()-1;
			}
			else{
				paramstack.push_back(table);
				return paramstack.size()-1;
			}
			
			return 0;
		}
		
		
		int newstack(Stack& table, bool general_stack=true){
			if(general_stack){
				stack.push_back(table);
				curstack = stack.size()-1;
				return curstack;
			}
			else{
				paramstack.push_back(table);
				return paramstack.size()-1;
			}
			
			return 0;
		}
		
		
		Stack getstack(int stackindex, bool general_stack=true){
			Stack stackframe;
			if(!general_stack){
				if(getstacksize(false) > stackindex){
					stackframe = paramstack.at(stackindex);
				}
			}
			else{
				if(stack.size() > stackindex){
					stackframe = stack.at(stackindex);
				}
			}
			return stackframe;
		}
		
		
		Stack getfunctionstack(std::string funcname){
			Stack stk;
			int i=0;
			for(; i < paramstack.size(); i++){
				if(paramstack.at(i).getstackowner() == funcname){
					stk = paramstack.at(i);
					//paramstack.erase(paramstack.begin()+i);
					break;
				}
			}
			return stk;
		}
		
		
		Stack popstack(bool general_stack=true){
			Stack stk;
			if(general_stack){
				if(curstack > 0){
					stk = stack.at(stack.size()-1);
					stack.erase(stack.end()-1);
					curstack = stack.size()-1;
				}
			}
			else{
				int stacksize = paramstack.size();
				if(stacksize > 0){
					stk = paramstack.at(stacksize-1);
					paramstack.erase(paramstack.end()-1);
				}
			}
			return stk;
		}
		
		
		
		Stack dropstack(int stackindex, bool general_stack=true){
			Stack stk;
			if(general_stack){
				if(curstack >= stackindex){
					stk = stack.at(stackindex);
					stack.erase(stack.begin()+stackindex);
					curstack = stack.size()-1;
				}
			}
			else{
				int stacksize = paramstack.size();
				if(stacksize > stackindex){
					stk = paramstack.at(stacksize);
					paramstack.erase(paramstack.begin()+stackindex);
				}
			}
			return stk;
		}
		
		
		Value run(std::string obj){
			Symbol s = getsymbol(obj);
			std::cout << "environment.run(): Function called...\n";
			std::cout << s.getvalue() << std::endl;
			
			//Parser parse(s.getvalue());
			
			return s.getvalue();
		}
};
*/





class Environ{
	private:
		vector<Stack*> env_stack, paramstack;
		int curstack;
	
	public:
		Environ(void){
			Stack stk;
			curstack = newstack(&stk);	// use default second argument: true
		}
		
		void addsymbol(Symbol sym, bool to_general=true){
			if(to_general){
				//sym.setstackindex(curstack);
				env_stack.at(curstack)->addsymbol(sym);
			}
			else{
				int stackindex = paramstack.size()-1;
				if(stackindex >= 0)
					paramstack.at(stackindex)->addsymbol(sym);
			}
			//std::cout << sym.getname() << " added to stack. Stack size: " << env_stack.at(curstack)->getstacksize() << "\n";
		}
		
		
		bool dropsymbol(std::string symname, bool general_stack=true){
			if(general_stack && symbolexists(symname))
				return env_stack.at(curstack)->dropsymbol(symname);
			else if(symbolexists(symname, false)){
				int curstack = paramstack.size()-1;
				return paramstack.at(curstack)->dropsymbol(symname);
			}
			return false;
		}
		
		
		bool symbolexists(std::string name, bool general_stack=true){
			if(general_stack)
				return env_stack.at(curstack)->symbolexists(name);
			else{
				int curstack = paramstack.size()-1;
				return paramstack.at(curstack)->symbolexists(name);
			}
			return false;
		}
		
		
		
		Symbol getsymbol(string name, bool from_general=true){
			Symbol sym;
			if(from_general){
				for(int i=curstack; i>=0; i--){
					sym = env_stack.at(i)->getsymbol(name);
					if(sym.getname() == name)
						break;
				}
			}
			else{
				int stackindex = paramstack.size()-1;
				for(int i=stackindex; i>=0; i--){
					sym = paramstack.at(i)->getsymbol(name);
					if(sym.getname() == name)
						break;
				}
			}
			return sym;
		}
		
		
		/*
			This method returns a symbol at index 'symindex' 
				in the current stack frame
		*/
		Symbol getsymbol(int symindex, bool from_general=true){
			Symbol sym;
			if(from_general){
				sym = env_stack.at(curstack)->getsymbol(symindex);
			}
			else{
				int stackindex = paramstack.size()-1;
				sym = paramstack.at(stackindex)->getsymbol(symindex);
			}
			return sym;
		}
		
		
		
		bool updatesymbol(Symbol& sym, bool in_general=true){
			if(in_general){
				for(int i=curstack; i>=0; i--){
					if(env_stack.at(i)->updatesymbol(sym))
						return true;
				}
			}
			else{
				int stackindex = paramstack.size()-1;
				for(int i=stackindex; i>=0; i--){
					if(paramstack.at(i)->updatesymbol(sym))
						return true;
				}
			}
			return false;
		}
		
		
		int getstacksize(bool general_stack = true){
			if(general_stack)
				return env_stack.at(curstack)->getstacksize();
			else
				return paramstack.at(paramstack.size()-1)->getstacksize();
			
			return 0;
		}
		
		
		int getcurstack(void){
			return curstack;
		}
		
		
		
		int newstack(std::string belongs_to="none", bool general_stack=true){
			Stack table(belongs_to);
			if(general_stack){
				env_stack.push_back(&table);
				//std::cout << "New general stack installed. Stack frames: " << env_stack.size() << "\n";
				return env_stack.size()-1;
			}
			else{
				paramstack.push_back(&table);
				return paramstack.size()-1;
			}
			
			return 0;
		}
		
		
		int newstack(Stack* table, bool general_stack=true){
			if(general_stack){
				env_stack.push_back(table);
				curstack = env_stack.size()-1;
				//std::cout << "New general stack installed. Stack frames: " << env_stack.size() << "\n";
				return curstack;
			}
			else{
				paramstack.push_back(table);
				return paramstack.size()-1;
			}
			
			return 0;
		}
		
		
		Stack* getstack(int stackindex, bool general_stack=true){
			Stack* stackframe;
			if(!general_stack){
				if(getstacksize(false) > stackindex){
					stackframe = paramstack.at(stackindex);
				}
			}
			else{
				if(env_stack.size() > stackindex){
					stackframe = env_stack.at(stackindex);
				}
			}
			return stackframe;
		}
		
		
		Stack* getfunctionstack(std::string funcname){
			Stack* stk;
			int i=0;
			for(; i < paramstack.size(); i++){
				if(paramstack.at(i)->getstackowner() == funcname){
					stk = paramstack.at(i);
					//paramstack.erase(paramstack.begin()+i);
					break;
				}
			}
			return stk;
		}
		
		
		Stack* popstack(bool general_stack=true){
			Stack* stk;
			if(general_stack){
				if(curstack > 0){
					stk = env_stack.at(env_stack.size()-1);
					env_stack.erase(env_stack.end()-1);
					curstack = env_stack.size()-1;
					//std::cout << "Stack popped. Stack frames: " << env_stack.size() << "\n";
				}
			}
			else{
				int stacksize = paramstack.size();
				if(stacksize > 0){
					stk = paramstack.at(stacksize-1);
					paramstack.erase(paramstack.end()-1);
				}
			}
			return stk;
		}
		
		
		
		Stack* dropstack(int stackindex, bool general_stack=true){
			Stack* stk;
			if(general_stack){
				if(curstack >= stackindex){
					stk = env_stack.at(stackindex);
					env_stack.erase(env_stack.begin()+stackindex);
					curstack = env_stack.size()-1;
				}
			}
			else{
				int stacksize = paramstack.size();
				if(stacksize > stackindex){
					stk = paramstack.at(stacksize);
					paramstack.erase(paramstack.begin()+stackindex);
				}
			}
			return stk;
		}
		
};




Environ environment;

#endif
