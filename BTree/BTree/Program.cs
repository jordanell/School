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

           Console.Read();
        }
    }
}
