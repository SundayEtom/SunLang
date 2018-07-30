#ifndef VALUE_HPP
#define VALUE_HPP

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include "defs.hpp"



class Value{
	private:
		std::string value;
		Tokens type;
	
	public:
		
		Value(void){
			value = "";
			type = Tokens::none;
		}
		
		Value(std::string val){
			value = val;
		}
		
		Value(std::string val, Tokens t){
			value = val;
			type = t;
		}
		
		std::string get(void){
			return value;
		}
		
		
		void set(std::string val){
			value = val;
		}
		
		
		Tokens gettype(void){
			return type;
		}
		
		void settype(Tokens t){
			type = t;
		}
		
		double tonumber(void){
			double num;
			std::stringstream ss(value);
			ss >> num;
			return num;
		}
		
		std::string tostring(double d){
			std::ostringstream oss;
			oss << d;
			return oss.str();
		}
		
		
		std::string operator/=(Value val){
			switch(type){
				case Tokens::number:{
					if(val.gettype() == Tokens::number){
						double lhs = tonumber();
						double rhs = val.tonumber();
						if(rhs > 0)
							value = tostring(lhs/rhs);
						else std::cout << "Error: Division by zero.\n";
					}
				}break;
					
				default: break;
			}
			return value;
		}
		
		
		
		std::string operator+=(Value val){
			switch(type){
				case Tokens::number:{
					if(val.gettype() == Tokens::number){
						double lhs = tonumber();
						double rhs = val.tonumber();
							
						value = tostring(lhs+rhs);
					}
					else if(val.gettype() == Tokens::string || 
						val.gettype() == Tokens::character
					){
						value = value + val.get();
					}
				}break;
					
				case Tokens::string:{
					value = value + val.get();
				}break;
					
				case Tokens::character:{
					if(
						val.gettype() == Tokens::string ||
						val.gettype() == Tokens::character
					){
						// promote character type to string
						value = value + val.get();
						settype(Tokens::string);
					}
					else if(val.gettype() == Tokens::number){
						double chval = value.at(0);
						chval += val.tonumber();
						value = tostring(chval);
						settype(Tokens::number);
					}
				}break;
					
				default: break;
			}
			return value;
		}
		
		
		
		std::string operator-=(Value val){
			switch(type){
				case Tokens::number:{
					if(val.gettype() == Tokens::number){
						double lhs = tonumber();
						double rhs = val.tonumber();
						value = tostring(lhs-rhs);
					}
				}break;
					
				default: break;
			}
			return value;
		}
		
		
		std::string operator*=(Value val){
			switch(type){
				case Tokens::number:{
					if(val.gettype() == Tokens::number){
						double lhs = tonumber();
						double rhs = val.tonumber();
						value = tostring(lhs*rhs);
					}
				}break;
					
				case Tokens::string:{
					if(val.gettype() == Tokens::number){
						int times = (int)val.tonumber();
						std::string str = "";
						for(int i=0; i<times; i++)
							str += value;
						value = str;
					}
				}break;
					
				default: break;
			}
			return value;
		}
		
		
		std::string operator>(Value val){
			switch(type){
				case Tokens::number:{
					if(val.gettype() == Tokens::number){
						double lhs = tonumber();
						double rhs = val.tonumber();
							
						if(lhs > rhs)
							return "true";
						return "false";
					}
				}break;
					
				default: break;
			}
			return value;
		}
		
		
		std::string operator<(Value val){
			switch(type){
				case Tokens::number:{
					if(val.gettype() == Tokens::number){
						double lhs = tonumber();
						double rhs = val.tonumber();
							
						if(lhs < rhs)
							return "true";
						return "false";
					}
				}break;
					
				default: break;
			}
			return value;
		}
		
		
		std::string operator==(Value val){
			if(type == val.gettype() && value == val.get())
				return "true";
			
			return "false";
		}
		
		
		std::string operator!=(Value val){
			if(type != val.gettype() || value != val.get())
				return "true";
			
			return "false";
		}
		
		
		std::string operator>=(Value val){
			switch(type){
				case Tokens::number:{
					if(val.gettype() == Tokens::number){
						double lhs = tonumber();
						double rhs = val.tonumber();
							
						if((lhs > rhs) || (lhs == rhs))
							return "true";
						return "false";
					}
				}break;
				
				default: break;
			}
			
			return "false";
		}
		
		
		std::string operator<=(Value val){
			switch(type){
				case Tokens::number:{
					if(val.gettype() == Tokens::number){
						double lhs = tonumber();
						double rhs = val.tonumber();
							
						if(lhs <= rhs)
							return "true";
						return "false";
					}
				}break;
				
				default: break;
			}
			
			return "false";
		}
		
		
		std::string operator=(Value val){
			value = val.get();
			type = val.gettype();
			
			return value;
		}
		
		
		std::string operator=(std::string val){
			value = val;
			return val;
		}
		
		
		std::string operator&&(Value val){
			std::string s = "";
			if(value == "true" && val.get() == "true")
				s = "true";
			else s = "false";
			
			return s;
		}
		
		
		std::string operator||(Value val){
			std::string s = "";
			if(value == "true" || val.get() == "true")
				s = "true";
			else s = "false";
			
			return s;
		}
		
};

#endif
