public class Lifetime
{
    void NestedScoping()
    {
        /* owned */ a = new A();
        /* owned */ b = new B(a); // ok because a's lifetime encompasses b's
        /* owned */ c = new C(b); // ok because b's lifetime encompasses c's

        /* destroy c */
        /* destroy b */
        /* destroy a */
    }

    C Factory()
    {
        /* owned */ a = new A();
        /* owned */ b = new B(a); // ok because a's lifetime encompasses b's

        c = new C(b); // NOT ALLOWED
        /*
          But this could work if a and b were in the Scope of c.
         */

        return c; // this would break because a, b are going away

        /* destroy c */
        /* destroy b */
        /* destroy a */
    }

    void PureReference(/* owned */ A a)
    {
        /* this is OK iff *this* is also owned by the caller */
        /* owned */ b = new B(a);
    }

    void Impurity(/* owned */ A a)
    {
        memberVariable = a;
        /*
          This can work if *this* is in the scope of a, otherwise not; so how
          do we detect that from external declarations?
         */
    }
}
