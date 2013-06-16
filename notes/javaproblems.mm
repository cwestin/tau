<map version="0.9.0">
<!-- To view this file, download free mind mapping software FreeMind from http://freemind.sourceforge.net -->
<node CREATED="1356197282515" ID="ID_1534710506" MODIFIED="1358628263866" TEXT="Java problems">
<font NAME="SansSerif" SIZE="12"/>
<node CREATED="1356197364982" ID="ID_1344072346" MODIFIED="1356199086948" POSITION="right" TEXT="Generics">
<font NAME="SansSerif" SIZE="12"/>
<node CREATED="1356197381973" ID="ID_1396060468" MODIFIED="1356199086947" TEXT="No typedefs - retyping type parameters constantly">
<font NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1356197413109" ID="ID_1698960349" MODIFIED="1356199086947" TEXT="No runtime type information">
<font NAME="SansSerif" SIZE="12"/>
<node CREATED="1356197495763" ID="ID_1463432384" MODIFIED="1356199086947" TEXT="proper reflection methods">
<font NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1356197508124" ID="ID_1203820911" MODIFIED="1358645730165" TEXT="inability to refer to type parameters where Class&lt;?&gt; is required">
<font NAME="SansSerif" SIZE="12"/>
</node>
</node>
</node>
<node CREATED="1356197635216" ID="ID_1365191997" MODIFIED="1356199086946" POSITION="right" TEXT="Exceptions">
<font NAME="SansSerif" SIZE="12"/>
<node CREATED="1356197654544" ID="ID_344868798" MODIFIED="1356199086946" TEXT="Checked Exceptions - ubiquitous try-catch blocks">
<font NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1356197692544" ID="ID_1348455349" MODIFIED="1358628866884" TEXT="Cleanup is user&apos;s responsibility to remember and perform (try-finally)">
<edge STYLE="linear"/>
<arrowlink DESTINATION="ID_195921845" ENDARROW="Default" ENDINCLINATION="249;0;" ID="Arrow_ID_1586107839" STARTARROW="None" STARTINCLINATION="124;0;"/>
<font NAME="SansSerif" SIZE="12"/>
</node>
</node>
<node CREATED="1356197840382" ID="ID_1442998199" MODIFIED="1358629266277" POSITION="right" TEXT="Resource management">
<font NAME="SansSerif" SIZE="12"/>
<node CREATED="1356197872163" ID="ID_195921845" MODIFIED="1358628866884" TEXT="No destructors - cleanup is caller&apos;s responsibility">
<font NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1358629269033" ID="ID_404624507" MODIFIED="1358629296649" TEXT="Locking">
<node CREATED="1358629318329" ID="ID_1284685960" MODIFIED="1358629374315" TEXT="Locking unnecessary if no shared state"/>
</node>
</node>
<node CREATED="1356197564906" ID="ID_1428881417" MODIFIED="1356199348445" POSITION="right" TEXT="Memory management">
<font NAME="SansSerif" SIZE="12"/>
<node CREATED="1356197577865" ID="ID_760223064" MODIFIED="1356199086946" TEXT="Stop the world">
<font NAME="SansSerif" SIZE="12"/>
<node CREATED="1356197596745" ID="ID_1478994736" MODIFIED="1359827713872" TEXT="lack of scoping">
<arrowlink DESTINATION="ID_734000634" ENDARROW="Default" ENDINCLINATION="324;0;" ID="Arrow_ID_896614791" STARTARROW="None" STARTINCLINATION="324;0;"/>
<font NAME="SansSerif" SIZE="12"/>
<node CREATED="1358629111077" ID="ID_766004188" MODIFIED="1358631476793">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      sharing state/memory across threads
    </p>
    <p>
      go and erlang channels and other models of actors seem to be the way to go, but how
    </p>
    <p>
      to avoid serializing and de-serializing too much data to pass across thread boundaries if we disallow
    </p>
    <p>
      sharing memory?
    </p>
  </body>
