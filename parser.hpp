#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <sstream>
#include <cstdlib>
#include <vector>
#include "scanner.hpp"
#include "value.hpp"
#include "symtab.hpp"


/*
This keeps information for every token.
*/
struct History{
	std::string lexeme;	// The string representation of the token
	Tokens token;	// The token type returned by the scanner
	Tokens operation;	// This tells which type of operation we're in
	Symbol symbol;	// The last symbol or identifier encountered
	int lineno, charno;	// Line and character number at which the current token occurs
	
	History(void){ 	// Constructor to initialize ...
		operation = Tokens::none;	// operation: at first, there is no operation
		lineno = 1;	// lineno:
		charno = 1;	// and charno:	we set these to 1
	}
};


/*
This contains function return address.
Return address is simply a point in the source
text to which the scanner can be reset after each
function call. It's a FIFO structure
*/
struct ReturnAddress{
	std::vector<int> locations;	// Our address stack
	
	void push(int adr){	// Push an address onto the stack
		locations.push_back(adr);
	}
	int pop(void){	// Remove and return an address from the top of the stack
		if(locations.size() > 0){
			int val = locations.at(locations.size()-1);
			locations.erase(locations.end()-1);
			return val;
		}
		return -1;
	}
};


/*
This structure holds the values returned from
functions. 
*/
struct ReturnValues{
	vector<Value> retstack;
	
	// Add a return value to the end of the stack
	void push(Value val){
		retstack.push_back(val);
	}
	
	// Remove and return the last value pushed onto the stack
	Value pop(void){
		Value val;
		if(retstack.size() > 0){
			val = retstack.at(retstack.size()-1);
			retstack.erase(retstack.end()-1);
		}
		return val;
	}
	
	/* 
	Get the last value added to, 
	but don't remove it from, the stack
	*/
	Value get(void){
		Value val;
		if(retstack.size() > 0){
			val = retstack.at(retstack.size()-1);
		}
		return val;
	}
	
	// Tell how many values are on the stack
	int size(void){
		return retstack.size();
	}
};


/*
This defines a structure that holds the names
of functions currently running, both parent
and child functions.
*/
struct FunctionStack{
	vector<std::string> funcs;
	
	int push(std::string name){
		funcs.push_back(name);
		return funcs.size()-1;
	}
	
	int pop(void){
		int cursize = funcs.size();
		if(cursize > 0){
			funcs.erase(funcs.end()-1);
			cursize = funcs.size();
		}
		return cursize;
	}
	
	std::string current(void){
		if(funcs.size() > 0)
			return funcs.at(funcs.size()-1);
		return "";
	}
	
	int count(void){
		return funcs.size();
	}
};


/*
	Different functions can push their parameters onto
	this stack. To separate a function's set of parameters
	from the next, each function should push an empty value
	first, before pushing its actual parameters.
*/
struct Parameter{
	vector<Value> values;

	Value pop(void){
		Value val;
		
		if(values.size() > 0){
			val = values.at(values.size()-1);
			values.erase(values.end()-1);
		}
		return val;
	}
	
	
	Value get(int index){
		Value v("null", Tokens::none);
		if(index < values.size() && index >= 0)
			v = values.at(index);
		return v;
	}
	
	
	void push(Value val){
		values.push_back(val);
	}
	
	int size(void){
		return values.size();
	}
};



class Function{
	private:
		std::string name;
		std::string owner;
		int stackindex;
		int blockstart;
		int blockend;
		int next_index;
		Parameter last_args;
		Stack* fstack;
	
	public:
	
