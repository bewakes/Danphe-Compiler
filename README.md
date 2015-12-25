# Danphe-Compiler

A simple compiler for a custom programming language.

The compiler is quite unoptimized but shows how to parse and translate a high level language program to low level language.

The language supports:

- integer, double, boolean and string types

```
integer a
double b
string c
boolean d
```

- Structures (user defined types)

```
structure mystruc
{
  integer myInt
  string myStr
}
mystruc x
x.myInt = 23
```

- Simple I/O

```
write a
read b
```

- User Defined Functions

```
integer MyFunc(mystruc a, integer b)
{
  return a.myInt + b
}
```

- pointers

```
string *a
string **b
b = &a
string c
c = **b
```

and many more.


> The project is currently discontinued but is helpful for any beginners to learn through code about basic compiler development.
