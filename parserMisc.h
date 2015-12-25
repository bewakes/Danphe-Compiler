#ifndef PARSERMISC_H_INCLUDED
#define PARSERMISC_H_INCLUDED

struct OprInfo
{
    string Name;
    int LtCls;
    int RtCls;
    int RetCls;
    int Pd; //Precidence
};
struct UnOprInfo
{
    string Name;
    int Cls;
    int RtCls;
};
struct VarInfo
{
    string Name;
    int Type;

    string Remarks;
};
struct FuncInfo
{
    string Name;
    vector<VarInfo> Parameters;
    int Type;
    unsigned long LocalSize;
    unsigned long ParamSize;
};
struct ClassInfo
{
    string Name;
    unsigned long Size;
    vector<VarInfo> Members;
};

extern int CurrentFunction;

extern vector <VarInfo> Vars;
extern vector <FuncInfo> Funcs;
extern vector <ClassInfo> Cls;
extern vector <OprInfo> Oprs;
extern vector <UnOprInfo> UnOprs;

int AddPtrClass(int Class, int nPtr);

unsigned long GetSize(int Type);
unsigned long GetLocalSize(string Name);
bool CheckVar(string var);
bool CheckFunc(string var);
int GetMember(int Class, string Member);
unsigned long GetMemOffset(int Class, string Member);
int GetFuncIndex(string var);
bool IsNumber(string Type);
int GetType(string var);
int GetClass(string ClassName);
bool IsClass(string token);
bool IsReserved(string var);
bool IsOpr(string token);
int GetUnOpr(string token);
int GetPd(string Opr);
bool CheckTypes(int cls1, int cls2);
int CheckUnOprType(int UnId, int Type);
unsigned long SzParam(string Function);

int CheckIfClass(int Type);
#define LEFT 0
#define RIGHT 1
int GetOprCls(string Opr, int Side, int Need, int LNeed=-1);
int GetRetOpr(string Opr,int LNeed,int RNeed);

void BuildClass();

#endif // PARSERMISC_H_INCLUDED
