#include <iostream>
#include "parser.hpp"

int main(void){
	Parser parser("test.sun");
	/*
	Value val1("34", Tokens::number);
	Value val2("2", Tokens::number);
	//val1 /= val2;
	
	std::cout << val1+val2 << std::endl;
	std::cout << val1/val2 << std::endl;
	std::cout << val1.get() << std::endl;
	
	
	Scanner scanner("test.sun");
	Tokens token;
	
	while((token = scanner.scan()) != Tokens::done)
	std::cout << scanner.getlexeme() << std::endl;
	*/
	
	return 0;
}
