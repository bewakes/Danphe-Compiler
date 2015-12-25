extern calloc
extern free
extern exit
extern strcpy
extern strcat
extern strlen
extern strcmp
extern printf



section .data
 istr db '%d',0
 dstr db '%lf',0
 tmpStr dd 0
 tmpInt dd 0
 tmpDbl1 dq 0.0
 tmpDbl2 dq 0.0

tmp0 dq 2.6
tmp1 db ' hi ',0
tmp2 db 'tie',0
tmp3 dd 0
tmp4 db ' lasso ',0

section .bss

TUTASM  resb  32
BUTASM  resb  32
SUTASM  resb  16
ABASM  resb  4
AASM  resb  8

section .text
 global START
START:



push dword 1
push dword 1
call calloc
add esp,8
mov dword[TUTASM+12+12],eax
add esp, 4
push dword 1
push dword 1
call calloc
add esp,8
mov dword[TUTASM+28],eax
add esp, 4

push dword 1
push dword 1
call calloc
add esp,8
mov dword[BUTASM+12+12],eax
add esp, 4
push dword 1
push dword 1
call calloc
add esp,8
mov dword[BUTASM+28],eax
add esp, 4

push dword 1
push dword 1
call calloc
add esp,8
mov dword[SUTASM+12],eax
add esp, 4

push dword 1
push dword 1
call calloc
add esp, 8
mov dword[ABASM], eax

push dword[tmp0]
push dword[tmp0+4]
pop dword[TUTASM+4+4]
pop dword[TUTASM+4]

push dword 1
push tmp1
call strlen
add esp, 4
push eax
call calloc
add esp, 8
mov dword[tmpStr],eax
push tmp1
push dword[tmpStr]
call strcat
add esp, 8
push dword[tmpStr]
push dword[TUTASM+28]
call free
add esp, 4
pop dword[TUTASM+28]

push dword 1
push tmp2
call strlen
add esp, 4
push eax
call calloc
add esp, 8
mov dword[tmpStr],eax
push tmp2
push dword[tmpStr]
call strcat
add esp, 8
push dword[tmpStr]
push dword[TUTASM+12]
call free
add esp, 4
pop dword[TUTASM+12]

push dword 1
push dword[TUTASM+28]
call strlen
add esp, 4
push eax
call calloc
add esp, 8
mov dword[tmpStr],eax
push dword[TUTASM+28]
push dword[tmpStr]
call strcat
add esp, 8
push dword[tmpStr]
push dword[ABASM]
call free
add esp, 4
pop dword[ABASM]

push dword[ABASM]
call printf
add esp, 4

push dword 1
push dword[TUTASM+28]
call strlen
add esp,4
push eax
call calloc
add esp,8
mov dword[tmpStr],eax
push dword[TUTASM+28]
push dword[tmpStr]
call strcat
add esp, 8
push dword[tmpStr]
push dword 1
push dword[TUTASM+12+12]
call strlen
add esp,4
push eax
call calloc
add esp,8
mov dword[tmpStr],eax
push dword[TUTASM+12+12]
push dword[tmpStr]
call strcat
add esp, 8
push dword[tmpStr]
push dword[TUTASM+12+4+4]
push dword[TUTASM+12+4]
push dword[TUTASM+12+0]
push dword[TUTASM+4+4]
push dword[TUTASM+4]
push dword[TUTASM+0]
pop dword[BUTASM+0]
pop dword[BUTASM+4]
pop dword[BUTASM+4+4]
pop dword[BUTASM+12+0]
pop dword[BUTASM+12+4]
pop dword[BUTASM+12+4+4]
push dword[BUTASM+12+12]
call free
add esp, 4
pop dword[BUTASM+12+12]
push dword[BUTASM+28]
call free
add esp, 4
pop dword[BUTASM+28]

push dword 1
push dword[BUTASM+28]
call strlen
add esp, 4
push eax
call calloc
add esp, 8
mov dword[tmpStr],eax
push dword[BUTASM+28]
push dword[tmpStr]
call strcat
add esp, 8
push dword[tmpStr]
push dword[ABASM]
call free
add esp, 4
pop dword[ABASM]

push dword[ABASM]
call printf
add esp, 4

push dword 1
push dword[BUTASM+12]
call strlen
add esp, 4
push eax
call calloc
add esp, 8
mov dword[tmpStr],eax
push dword[BUTASM+12]
push dword[tmpStr]
call strcat
add esp, 8
push dword[tmpStr]
push dword[ABASM]
call free
add esp, 4
pop dword[ABASM]

push dword[ABASM]
call printf
add esp, 4


push dword[BUTASM+4]
push dword[BUTASM+4+4]
pop dword[AASM+4]
pop dword[AASM]

