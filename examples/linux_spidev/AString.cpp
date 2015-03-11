#include "AString.h"

String::String(){}

String::String(std::string from){
	data = from;
}

int String::length(){
	return data.size();
}

String String::substring(int pos, int length){
	String out(data.substr(pos, length));
	return out;
	
}
char String::operator[](int pos){
	return data[pos];
}
void String::operator+=(char* val){
	data.append(val, 1);
}
void String::operator+=(char val){
	data.append(&val, 1);
}
void String::concat(char toAdd){
	data.append(&toAdd, 1);
}
void String::concat(char* toAdd){
	data.append(toAdd, 1);
}
void String::concat(const char* toAdd){
	data.append(toAdd, 1);
}
void String::trim(){
	data.erase(std::find_if(data.rbegin(), data.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), data.end());
}

