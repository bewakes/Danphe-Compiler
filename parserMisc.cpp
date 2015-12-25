#include "stdinc.h"
#include "parserMisc.h"

vector <VarInfo> Vars;
vector <FuncInfo> Funcs;
vector <ClassInfo> Cls;
vector <OprInfo> Oprs;
vector <UnOprInfo> UnOprs;

int AddPtrClass(int Class, int nPtr)
{
    string name="PTR_"+Cls[Class].Name;
    if (!IsClass(name))
    {
        ClassInfo cls;
        cls.Name=name;
        cls.Size=4;
        Cls.push_back(cls);

        UnOprInfo uopr;
        uopr.Name="*";
        uopr.Cls=GetClass(name);
        uopr.RtCls=Class;
        UnOprs.push_back(uopr);
    }
    if (nPtr>1) return AddPtrClass(GetClass(name), nPtr-1);
    return GetClass(name);
}

unsigned long GetSize(int Type)
{
    return Cls[Type].Size;
}
unsigned long GetLocalSize(string Name)
{
    for (unsigned i=0;i<Funcs.size();i++)
        if (Funcs[i].Name==Name)return Funcs[i].LocalSize;
    return 0;
}
// Check if a variable exists in the list
bool CheckVar(string var)
{
    for (unsigned i=0;i<Vars.size();i++)
        if (Vars[i].Name==var)return true;
    if (CurrentFunction!=-1)
    {
        for (unsigned i=0;i<Funcs[CurrentFunction].Parameters.size();i++)
            if (Funcs[CurrentFunction].Parameters[i].Name==var) return true;
    }
    return false;
}
bool CheckFunc(string var)
{
    for (unsigned i=0;i<Funcs.size();i++)
        if (Funcs[i].Name==var)return true;
    return false;
}
int GetMember(int Class, string Member)
{
    for (unsigned i=0;i<Cls[Class].Members.size();i++)
        if (Cls[Class].Members[i].Name==Member)return i;
    return -1;
}
unsigned long GetMemOffset(int Class, string Member)
{
    unsigned long sz=0;
    for (unsigned i=0;i<Cls[Class].Members.size();i++)
    {
        if (Cls[Class].Members[i].Name==Member)break;
        sz+= GetSize(Cls[Class].Members[i].Type);
    }
    return sz;
}
int GetFuncIndex(string var)
{
    for (unsigned i=0;i<Funcs.size();i++)
        if (Funcs[i].Name==var)return i;
    return -1;
}
bool IsNumber(string Type)
{
    if (Type=="DOUBLE" || Type=="INTEGER" ||
                Type=="BYTE" || Type=="WORD" || Type=="SINGLE" || Type=="NUMBER")
        return true;
    return false;
}

// Get the type of a variable existing in the list
int GetType(string var)
{
    for (unsigned i=0;i<Vars.size();i++)
    {
        if (Vars[i].Name==var)
            return Vars[i].Type;

    }
    for (unsigned i=0;i<Funcs.size();i++)
    {
        if (Funcs[i].Name==var)
            return Funcs[i].Type;

    }
    if (CurrentFunction!=-1)
    {
        for (unsigned i=0;i<Funcs[CurrentFunction].Parameters.size();i++)
            if (Funcs[CurrentFunction].Parameters[i].Name==var)
                return Funcs[CurrentFunction].Parameters[i].Type;
    }
    return -1;
}
int GetClass(string ClassName)
{
    for (unsigned i=0;i<Cls.size();i++)
    {
        if (Cls[i].Name==ClassName)return i;
    }
    return -1;
}
bool IsClass(string token)
{
    for (unsigned i=0;i<Cls.size();i++)
    {
        if (Cls[i].Name==token)return true;
    }
    return false;
}
// Check for invalidity of a variable being created
bool IsReserved(string var)
{
    if (IsClass(var) || CheckVar(var)
        || CheckFunc(var))
        return true;
    return false;
}
bool IsOpr(string token)
{
    for (unsigned i=0;i<Oprs.size();i++)
    {
        if (Oprs[i].Name==token)return true;
    }
    return false;
}
int GetUnOpr(string token)
{
    for (unsigned i=0;i<UnOprs.size();i++)
    {
        if (UnOprs[i].Name==token)return i;
    }
    return -1;
}

