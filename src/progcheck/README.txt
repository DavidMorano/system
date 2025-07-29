PROGCHECK

This program performs some source-code program checking.  We check
for unbalanced:
	parentheses, braces, backets, comments, quotes, and literals.

Synopsis:
$ progcheck <file(s)> [-c] [-o <opt(s)>] [-v[=<n>]] [-V]

Arguments:
<file(s)>	C-language file(s) to check for correctness
-c		in addition a regular report, report on counts (of stuff)
-o <opt(s)>	options: counts[={0|1}] (default is OFF)
-v[=<n>]	verbositity level
-V		print program version to standard-error and then exit

Notes:
I am looking at the source code of this program now (2025) after
the fact (years after writing this) and I must have been on drugs
when I wrote it.  According to the dates within the source files,
I wrote this a long time ago (as far back as 2004) with some common
or similar code from 2000.  This (PROHCHECK) is an updated and
combined version (or a sort) of three previous programs of mine
that performed these functions separately.  The previous programs
(now gone, I just tried to find one or more of them) were: PROGBRA
(brace checking), PROCBRACK (bracket checking), and PROGPAR
(parenthesis checking).  Those previous programs date from the 1980s
(the old days indeed).  I cannot find any of those old programs
now.  I wanted to see if any of those older programs were as ...
drug indused as this present program.  Why did I even come back to
this after all of these years?  I wanted to see if any of the code
here could be used in a new program that needed to parse C++ source
code.  But given the complexity of this code (at least for now) I
am not going to try to decipher how this code worked.  It looks
like some objects (LINEHIST and LANGSTATE) used in this code were
written as late as 2008 and then updated in 2017.  Those are the
hardest pieces to understand.

