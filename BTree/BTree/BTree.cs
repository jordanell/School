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

        public Node(T value)
        {
            this.value = value;
            leftChild = null;
            rightChild = null;
        }
    }

    private Node root;
    public int Count { get { return Count; }
                       set { Count = value; } }
    public bool IsReadOnly { get { return false; } }


    public BTree()
    {
        root = null;
    }

    /* Following two functions are for adding a node */
    private void RecursiveAdd(T x, Node node)
    {
        if (node == null)
        {
            node = new Node(x);
            Count = Count + 1;
        }

        else
        {
            if (node.value.CompareTo(x) == 1)
                RecursiveAdd(x, node.leftChild);
            else
                RecursiveAdd(x, node.rightChild);
        }
    }

    public void Add(T x)
    {
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
        if (node.value.CompareTo(x) == 0)
            return true;
        else if (node.value.CompareTo(x) == 1)
            RecursiveContains(x, node.leftChild);
        else
            RecursiveContains(x, node.rightChild);

        return false;
    }

    public bool Contains(T x)
    {
        return RecursiveContains(x, root);
    }

    /* The copy to function */
    private void RecursiveCopyTo(T[] array, int arrayIndex, Node node)
    {
        if (node.leftChild != null)
        {
            RecursiveCopyTo(array, arrayIndex, node.leftChild);
            arrayIndex++;
        }

        array[arrayIndex] = node.value;

        if (node.rightChild != null)
        {
            RecursiveCopyTo(array, arrayIndex, node.rightChild);
            arrayIndex++;
        }
    }

    public void CopyTo(T[] array, int arrayIndex)
    {
        RecursiveCopyTo(array, arrayIndex, root);
    }

    /* Get the enumerator */
    private IEnumerator<T> RecursiveGetEnumerator(Node node)
    {
        if (node.leftChild != null)
            RecursiveGetEnumerator(node.leftChild);

        yield return node.value;

        if (node.rightChild != null)
            RecursiveGetEnumerator(node.rightChild);

        if (node == root)
            yield break;
    }

    public IEnumerator<T> GetEnumerator()
    {
        return RecursiveGetEnumerator(root);
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
                node = null;

            // Has only one child
            if (node.leftChild == null && node.rightChild != null)
                node = node.rightChild;
            else if (node.leftChild != null && node.rightChild == null)
                node = node.leftChild;

            // Has two children
            if (node.leftChild != null && node.rightChild != null)
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
    private void InsertNodeChildren(Node newnode, Node oldnode)
    {
        if (oldnode.leftChild != null)
        {
            newnode.leftChild = new Node(oldnode.leftChild.value);
            InsertNodeChildren(newnode.leftChild, oldnode.leftChild);
        }

        if (oldnode.rightChild != null)
        {
            newnode.rightChild = new Node(oldnode.rightChild.value);
            InsertNodeChildren(newnode.rightChild, oldnode.rightChild);
        }
    }
    public BTree<T> Clone()
    {
        BTree<T> newTree = new BTree<T>();
        //Insert the root node
        if (this.root != null)
        {
            newTree.Add(this.root.value);
            InsertNodeChildren(this.root, newTree.root);
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
    private StringBuilder RecursiveToString(StringBuilder str, Node node)
    {
        if (node.leftChild != null)
        {
            str.Append("(");
            str = RecursiveToString(str, node.leftChild);
            str.Append(")");
            str.Append(" ");
        }

        if (node.leftChild == null)
            str.Append(node.value.ToString());

        if (node.rightChild != null)
        {
            str.Append(" ");
            str.Append("(");
            str = RecursiveToString(str, node.rightChild);
            str.Append(")");
        }

        return str;
    }
    public override string ToString()
    {
        StringBuilder str = new StringBuilder();
        str.Append("(");
        str = RecursiveToString(str, root);
        str.Append(")");

        return RecursiveToString(str, root).ToString();
    }
}