int CheckIfClass(int Type)
{
    if (Cls[Type].Name=="STRING" || Cls[Type].Name=="INTEGER" || Cls[Type].Name=="BOOLEAN"
         || Cls[Type].Name=="DOUBLE" || Cls[Type].Name.substr(0,3)=="PTR")
         return false;
    return true;
}
int GetPd(string Opr)
{
    for (unsigned i=0;i<Oprs.size();i++)
    {
        if (Oprs[i].Name==Opr)return Oprs[i].Pd;
    }
    return -1;
}
bool CheckTypes(int cls1, int cls2)
{
    if (cls1==-1||cls2==-1)return false;
    if (cls1==cls2) return true;
    if (IsNumber(Cls[cls1].Name)&&IsNumber(Cls[cls2].Name))return true;
    return false;
}
int CheckUnOprType(int UnId, int Type)
{
    for (unsigned i=0;i<UnOprs.size();i++)
    {
        if (UnOprs[i].Name==UnOprs[UnId].Name)
        {
            if (CheckTypes(UnOprs[i].Cls,Type))
                return i;
        }
    }
    return -1;
}
int GetOprCls(string Opr, int Side, int Need, int LNeed)
{
    for (unsigned i=0;i<Oprs.size();i++)
    {
        if (Oprs[i].Name==Opr)
        {
            if (Side==LEFT && CheckTypes(Oprs[i].LtCls,Need))
                return Oprs[i].LtCls;
            else if (CheckTypes(Oprs[i].RtCls,Need))
            {
                if (LNeed!=-1)
                {
                    if (CheckTypes(Oprs[i].LtCls,LNeed))
                        return Oprs[i].RtCls;
                }
                else
                    return Oprs[i].RtCls;
            }
        }
    }
    return -1;
}
int GetRetOpr(string Opr,int LNeed,int RNeed)
{
    for (unsigned i=0;i<Oprs.size();i++)
    {
        if (Oprs[i].Name==Opr)
        {
            if (CheckTypes(Oprs[i].LtCls,LNeed)
                && CheckTypes(Oprs[i].LtCls,RNeed))
                return Oprs[i].RetCls;
        }
    }
    return -1;
}
unsigned long SzParam(string Function)
{
    return Funcs[GetFuncIndex(Function)].ParamSize;
}

void BuildClass()
{
    ClassInfo VClass;
    VClass.Name="INTEGER";
    VClass.Size=4;
    Cls.push_back(VClass);
    VClass.Name="DOUBLE";
    VClass.Size=8;
    Cls.push_back(VClass);
    VClass.Name="BYTE";
    VClass.Size=1;
    Cls.push_back(VClass);
    VClass.Name="SINGLE";
    VClass.Size=4;
    Cls.push_back(VClass);

    VClass.Name="STRING";
    VClass.Size=4;
    Cls.push_back(VClass);

    VClass.Name="BOOLEAN";
    VClass.Size=4;
    Cls.push_back(VClass);

    OprInfo opr;
    opr.Name="+";
    opr.RetCls=opr.RtCls=opr.LtCls=GetClass("DOUBLE");
    opr.Pd=3;
    Oprs.push_back(opr);
    opr.Name="-";
    Oprs.push_back(opr);
    opr.Name="*";
    opr.Pd=2;
    Oprs.push_back(opr);
    opr.Name="/";
    Oprs.push_back(opr);

    opr.Name="+";
    opr.RetCls=opr.RtCls=opr.LtCls=GetClass("STRING");
    opr.Pd=1;
    Oprs.push_back(opr);

    opr.Name="<";
    opr.Pd=4;
    opr.RetCls=GetClass("BOOLEAN");
    opr.LtCls=opr.RtCls=GetClass("DOUBLE");
    Oprs.push_back(opr);
    opr.LtCls=opr.RtCls=GetClass("STRING");
    Oprs.push_back(opr);
    opr.Name=">";
    Oprs.push_back(opr);
    opr.LtCls=opr.RtCls=GetClass("DOUBLE");
    Oprs.push_back(opr);
    opr.Name="<=";
    Oprs.push_back(opr);
    opr.LtCls=opr.RtCls=GetClass("STRING");
    Oprs.push_back(opr);
    opr.Name=">=";
    Oprs.push_back(opr);
    opr.LtCls=opr.RtCls=GetClass("DOUBLE");
    Oprs.push_back(opr);
    opr.Name="!=";
    Oprs.push_back(opr);
    opr.LtCls=opr.RtCls=GetClass("STRING");
    Oprs.push_back(opr);
    opr.LtCls=opr.RtCls=GetClass("BOOLEAN");
    Oprs.push_back(opr);
    opr.Name="==";
    Oprs.push_back(opr);
    opr.LtCls=opr.RtCls=GetClass("DOUBLE");
    Oprs.push_back(opr);
    opr.LtCls=opr.RtCls=GetClass("STRING");
    Oprs.push_back(opr);

    opr.Name="AND";
    opr.Pd=5;
    opr.RetCls=opr.LtCls=opr.RtCls=GetClass("BOOLEAN");
    Oprs.push_back(opr);
    opr.Name="OR";
    Oprs.push_back(opr);

    UnOprInfo uopr;
    uopr.Name="-";
    uopr.Cls=uopr.RtCls=GetClass("DOUBLE");
    UnOprs.push_back(uopr);

    uopr.Name="NOT";
    uopr.Cls=uopr.RtCls=GetClass("BOOLEAN");
    UnOprs.push_back(uopr);
}
