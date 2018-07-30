#include <iostream>
//#include <vector>
//#include <string>
#include "parser.hpp"

using namespace std;

/*
class Symbol{
	private:
		string name;
		string value;
		string type;
	
	public:
		void setname(string n){
			name = n;
		}
		string getname(void){
			return name;
		}
		
		void setvalue(string val){
			value = val;
		}
		string getvalue(void){
			return value;
		}
		
		void settype(string t){
			type = t;
		}
		string gettype(void){
			return type;
		}
};


class Stack{
	private:
		vector<Symbol> table;
		int index;
	
	public:
		void push(Symbol sym){
			table.push_back(sym);
			index = table.size()-1;
		}
		
		Symbol pop(void){
			Symbol sym;
			if(index < table.size()){
				sym = table.at(table.size()-1);
				table.erase(table.end()-1);
				index = table.size()-1;
			}
			return sym;
		}
};


vector<Stack*> mstack;
int curstack;


void newstack(Stack* stk){
	mstack.push_back(stk);
	curstack = mstack.size()-1;
}
*/

int main(void){
	Parser parser("test.sun");
	
	/*
	Stack stk;
	newstack(&stk);
	
	while(true){
		cout << "NUM>> ";
		int num;
		cin >> num;
		if(num < 0)
			break;
		Symbol sym;
		sym.setvalue(to_string(num));
		mstack.at(curstack)->push(sym);
	}
	
	Symbol sym;
	while(true){
		sym = mstack.at(curstack)->pop();
		cout << sym.getvalue() << endl;
		if(sym.getvalue() == "")
			break;
	}
	*/
	
	return 0;
}
