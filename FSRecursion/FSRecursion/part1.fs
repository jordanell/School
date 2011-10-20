// CSC 330 Assignment 2
module part1
open System

//////////////////////////////////////////////////////////////

// Does the actual recussion for AllRotations
let rec RecursiveRotations list appendlist length = 
    if length = 0 then
        appendlist
    else
        let newList = List.append (List.tail list) ([List.head list])
        RecursiveRotations newList (newList :: appendlist) (length-1)

// All Rotations function
let AllRotations list =
   let length = List.length list
   RecursiveRotations list [] length

/////////////////////////////////////////////////////////////

// Distrubutes e over all possitions of L
let distrib e list =
    let rec aux pre post = 
        seq {
            match post with
            | [] -> yield (list @ [e])
            | h::t -> yield (List.rev pre @ [e] @ post)
                      yield! aux (h::pre) t 
        }
    aux [] list

// Does the actual recursion for All Permutations
let rec RecursivePermutations = function 
    | [] -> Seq.singleton []
    | h::t -> Seq.collect (distrib h) (RecursivePermutations t)

// All Permutations function
let AllPerms list  = 
    Seq.toList (RecursivePermutations list)

////////////////////////////////////////////////////////////

let OddEven list =
    




