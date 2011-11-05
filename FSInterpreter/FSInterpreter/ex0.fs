module ex0
open ex1
open Language

let Run file =
    ex1.Interpret (ReadProgram file)
    

