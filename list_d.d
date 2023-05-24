template List(T)
{
    extern(C++) final class Node
    {
        extern(C) T value;
        extern(C) Node next;
    }

    extern(C++) final struct List
    {
        extern(C) Node head;
        extern(C) Node tail;

        /*this()
        {
            head = null;
            tail = null;
        }*/

        bool isEmpty() {
            if (head is null)
                return true;
            return false;
        }

        T* getFirst() {
            return &head.value;
        }

        void add(T value)
        {
            import core.stdc.stdlib : calloc;
            auto newNode = cast(Node) calloc(1, Node.sizeof);
            newNode.value = value;
            newNode.next = null;

            if (head is null)
            {
                head = newNode;
                tail = newNode;
            }
            else
            {
                tail.next = newNode;
                tail = newNode;
            }
        }

        void remove(T value)
        {
            Node prevNode = null;
            Node currentNode = head;

            while (currentNode !is null)
            {
                if (currentNode.value == value)
                {
                    if (prevNode is null)
                    {
                        head = currentNode.next;
                    }
                    else
                    {
                        prevNode.next = currentNode.next;
                    }

                    if (currentNode is tail)
                    {
                        tail = prevNode;
                    }

                    import core.stdc.stdlib : free;
                    free(cast(void *) currentNode);
                    break;
                }

                prevNode = currentNode;
                currentNode = currentNode.next;
            }
        }

        Cursor createCursor()
        {
            return Cursor(this);
        }
    }

    extern(C++) final struct Cursor
    {
        extern(C) Node current;
        extern(C) List list;

        this(List list)
        {
            this.list = list;
            current = list.head;
        }

        bool isValid()
        {
            return current !is null;
        }

        T get()
        {
            assert (isValid());
            return current.value;
        }

        void next()
        {
            assert (isValid());
            current = current.next;
        }
    }
}

