#include "stdinc.h"
#include "lex.h"

// Some constants that define the type of a token
#define UNKNOWN     -1
#define EOL         0
#define NUMBER      1
#define STRING      2
#define SYMBOL      3
#define IDENTIFIER  4

string Program="";          // The complete program code

long unsigned pos=0;        // current position of scanning
long unsigned lineStart=0;  // starting position, in the program, of the current line being scanned
long unsigned ln=0;         // index of current line being scanned/parsed
long unsigned lines=0;      // total number of lines in the program

vector<_Token> Tokens;
_Token Token;

string GetLine(long unsigned Pos)
{
    string ret="";
    //  Get the text till the end of line
    while (Program[Pos]!='\n')
    {
        ret+=Program[Pos];
        Pos++;
    }
    return ret;
}

int OpenProgram(string FileName)
{
    ifstream file(FileName.c_str());
    if (!file.is_open())
    {
        cout << "Error:\n\tCan't open file \"" + FileName + "\"\n";
        return -1;
    }
    string tmp="";
    while (getline(file, tmp))
    {
        Program = Program + tmp + "\n";
        lines += 1;
    }
    file.close();

    if (Program.length() == 0)
    {
        cout << "Error:\n\tEmpty File\n";
        return -1;
    }
    return 0;
}

//  To skip the spaces and tabs as well as the comments that the parser won't need
void SkipWhiteSpace()
{
    while (isspace(Program[pos]))		//  as long as the current character is a space or a tab, skip it
    {
        if (Program[pos]=='\n')
            break;			            //  but do not skip the end of line '\n' character
        pos++;                          //  because that is not useless: EOL marks the end of
                                        //  programming statement in 0x
    }
    if (Program[pos]=='/' && Program[pos+1]=='/')
    {
        while (Program[pos]!='\n')		//  if the current and the next character are both
            pos++;			            //  ‘/’, then it is a comment: //COMMENT
    }
}

// To get the identifier type of token
int GetIdentifier()
{
    do
    {
        Token.Str += toupper(Program[pos]);     //  The toupper is to make sure every character is uppercase
        pos++;                                  //  so that our compiler is not case sensitive
    }while (isalnum(Program[pos]) || Program[pos]=='_');  // Alphabet, number or undescore can be included
    Token.Type = IDENTIFIER;
    return IDENTIFIER;
}

//  To get the number type of token
int GetNumber()
{
    bool dot=false;
    do
    {
        if (Program[pos]=='.')
            dot=true;
        Token.Str += Program[pos];
        pos++;
    }while (isdigit(Program[pos]) || (Program[pos]=='.' && !dot));
    Token.Type = NUMBER;
    return NUMBER;
}

//  To get the string type of token
int GetString()
{
    pos++;
    do
    {
        if (Program[pos]=='\n')     // Of course, we can't end a line in the middle of a string
            Error("Expected ending quotation mark '\"'...\n\t\tin line:\n\t",lineStart);
        Token.Str += Program[pos];
        pos++;
    }while (Program[pos]!='"');  // Scan till we get the ending quotation mark
    pos++;
    Token.Type = STRING;
    return STRING;
}

//  To get the symbol type of token
int GetSymbol()
{
    Token.Str += Program[pos]; //  Normally a single character is all that is a token
            //  But in case of <=, >= or != we have two characters as token
    if ((Program[pos]=='<'||Program[pos]=='>'||Program[pos]=='!'||Program[pos]=='=')&&Program[pos+1]=='=')
    {
        pos++;
        Token.Str += Program[pos];
    }
    pos++;
    Token.Type = SYMBOL;
    return SYMBOL;
}


//  To get te next token once the current one is scanned
int GetNextToken()
{
    //  By default...
    Token.Str = "";
    Token.Type = UNKNOWN;

    //  Start by skipping all the spaces and useless comments
    SkipWhiteSpace();

    ////  NEW CODE /////
    Token.LineStart = lineStart;
    ////  NEW CODE /////

    // If the first character is...
    //  ...an end of line then EOL
    if (Program[pos]=='\n')
        {Token.Type = EOL; return EOL;}
    //  ...an alphabet then its an identifier
    if (isalpha(Program[pos]))
        return GetIdentifier();
    //  ...a digit then its a number
    if ((isdigit(Program[pos]))||(Program[pos]=='.' && isdigit(Program[pos+1])))
        return GetNumber();
    //  ...a '"' mark then a string is starting
    if (Program[pos]=='"')
        return GetString();
    //  ...a symbol then its a symbol
    if (ispunct(Program[pos]))
        return GetSymbol();

    //  Non of the above, throw an error
    Error("Unrecognized Token...\n\t\tin line:\n\t",lineStart);
    return UNKNOWN;
}

//  Prepare a list of all the tokens in the program
void PrepareTokensList()
{
    //  Start from the first character
    pos=ln=lineStart=0;
    //  Scan next token till we pass through the end of file
    while (ln<lines) //  i.e. as long as currentLineIndex < TotalLines
    {
        GetNextToken(); //  Get the token
        if (Token.Type==EOL){ // An EOL means we have to update some variables
            lineStart=pos+1;
            ln++;
            pos++;
        }
        Tokens.push_back(Token);  // And add the the token to the list
    }
}
