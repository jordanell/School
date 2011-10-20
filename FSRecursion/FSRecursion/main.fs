module main
open System

let testList = [1;2;3;]

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
    Console.ReadKey()

