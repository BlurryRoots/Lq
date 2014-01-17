# Lq - A lisp like language

## What does Lq mean?

The name **Lq** is derived from *Lisp like language* which could be abbreviated with L³, which is spoken like [elkjubd] which kinda sounds like **Lq**. Not the most creative way to name this project, but it'll do for now.

## Why reinventing the wheel?

Well i'm very interested in how programming languages work. So i figured one of the best ideas to get a glimbs of the inner workings of a programming language, is to write one for yourself.

## Disclaimer

This is not my first attemt to create a programming language, but i still consider myself relativly new to the field. Hence this project does not really attempt to be something actually used in production. I think you technically could, but i would strongly advice you to use something more elaborated and mature, like [newLISP](http://www.newlisp.org/).

## What features does Lq have?

These are the main components.

<table>
  <tr>
    <th>Component</th><th>Description</th>
  </tr>
  <tr>
    <td>Lexer/Scanner</td><td>Responsible for taking a piece of code and produce a collection of tokens.</td>
  </tr>
  <tr>
    <td>Parser</td><td>Takes a collection of tokens and tries to create a parse tree.</td>
  </tr>
  <tr>
    <td>TODO: Interpreter</td><td>Takes a parse tree and tries to interpret it.</td>
  </tr>
</table>