</html>
</richcontent>
<arrowlink DESTINATION="ID_1284685960" ENDARROW="Default" ENDINCLINATION="514;0;" ID="Arrow_ID_1313288186" STARTARROW="None" STARTINCLINATION="514;0;"/>
</node>
</node>
<node CREATED="1356197608346" ID="ID_1345632621" MODIFIED="1356199086946" TEXT="limitations of escape analysis">
<font NAME="SansSerif" SIZE="12"/>
</node>
</node>
</node>
<node CREATED="1356198813392" HGAP="15" ID="ID_1674451848" MODIFIED="1358630249922" POSITION="right" TEXT="Too many threads" VSHIFT="24">
<font NAME="SansSerif" SIZE="12"/>
<node CREATED="1356198832102" ID="ID_1362132935" MODIFIED="1356199086944" TEXT="Not using green threads">
<font NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1358628984523" ID="ID_373770375" MODIFIED="1358628999244" TEXT="Need asynchronous I/O &quot;under the covers&quot;"/>
</node>
<node CREATED="1356198841974" ID="ID_1696706605" MODIFIED="1358631456082" POSITION="right" TEXT="Closures" VSHIFT="10">
<arrowlink DESTINATION="ID_1302788337" ENDARROW="Default" ENDINCLINATION="-43;-9;" ID="Arrow_ID_1024507632" STARTARROW="None" STARTINCLINATION="64;57;"/>
<font NAME="SansSerif" SIZE="12"/>
<node CREATED="1356198849030" ID="ID_77458385" MODIFIED="1356199086943" TEXT="Referenced values must be final">
<font NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1356198867518" ID="ID_424847355" MODIFIED="1356199086943" TEXT="Closures cannot be returned">
<font NAME="SansSerif" SIZE="12"/>
</node>
</node>
<node CREATED="1358630033633" HGAP="-17" ID="ID_649935780" MODIFIED="1359840657529" POSITION="right" TEXT="Ideas from Other Languages or Systems" VSHIFT="116">
<node CREATED="1359827672923" HGAP="123" ID="ID_734000634" MODIFIED="1359827713872" TEXT="Cyclone arenas" VSHIFT="24"/>
<node CREATED="1358630794803" HGAP="112" ID="ID_1302788337" MODIFIED="1359826201575" TEXT="Pascal nested functions" VSHIFT="41">
<node CREATED="1358630815292" ID="ID_851515643" MODIFIED="1358630826350" TEXT="Implemented via &quot;display&quot;"/>
</node>
<node CREATED="1358630131952" HGAP="117" ID="ID_93599223" MODIFIED="1359826142119" TEXT="Akka-style Actors" VSHIFT="51">
<arrowlink DESTINATION="ID_766004188" ENDARROW="Default" ENDINCLINATION="231;-4;" ID="Arrow_ID_1350137962" STARTARROW="None" STARTINCLINATION="656;0;"/>
<node CREATED="1358630203695" HGAP="18" ID="ID_743351481" MODIFIED="1358631479577" VSHIFT="29">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Ability to fork?
    </p>
    <p>
      Load balancing?
    </p>
    <p>
      Organic Analog: cells
    </p>
  </body>
</html>
</richcontent>
</node>
</node>
<node CREATED="1358627992527" HGAP="78" ID="ID_1784669145" MODIFIED="1359826142121" TEXT="C/C++-Style Macros" VSHIFT="85">
<node CREATED="1358628035890" ID="ID_1233260118" MODIFIED="1358628058277" TEXT="Desire to remove debugging code"/>
<node CREATED="1358628063039" ID="ID_1190727711" MODIFIED="1358628243940">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Avoid function argument expression evaluation when functions aren't going to do anything
    </p>
    <p>
      Note the new log4j attempt to use {} for variable replacement, but this still doesn't avoid argument expression, just the previous string concatenation used to build up a complex string. Also doesn't avoid calling the function just to have it return. <font color="#0033ff">Investigate how good current Java assert is in this regard.</font>
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1359825815698" HGAP="121" ID="ID_53064119" MODIFIED="1359826161716" VSHIFT="7">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      C-style function pointers
    </p>
    <p>
      Java's lack of first-class functions is a sore point for certain types of functional programming, and the claim is that (with closures) it should have these. This doesn't mean they would have to be free functions: static class methods could fill this need if there were a way to declare them as arguments. However, note that creating closures requires currying since the order of definition of classes isn't defined (they happen to be lazily loaded), and with only one class per file, there's no clearly defined prior environment. This could be solved with nested functions.
    </p>
  </body>
