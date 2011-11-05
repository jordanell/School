module ex0
open ex1
open Language

let Run file =
    ex4.Interpret (ReadProgram file)
    

