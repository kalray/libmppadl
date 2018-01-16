#!/usr/bin/env python
import sys
import textwrap
s=open(sys.argv[1], 'rb').read().encode("hex").upper()
t="".join(["\\x"+x+y for (x,y) in zip(s[0::2], s[1::2])])
print "const char %s[] = \\\n\t\"%s\";" % (sys.argv[2], "\" \\\n\t\"".join(textwrap.wrap(t,80)))
