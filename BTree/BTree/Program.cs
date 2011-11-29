using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BTree
{
    class Program
    {
       static void Main(string[] args)
        {
           BTree<string> tree = new BTree<string>();
           
           tree.Add("programming");
           tree.Add("languages");
           tree.Add("provide");
           tree.Add("lots");
           tree.Add("opportunity");
           tree.Add("for");
           tree.Add("mistakes");
           //tree.Add("zelda");

           BTree<string> tree2 = new BTree<string>();

           tree2.Add("golf");
           tree2.Add("Hockey");
           tree2.Add("Baseball");
           tree2.Add("Swimming");

           Console.WriteLine(tree.printTest());

           foreach(string n in tree)
           {
               Console.WriteLine(n.ToString());
           }

           if (tree.Contains("lots"))
           {
               Console.WriteLine("Contains is working!");
           }

           Console.WriteLine(tree.Count.ToString());

           ///////////////////////////////////////////////////

           String[] strArray = new string[10];
           tree.CopyTo(strArray, 0);
           foreach (string s in strArray)
           {
               Console.WriteLine(s);
           }

           ///////////////////////////////////////////////////

           /*
           tree.Remove("languages");
           foreach (string n in tree)
           {
               Console.WriteLine(n.ToString());
           }
           */

           ///////////////////////////////////////////////////

           Console.WriteLine(tree.ToString());

           ///////////////////////////////////////////////////

           tree = tree + tree2;
           foreach (string n in tree)
           {
               Console.WriteLine(n.ToString());
           }
           Console.WriteLine(tree.ToString());

           ///////////////////////////////////////////////////

           BTree<string> newTree = tree.Clone();
           Console.WriteLine(newTree.ToString());

           newTree.Remove("Hockey");

           Console.WriteLine(newTree.ToString());
           Console.WriteLine(tree.ToString());

           Console.Read();
        }
    }
}
