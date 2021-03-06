﻿/* Jordan Ell V00660306 */

using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;

class BTree<T> : ICollection<T> where T : IComparable<T>
{
    class Node
    {
        public T value;
        public Node leftChild;
        public Node rightChild;
        public Node parent;

        public Node(T value)
        {
            this.value = value;
            leftChild = null;
            rightChild = null;
            parent = null;
        }

        public IEnumerator<T> RecursiveGetEnumerator(Node node)
        {
            if (node.leftChild != null)
            {
                foreach (T x in node.leftChild)
                {
                    yield return x;
                }
            }

            yield return node.value;

            if (node.rightChild != null)
            {
                foreach (T x in node.rightChild)
                {
                    yield return x;
                }
            }
            
            yield break;
        }

        public IEnumerator<T> GetEnumerator()
        {
            return RecursiveGetEnumerator(this);
        }
    }

    private Node root;
    private int count;
    public int Count 
    { 
        get { return count; }
        set { count = value; } 
    }
    public bool IsReadOnly
    { 
        get { return false; } 
    }

    public BTree()
    {
        root = null;
    }

    public string printTest()
    {
        return root.leftChild.leftChild.value.ToString();
    }

    /* Following two functions are for adding a node */
    private void RecursiveAdd(T x, Node node)
    {
        if (node.value.CompareTo(x) == 1)
        {
            if (node.leftChild == null)
            {
                node.leftChild = new Node(x);
                node.leftChild.parent = node;
                Count += 1;
            }
            else
                RecursiveAdd(x, node.leftChild);
        }
        else
        {
            if (node.rightChild == null)
            {
                node.rightChild = new Node(x);
                node.rightChild.parent = node;
                Count += 1;
            }
            else
                RecursiveAdd(x, node.rightChild);
        }
    }

    public void Add(T x)
    {
        if (root == null)
        {
            Count += 1;
            root = new Node(x);
        }
        else
            RecursiveAdd(x, root);
    }

    /* This function resets the birnary tree */
    public void Clear()
    {
        root = null;
    }

    /* The following two functions check to see if a value is in the tree */
    private bool RecursiveContains(T x, Node node)
    {
        if (node == null)
            return false;
        else if (node.value.CompareTo(x) == 0)
            return true;
        else if (node.value.CompareTo(x) >= 0)
            return RecursiveContains(x, node.leftChild);
        else
            return RecursiveContains(x, node.rightChild);
    }

    public bool Contains(T x)
    {
        return RecursiveContains(x, root);
    }

    /* The copy to function */
    private int RecursiveCopyTo(T[] array, int arrayIndex, Node node)
    {
        if (node.leftChild != null)
        {
            arrayIndex = RecursiveCopyTo(array, arrayIndex, node.leftChild);
        }

        array[arrayIndex] = node.value;
        arrayIndex++;

        if (node.rightChild != null)
        {
            arrayIndex = RecursiveCopyTo(array, arrayIndex, node.rightChild);
        }

        return arrayIndex;
    }

    public void CopyTo(T[] array, int arrayIndex)
    {
        try
        {
            RecursiveCopyTo(array, arrayIndex, root);
        }
        catch (Exception)
        {
            Console.WriteLine("Unable to copy to array. Check your array input size");
        }
    }

    /* Get the enumerator. Enumorator is actually through the nodes. */
    public IEnumerator<T> GetEnumerator()
    {
        return root.RecursiveGetEnumerator(root);
    }


    IEnumerator IEnumerable.GetEnumerator()
    {
        return this.GetEnumerator();
    }

    /* Remove a node from the tree */
    private Node FindMax(Node node)
    {
        Node max;
        if (node.rightChild != null)
        {
            max = FindMax(node.rightChild);
            return max;
        }
        else
            return node;
    }
    private void RecursiveRemove(T x, Node node, Node parent)
    {
        if (node.value.CompareTo(x) == 0)
        {
            // Is a leaf
            if (node.leftChild == null && node.rightChild == null)
            {
                // Is not the root
                if (node.parent != null)
                {
                    if (node.parent.leftChild != null && node.parent.leftChild.value.CompareTo(x) == 0)
                        node.parent.leftChild = null;
                    else
                        node.parent.rightChild = null;

                    node = null;
                }
                else
                {
                    root = null;
                }
            }
            // Has only one child
            else if (node.leftChild == null && node.rightChild != null)
            {
                node.value = node.rightChild.value;
                node.rightChild = null;
            }
            else if (node.leftChild != null && node.rightChild == null)
            {
                node.value = node.leftChild.value;
                node.leftChild = null;
            }

            // Has two children
            else if (node.leftChild != null && node.rightChild != null)
            {
                Node max = FindMax(node.leftChild);
                T value = max.value;
                node.value = value;
                RecursiveRemove(value, max, null);
            }
        }
        else
        {
            if (node.leftChild != null)
                RecursiveRemove(x, node.leftChild, node);
            if (node.rightChild != null)
                RecursiveRemove(x, node.rightChild, node);
        }
    }

    public bool Remove(T x)
    {
        if (Contains(x))
        {
            RecursiveRemove(x, root, null);
            return true;
        }
        return false;
    }

    /* Clone the tree functions */
    private BTree<T> InsertNodeChildren(BTree<T> newTree, Node oldnode)
    {
        if (oldnode.leftChild != null)
        {
            newTree.Add(oldnode.leftChild.value);
            newTree = InsertNodeChildren(newTree, oldnode.leftChild);
        }

        if (oldnode.rightChild != null)
        {
            newTree.Add(oldnode.rightChild.value);
            newTree = InsertNodeChildren(newTree, oldnode.rightChild);
        }

        return newTree;
    }
    public BTree<T> Clone()
    {
        BTree<T> newTree = new BTree<T>();
        //Insert the root node
        if (this.root != null)
        {
            newTree.Add(this.root.value);
            newTree = InsertNodeChildren(newTree, this.root);
        }
        
        return newTree;
    }

    /* Overloading the + operator functions */
    private void RecursiveAdd(BTree<T> tree, Node node2)
    {
        if(node2 != null)
            tree.Add(node2.value);

        if(node2.leftChild != null)
            RecursiveAdd(tree, node2.leftChild);
        if (node2.rightChild != null)
            RecursiveAdd(tree, node2.rightChild);
    }

    public static BTree<T> operator+(BTree<T> T1, BTree<T> T2)
    {
        T1.RecursiveAdd(T1, T2.root);
        return T1;
    }

    /* The ToString method functions */
    private StringBuilder RecursiveToString(Node node)
    {
        StringBuilder str = new StringBuilder();
        if (node.leftChild != null)
        {
            str.Append("(");
            str.Append(RecursiveToString(node.leftChild));
            str.Append(")");
            str.Append(" ");
        }

        str.Append(node.value.ToString());

        if (node.rightChild != null)
        {
            str.Append(" ");
            str.Append("(");
            str.Append(RecursiveToString(node.rightChild));
            str.Append(")");
        }

        return str;
    }
    public override string ToString()
    {
        StringBuilder str = new StringBuilder();
        str.Append("(");
        str.Append(RecursiveToString(root));
        str.Append(")");

        return str.ToString();
    }
}
