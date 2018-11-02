#include "Compiler.h"

Compiler::Compiler()
{
	listLocation = 0;
}

Compiler::Compiler(std::string inputFileName, std::string outputFileName)
{
	outputFile = outputFileName;
	inputFile = inputFileName;
}

void Compiler::CreateTokenList()
{
	std::fstream iFile;
	std::string sourceString;
	Token tempToken;
	Lexer ourLexer;

	iFile.open(inputFile);

	if (!iFile.is_open())
	{
		std::cout << "File not found.\n";
	}
	else
	{
		while (!iFile.eof())
		{
			if (ourLexer.lineEmpty())
			{
				getline(iFile, sourceString);
				ourLexer.getLine(sourceString);
			}

			tempToken = ourLexer.getToken();
			ourLexer.clearToken();

			if (tempToken.getLexeme() != "")
			{
				tokenList.push_back(tempToken);
				tempToken.changeLexeme("");
				tempToken.changeType(UNKNOWN);
			}			
		}
	}
}

void Compiler::ParseTokenList()
{
	oFile.open(outputFile);
	iter = tokenList.begin();
	tokenPosition = 0;

	// Iterates through the entire list of tokens
	while (iter != tokenList.end())
	{
		Rat18F();
		iter++;	// Moves to the next token
		tokenPosition++;	// Keeps track of current place in TokenList
	}
}

void Compiler::OutputTokenList()
{
	Token tempToken;
	for (std::vector<Token>::const_iterator i = tokenList.begin(); i != tokenList.end(); i++)
	{
		tempToken = *i;
		std::cout << tempToken.getLexeme() << " " << tempToken.GetTypeString() << std::endl;
	}
}

void Compiler::Rat18F()
{
	if (printFunctions)
		oFile << "<Opt Function Definitions>   $$  <Opt Declaration List>  <Statement List>  $$/n";

	while (iter != tokenList.end())
	{
		while (iter->getLexeme() != "$$")
		{
			OptFuncDef();
		}
		while (iter->getLexeme() != "$$")
		{
			OptDeclarationList();
			StatementList();
		}
	}
}

void Compiler::OptFuncDef()
{
	if (printFunctions)
		oFile << "<Function Definitions>     |  <Empty>/n";

	if (iter->getLexeme() == "function")
		FuncDef();
	else
	{
		iter++;
	}
}

void Compiler::FuncDef()
{
	if (printFunctions)
		oFile << "<Function> | <Function> <Function Definitions>/n";
	Function();

	while (iter->getLexeme() == "function")
	{
		Function();
	}
}

void Compiler::Function()
{
	if (printFunctions)
		oFile << "function  <Identifier>(<Opt Parameter List>)  <Opt Declaration List>  <Body>/n";
	
	iter++;
	oFile << iter->printToken();
	if (iter->getType() == LETTER)
	{
		iter++;
		oFile << iter->printToken();
		if (iter->getLexeme() == "(")
		{
			OptParameterList();
		}

		if (iter->getLexeme() != ")")
			Error();

		OptDeclarationList();
		Body();
	}
	else
		Error();
}

