/*
  Cyclone region declaration syntax appears to have made the same mistake
  Microsoft made with their near and far type qualifiers. See 
  http://cyclone.thelanguage.org/wiki/Cyclone%20for%20C%20Programmers/#Pointers .

  Instead, we can model this the same way as 'const' is modeled for The
  Golden Rule of Pointers:
 */
void foo()
{
    arena(foo) int *pi = arena(foo).new int;
    
    foo @int *pi = foo.new int;

    arena<foo> int *pi = arena<foo>.new int;

    region(`foo) int *pi = rnew(`foo) int;

  R:{
        // within region R
    }
}
/*
  It's not the region that contains the pointer that's interesting, it's the
  region that the pointer points at.
 */

/*
The problems associated with this seem very similar to those that come with
type erasure: most of the time you just want things to match up, or use
a compatible descendent domain, but sometimes you need to reference the
variable (a la T.class).
 */
