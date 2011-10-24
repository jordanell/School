// CSC 330 Assignment 2
module part2
open System
open Language

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
                        Some(EvalBinaryToken t (EvalExp0 left) (EvalExp0 right))
                    else
                        None
        | UnaryOpExp(t,opnd) -> Some(EvalUnaryToken t (EvalExp0 opnd))
        | IntConstExp(n) -> Some(n)
        | IdentifierExp(name) -> let a = (LookUp list name)
                                 if ( a = Option.None ) then
                                     printf "Error: identifier %A is unbound" name
                                     None
                                 else
                                     Some(Option.get a)

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
