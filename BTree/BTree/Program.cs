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

            Console.WriteLine("test");
        }
    }
}
