﻿module main
open System
open System.Text.RegularExpressions
open Language

let testList1 = [1;2;3;]
let testList0 = [];

let testList = [1;3;5;]
let testList2 = [2;4;6]
let testList3 = [1;6;3;5;2;4;]

let vals = [ ("Num", 37); ("x", 42); ("y", -1); ("Num", 11) ]

//TEST VALUES
let testExp = [ Language.IntConst(99); Language.AddOp; Language.IntConst(40); Language.EOF ]

let exp1 = [ Identifier("x"); MulOp; IntConst(10); AddOp; Identifier("y"); EOF ]
let exp2 = [ Language.Identifier("x"); Language.MulOp; Language.Identifier("foo"); Language.EOF ]
let (expTree1,_) = Language.ParseExp exp1
let (expTree2,_) = Language.ParseExp exp2

let testStmt1 = [ VarKeyword; Identifier("a"); Comma; Identifier("b"); Semicolon; EOF ]


let sl1 = Convert "x = Num * 10 ;"
let sl2 = Convert "var a, x;"
let sl3 = Convert "foo = x*666;"

let main =
    Console.WriteLine("Running FSRecurssion...");
    Console.WriteLine("Part1, Excercise 1")
    let a = part1.AllRotations testList1
    printfn "%A" a
    Console.WriteLine("Excercise 1 done.");
    Console.WriteLine("Part1, Excercise 2")
    let a = part1.AllPerms testList1
    printfn "%A" a
    Console.WriteLine("Excercise 2 done.");
    Console.WriteLine("Part1, Excercise 3")
    let a = part1.OddEven testList1
    printfn "%A" a
    Console.WriteLine("Excercise 3 done.");
    Console.WriteLine("Part1, Excercise 4")
    let a = part1.Merge testList testList2
    printfn "%A" a
    Console.WriteLine("Excercise 4 done.");
    Console.WriteLine("Part1, Excercise 5")
    let a = part1.MergeSort testList
    printfn "%A" a
    Console.WriteLine("Excercise 5 done.");

    /////////////////////////////////////////////////////

    Console.WriteLine("Part2, Excercise 6")
    let a = part2.LookUp vals "Num"
    printfn "%A" a
    Console.WriteLine("Excercise 6 done.");

    Console.WriteLine("Part2, Excercise 7")
    let a = part2.Eval vals expTree1
    printfn "%A" a
    Console.WriteLine("Excercise 7 done.");

    ////////////////////////////////////////////////////

    Console.WriteLine("Part2, Excercise 8")
    let a = part2.Update vals "foo" 666
    printfn "%A" a
    Console.WriteLine("Excercise 8 done.");

    Console.WriteLine("Part2, Excercise 9")
    let a = Convert   "foo = x*666;"
    let b = part2.SingleStep vals sl3
    printfn "%A" b
    Console.WriteLine("Excercise 9 done.");

    ///////////////////////////////////////////////////

    Console.WriteLine("Part2, Excercise 10")
    let code = ReadProgram () ;
    let a = part2.Interpret code
    //printfn "%A" a
    Console.WriteLine("Excercise 10 done.");

    Console.ReadKey()