</html>
</richcontent>
<arrowlink DESTINATION="ID_1302788337" ENDARROW="Default" ENDINCLINATION="696;0;" ID="Arrow_ID_1493533788" STARTARROW="None" STARTINCLINATION="696;0;"/>
</node>
<node CREATED="1358630472345" HGAP="92" ID="ID_1624146855" MODIFIED="1359840549753" TEXT="C-style arrays" VSHIFT="4">
<node CREATED="1358630482289" ID="ID_539491578" MODIFIED="1358631105914" TEXT="Required for &quot;systems programming&quot;">
<node CREATED="1358631084493" ID="ID_1496276022" MODIFIED="1358631098937" TEXT="file formats"/>
<node CREATED="1358631114462" ID="ID_549441516" MODIFIED="1358631117913" TEXT="memory handling"/>
</node>
<node CREATED="1358630527143" ID="ID_1350830868" MODIFIED="1359840570182">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      lack of bounds checking is a security hole
    </p>
    <p>
      Perhaps the trick is not to allow these as locals or in classes, but only in structs. Or,
    </p>
    <p>
      considering the need to read data from files, only as pointers to allocated items, not
    </p>
    <p>
      embedded as locals in stack frames. This would be consistent with not allowing them
    </p>
    <p>
      in classes if we treat stack frames as classes.
    </p>
  </body>
</html>
</richcontent>
<arrowlink DESTINATION="ID_1538341953" ENDARROW="Default" ENDINCLINATION="193;0;" ID="Arrow_ID_364944419" STARTARROW="None" STARTINCLINATION="465;0;"/>
<arrowlink DESTINATION="ID_111883929" ENDARROW="Default" ENDINCLINATION="92;0;" ID="Arrow_ID_1007674287" STARTARROW="None" STARTINCLINATION="92;0;"/>
</node>
</node>
<node CREATED="1359840477647" HGAP="154" ID="ID_111883929" MODIFIED="1359840685902" VSHIFT="30">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      go/python slices
    </p>
    <p>
      These may be a solution to the bounds checking security hole -- if array references are always wide references that can be bounds checked.
    </p>
  </body>
</html>
</richcontent>
</node>
<node CREATED="1358630595462" HGAP="156" ID="ID_1489680044" MODIFIED="1359840683790" TEXT="C-style structures" VSHIFT="8">
<node CREATED="1358630651605" ID="ID_156144910" MODIFIED="1358631051316" TEXT="Required for &quot;systems programming&quot;">
<node CREATED="1358631026485" ID="ID_278158422" MODIFIED="1358631034972" TEXT="file formats"/>
<node CREATED="1358631036638" ID="ID_1508049207" MODIFIED="1358631042834" TEXT="memory handling"/>
</node>
<node CREATED="1358630666757" ID="ID_1805709306" MODIFIED="1358909011916" TEXT="Native memory representation at odds with type erasure/C++ template replacement">
<node CREATED="1358630735140" HGAP="144" ID="ID_1538341953" MODIFIED="1359840676110" VSHIFT="-47">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Perhaps structs and classes need to be different, unlike C++
    </p>
    <p>
      Interestingly, no one has complained about the discrepancies between
    </p>
    <p>
      Java arrays and Vector&lt;&gt; and ArrayList&lt;&gt;.
    </p>
  </body>
