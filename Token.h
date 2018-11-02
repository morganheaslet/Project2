#include <string>
#include <iostream>
#include <fstream>
#include <sstream>  
#include <iomanip>
#include "state.h"

class Token
{
public:
	Token();
	Token(std::string lex, state type);
	void changeLexeme(std::string newLexeme);
	void addLexeme(char newChar);
	void changeType(state newType);
	std::string getLexeme();
	state getType();
	std::string GetTypeString();
    std::string printToken();
private:
	std::string lexeme; // Lexeme
	state tokenType;    // Token type the lexeme belongs to (uses 'state' enum for readability)
};
