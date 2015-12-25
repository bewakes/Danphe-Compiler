#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED


int ParseExpression(Node*&nd, int Pd);
void ParseStatement(Node* nd);

void ParseProgram();
extern Node* ParseTree;

bool CheckTypes(string cls1, string cls2);
bool IsNumber(string Type);
#endif // PARSER_H_INCLUDED
