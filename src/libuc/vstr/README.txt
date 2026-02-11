VSTR (libuc)

These subroutines facilitate sorting and searching those array
structures that require a double pointer indirection to the
stored data.  In the presenr case (VSTR), the stored data
are c-strings.

subroutine	description
--------------------------------------------------------------------------------

vstrcmpx	these are the obverse and reverse c-string comparisons
vstrkeycmpx	this is th very faous key-string comparisons (fwd + rev)
vstrkeydictcmp	this is the key-dictionånry comparison (only forward)


Notes:

1. 2026-02-07 (David A-D- Morano) Reviewing the code for the
|vstrkeycmpx(3uc)| subroutines, I have to marvel at host old they
are, and especially the |vstrkeycmp(3uc)| variant.  They are
officially dated to 1998, but I know that they really existed from
the dawn of the introduction of structured "vectors" of c-strings
themselves.  They were there because they were foundational to the
sorting and searching of c-strings with the containers of c-strings.

2. 2026-02-09 (David A-D- Morano) The |vstrkeycmp(3uc)| subroutine
is ridiculously popular in much (older) code.

