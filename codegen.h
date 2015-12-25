#ifndef CODEGEN_H_INCLUDED
#define CODEGEN_H_INCLUDED

void AddAsmName(string Name, string AsmName);
string GetAsmName(string Name);

void CodeGen(Node* nd);

void AddData(string st);
void AddCode(string st);
void AddEnd(string st);
#define QWORD 0
#define BYTE 1
#define DWORD 2
string GetTmp(string ForValue, int Type);
string GetNewTmp();

void GetAddress(Node* nd);

void PassParameters(Node*nd);
void CleanStack(Node*nd, int n);

void CodeExpr(Node*nd,string Type);

int ClassMember(Node* nd, int cls, string &str);
void CodeInt(Node*nd);
void CodeDbl(Node*nd);
void CodeBln(Node*nd);
void CodeStr(Node*nd);
void StrLen(Node*& nd);
void CodeCls(Node*& nd);

void FreeCls(string Name, int Cls);

extern string Data;
extern string Bss;
extern string Code;
extern string Final;
extern string FuncCode;


#endif // CODEGEN_H_INCLUDED