</html>
</richcontent>
</node>
<node CREATED="1358909015161" HGAP="96" ID="ID_1953265872" MODIFIED="1359840671646" VSHIFT="13">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Structures used for file layout don't need pointers
    </p>
    <p>
      Perhaps these only allow embedding of other structs or
    </p>
    <p>
      arrays. A reference to these should only be necessary from locals and
    </p>
    <p>
      classes, but not from other structs. But this does limit the ability to do low
    </p>
    <p>
      level memory work. Or does it? Would that still be ok if we don't allow
    </p>
    <p>
      pointer arithmetic? Or if we use fat pointers for such references?
    </p>
  </body>
</html>
</richcontent>
</node>
</node>
<node CREATED="1358630901326" ID="ID_1087814206" MODIFIED="1358630962438" TEXT="Java &quot;buffer&quot; classes with their multiple &quot;views&quot; are a pain to use"/>
</node>
<node CREATED="1361021291815" HGAP="201" ID="ID_101917493" MODIFIED="1361021363829" VSHIFT="57">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Assembly language TST intruction
    </p>
    <p>
      Three way branches are possible in assembly language with only one comparison; we see this possible from functions like strcmp() and other comparators as well. This should be a language construct. &lt;&gt; ?
    </p>
  </body>
</html>
</richcontent>
</node>
<node CREATED="1360527262055" HGAP="189" ID="ID_1697641194" MODIFIED="1360527386396" VSHIFT="50">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Recursive Implementation
    </p>
    <p>
      A real sign of a language's power is the ability to implement it in terms of itself. Classic examples are lisp and ML REPLs, as well as C/C++ compilers. Could a higher-level language include features listed here as well as enough to be able to implement its own memory management in itself?
    </p>
  </body>
</html>
</richcontent>
</node>
<node CREATED="1359566821540" HGAP="148" ID="ID_1332788997" MODIFIED="1359840680862" TEXT="lint hints" VSHIFT="47">
<node CREATED="1359566841670" ID="ID_1815839137" MODIFIED="1359567031448">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      NOTREACHED
    </p>
    <p>
      Some functions unconditionally throw an exception and don't return. Yet compilers don't enforce or do anything about that, and force you to include return value statements when they can't detect this. Require declarations to specify this if exceptions are thrown unconditionally.
    </p>
  </body>
</html>
</richcontent>
</node>
<node CREATED="1359566936851" ID="ID_284706095" MODIFIED="1359567021160">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      FALLTHROUGH
    </p>
    <p>
      Very useful on switch statement cases to indicate that a break (or return) was intentionally left out.
    </p>
  </body>
</html>
</richcontent>
</node>
</node>
<node CREATED="1358641624384" HGAP="112" ID="ID_1748594003" MODIFIED="1359840839391" VSHIFT="30">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ML Options
    </p>
    <p>
      I seem to recall seeing this in some recent blog, but the author was espousing this as a way to &quot;never have to check for null.&quot; Of course that's wrong because you still have to check the option for a value.
    </p>
    <p>
      
    </p>
    <p>
      What is interesting here is the ability to add a sentinel value outside the normal range for a function. In The Coursera UW Programming Languages course, the example was given of writing a function to find the maximum value in a list of integers. The problem is, what to do when being passed an empty list -- there is no max. The classic Java solution would be to throw IllegalArgumentException. But the case might be &quot;normal,&quot; and this induces the need for the usual painful try/catch. In C\C++, the solution might be to pass a bool by reference, and then check that, which approximates options, but requires explicit user coding. Note that domain of pointers already include this special value, but other natural domains such as integers don't.
    </p>
    <p>
      
    </p>
    <p>
      The Coursera UWPL class also included an optional module on implementing functional analogs in Java, and ones of those demonstrated that the null problem was best solved by using a sentinel that was a special &quot;empty set/list&quot; construction. This was interesting because it wasn't null, and you could still use all the class' methods on it without blowing up. Perhaps types all need to have a special case like this?
    </p>
  </body>
</html>
</richcontent>
</node>
</node>
</node>
</map>
