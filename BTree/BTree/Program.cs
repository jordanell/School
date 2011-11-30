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
            BTree<float> tree = new BTree<float>();
           
           tree.Add(5.0f);
           tree.Add(3.1f);
           tree.Add(4.3f);
           tree.Add(1.0f);
           tree.Add(6.8f);
           //tree.Add("zelda");

           BTree<float> tree2 = new BTree<float>();

           tree2.Add(5.6f);
           tree2.Add(7.8f);
           tree2.Add(4.2f);
           tree2.Add(2.3f);

           Console.WriteLine(tree.printTest());

           foreach(float n in tree)
           {
               Console.WriteLine(n.ToString());
           }

           if (tree.Contains(10.3f))
           {
               Console.WriteLine("Contains is working!");
           }

           Console.WriteLine(tree.Count.ToString());

           ///////////////////////////////////////////////////

           float[] strArray = new float[10];
           tree.CopyTo(strArray, 0);
           foreach (float s in strArray)
           {
               Console.WriteLine(s);
           }

           ///////////////////////////////////////////////////

           
           tree.Remove(10.0f);
           foreach (float n in tree)
           {
               Console.WriteLine(n.ToString());
           }
           

           ///////////////////////////////////////////////////

           Console.WriteLine(tree.ToString());

           ///////////////////////////////////////////////////

           tree = tree + tree2;
           foreach (float n in tree)
           {
               Console.WriteLine(n.ToString());
           }
           Console.WriteLine(tree.ToString());

           ///////////////////////////////////////////////////

           BTree<float> newTree = tree.Clone();
           Console.WriteLine(newTree.ToString());

           //newTree.Remove("Hockey");

           Console.WriteLine(newTree.ToString());
           Console.WriteLine(tree.ToString());

           Console.Read();
        }
    }
}
