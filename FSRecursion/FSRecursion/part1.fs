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

// Does the actual recursion for the even and odd function
let rec RecursiveOddEven list even odd =
    match list with
    | [] -> (even, odd)
    | hd::tl -> if ((List.head list) % 2 = 0) then
                    RecursiveOddEven (List.tail list) (even @ [(List.head list)]) odd
                else
                    RecursiveOddEven (List.tail list) even (odd @ [(List.head list)])

// OddEven function
let OddEven list =
    RecursiveOddEven list [] []

////////////////////////////////////////////////////////////

// Actual recursion for the merge function
let rec RecursionMerge final lista listb =
    match lista with
    | [] -> List.append final listb
    | hd::tl -> if (listb = []) then
                    List.append final lista
                else if ((List.head lista) <= (List.head listb)) then
                    RecursionMerge (final @ [(List.head lista)]) (List.tail lista) listb
                else
                    RecursionMerge (final @ [(List.head listb)]) lista (List.tail listb)

// Merge function
let Merge lista listb =
    RecursionMerge [] lista listb

////////////////////////////////////////////////////////////

// Cuts list in half
let gencut(n, list) = 
    let firstList = list |> Seq.take n |> Seq.toList
    let secondList = list |> Seq.skip n |> Seq.toList
    (firstList, secondList)
    
// MergeSort function
let rec MergeSort list =
    if List.length list <= 1 then
        list
    else
       let (left, right) = gencut (List.length list / 2, list)
       let leftm = MergeSort left
       let rightm = MergeSort right
       let result = Merge leftm rightm
       result

///////////////////////////////////////////////////////////





