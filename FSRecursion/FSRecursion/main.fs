module main
open System
open System.Text.RegularExpressions
open Language

let testList = [1;3;5;]
let testList2 = [2;4;6]
let testList3 = [1;6;3;5;2;4;]

let vals = [ ("Num", 37); ("x", 42); ("y", -1); ("Num", 11) ]

//TEST VALUES
let testExp = [ Language.IntConst(99); Language.AddOp; Language.IntConst(40); Language.EOF ]

let exp1 = [ Identifier("x"); MulOp; IntConst(99); AddOp; Identifier("x"); SubOp; IntConst(37); DivOp; SubOp; Identifier("x"); EOF ]
let exp2 = [ Language.Identifier("x"); Language.MulOp; Language.Identifier("foo"); Language.EOF ]
let (expTree1,_) = Language.ParseExp exp1
let (expTree2,_) = Language.ParseExp exp2

let testStmt1 = [ VarKeyword; Identifier("a"); Comma; Identifier("b"); Semicolon; EOF ]

let main =
    Console.WriteLine("Running FSRecurssion...");
    Console.WriteLine("Part1, Excercise 1")
    let a = part1.AllRotations testList
    printfn "%A" a
    Console.WriteLine("Excercise 1 done.");
    Console.WriteLine("Part1, Excercise 2")
    let a = part1.AllPerms testList
    printfn "%A" a
    Console.WriteLine("Excercise 2 done.");
    Console.WriteLine("Part1, Excercise 3")
    let a = part1.OddEven testList
    printfn "%A" a
    Console.WriteLine("Excercise 3 done.");
    Console.WriteLine("Part1, Excercise 4")
    let a = part1.Merge testList testList2
    printfn "%A" a
    Console.WriteLine("Excercise 4 done.");
    Console.WriteLine("Part1, Excercise 5")
    let a = part1.MergeSort testList3
    printfn "%A" a
    Console.WriteLine("Excercise 5 done.");

    /////////////////////////////////////////////////////

    Console.WriteLine("Part2, Excercise 6")
    let a = part2.LookUp vals "Foo"
    printfn "%A" a
    Console.WriteLine("Excercise 6 done.");
    Console.WriteLine("Part2, Excercise 7")
    let a = part2.Eval vals expTree1
    printfn "%A" a
    Console.WriteLine("Excercise 7 done.");
    Console.WriteLine("Part2, Excercise 8")
    let a = part2.Update vals "foo" 666
    printfn "%A" a
    Console.WriteLine("Excercise 8 done.");

    ////////////////////////////////////////////////////

    Console.WriteLine("Part2, Excercise 9")
    let a = Convert   "foo = x*666;"
    let b = part2.SingleStep vals a
    printfn "%A" b
    Console.WriteLine("Excercise 9 done.");

    Console.ReadKey()

