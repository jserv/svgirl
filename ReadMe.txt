
1/16/2016

I (David Phillip Oster) hate dependencies. This just opens and builds using Xcode. It removes the dependency on
colors.gperf, and the dependency on the 'dom' parsing library, using the libxml2 that is
pre-installed on OS X and iOS.

There are three targets:
libsvgtiny, a static library that does the parse,
libsvgtinywriter, a static library that does the unparse

and a trivial app, which just parses a SVG into a diagram, unparses to a string, writes that to the output, then takes the first result
and parses THAT to a diagram, unparses to a string, and writes that to the output.

So now I have a starting point: I can read in one of the SVG files I care about, parse it to a data structure I can manipulate, and write it out again.

This library has in its Xcode target settings:

Header search paths:
/usr/include/libxml2

Preprocessor macro defines: USE_XML2


You app target also needs:

Other linker flags:
-lxml2


Note:
* Gradients are parsed, but discarded. Eventually there will be additional API to exract them from the svgtiny_shape
* I'm not that interested in gradients, but I am interested in text. I'll be adding a text properties with font info.
* When parsing, there should be an option to derive the viewbox from the input file, rather than having to pass it in.
