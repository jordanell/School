using System;
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
    public void CopyTo(T[] array, int arrayIndex)
    {

    }

    /* Get the enumerator */
    
    public IEnumerator<T> GetEnumerator()
    {
        return root.value;
    }


    IEnumerator IEnumerable.GetEnumerator()
    {
        return this.GetEnumerator();
    }

    /* Remove a node from the tree */
    public bool Remove(T x)
    {
        return false;
    }
}
