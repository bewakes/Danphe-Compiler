#ifndef STDINC_H_INCLUDED
#define STDINC_H_INCLUDED

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <algorithm>

using namespace std;

#define UNKNOWN     -1
#define EOL         0
#define NUMBER      1
#define STRING      2
#define SYMBOL      3
#define IDENTIFIER  4

void Error(string err, long unsigned LineStart);

struct Node
{
    string Value;
    string Attribs[3];
    Node * Left, * Right;
};

//The information about the token
struct _Token
{
    string Str;
    int Type;
    long unsigned LineStart;    // position at the program where the line containing token starts

};

string ToStr(long unsigned Num);

void CreateTree(Node*& pt);
void DeleteTree(Node*& pt);

extern int MaxOprPd;

#endif // STDINC_H_INCLUDED
