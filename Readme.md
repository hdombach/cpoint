# CPoint

This repo contains an interpreter for an esoteric language called cpoint.
CPoint also known as `C*` is a language based solely around pointers.
The goal is to isolate the aspects of pointers that make them so great in
other languages like `C`. These apsects include their flexibility, intuitiveness,
and legibility.

# Running
Build and run hello world example
```
./setup.sh
cmake --build build
build/cpoint examples/hello_world.cp
```

Build and run fibonacci sequence example in decimal mode
```
./setup.sh
cmake --build build
build/cpoint examples/hello_world.cp
```

Get help message
```
build/cpoint --help
```


# Language Overview
In `C*`, all the functionality is done through pointers and there associated types. There
are a collection of different pointers that have side effects when you
dereference them. It is through these side effects that you can get the functionality
of any other language.

The full list of pointers are listed below.

| Name | Syntax | Description |
| ---- | ------ | ------- |
| Normal | `int *value;` | No side effect |
| Incriment | `int +value;` | Incriments the variable that is pointed too. |
| Decriment | `int -value;` | Decriment the variable that is pointed too. |
| Jump pointer | `int value^;` | Sets the program counter to the address value. Effectively starts running from the variable pointed too. |
| Ternary | `int value?;` | Will only dereference if the value before it is 0. |
| Read pointer | `int >>value;` | Read a value from stdin. Doesn't use the value that it is initialized with. |
| Read pointer | `int <<value;` | Write the value it was initialized with to stdout. It will return a normal dereference which in most situations won't be helpful. |

**Misc Notes:**

- A program consists of a series of variable declarations. Not every variable needs
  to be initialized however the only valid expression is pointer dereferencing.
- Every pointer is a 32 bit signed integer.
- Address values reference consective integers (as opposed to the bits inside an
  integer). This makes it easy to use address for miscellaneous values. For example, since
  numeric constants like `5` are not allowed, you can just take the reference
  of the sixth variable.
- The program counter corresponds to the address of the variable that is being
  processed.
- Variables without an initialization are set to 0.
- Variables that take the address of another variable (`int *value = &ptr;`)
  are initialized before the program starts.
- Dereferencing `0` will cause the program to halt.

## Computation Class
`C*` is turing complete. This can be shown by creating an equivilance relation to
the BF programming language.

Setup: in order to setup the tape, you can either incriment a pointer the to middle
of nowhere or create a program with many empty variables.

Option 1:
```
int *tape;
int ++++++++++++ ... ++++++shift_tape = &tape;
int *_tmp = ******** ... *******shift_tape;
```

Option 2:
```
int &tape = &tape_start;
... x many many variables
int *tape_start;
```

\pagebreak

**Equivilance to BF:**

| BF | `C*` |
| -- | ---- |
| `>` | `int +mv_right = &tape;`  |
|     | `int *tmp = *mv_right;`  |
| `<` | `int +mv_left = &tape;`  |
|     | `int *tmp = *mv_left;`  |
| `+` | `int *+inc = &tape;` |
|     | `int *val = **inc;`  |
| `-` | `int *+dec = &tape;` |
|     | `int *val = **dec;`  |
| `.` | `int *<<output = &tape;` |
|     | `int *val = *output;`    |
| `.` | `int *<<output = &tape;` |
|     | `int *val = *output;`    |
| `,` | `int >>input;` |
|     | `int *value_in = *input;` |
|     | `Int –decrease_in = &value_in;` |
|     | `int *begin_reset = &begin_copy;` |
|     | `int *val =* &decrease_tape;` |
|     | `int jump^? = &begin_reset;` |
|     | `int *val2 = **jump;` |
|     | `int *begin_copy = &end_copy;` |
|     | `int *val3 = *increase_tape;` |
|     | `int *val4 = *decrease_in;` |
|     | `int jump2^? &begin_copy;` |
|     | `int *end_copy;` |
| `[` | `int *value= *tape;` |
|     | `int jump^?=&left_bracket;` |
|     | `int *value2 = **jump;` |
|     | `int *left_bracket=&right_bracket;` |
| `]` | `int *back^=&value;` |
|     | `int *_temp = *back;` |
|     | `int *right_bracket;` |

These commands also show how you could construct some more complicated logic
such as chaining together jump and ternary pointers in order to form an if statement.


