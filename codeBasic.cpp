#include "stdinc.h"
#include "parserMisc.h"
#include "codegen.h"


int ClassMember(Node* nd, int cls, string &str)
{
    string ofs = ToStr(GetMemOffset(cls, nd->Value));
    str+="+"+ofs;
    if (nd->Attribs[1]=="CLASS")
        return ClassMember(nd->Right, Cls[cls].Members[GetMember(cls, nd->Value)].Type,str);
    else
        return Cls[cls].Members[GetMember(cls, nd->Value)].Type;
}

void CodeBlnOpr(string Opr, string Lbl0, string Lbl1, bool Unsign)
{
    if (Opr=="<")
    {
        if (Unsign) AddCode("jb "+Lbl0);
        else AddCode("jl "+Lbl0);
    }
    else if (Opr==">")
    {
        if (Unsign) AddCode("ja "+Lbl0);
        else AddCode("jg "+Lbl0);
    }
    else if (Opr=="<=")
    {
        if (Unsign) AddCode("jbe "+Lbl0);
        else AddCode("jle "+Lbl0);
    }
    else if (Opr==">=")
    {
        if (Unsign) AddCode("jae "+Lbl0);
        else AddCode("jge "+Lbl0);
    }
    else if (Opr=="!=") AddCode("jne "+Lbl0);
    else if (Opr=="==") AddCode("je "+Lbl0);

    AddCode("push dword 0");
    AddCode("jmp "+Lbl1);
    AddCode(Lbl0+":");
    AddCode("push dword 1");
    AddCode(Lbl1+":");
}
void NumValue(Node* nd)
{
    if (nd->Attribs[0]=="INTEGER")
        AddCode("push dword " + nd->Value);
    else if (nd->Attribs[0]=="BOOLEAN")
    {
        if (nd->Value=="TRUE") AddCode("push dword 1");
        else AddCode("push dword 0");
    }
    else if (nd->Attribs[0]=="DOUBLE")
    {
        string var=GetTmp(nd->Value,QWORD);
        AddCode("push dword["+var+"]");
        AddCode("push dword["+var+"+4]");
    }
}
void NumVar(Node* nd)
{
    if (nd->Attribs[0]=="INTEGER" || nd->Attribs[0]=="BOOLEAN" || nd->Attribs[0].substr(0,3)=="PTR")
        AddCode("push dword["+GetAsmName(nd->Value)+"]");
    else if (nd->Attribs[0]=="DOUBLE")
    {
        AddCode("push dword["+GetAsmName(nd->Value)+"]");
        AddCode("push dword["+GetAsmName(nd->Value)+"+4]");
    }
}
void NumFunc(Node* nd)
{
    PassParameters(nd->Left);
    AddCode("call "+nd->Value+"ASM");
    if (nd->Attribs[0]=="DOUBLE")AddCode("FSTP qword[tmpDbl1]");
    CleanStack(nd->Left,SzParam(nd->Value));
    if (nd->Attribs[0]=="INTEGER" || nd->Attribs[0]=="BOOLEAN" || nd->Attribs[0].substr(0,3)=="PTR")
        AddCode("push ebx");
    else if (nd->Attribs[0]=="DOUBLE")
    {
        AddCode("push dword[tmpDbl1]");
        AddCode("push dword[tmpDbl1+4]");
    }
}
void NumClass(Node* nd)
{
    string str = GetAsmName(nd->Value);
    ClassMember(nd->Right,GetType(nd->Value),str);
    if (nd->Attribs[0]=="INTEGER" || nd->Attribs[0]=="BOOLEAN" || nd->Attribs[0].substr(0,3)=="PTR")
        AddCode("push dword["+str+"]");
    else if (nd->Attribs[0]=="DOUBLE")
    {
        AddCode("push dword["+str+"]");
        AddCode("push dword["+str+"+4]");
    }
}
void NumFuncClass(Node* nd)
{
    PassParameters(nd->Left);
    AddCode("call "+nd->Value+"ASM");
    CleanStack(nd->Left,SzParam(nd->Value));
    string str = "ebx";
    ClassMember(nd->Right,GetType(nd->Value),str);
    if (nd->Attribs[0]=="INTEGER" || nd->Attribs[0]=="BOOLEAN" || nd->Attribs[0].substr(0,3)=="PTR")
        AddCode("push dword["+str+"]");
    else if (nd->Attribs[0]=="DOUBLE")
    {
        AddCode("push dword["+str+"]");
        AddCode("push dword["+str+"+4]");
    }
}
void CodeNumBln(Node* nd)
{
    if (nd->Attribs[1]=="VALUE")
        NumValue(nd);
    else if (nd->Attribs[1]=="VARIABLE")
        NumVar(nd);
    else if (nd->Attribs[1]=="FUNCTION")
        NumFunc(nd);
    else if (nd->Attribs[1]=="CLASS")
        NumClass(nd);
    else if (nd->Attribs[1]=="FUNCTION_CLASS")
        NumFuncClass(nd);
}
void CodeBln(Node*nd)
{
    if (nd->Attribs[1]=="UNARY_OPERATOR" && nd->Value=="NOT")
    {
        CodeBln(nd->Left);
        AddCode("pop eax");
        AddCode("mov ebx,1");
        AddCode("sub ebx, eax");
        AddCode("push ebx");
    }
    else if (nd->Attribs[1]=="UNARY_OPERATOR" && nd->Value=="*")
    {
        CodeExpr(nd->Left,nd->Left->Attribs[0]);
        AddCode("pop eax");
        AddCode("push dword[eax]");
    }
    else if (nd->Attribs[1]=="OPERATOR")
    {
        if (nd->Value=="AND")
        {
            CodeBln(nd->Left);
            CodeBln(nd->Right);
            AddCode("pop eax");
            AddCode("pop ebx");
            AddCode("and eax, ebx");
            AddCode("push eax");
        }
        else if (nd->Value=="OR")
        {
            CodeBln(nd->Left);
            CodeBln(nd->Right);
            AddCode("pop eax");
            AddCode("pop ebx");
            AddCode("or eax, ebx");
            AddCode("push eax");
        }
        else
        {
            string Type = nd->Left->Attribs[0];
            CodeExpr(nd->Left,Type);
            CodeExpr(nd->Right,Type);

            string TmpLbl0=GetNewTmp();
            string TmpLbl1=GetNewTmp();
            if (Type=="INTEGER" || Type=="BOOLEAN")
            {
                AddCode("pop ebx");
                AddCode("pop eax");
                AddCode("cmp eax,ebx");
                CodeBlnOpr(nd->Value,TmpLbl0,TmpLbl1,false);
            }
            else if (Type=="DOUBLE")
            {
                AddCode("pop dword[tmpDbl1+4]");
                AddCode("pop dword[tmpDbl1]");
                AddCode("pop dword[tmpDbl2+4]");
                AddCode("pop dword[tmpDbl2]");

                AddCode("FLD qword[tmpDbl1]");
                AddCode("FLD qword[tmpDbl2]");
                AddCode("FCOMI ST1");
                CodeBlnOpr(nd->Value,TmpLbl0,TmpLbl1,true);
                AddCode("FINIT");
            }
            else if (Type=="STRING")
            {
                AddCode("call strcmp");
                AddCode("mov ebx,0");
                AddCode("cmp ebx,eax");
                CodeBlnOpr(nd->Value,TmpLbl0,TmpLbl1,false);
                // Save boolean result before cleaning strings
                AddCode("pop dword[tmpDbl1]");
                AddCode("call free");
                AddCode("add esp,4");
                AddCode("call free");
                AddCode("add esp,4");
                // Get the boolean result back
                AddCode("push dword[tmpDbl1]");
            }
        }
    }
    else
        CodeNumBln(nd);

}
void CodeInt(Node*nd)
{
    if (nd->Attribs[1]=="UNARY_OPERATOR" && nd->Value=="-")
    {
        CodeInt(nd->Left);
        AddCode("pop eax");
        AddCode("neg eax");
        AddCode("push eax");
    }
    else if (nd->Attribs[1]=="UNARY_OPERATOR" && nd->Value=="*")
    {
        CodeExpr(nd->Left,nd->Left->Attribs[0]);
        AddCode("pop eax");
        AddCode("push dword[eax]");
    }
    else if (nd->Attribs[1]=="OPERATOR")
    {
        CodeInt(nd->Left);
        CodeInt(nd->Right);
        AddCode("pop ebx");
        AddCode("pop eax");

        if (nd->Value=="+") AddCode("add eax,ebx");
        else if (nd->Value=="-") AddCode("sub eax,ebx");
        else if (nd->Value=="*") AddCode("imul eax,ebx");
        else if (nd->Value=="/") {AddCode("cdq"); AddCode("idiv ebx");}
        AddCode("push eax");
    }
    else
    {
        CodeNumBln(nd);
        if (nd->Attribs[0]=="DOUBLE")
        {
            AddCode("pop dword[tmpDbl1+4]");
            AddCode("pop dword[tmpDbl1]");
            AddCode("FLD qword[tmpDbl1]");
            AddCode("FISTP dword[tmpInt]");
            AddCode("push dword[tmpInt]");
        }
    }

}
void CodeDbl(Node*nd)
{
    if (nd->Attribs[1]=="UNARY_OPERATOR" && nd->Value=="-")
    {
        CodeDbl(nd->Left);
        AddCode("pop dword[tmpDbl1+4]");
        AddCode("pop dword[tmpDbl1]");

        AddCode("FLD qword[tmpDbl1]");
        AddCode("FCHS");

        AddCode("FSTP qword[tmpDbl1]");
        AddCode("push dword[tmpDbl1]");
        AddCode("push dword[tmpDbl1+4]");
    }
    else if (nd->Attribs[1]=="UNARY_OPERATOR" && nd->Value=="*")
    {
        CodeExpr(nd->Left,nd->Left->Attribs[0]);
        AddCode("pop eax");
        AddCode("push dword[eax]");
        AddCode("push dword[eax+4]");
    }
    else if (nd->Attribs[1]=="OPERATOR")
    {
        CodeDbl(nd->Left);
        CodeDbl(nd->Right);
        AddCode("pop dword[tmpDbl1+4]");
        AddCode("pop dword[tmpDbl1]");
        AddCode("pop dword[tmpDbl2+4]");
        AddCode("pop dword[tmpDbl2]");

        AddCode("FLD qword[tmpDbl2]");

        if (nd->Value=="+") AddCode("FADD qword[tmpDbl1]");
        else if (nd->Value=="-") AddCode("FSUB qword[tmpDbl1]");
        else if (nd->Value=="*") AddCode("FMUL qword[tmpDbl1]");
        else if (nd->Value=="/") AddCode("FDIV qword[tmpDbl1]");
        AddCode("FSTP qword[tmpDbl1]");
        AddCode("push dword[tmpDbl1]");
        AddCode("push dword[tmpDbl1+4]");

    }
    else
    {
        CodeNumBln(nd);
        if (nd->Attribs[0]=="INTEGER")
        {
            AddCode("pop dword[tmpInt]");
            AddCode("FILD dword[tmpInt]");
            AddCode("FSTP qword[tmpDbl1]");
            AddCode("push dword[tmpDbl1]");
            AddCode("push dword[tmpDbl1+4]");
        }
    }
}

