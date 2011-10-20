module main
open System

let testList = [1;3;5;]
let testList2 = [2;4;6]
let testList3 = [1;6;3;5;2;4;]

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
    Console.ReadKey()

