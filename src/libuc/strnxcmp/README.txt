STRNXCMP (libuc)

Special counted c-string comparison subroutines.

subroutines	description
--------------------------------------------------------------------------------

strnfoldcmp	compare insensitivelt uysing folded case
strnkeycmp	compare the key parts of the given c-strings
strnleadcmp	compare the leading part of a c-string
strnncmp	the famous double counted c-string compare (case sensitive)
strnndictcmp	a double-counted c-string dictionary comparison
strnvalcmp	compare the value parts of given c-strings

Notes:
1. 2026-02-07 (David A-D- Morano) The subroutine |strnndictcmp(3uc)|
is too complicated to easily verify its correctness.

2. 2026-02-07 (David A-D- Morano) Why is the subroutine |strvalcmp(3uc)|
appear to be more complicated than is necesssary?

3. 2026-02-07 (David A-D- Morano) There is no current use of the
|strnleadcmp(3uc)| subroutine in any of the active software
distributions (repositories).

