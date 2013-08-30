C Run-Time Library Tour
=======================

This provides a brief survey of some of the C Run-Time support that has been
built in order to explore the proposed feature set of *tau*.

The tour is really more of a cross-reference that points to the classes that
will be used to implement certain high-level features; hopefullly this is more
useful than just seeing a directory with a bunch of files in it.

Some Conventions
----------------

The C run-time modules' and "classes"' names are prefixed with "px".

Not everything in the px namespace will be exported into "tau space," i.e.,
not everything will be visible in the target programming language, although
many things will be.

Interfaces and polymorphism are great tools. Without direct support in C,
hand-rolled C++-style vtables are used to obtain these effects. This boils
down to objects having a pointer to a structure full of function pointers.
The structure declaration represents the abstract class' methods. However, there
are some key differences from what you might expect in a straight C++
implementation:
* For objects I expect to export into tau space, compilation independence is
  strictly maintained by only publishing the abstract class in a header file.
* Some vtables contain virtual functions and *virtual data* (a feature I've
  longed for in C++ for many years). Virtual data is used to implement
  data-driven implementation of things that require class reflection
  information. For example, the Object vtable contains a pointer to an array
  of member descriptors that are used for a variety of purposes, such as pointer
  identification during garbage collection or cloning.

  I don't expect virtual data to make it into tau space, but it appears to
  be very convenient for the current code generation strategy (i.e., generate
  C code and compile it).
* When exported into tau space, class/interface/structure names will loose
  any leading px prefix.
* The CRTL is implemented in tau/crtl, where you'll find include and src
  directories, as well as a testsrc directory that contains unit tests you
  can look at to see how various things work in isolation.

Features and their Support
--------------------------
Some features will be emergent properties of the generated code, while others
will directly rely on common run-time support. This lists some of the latter
which have been implemented so far.

* Java-style OOP support
  At the root of everything are pxInterface and pxObject The function of
  these are conflated in most languages. (pxInterface is modeled on COM's
  QueryInterface, if you're familiar with that.)

  pxInterface is the base class of all interfaces. pxInterface provides a
  single method, which is to ask for another interface on the object. All
  other interfaces include this, without exception.

  pxObject is an interface that tau will provide on every Object.
  pxObject has methods for mixing, reflection, cloning, and destruction.

* C++-style Destructors
  Within a scope, the lifetime of an object can be controlled by using the
  pxMinder object. This provides a means to register and deregister Objects.
  When a minder is destroyed, all the currently registered Objects are
  destroyed in reverse order of their registration.

  I expect that stack frames will use minders to handle the (compilation
  independent) objects that are created in those scopes.

* Ruby-style Mixins
  It is possible to compose classes in such a way that the result is
  indistinguishable from a single object that simply has more interfaces.
  (For those that remember it, this is modeled on COM Aggregation.)

  This capability is supported by pxObject. pxObject's implementation of
  pxInterface understands how to traverse the collection of mixins to find
  a requested interface. An Object can have any number of mixins. Mixins must
  be added at the time the Object is instantiated. Mixin instantiation takes a
  pointer to the owning object.

* Cyclone-style Memory Management
  At the heart of this are components for supporting arena-based memory
  management. pxAlloc and pxFree are abstract interfaces for memory management.
  There are several concrete implementations for different purposes. The
  simplest is pxAllocExtent, the "extent allocator." Basically, this maintains
  a large piece of memory, and allocation simply bumps a pointer along until
  the extent is used up. The next request will allocate another extent and
  add it to a linked list of the same. The extent allocator does not support
  freeing memory. Instead, the intention is to do a mini- local garbage
  collection by copying the contents of one extent into another, much the
  same as JVMs that use copy-and-collect. However, we will not stop the world,
  only the cell using the target extent. Since we won't share memory between
  cells, this will be safe. Once the user is finished with everything in an
  allocator, the allocator itself is destroyed.

  Allocators are hierarchical. The factories for allocators take another
  allocator as an argument. This will provide the allocator version of nested
  blocks.

  Allocators are only half the story. It's easy to imagine having an allocator
  associated with a stack frame. If we can use Cyclone-style region
  declarations to control the scope of references (thus avoiding the general
  inability Java has to perform escape analysis for any but the simplest
  cases), then we can tie allocator scope to object lifetime. The final brick
  in this construction is the ability to mixin a minder (see above) with an
  allocator. The upshot is that if you have an
  object holding an open non-memory resource (e.g., a file handle, or database
  transaction), the destruction of the allocator's minder (mixins are
  destroyed prior to their owning objects) will destroy those objects prior to
  the memory being reclaimed, and this is where we get C++-style destructor
  behavior. This union of an allocator and a minder is called an *arena*.

  All objects are allocated from an arena. There will be arenas associated with
  cells and stack frames.

* Erlang-style Processes
  pxLoom provides support for this. pxLoom fakes co-routines in C; see the
  header file for more detail.