void CodeStr(Node*nd)
{
    if (nd->Value=="+")
    {
        CodeStr(nd->Left);
        CodeStr(nd->Right);
    }
    else
    {
        if (nd->Attribs[1]=="UNARY_OPERATOR" && nd->Value=="*")
        {
            CodeExpr(nd->Left,nd->Left->Attribs[0]);
            AddCode("pop eax");
            AddCode("push dword[eax]");
        }
        else
            AddCode("push "+ nd->Attribs[2]);

        AddCode("push dword[tmpStr]");
        AddCode("call strcat");
        AddCode("add esp, 8");
        if (nd->Attribs[1]=="FUNCTION")
        {
            AddCode("push "+nd->Attribs[2]);
            AddCode("call free");
            AddCode("add esp,4");
        }
    }
}
void StrLen(Node*& nd)
{
    if (nd->Value=="+")
    {
        StrLen(nd->Left);
        StrLen(nd->Right);
        AddCode("pop eax");
        AddCode("pop ebx");
        AddCode("add eax,ebx");
        AddCode("dec eax");
        AddCode("push eax");
    }
    else
    {
        string varName;
        if (nd->Attribs[1]=="VALUE")
        {
            varName = GetTmp("'"+nd->Value+"',0",BYTE);
            nd->Attribs[2]=varName;
        }
        else if (nd->Attribs[1]=="VARIABLE")
        {
            varName="dword["+GetAsmName(nd->Value)+"]";
            nd->Attribs[2]=varName;
        }
        else if (nd->Attribs[1]=="FUNCTION")
        {
            PassParameters(nd->Left);
            AddCode("call "+nd->Value+"ASM");
            CleanStack(nd->Left,SzParam(nd->Value));

            string tmp = GetNewTmp();
            AddData(tmp +" dd 0");
            AddCode("mov dword["+tmp+"],ebx");
            varName = "dword["+tmp+"]";
            nd->Attribs[2]=varName;
        }
        else if (nd->Attribs[1]=="CLASS")
        {
            varName = GetAsmName(nd->Value);
            ClassMember(nd->Right,GetType(nd->Value),varName);
            varName = "dword["+varName+"]";
            nd->Attribs[2]=varName;
        }
        else if (nd->Attribs[1]=="FUNCTION_CLASS")
        {
            PassParameters(nd->Left);
            AddCode("call "+nd->Value+"ASM");
            CleanStack(nd->Left,SzParam(nd->Value));

            string tmp = GetNewTmp();
            AddData(tmp +" dd 0");

            varName = "ebx";
            ClassMember(nd->Right,GetType(nd->Value),varName);
            AddCode("mov eax,["+varName+"]");
            AddCode("mov dword["+tmp+"],eax");

            varName = "dword["+tmp+"]";
            nd->Attribs[2]=varName;
        }
        else if (nd->Attribs[1]=="UNARY_OPERATOR" && nd->Value=="*")
        {
            CodeExpr(nd->Left,nd->Left->Attribs[0]);
            AddCode("pop eax");
            varName = "dword[eax]";
        }

        AddCode("push "+ varName);
        AddCode("call strlen");
        AddCode("add esp, 4");
        AddCode("push eax");
    }
}

