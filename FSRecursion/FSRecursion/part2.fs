// CSC 330 Assignment 2
module part2
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
