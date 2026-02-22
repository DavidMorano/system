VSTR (libuc)

These subroutines facilitate sorting and searching those array
structures that require a double pointer indirection to the stored
data (c-strings).  In the presenr case (VSTR), the stored data are
c-strings.

subroutine	description
--------------------------------------------------------------------------------

vstrcmp{x}	these are the obverse and reverse c-string comparisons
vstrkeycmp{x}	this is th very faous key-string comparisons (fwd + rev)
vstrkeydictcmp	this is the key-dictionånry comparison (only forward)


Usages:
These subroutines are usedul for those sorting algorithms that
take indirect pointers to the c-string data.
Some sorting facilities or containers that take these kind of
subroutines are:

vecstr
vecpstr
vechand		(when the contained data are c-strings)
vecobj		(when the contained data are c-strings)
raqelem		(when the contained data are c-strings)
recarrj		(when the contained data are c-strings)
raqhand		(when the contained data are c-strings)


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

