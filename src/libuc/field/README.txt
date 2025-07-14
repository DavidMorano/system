FIELD

This is the famous (infamous) FIELD object.  This is a very old
piece of software.  It was written as an object (like with the word
used in Object-Oriented-Programming) from the beginning.  It dates
from the late 1970s or early 1980s and was originally written
entirely in assembly language (it was VAX-11/780 assembly language).
Sometime in about 1982 (at AT&T Bell Laboratories, originally known
as Bell Telephone Laboratories back then), this was rewritten (really
transcribed) into C-language.  It has largely kept its original
form ever since (enshrined within this directory).  It has not been
rewritten or modified from its original days when first trascribed
into C-lanauge from assembly.  This is why is looks (I guess) pretty
bad from a modern set of software (computer-science) eyes.  It
consists (with no outright apology as yet) as large subroutines not
broken down into smaller pieces.  Pertty much each method otf the
object consists of a single large subroutine.  Yes, it is hard to
follow now-a-days, and it was hard to follow when it was originally
written also.  And that is why (I think) it has largely stayed
untouched all of these past years.  I not anyone else, has dared
touch this code in the mean time for fear of (obviously) breaking
it.  From its beginning in assembly language, it was widely used
in parsing of source files of one kind or another.  The idea of the
terminator block (a bit array used to hold character classes) was
taken from that same use in the Digital-Equipment-Corporation (DEV)
VMS operating system at the time.  The original VAX (the VAX-11/780)
running VMS (its original operaring system from DEC) was first
widely introduced in 1978.  I actually worked for MIT (Massachusetts
Institute of Technology) in 1979 and I witnessed my department
(whatever one wants to call it) get its first VAX-11/780 at that
time.  We were using PDP-11s before (and after) then.  As something
of an aside, the VAX machine architectue had a single instruction
that indexed an array of bytes, treated as a bit array, to test a
single bit within.  That is where the idea of using an array of
bytes as a character-class like bit-array came from.  To this day,
with all of the machine architectues that have come and gone (some
very sadly gone I will say), I do not know of one that has had such
an instruction.  The use of the |batst| macro-subroutine (whatever)
was meant to mimic the use of that bit-array test intruction from
the original VAC machine.

I guess I could say more about this piece of software, but I will
leave it there for now.

Enjoy.