void PushCls(string Name, int cls)
{
    int offset=Cls[cls].Size-4;
    int i=Cls[cls].Members.size()-1;
    while (i>=0)
    {
        string varName = Name+"+"+ToStr(offset);
        if (Cls[cls].Members[i].Type==GetClass("STRING"))
        {
            AddCode("push dword 1");
            AddCode("push dword["+varName+"]");
            AddCode("call strlen");
            AddCode("add esp,4");
            AddCode("push eax");
            AddCode("call calloc");
            AddCode("add esp,8");
            AddCode("mov dword[tmpStr],eax");
            AddCode("push dword["+varName+"]");
            AddCode("push dword[tmpStr]");
            AddCode("call strcat");
            AddCode("add esp, 8");
            AddCode("push dword[tmpStr]");
        }
        else if (Cls[cls].Members[i].Type==GetClass("DOUBLE"))
        {
            offset-=4;
            varName = Name+"+"+ToStr(offset);
            AddCode("push dword["+varName+"+4]");
            AddCode("push dword["+varName+"]");
        }
        else if (CheckIfClass(Cls[cls].Members[i].Type))
        {
            offset-=Cls[Cls[cls].Members[i].Type].Size-4;
            varName = Name+"+"+ToStr(offset);
            PushCls(varName,Cls[cls].Members[i].Type);
        }
        else
            AddCode("push dword["+varName+"]");
        offset-=4;
        i--;
    }
}

