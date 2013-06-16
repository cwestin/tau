public class Destruction
{
    /*
      Escape analysis is hampered by passing objects in to functions.

      Ideally, the default behavior would be that pointers must be handled in
      a pure fashion, and cannot be retained. If they are to be retained, then
      the caller must specify that.
     */
    public void example1()
    {
        owned StringBuilder sb = new StringBuilder(); // should be the default?

        f1(sb); // sb cannot be retained
        f2(sb); // sb cannot be retained
        System.out.println(sb.toString());
        
        /* sb is (can be) deallocated on return, as if it were a local variable */
    }

    /*
      Next most important is to support destruction in this case.
      An owned object is guaranteed to be destroyed, but not necessarily
      deallocated, although once destroyed, it is no longer accessible. After
      this, the memory may be reclaimed at any time the implementation deems
      convenient.

      To this end, Object needs to have a method

      void destroy();

      For most memory based objects, this does nothing. But any objects that
      require cleanup of non-memory resources must take care of that.

      Class member variables may need similar treatment, but we need to
      allow for retention of things that are in scope for the lifetime of
      the class instance -- these are not owned, but are guaranteed to
      outlive the class.

      scoping wrt lifetime
      - factories
      - data structures: adding and removing elements; re-organizing elements
     */
}
