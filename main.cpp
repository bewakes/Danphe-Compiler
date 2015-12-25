#include "stdinc.h"
#include "lex.h"
#include "parser.h"
#include "codegen.h"

int MaxOprPd = 5;

void DeleteTree(Node*& pt)
{
    if (!pt) return;
    DeleteTree(pt->Left);
    DeleteTree(pt->Right);
    delete pt;
    pt=0;
}
void CreateTree(Node*& pt)
{
    pt = new Node();
    pt->Value="";
    pt->Attribs[0]=pt->Attribs[1]=pt->Attribs[2]="";
    pt->Left=pt->Right=0;
}

void Error(string err, long unsigned LineStart)
{
    cout << "Error:\n\t";
    cout << err << "\n\t\t";
    cout << GetLine(LineStart);
    cout << "\nLine Number:" << ln+1;
    DeleteTree(ParseTree);
    exit(-1);

}
string ToStr(long unsigned Num)
{
    char buffer[20];
    sprintf(buffer, "%lu",Num);
    return buffer;
}

int main(int argc, char **argv)
{
    string FileName;
    if (argc != 2){
        cout << "Usage: Danphe <filename>\nEnter Filename: ";
        getline(cin,FileName);
        cout << "\r\n";
    }
    else
        FileName = argv[1];
    cout << "Compiling file: " << FileName << "\n";
    if (OpenProgram(FileName)!=0)
        return -1;
                                                                        cout << ".";
    PrepareTokensList();                                                cout << ".";
    ParseProgram();cout << ".";
    CodeGen(ParseTree);                                                 cout << ".";

    ofstream AsmFile ("Tmp.asm");
    if (AsmFile.is_open())
    {
        AsmFile << "extern calloc\nextern free\nextern exit"
                "\nextern strcpy\nextern strcat\nextern strlen\nextern strcmp\nextern printf\n";
        AsmFile << "\n\n\nsection .data\n"
                " istr db '%d',0\n"
                " dstr db '%lf',0\n"
                " tmpStr dd 0\n tmpInt dd 0\n tmpDbl1 dq 0.0\n tmpDbl2 dq 0.0\n" << Data;
        AsmFile << "\n\nsection .bss\n" << Bss;
        AsmFile << "\n\nsection .text\n global START\nSTART:\n" << Code;
        AsmFile << "\n" << Final << "\npush 0\ncall exit\nadd esp, 4" << FuncCode;
        AsmFile.close();
        system("NAsm -f win32 \"Tmp.asm\"");                            cout << ".";
        system("GoLink /console /ni \"Tmp.obj\" msvcrt.dll");           cout << ".";
    }
    DeleteTree(ParseTree);                                              cout << ".";

    cout << "\nDone!!!";
    return 0;
}