		Function(void){
			fstack = new Stack();
		}
		void setname(std::string n){
			name = n;
		}
		void setowner(std::string o){
			owner = o;
		}
		void setstack(Stack* stk){
			fstack = stk;
		}
		void setstackindex(int index){
			stackindex = index;
		}
		void setblockstart(int start){
			blockstart = start;
		}
		void setblockend(int end){
			blockend = end;
		}
		std::string getname(void){
			return name;
		}
		std::string getowner(void){
			return owner;
		}
		int getstackindex(void){
			return stackindex;
		}
		int getblockstart(void){
			return blockstart;
		}
		int getblockend(void){
			return blockend;
		}
		void push_arg(Value val){
			last_args.push(val);
			next_index = last_args.size();
		}
		Value get_arg(int index){
			return last_args.get(index);
		}
		Value next_arg(void){
			next_index--;
			return get_arg(next_index);
		}
		int args_count(void){
			return last_args.size();
		}
		void install_stack(void){
			environment.newstack(fstack);
		}
		void drop_stack(void){
			fstack = environment.popstack(stackindex);
		}
		int stacksize(void){
			return fstack->getstacksize();
		}
		Stack* getstack(void){
			return fstack;
		}
};


struct Functions{
	vector<Function> funcs;
	
	int push(Function func){
		funcs.push_back(func);
		return funcs.size()-1;
	}
	
	int pop(void){
		int cursize = funcs.size();
		if(cursize > 0){
			funcs.erase(funcs.end()-1);
			cursize = funcs.size();
		}
		return cursize;
	}
	
	Function get(std::string name){
		Function func;
		for(Function f : funcs){
			if(f.getname() == name){
				func = f;
				break;
			}
		}
		return func;
	}
	
	
	void push_arg(std::string func, Value val){
		for(Function& f : funcs)
			if(f.getname() == func)
				f.push_arg(val);
	}
	
	
	Value get_arg(std::string func, int index){
		Value val;
		for(Function f : funcs)
			if(f.getname() == func)
				val = f.get_arg(index);
		return val;
	}
	
	
	void setstack(std::string func, Stack* stk){
		for(Function& f : funcs)
			if(f.getname() == func)
				f.setstack(stk);
	}
	
	
	void install_stack(std::string func){
		for(Function& f : funcs)
			if(f.getname() == func)
				f.install_stack();
	}
	
	
	void drop_stack(std::string func){
		for(Function& f : funcs)
			if(f.getname() == func)
				f.drop_stack();
	}
	
	
	int count(void){
		return funcs.size();
	}
	
};



std::string typespecs[] = 
{ 
	"num", "string", "char", "array", "function", 
	"bool" 
};

std::string keywords [] = 
{ 
	"print", "println", "return", "typeof", "read", "if", 
	"else", "sizeof", "until", "break", "continue", "_value"
};



class Parser{
	private:
		Scanner scanner;
		History current, previous, next;
		ReturnAddress returnaddress, loopreturnaddress;
		ReturnValues returnvalues;
		Parameter parameters;
		Functions functions;
		FunctionStack running_functions;
	
		bool inloop, in_function;
		int loop_line_count, function_line_count;	// To be used in keeping track of line numbers in loops and functions
	
	public:
		Parser(void){}
		Parser(std::string filename){
			scanner = Scanner(filename);
			linecount = 1;
			inloop = false;
			in_function = false;
			loop_line_count = 0;
			function_line_count = 0;
			
			advance();	// Initialize the scanner
			while(!match(Tokens::done)){
				Value val = expression();
				//returnvalues.push(val);
			}
		}
		
	
	private:
		bool match(Tokens tok){
			if(current.token == tok)
				return true;
			return false;
		}
		
		
		void terminate(void){
			exit(EXIT_SUCCESS);
		}
		
		
		bool advance(void){
			previous = current;
			current.token = scanner.scan();
			if(match(Tokens::done))
				return false;
			
			else if(match(Tokens::newline)){
				linecount++;
				charcount = 1;
				
				if(inloop)
					loop_line_count++;
				if(in_function)
					function_line_count++;
				
				advance();
			}
			
			current.lineno = linecount;
			current.charno = charcount;
			
			current.lexeme = scanner.getlexeme();
			return true;
		}
		
		
		void push_loop_return_address(int address){
			loopreturnaddress.push(address);
		}
		
