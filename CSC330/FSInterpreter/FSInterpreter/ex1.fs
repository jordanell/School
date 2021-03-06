﻿module ex1
open Language
open System

//////////////////////////////////////////////////////////////

// Look Up function
let rec LookUp list value =
    match list with
    | [] -> None
    | hd::tl -> if (fst (List.head list) = value) then
                    Some (snd  (List.head list))
                else
                    LookUp (List.tail list) value


/////////////////////////////////////////////////////////////

// Eval function
let Eval list exp = 
    let rec EvalExp0 exp =

        let EvalBinaryToken t left right =
            match t with
            | AddOp -> Option.get left + Option.get right
            | SubOp -> Option.get left - Option.get right
            | MulOp -> Option.get left * Option.get right
            | DivOp -> Option.get left / Option.get right
            | _ -> 0

        let EvalUnaryToken t left =
            match t with
            | SubOp -> Option.get left * -1
            | _ -> 0

        match exp with
        | BinaryOpExp(t,left,right) ->
                    let l = EvalExp0 left
                    let r = EvalExp0 right
                    if (l <> None && r <> None) then
                        Some(EvalBinaryToken t (l) (r))
                    else
                        None
        | UnaryOpExp(t,opnd) -> Some(EvalUnaryToken t (EvalExp0 opnd))
        | IntConstExp(n) -> Some(n)
        | IdentifierExp(name) -> let a = (LookUp list name)
                                 if ( a = Option.None ) then
                                     printfn "Error: identifier %A is unbound" name
                                     None
                                 else
                                     Some(Option.get a)
        | InputExp -> printf "Enter a number: "
                      let num = Console.ReadLine()
                      Some(Convert.ToInt32 num)

    EvalExp0 exp

/////////////////////////////////////////////////////////////

// Update function
let rec Update list string num =
    match list with
    | [] -> list
    | hd::tl -> if (fst (List.head list) = string) then
                    [(string, num)] @ (List.tail list)
                else
                    [(List.head list)] @ Update (List.tail list) string num

 
/////////////////////////////////////////////////////////////

let Add list string num =
    ([string, num]) @ list

let rec SingleStep list stmt =
    let rec AddVarList list vl =
        match vl with
        | [] -> list
        | [v] -> Add list v 0
        | v :: tail -> AddVarList (Add list v 0) tail

    match stmt with
    | Declaration(lista) -> match lista with
                            | [] -> list
                            | hd::tl -> AddVarList list lista
                                        
    | Assignment(name,expression) -> let x = Eval list expression
                                     if x = None then
                                        list
                                     else
                                        let a = LookUp list name
                                        if a = None then
                                            printfn "Error: name %A is undeclared" name
                                            list
                                        else
                                            Update list name (Option.get x)
    | Output(rhs) -> let x = (Eval list rhs)
                     if x = None then
                        printfn "Output is None"
                        list
                     else
                        printfn "%A" (Option.get x)
                        list


/////////////////////////////////////////////////////////////

let Interpret stmts =
    let rec recInterpret list stmts =
        match stmts with
        | [] -> list
        | hd::tail -> let list = SingleStep list hd
                      recInterpret list tail
    let rec printInterpret list = 
        match list with
        | [] -> printf ""
        | hd::tail -> printfn "%A = %A" (fst(hd)) (snd(hd))
                      printInterpret tail
    let a = recInterpret [] stmts
    printfn "Interpretation ended normally with\n"
    printInterpret a