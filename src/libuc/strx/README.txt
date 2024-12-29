STRX

These are (this directory contains) miscellaneous subroutines to
operate on c-strings.

Notes:

1. 2024-12-16, David A-D- Morano
I retired the ass of the subroutine (|strwildsub(3uc)|).  It was
garbage from its beginning.  I did not write its ass, but I had to
maintain it for a while for some reason that I no longer can find.
So its ass is now retired.  I did not find any use of it in any
current code bases that I have checked.

2.  2024-12-16, David A-D- Morano
Hint: the skinny is that the subroutines |strbasename(3uc)| and
|strdirname(3uc)| are both on the chopping block for retirement.
If I find any use of them in the code, it (the code) will likely be
refactord to elide the use of those subroutines in order to facilitate
their eventual retirement.

