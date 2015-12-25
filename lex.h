#ifndef LEX_H_INCLUDED
#define LEX_H_INCLUDED


int OpenProgram(string FileName);

string GetLine(long unsigned Pos);

extern vector<_Token> Tokens;
void PrepareTokensList();

extern long unsigned ln;         // index of current line being scanned/parsed
extern long unsigned lines;      // total number of lines in the program


#endif // LEX_H_INCLUDED