push dword[AASM+4]
push dword[AASM]
push dstr
call printf
add esp, 8

push dword 1
push dword 1
push dword[ABASM]
call strlen
add esp, 4
push eax
call calloc
add esp, 8
mov dword[tmpStr],eax
push dword[ABASM]
push dword[tmpStr]
call strcat
add esp, 8
push dword[tmpStr]
push dword 1
push dword[TUTASM+28]
call strlen
add esp,4
push eax
call calloc
add esp,8
mov dword[tmpStr],eax
push dword[TUTASM+28]
push dword[tmpStr]
call strcat
add esp, 8
push dword[tmpStr]
push dword 1
push dword[TUTASM+12+12]
call strlen
add esp,4
push eax
call calloc
add esp,8
mov dword[tmpStr],eax
push dword[TUTASM+12+12]
push dword[tmpStr]
call strcat
add esp, 8
push dword[tmpStr]
push dword[TUTASM+12+4+4]
push dword[TUTASM+12+4]
push dword[TUTASM+12+0]
push dword[TUTASM+4+4]
push dword[TUTASM+4]
push dword[TUTASM+0]
call TAPAPASM
mov ebx, eax
push dword[esp+32]
push dword[esp+24]
push dword[esp+28]
call free
add esp, 4
call free
add esp, 4
call free
add esp, 4
add esp,36
mov eax,[ebx+28]
mov dword[tmp3],eax
push dword[tmp3]
call strlen
add esp, 4
push eax
call calloc
add esp, 8
mov dword[tmpStr],eax
push dword[tmp3]
push dword[tmpStr]
call strcat
add esp, 8
push dword[tmpStr]
push dword[ABASM]
call free
add esp, 4
pop dword[ABASM]

push dword[ABASM]
call printf
add esp, 4

push dword 1
push dword[BUTASM+12+12]
call strlen
add esp,4
push eax
call calloc
add esp,8
mov dword[tmpStr],eax
push dword[BUTASM+12+12]
push dword[tmpStr]
call strcat
add esp, 8
push dword[tmpStr]
push dword[BUTASM+12+4+4]
push dword[BUTASM+12+4]
push dword[BUTASM+12+0]
pop dword[SUTASM+0]
pop dword[SUTASM+4]
pop dword[SUTASM+4+4]
push dword[SUTASM+12]
call free
add esp, 4
pop dword[SUTASM+12]

push dword 1
push dword[BUTASM+12]
call strlen
add esp, 4
push eax
call calloc
add esp, 8
mov dword[tmpStr],eax
push dword[BUTASM+12]
push dword[tmpStr]
call strcat
add esp, 8
push dword[tmpStr]
push dword[ABASM]
call free
add esp, 4
pop dword[ABASM]

push dword[ABASM]
call printf
add esp, 4




push dword[TUTASM+12+12]
call free
push dword[TUTASM+28]
call free
push dword[BUTASM+12+12]
call free
push dword[BUTASM+28]
call free
push dword[SUTASM+12]
call free
push dword[ABASM]
call free
add esp, 4
push 0
call exit
add esp, 4

TAPAPASM:
enter 0,0

push dword 1
push tmp4
call strlen
add esp, 4
push eax
call calloc
add esp, 8
mov dword[tmpStr],eax
push tmp4
push dword[tmpStr]
call strcat
add esp, 8
push dword[tmpStr]
push dword[ebp+8+0+28]
call free
add esp, 4
pop dword[ebp+8+0+28]

push dword 1
push dword[ebp+8+0+28]
call strlen
add esp,4
push eax
call calloc
add esp,8
mov dword[tmpStr],eax
push dword[ebp+8+0+28]
push dword[tmpStr]
call strcat
add esp, 8
push dword[tmpStr]
push dword 1
push dword[ebp+8+0+12+12]
call strlen
add esp,4
push eax
call calloc
add esp,8
mov dword[tmpStr],eax
push dword[ebp+8+0+12+12]
push dword[tmpStr]
call strcat
add esp, 8
push dword[tmpStr]
push dword[ebp+8+0+12+4+4]
push dword[ebp+8+0+12+4]
push dword[ebp+8+0+12+0]
push dword[ebp+8+0+4+4]
push dword[ebp+8+0+4]
push dword[ebp+8+0+0]
push dword 1
push dword 32
call calloc
add esp,8
mov ebx, eax
pop dword[ebx+0]
pop dword[ebx+4]
pop dword[ebx+4+4]
pop dword[ebx+12+0]
pop dword[ebx+12+4]
pop dword[ebx+12+4+4]
pop dword[ebx+12+12]
pop dword[ebx+28]
mov eax, ebx
leave
ret

