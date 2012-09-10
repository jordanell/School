module ex0
open ex1
open Language

let Run file =
    ex5.Interpret (ReadProgram file)
    