		void pushaddress(int address){
			returnaddress.push(address);
		}
		
		
		int pop_loop_return_address(void){
			int addr = loopreturnaddress.pop();
			scanner.setpos(addr);
			
			return addr;
		}
		
		int popaddress(void){
			int addr = returnaddress.pop();
			scanner.setpos(addr);
			
			return addr;
		}
		
		
		void pushback(void){
			//std::cout << "pushback() to: " << current.lexeme << "\n";
			int oldpos = scanner.getpos() - current.lexeme.length()+1;
			scanner.setpos(oldpos);
			current = previous;
		}
		
		
		bool infunction(void){
			if(running_functions.count() > 0)
				return true;
			return false;
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
		
		
		bool is_boolean(std::string lex){
			if(lex == "true" || lex == "false")
				return true;
			return false;
		}
		
		
		bool is_identifier(std::string lex){
			Symbol sym = environment.getsymbol(lex);
			if(sym.getname() == lex /*&& sym.getstackindex() == environment.getcurstack()*/)
				return true;
			return false;
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
			else if(lex == "typeof") return Tokens::typeof;
			else if(lex == "read") return Tokens::read;
			else if(lex == "if") return Tokens::if_cond;
			else if(lex == "else") return Tokens::else_cond;
			else if(lex == "sizeof") return Tokens::obsize;
			else if(lex == "until") return Tokens::until;
			else if(lex == "break") return Tokens::brk_loop;
			else if(lex == "continue") return Tokens::cont_loop;
			else if(lex == "_value") return Tokens::return_value;
			
			return Tokens::none;
		}
		
		
		/*
			Note: The last token after every call to skipblock()
				is a closing curly brace (Tokens::c_brace. 
				This is exactly where the next expression 
				expects to begin, so do not advance() after 
				the while() loop.
		*/
		int skipblock(void){
			int o_brace=0, c_brace=0;
			if(match(Tokens::o_brace)){
				//std::cout << "skipblock(): skipping from '" << current.lexeme << "'\n";
				o_brace++;
				while(advance()){
					if(match(Tokens::o_brace))
						o_brace++;
					else if(match(Tokens::c_brace)){
						c_brace++;
						if(c_brace == o_brace)
							break;
					}
				}
				//advance();
				//std::cout << "skipblock(): skipped to '" << current.lexeme << "'\n";
			}
			else{
				while(!match(Tokens::semicolon))
					advance();
			}
			return scanner.getpos();
		}
		
		
		
		/*
			execblock() executes a block of code and stops 
				exactly at the closing brace of the
				block. That means a subsequent call to advance points
				at the very next token to the block.
		*/
		Value execblock(void){
			//Stack stk;
			environment.newstack(new Stack());	// We hope this resource will be garbage-collected for us
			Value val;
			
			if(match(Tokens::o_brace)){
				//std::cout << "execblock(): entering block...\n";
				advance();
				
				while(!match(Tokens::c_brace)){
					val = expression();
					/*
					if(match(Tokens::o_brace)){
						val = execblock(breakloop);
					}
					*/
				}
				/* 
					This is commented out because we do not 
					expect a semicolon to terminate a block.
				*/
				//if(match(Tokens::semicolon))
				//	advance();
			}
			else{
				//std::cout << "execblock(): before expression: '" << current.lexeme << "'\n";
				val = expression();
				//std::cout << "execblock(): current lexeme: '" << current.lexeme << "'\n";
			}
			environment.popstack();
			return val;
		}
		
		
		/*
			This function prepares the stack for function execution.
			It installs all stacks relevant to the work of a function
			object, starting from the root stack which belongs to the
			function itself.
		*/
		void setup_function_stack(std::string funcname){
			vector<Function> func_family;
			Function func = functions.get(funcname);
			while(func.getowner() != "environment"){
				func_family.push_back(func);
				func = functions.get(func.getowner());
			}
			func_family.push_back(func);
			
			for(int i=func_family.size()-1; i>=0; i--){
				std::string fname = func_family.at(i).getname();
				functions.install_stack(fname);
				//std::cout << "setup(): Function='" << fname << "', Owner='" << func.getowner() << "'\n";
				
				//if(func.getowner() == "environment"){
					int stacksize = environment.getstacksize(); // = func.args_count();
					int n=0;
					for(; n<stacksize; n++){
						Value v = func.get_arg(n);
						if(v.get() == "null")
							continue;
						//std::cout << "setup(): Argument: '" << v.get() << "'\n";
						Symbol sym = environment.getsymbol(n);
						//std::cout << "setup(): Assigned to: '" << sym.getname() << "\n";
						sym.setvalue(v);
						environment.updatesymbol(sym);
					}
				//}
				//std::cout << fname << "'s stack size: " << environment.getstacksize() << "\n";
			}
		}
		
		
		
		Value subfunction(std::string funcname, std::string parent, int parent_address){
			Value val;
			setup_function_stack(funcname);
			Function mainfunc = functions.get(parent);
			Function subfunc = functions.get(funcname);
			
			/*
				There is no need to advance here because function() 
				places us either at an o_bracket if this sub-function 
				takes some arguments, or at a semicolon if it does not.
			*/
			if(match(Tokens::o_bracket)){	// Subfunction takes arguments
				advance();
				//vector<Value> vals;
				int i = 0;
				
				while(!match(Tokens::c_bracket)){
					Value v = expression();
					if(match(Tokens::comma))
						advance();
					//vals.push_back(v);
					
					//std::cout << "Argument: '" << v.get() << "'\n";
					
					Symbol sym = environment.getsymbol(i++);
					//std::cout << "Assigned to: '" << sym.getname() << "'\n";
					sym.setvalue(v);
					environment.updatesymbol(sym);
				}
				advance();	// Move to semicolon
				
				/*
				Value v("null", Tokens::none);
				vals.push_back(v);
				
				int arg_count = vals.size()-1;
				for(; arg_count >= 0; arg_count--)
					parameters.push(vals.at(arg_count));
				*/
			}
			
			/*
				We're now at a semicolon. We need to subtract 2 from 
				current scanner position so that upon resetting scanner 
				to this position later, we can advance() correctly to 
				this same point and then proceed with the next expression.
			*/
			int reset_to = scanner.getpos();	// Read and save scanner return position
			
			pushaddress(parent_address);
			popaddress();
			
			while(advance()){
				if(current.lexeme == subfunc.getname()){
					advance();
					if(match(Tokens::assignment))
						break;
				}
			}
			if(previous.lexeme == subfunc.getname())
				while(!match(Tokens::o_brace))
					advance();
			
			//std::cout << "Launching '" << subfunc.getname() << "' from: " << current.lexeme << "\n";
			running_functions.push(funcname);
			val = execblock();
			environment.popstack();	//functions.setstack(subfunc.getname(), environment.popstack());
			environment.popstack();
			
			pushaddress(reset_to);
			popaddress();
			advance();
		
			return val;	//returnvalues.pop();
		}
		
		
		
		
		Value function(std::string funcname, bool exec_sub=false){
			//std::cout << "function() called with arg: " << funcname << "\n";
			
			Symbol sym = environment.getsymbol(funcname);
			int start = sym.getstartindex();
			Function func = functions.get(funcname);
			
			std::string parent_function = current.lexeme;
			
			// Let's try to call an inner function: mainfunc@subfunc
			int scanner_pos = scanner.getpos()-(current.lexeme.length()+1);	// We'll return to this point if there is no subfunction to execute
			advance();
			if(match(Tokens::subfunc)){
				std::string func_name;
				while(match(Tokens::subfunc)){
					advance();
					if(match(Tokens::cstring)){
						func_name = current.lexeme;
						advance();
					}
				}
				//std::cout << "Drilled down to: '" << func_name << "'\n";
				return subfunction(func_name, parent_function, start);
			}
			setup_function_stack(funcname);
			
			
			bool withparams = false;
			running_functions.push(funcname);	// Notify us that we're executing a function
			int stacksize, return_to;
			
			if((stacksize = environment.getstacksize()) > 0)
				withparams = true;
			

			//std::cout << funcname << " was defined to take " << stacksize << " arguments\n";
			if(withparams){
				int symindex = 0;
				if(match(Tokens::o_bracket)){
					advance();
					while(!match(Tokens::c_bracket)){
						if(symindex >= stacksize){
							while(!match(Tokens::c_bracket))
								advance();
							break;
						}
						if(match(Tokens::comma))
							advance();
						
						Value v = expression();
						functions.push_arg(funcname, v);
						//std::cout << "Argument: '" << v.get() << "'\n";
						Symbol sym = environment.getsymbol(symindex);
						//std::cout << "Assigned to: '" << sym.getname() << "\n";
						sym.setvalue(v);
						environment.updatesymbol(sym);
						symindex++;
					}
					//advance();	// Advance to semicolon after parameter list
				}
				else{
					if(parameters.size() <= 0){
						std::cout << "Missing parameter list for function '" << sym.getname() << "'.\n";
						terminate();
					}
					
					for(int i=0; i<stacksize; i++){
						Value v = parameters.pop();
						functions.push_arg(funcname, v);
						if(v.get() == "null")	// If the value is "null", then this marks the end of this function's parameter list
							break;
						//std::cout << "Argument: '" << v.get() << "'\n";
						Symbol sym = environment.getsymbol(i);
						//std::cout << "Assigned to: '" << sym.getname() << "\n";
						sym.setvalue(v);
						//sym.setstackindex(curstack);
						environment.updatesymbol(sym);
					}
				}
			}
			//std::cout << "function(): lexeme after param list: " << current.lexeme << "\n";
			return_to = scanner.getpos()-1;
			
			pushaddress(start);
			popaddress(); // set new scanner position
			advance();
			//std::cout << "function(): lexeme after scanner reset: " << current.lexeme << " at index " << scanner.getpos() << "\n";
			
			push_loop_return_address(return_to);
			Value val = execblock();
			/* 
				We are accessing stacks through pointers, so no need 
				to write current stack to function stack: they're one 
				and the same stack. Otherwise, we would have done this:
					functions.setstack(funcname, environment.popstack());
			*/
			//environment.popstack();
			//popaddress();
			//advance();
			
			linecount -= function_line_count;
			function_line_count = 0;
			
			return val;	// returnvalues.pop();	//functions.get(funcname).getvalue();
		}
		
		
		
		
		Value print(void){
			//std::cout << "Printing now: current lexeme: " << current.lexeme << "\n";
			Value val;
			current.operation = Tokens::print;
			std::string lex = current.lexeme;
			advance();
			
			if(match(Tokens::semicolon) && lex == "println"){
				std::cout << "\n";
				return val;
			}
			else{
				while(!match(Tokens::semicolon)){
					Value val = expression();
					
					if(lex == "println")
						std::cout << val.get() << std::endl;
					else if(lex == "print")
						std::cout << val.get()/* << " "*/;
					
					//advance();
					if(match(Tokens::comma))
						advance();
				}
				//advance();
				// Don't advance here because expression() will 
				// handle the terminating semicolon through factor()
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
		
		
		Value until(void){
			Value v, val;
			int position = scanner.getpos();
			int lex_length = current.lexeme.length()+1;
			int end = -1;
			bool endpushed = false;
			
			//std::cout << "Entering loop...\n";
			//std::cout << "Current location and index before loop: " << current.lexeme << ", " << scanner.getpos() << "\n";
			
			while(true){
				//std::cout << "Current location and index: " << current.lexeme << ", " << scanner.getpos() << "\n";
				advance();
				v = expression();

				//std::cout << "Current lexeme and index: " << current.lexeme << ", " << scanner.getpos() << "\n";
				
				if(end < 0){
					/*
						We subtract 2 from the current scanner position because 
						expression() above leaves us directly on the loop's opening brace.
						Pushing the exact scanner position to the stack, popping it off 
						and then advancing to effect the reset will take us directly into the 
						block, even before we call execblock(). So the scanner needs to 
						be reset 2 positions behind the opening brace so that advance()
						will land us exactly on the loop block's opening brace.
					*/
					pushaddress(scanner.getpos()-2);
					end = skipblock()-1;	// Save the end of the loop's block minus 1
					popaddress();
					advance();
				}
				
				if(!endpushed){
					push_loop_return_address(end);
					endpushed = true;
				}
				
				if(v.get() != "" && v.get() == "false"){
					inloop = true;	// Tell advance() we're in a loop
					pushaddress(position-lex_length);
					val = execblock();
					
					popaddress();
					advance();
					//std::cout << "loop_line_count: " << loop_line_count << ", linecount: " << linecount << "\n";
					linecount -= loop_line_count;
					loop_line_count = 0;
					//std::cout << "loop_line_count: " << loop_line_count << ", linecount: " << linecount << "\n";
				}
				else{
					skipblock();
					if(inloop){
						inloop = false;
						linecount -= loop_line_count+1;
						loop_line_count = 0;
					}
					pop_loop_return_address();
					/* 
						Do not advance here because we are returning to factor(), 
						which will still advance anyway. Such double advance 
						will lead to loop bugs. You can try it by uncommenting
						advance() below.
					*/
					//advance();
					//std::cout << "until(): after skipping: " << current.lexeme << "\n";
					return val;
				}
			}
			return val;
		}
		
		
		
		Value if_condition(void){
			current.operation = Tokens::condition;
			advance();
			Value val = expression();
			
			if((val.get() != "" && val.get() == "true")/* || val.tonumber() != 0*/){
				val = execblock();
				advance();
				
				//std::cout << "if_condition(): lexeme after block: " << current.lexeme << "\n";
				if(current.lexeme == "else"){
					advance();
					skipblock();
				}
				else{
					pushback();
					//std::cout << "if_condition(): current lexeme: " << current.lexeme << "\n";
				}
				
				return val;
			}
			else{
				skipblock();
				advance();
				
				if(current.lexeme == "else"){
					advance();
					val = execblock();
				}
				else{
					pushback();
				}
				//std::cout << "if_condition() lexeme: " << current.lexeme << ", previous: " << previous.lexeme << "\n";
			}
			return val;
		}
		
		
		
		Value read(void){
			Value val;
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
			val.settype(inptype);
			val.set(str);
					
			//advance();
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
		
		
		
		
		Value declaration(bool general_stack=true){
			Value val;
			bool isfunc = false;
			Tokens type = Tokens::none;
			
			if(is_typespec(current.lexeme)){
				type = tellvaltype(current.lexeme);
				advance();
				
				if(current.lexeme == "_f"){
					isfunc = true;
					advance();
				}
			}
			else if(is_unknown(current.lexeme)){
				if(previous.operation == Tokens::declaration ||
					previous.operation == Tokens::assignment ||
					previous.operation == Tokens::identifier
				){
					//std::cout << "declaration(): declaration with inferred type: '" << current.lexeme << "'\n";
					isfunc = previous.symbol.isfunction();
					type = previous.symbol.gettype();
				} 
			}
			else{
				//std::cout << "declaration(): Symbol '" << current.lexeme << "' already exists.\n";
				if(is_identifier(current.lexeme)){
					Symbol sym = environment.getsymbol(current.lexeme);
					if(sym.isfunction()){
						while(!match(Tokens::o_brace))
							advance();
						skipblock();
						//advance();
					}
				}
				return val;
			}
				
			//if(match(Tokens::cstring) && is_unknown(current.lexeme)){
				while(!match(Tokens::semicolon)){
					Symbol sym(current.lexeme, type, isfunc);
					sym.setstackindex(environment.getcurstack());
					environment.addsymbol(sym, general_stack);
					
					// If it's a function, add it to the function stack
					if(isfunc){
						Function func;	// Constructor allocated a new stack frame for this new function
						func.setname(current.lexeme);
						if(infunction()){
							func.setowner(running_functions.current());
						}
						else func.setowner("environment");
						//func.setstack(new Stack());	// This is not necessary
						functions.push(func);
					}
					current.symbol = sym;
					current.operation = Tokens::declaration;
					advance();
					if(match(Tokens::assignment))
						val = assignment();
					else if(match(Tokens::comma))
						advance();
				}
				/*
					We must not advance past the current semicolon:
					it serves to separate this complete declaration from
					subsequent expressions.
				*/
			/*
			}
			else{
				std::cout << "Redeclaration of '" << current.lexeme << "' on line " << current.lineno << "." << std::endl;
			}
			*/
		
			return val;
		}
		
		
		
		
		
		Value assignment(void){
			Value val;
			
			if(previous.symbol.getname() != ""){
				Symbol s = environment.getsymbol(previous.symbol.getname());
				Tokens type = s.gettype();
				val.settype(type);
				
				advance();
				if(s.isarray()){
					while(current.token != Tokens::semicolon){
						val = expression();
						s.setvalue(val);
					}
				}
				else if(s.isfunction()){
					int stackindex = -1;
					int start;
					
					//std::cout << "assignment(): assigning to function: " << s.getname() << "\n";
					Function func = functions.get(s.getname());
					functions.install_stack(s.getname());
					
					if(match(Tokens::o_bracket)){
						//stackindex = environment.newstack(s.getname(), false);	// false ==> parameter stack
						advance();
						
						//std::cout << "assignment(): current lexeme: " << current.lexeme << "\n";
						while(!match(Tokens::c_bracket)){
							declaration();
							if(match(Tokens::semicolon))
								advance();
						}
						//advance();
						//std::cout << "assignment(): token after function param block: " << current.lexeme << "\n";
						/*
							After fetching the formal parameters of the function
							currently under definition, the scanner is positioned
							to fetch the opening brace of the function's block. But 
							since we're not executing the function now, we need to
							save the scanner's current position minus 1 so that
							jumping to it and advance()ing once will place us at the
							right point to execute the function's block later
						*/
						start = scanner.getpos()-1;
						advance();	// Now move past Tokens::c_bracket to Tokens::o_brace, start of funtion block
						//std::cout << "assignment(): lexeme after params: " << current.lexeme << "\n";
					}
					else{
						std::cout << "Error: On line " << linecount << ": Function definition requires a ";
						std::cout << "parameter list, even if empty.\n";
						terminate();
					}
					
					if(match(Tokens::o_brace)){
						//std::cout << "assignment(): skipping function block for: " << s.getname() << "\n";
						s.setindices(start, stackindex);
						int returnaddress = skipblock(); // Here, after skipblock(), we refuse to advance
						
						functions.get(s.getname()).setblockstart(start);
						functions.get(s.getname()).setblockend(returnaddress);
						
						//skipblock();
						//std::cout << "assignment(): after '" << s.getname() << "': " << current.lexeme << "\n";
						
						//advance();
						while(current.lexeme != ";")
							advance();
						//advance();
						
						//std::cout << "assignment(): after '" << s.getname() << "': " << current.lexeme << "\n";
						
						//std::cout << "assignment(): lexeme after skipping block: " << current.lexeme << ", at index " << returnaddress;
						//std::cout << ", where previous lexeme was " << previous.lexeme << "\n";
						environment.popstack();
						val.set("function");
						s.setvalue(val);
						//return val;
					}
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
					val = print();
				}break;
				
				case Tokens::typeof:{
					val = typeof();
				}break;
				
				case Tokens::obsize:{
					val = obsize();
				}break;
				
				case Tokens::if_cond:{
					val = if_condition();
				}break;
				
				case Tokens::read:{
					val = read();
				}break;
				
				case Tokens::until:{
					val = until();
				}break;
				
				case Tokens::brk_loop:{
					environment.popstack();
					pop_loop_return_address();
					advance();
				}break;
				
				case Tokens::ret:{
					advance();
					val = expression();
					returnvalues.push(val);
				}break;
				
				case Tokens::return_value:{
					val = returnvalues.pop();
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
			else if(match(Tokens::cstring)){
				//std::cout << "factor(): cstring: '" << current.lexeme << "'\n";
				if(is_boolean(current.lexeme)){
					val.settype(Tokens::boolean);
					if(current.lexeme == "true")
						val.set("true");
					else
						val.set("false");
				}
				else if(is_identifier(current.lexeme)){
					//std::cout << "factor(): identifier: '" << current.lexeme << "'\n";
					Symbol sym = environment.getsymbol(current.lexeme);
					
					if(sym.gettype() == Tokens::array){
						advance();
						if(match(Tokens::o_bracket)){
							advance();
							Value v = expression();
							int index = (int)v.tonumber();
					
							if(!match(Tokens::c_bracket)){
								std::cout << "Incomplete array indexing\n";
								terminate();
							}
							else{
								val = sym.getobjvalue(index);
								//advance();
								//return val;
							}
						}
						else{
							val.settype(Tokens::array);
							val.set("array");
							//advance();
							//return val;
						}
					}
					else if(sym.isfunction()){
						//std::cout << "factor(): calling function...\n";
						val = function(current.lexeme);
						//advance();
						//return val;
					}
					else{
						val = sym.getobjvalue();
						current.symbol = sym;
						current.operation = Tokens::identifier;
						//advance();
						//return val;
					}
				}
				else if(current.lexeme == "_f"){
					//std::cout << "factor(): after function: " << current.lexeme << "\n";
					advance();
					if(match(Tokens::semicolon)){
						if(infunction()){
							environment.popstack();
							pop_loop_return_address();
							running_functions.pop();
							return val;
						}
						else{
							std::cout << "No function to terminate.\n";
						}
					}
					else{
						//return declaration();
					}
				}
				else if(is_keyword(current.lexeme)){
					val = keyword();
					//advance();
					//return val;
				}
				else if(is_typespec(current.lexeme)){
					current.operation = Tokens::declaration;
					val = declaration();
					//advance();
					//return val;
				}
				else{
					val = declaration();
				}
			}
			else if(match(Tokens::o_bracket)){
				advance();
				val = expression();
				//advance();
				
				if(!match(Tokens::c_bracket) && !match(Tokens::semicolon)){
					std::cout << "Closing bracket expected but found '" << current.lexeme << "' on line " << current.lineno << "." << std::endl;
					terminate();
				}
				//else advance();
				//return val;
			}
			else if(
				match(Tokens::semicolon) ||
				match(Tokens::comma)	 ||
				match(Tokens::c_brace)
			){
				advance();
				return val;
			}
			else if(match(Tokens::assignment)){
				val = assignment();
				//advance();
				//return val;
			}
			else if(match(Tokens::seq_end)){
				current.operation = Tokens::none;
				advance();
				return val;
			}
			else{
				if(advance()){
					std::cout << "Error: Unexpected token '" << current.lexeme << "' on line " << current.lineno;
					std::cout << " after '" << previous.lexeme << "'.\n";
				}
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
						advance();
						val /= factor();
					}
					else if(current.lexeme == "*"){
						advance();
						val *= factor();
					}
					else break;
				}
			}
			
			return val;
		}
		
		
		Value expression(void){
			Value val = term();
			
			if(match(Tokens::mathop)){
				while(match(Tokens::mathop)){
					if(current.lexeme == "+"){
						advance();
						val += term();
					}
					else if(current.lexeme == "-"){
						advance();
						val -= term();
					}
					else break;
				}
			}
			
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
