/*
 * Using Objective-C syntax to show the use of blocks of anonymous code.
 *
 * TODO
 * But should it matter whether or not they are anonymous? What if they are
 * assigned to variables? And how should that differ from internal named
 * functions? Is the real distinction here that these are nested functions,
 * requiring a display to handle dynamic scoping? But then why should nested
 * functions be any different from non-nested ones? It's conceivable you would
 * write ones that don't require external state, or carry their own external
 * state in order to share them; this might be akin to creating a closure as
 * a stand-in for a class. Are these really the same? Are procedure activation
 * records (stack frames) just class instances?
 */
public class NestedClosure
{
    public static void main(String arg[])
    {
        List list = new List();

        list.prepend(3);
        list.prepend(2);
        list.prepend(1);

        int count = 0;
        int sum = 0;
        list.visit(^(int x)  // an anonymous function
        {
            ++count;
            sum += x;
        });

        System.out.println("There are " + count + " elements.");
    }

    public static class List // this could be a generic
    {
        private Item head;

        public static class Item
        {
            List next;
            int i;
        }

        public List()
        {
            head = null;
        }

        public void prepend(int x)
        {
            Item item = new Item();
            item.i = x;
            item.next = head;
            head = item;
        }

        public void visit(void (^f)(int x))
        {
            for(List l = head; l != null; l = l.next)
            {
                (^f)(l.i);
            }
        }
    }
}

