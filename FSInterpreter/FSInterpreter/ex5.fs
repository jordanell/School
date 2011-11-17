// Jordan Ell V00660306

module ex5
open Language
open System

exception InterpreterError

/////////////////////////////////////////////////////////////

let rec Search list string = 
    match list with
    | [] -> false
    | hd::tl -> if (fst (List.head list) = "***") then
                    false
                else
                    if (fst (List.head list) = string) then
                        printfn "Error: duplicate declaration for %A" string
                        raise InterpreterError
                        true
                    else
                        Search (List.tail list) string

/////////////////////////////////////////////////////////////

let rec RemoveTopScope list =
    match list with
    | [] -> list
    | hd::tl -> if (fst (List.head list) = "***") then
                    tl
                else
                    RemoveTopScope tl

let rec RemoveTopScopef funcs = 
    match funcs with
    | [] -> funcs
    | hd :: tl -> if (fst (List.head funcs) = "***") then
                      tl
                  else
                      RemoveTopScopef tl

/////////////////////////////////////////////////////////////

let NewScope list = 
    [("***", 0)] @ list

let NewScopef funcs = 
    [("***", Function("***",["***"],Declaration(["***"])))] @ funcs

//////////////////////////////////////////////////////////////

// Look Up function
let rec LookUp list value =
    match list with
    | [] -> printfn"Error: use of undeclared identifier %A" value
            raise InterpreterError
            None
    | hd::tl -> if (fst (List.head list) = value) then
                    Some (snd  (List.head list))
                else
                    LookUp (List.tail list) value

let rec LookUpf funcs name = 
    match funcs with
    |[] -> printfn "Error: function %A is not declared" name
           raise InterpreterError
           None
    | hd::tl -> if (fst (List.head funcs) = name) then
                    Some(snd(List.head funcs))
                else
                    LookUpf tl name


/////////////////////////////////////////////////////////////

let Add list string num =
        if (Search list string <> true) then
            ([string, num]) @ list
        else 
            list

let Addf funcs func = 
    match func with
    | Function(name,parms,body) -> [(name, func)] @ funcs
    | _ -> funcs

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

let rec SingleStep list funcs stmt =
    let rec AddVarList list vl =
        match vl with
        | [] -> list
        | [v] -> Add list v 0
        | v :: tail -> AddVarList (Add list v 0) tail

    let rec recInterpret list funcs stmts =
        match stmts with
        | [] -> list
        | hd::tail -> let a = SingleStep list funcs hd
                      recInterpret (fst(a)) (snd(a)) tail

    // Eval function
    let rec Eval list funcs exp = 
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

            let rec FunctionParams list parms args = 
                match parms with
                | [] -> list
                | hd::tl -> let list = Add list hd (Option.get (Eval list funcs (List.head args)))
                            FunctionParams list (List.tail parms) (List.tail args)

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
                                         printfn "Error: use of undeclared identifier %A" name
                                         raise InterpreterError
                                         None
                                     else
                                         Some(Option.get a)
            | InputExp -> printf "Enter a number: "
                          let num = Console.ReadLine()
                          Some(Convert.ToInt32 num)
            | FuncCall(name,args) -> let list = NewScope list
                                     let a = LookUpf funcs name
                                     let a = Option.get a
                                     match a with
                                     | Function(name,parms,body) -> let list = FunctionParams list parms args
                                                                    let list = NewScope list
                                                                    let funcs = NewScopef funcs
                                                                    let list = Add list "return" 0
                                                                    let x = SingleStep list funcs body
                                                                    let list = fst(x)
                                                                    let funcs = snd(x)
                                                                    let ret = Some(snd(List.head list))
                                                                    let funcs = RemoveTopScopef funcs
                                                                    let list = RemoveTopScope list
                                                                    ret

                                     | _ -> None
                                 

        EvalExp0 exp

    let rec OperateLoop list funcs exp body = 
        if((Option.get (Eval list funcs exp)) <> 0) then
            let b = SingleStep list funcs body
            OperateLoop (fst(b)) (snd(b)) exp body
        else
            (list, funcs)

    match stmt with
    | Declaration(lista) -> match lista with
                            | [] -> (list, funcs)
                            | hd::tl -> let list = AddVarList list lista
                                        (list, funcs)
                                        
    | Assignment(name,expression) -> let x = Eval list funcs expression
                                     if x = None then
                                        (list, funcs)
                                     else
                                        let a = LookUp list name
                                        if a = None then
                                            printfn "Error: name %A is undeclared" name
                                            raise InterpreterError
                                            (list, funcs)
                                        else
                                            let list = Update list name (Option.get x)
                                            (list, funcs)
    | Output(rhs) -> let x = (Eval list funcs rhs)
                     if x = None then
                        printfn "Output is None"
                        (list, funcs)
                     else
                        printfn "%A" (Option.get x)
                        (list, funcs)
    | If(exp,thenpart,elsepart) -> if((Option.get (Eval list funcs exp)) <> 0) then
                                       SingleStep list funcs thenpart
                                   else
                                       SingleStep list funcs elsepart
    | While(exp,body) -> let b = (OperateLoop list funcs exp body)
                         let list = fst(b)
                         let funcs = snd(b)
                         (list, funcs)
    | Block(stlst) -> let list = NewScope list
                      let funcs = NewScopef funcs
                      let list = recInterpret list funcs stlst
                      let funcs = RemoveTopScopef funcs
                      let list = RemoveTopScope list
                      (list, funcs)
    | Function(name,parms,body) -> let funcs = Addf funcs stmt
                                   (list, funcs)
    | Return(exp) -> let x = Eval list funcs exp
                     let list = Update list "return" (Option.get x)
                     (list, funcs)

/////////////////////////////////////////////////////////////

let Interpret stmts =
    let rec recInterpret list funcs stmts =
        match stmts with
        | [] -> list
        | hd::tail -> let a = SingleStep list funcs hd
                      recInterpret (fst(a)) (snd(a)) tail
    let rec printInterpret list = 
        match list with
        | [] -> printf ""
        | hd::tail -> printfn "%A = %A" (fst(hd)) (snd(hd))
                      printInterpret tail
    let a = recInterpret [] [] stmts
    printfn "Interpretation ended normally with\n"
    printInterpret a
