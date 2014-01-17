<Start> -> <Program> .

<Program> -> <empty> .
<Program> -> number <Program> .
<Program> -> string <Program> .
<Program> -> symbol <Program> .
<Program> -> <Bool> <Program> .
<Program> -> <List> <Program> .
<Program> -> quote <Quoted> .

<Quoted> -> symbol <Program> .
<Quoted> -> <List> <Program> .

<Bool> -> true .
<Bool> -> false .

<List> -> ( <Program> ) .