void CodeCls(Node*& nd)
{
    if (nd->Attribs[1]=="VARIABLE")
    {
        string varName = GetAsmName(nd->Value);
        PushCls(varName, GetType(nd->Value));
    }
    else if (nd->Attribs[1]=="FUNCTION")
    {
        PassParameters(nd->Left);
        AddCode("call "+nd->Value+"ASM");
        CleanStack(nd->Left,SzParam(nd->Value));
        PushCls("ebx", GetType(nd->Value));
        FreeCls("ebx", GetType(nd->Value));
        AddCode("push ebx");
        AddCode("call free");
        AddCode("add esp,4");
    }
    else if (nd->Attribs[1]=="CLASS")
    {
        string str = GetAsmName(nd->Value);
        int type = ClassMember(nd->Right,GetType(nd->Value),str);
        PushCls(str, type);
    }
    else if (nd->Attribs[1]=="FUNCTION_CLASS")
    {
        PassParameters(nd->Left);
        AddCode("call "+nd->Value+"ASM");
        CleanStack(nd->Left,SzParam(nd->Value));
        string str="ebx";
        int type = ClassMember(nd->Right,GetType(nd->Value),str);
        PushCls(str,type);
        FreeCls("ebx", GetType(nd->Value));
        AddCode("push ebx");
        AddCode("call free");
        AddCode("add esp,4");
    }
}