void Compiler::OptParameterList()
{
	if (printFunctions)
		oFile << "<Parameter List> | <Empty>/n";
	
	if (iter->GetTypeString() == "LETTER")
		ParameterList();
}
void Compiler::ParameterList()
{
	if (printFunctions)
		oFile << "<Parameter>    |     <Parameter> , <Parameter List>/n";

	Parameter();
	iter++;
	if (iter->GetTypeString() == "LETTER")
		ParameterList();
}
void Compiler::Parameter()
{
	if (printFunctions)
		oFile << "<IDs > : <Qualifier> /n";

	while (iter->GetTypeString() == "LETTER")
	{
		IDs();
		iter++;
	}

	if (iter->getLexeme() == "int" || iter->getLexeme() == "boolean" || iter->getLexeme() == "real")
		Qualifier();
	else
		Error();
}
void Compiler::Qualifier()
{
	if (printFunctions)
		oFile << "int     |    boolean    |  real /n";
	oFile << iter->printToken();
	iter++;
}
void Compiler::Body()
{
	if (printFunctions)
		oFile << "{  < Statement List>  }/n";
}
void Compiler::OptDeclarationList()
{
	if (printFunctions)
		oFile << "<Declaration List>   |    <Empty>/n";

}
void Compiler::DeclarationList()
{
	if (printFunctions)
		oFile << "<Declaration> ;     |      <Declaration> ; <Declaration List>/n";
}
void Compiler::Declaration()
{
	if (printFunctions)
		oFile << "<Qualifier > <IDs>    /n";

	if (iter->getLexeme() == "int" || iter->getLexeme() == "boolean" || iter->getLexeme() == "real")
		Qualifier();
	else
		Error();

	if (iter->GetTypeString() == "LETTER")
		IDs();
	else
		Error();
}
void Compiler::IDs()
{
	if (printFunctions)
		oFile << "<Identifier>    | <Identifier>, <IDs>/n";

	oFile << iter->printToken();
	iter++;
	if (iter->GetTypeString() == "LETTER")
		IDs();
	else
		iter++;
}
void Compiler::StatementList()
{
	if (printFunctions)
		oFile << "<Statement>   | <Statement> <Statement List> /n";

	while (iter->getLexeme() == "Compound" || iter->getLexeme() == "Assign" || iter->getLexeme() == "If" || iter->getLexeme() == "Return"
		|| iter->getLexeme() == "Print" || iter->getLexeme() == "Scan" || iter->getLexeme() == "While")
	{
		Statement();
	}
}
void Compiler::Statement()
{
	if (printFunctions)
		oFile << "<Compound>  |  <Assign>  |   <If>  |  <Return>   | <Print>   |   <Scan>   |  <While>/n";

	if (iter->getLexeme() == "compound")
		Compound();
	if (iter->getLexeme() == "assign")
		Assign();
	if (iter->getLexeme() == "if")
		If();
	if (iter->getLexeme() == "return")
		Return();
	if (iter->getLexeme() == "print")
		Print();
	if (iter->getLexeme() == "scan")
		Scan();
	if (iter->getLexeme() == "while")
		While();
} 
void Compiler::Compound()
{
	if (printFunctions)
		oFile << " {  <Statement List>  } /n";
}
void Compiler::Assign()
{
	if (printFunctions)
		oFile << "<Identifier> = <Expression> ;/n";
}
void Compiler::If()
{
	if (printFunctions)
		oFile << "if  ( <Condition>  ) <Statement>   ifend    |	if (<Condition>) <Statement>   else  <Statement>  ifend /n";
}
void Compiler::Return()
{
	if (printFunctions)
		oFile << "return ; |  return <Expression> ;/n";
}
void Compiler::Print()
{
	if (printFunctions)
		oFile << "put ( <Expression>);/n";
}
void Compiler::Scan()
{
	if (printFunctions)
		oFile << "get ( <IDs> );/n";
}
void Compiler::While()
{
	if (printFunctions)
		oFile << "while ( <Condition>  )  <Statement>  whileend/n";


}
void Compiler::Condition()
{
	if (printFunctions)
		oFile << "<Expression>  <Relop>   <Expression>/n";
}
void Compiler::Relop()
{
	if (printFunctions)
		oFile << "==   |   ^=    |   >     |   <    |   =>    |   =<  /n";
}
void Compiler::Expression()
{
	if (printFunctions)
		oFile << "<Expression> + <Term>    | <Expression>  - <Term>    |    <Term>/n";
}
void Compiler::Term()
{
	if (printFunctions)
		oFile << "<Term>  *  <Factor>     |   <Term>  /  <Factor>     |     <Factor>/n";
}
void Compiler::Factor()
{
	if (printFunctions)
		oFile << "<Primary>    |    <Primary>/n";
}
void Compiler::Primary() 
{
	if (printFunctions)
		oFile << "<Identifier>  |  <Integer>  |   <Identifier>  ( <IDs> )   |   ( <Expression> )   |   <Real> | true | false /n";
}
void Compiler::Empty()
{
	if (printFunctions)
		oFile << "/n";
}
void Compiler::Error()
{
	if (printFunctions)
		oFile << "ERROR - INVALID SYNTAX/n";
}