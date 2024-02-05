CTX (Convert-To-Xnything)

This directory contains subroutines that convert an integer type
to its c-string representation.

Amazingly, as you may already know, there are no standardized ways
(POSIX®, X/Open®, or otherwise) to perform these functions!  They
will tell you "Go see the function |snprintf(3c)| if you are
interested." But what about those of us who do not want to use
|snprintf(3c)| for everythng -- or use it for anything at all for
that matter?  Also, |snprintf(3c)| only handles base-10 (not counting
octal for octal and hexadecimal for hexadeciaml)!  Duh!  Are you
starting to get the picture?  Duh?

-- updated note on 2024-01-15
Oh, I just remembered.  A newer C++ standard just (relatively
recently) standardized the "convert-to-string" subroutine-templates
(forget what they are actually called).  So at least the world now
has something that is kind of standardized.  But regardless, I had
to make my own versions of these over 40 years ago now (and a few
times in between), and so here they are again (in this directory).

