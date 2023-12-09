/* hash_elf */
/* lang=C20 */

/* perform the hash done in processing ELF files */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.  The idea of this
	hash is taken from the ELF-type object manipulation procedures.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	hash_elf

	Description:
	This subroutine implements the ELF object file hashing
	function procedure.  This is the famous ELF hash function
	created by Peter Jay Weinberger for ELF files in AT&T UNIX®
	System V Release 4 operating system!

	Synopsis:
	uint hash_elf(cchar *sp,int sl) noex

	Arguments:
	sp		buffer to be hashed
	sl		len of buffer data to be hashed

	Returns:
	value		the hash value (unsigned)

	Notes:
	For the record, the GNU people came up with what they feel
	is a better hashing function for ELF file symbol tables.
	A modern ELF symbol table now contains the symbol table
	hashed by this hash (by Peter Jay Weinberger: the original)
	and the new one created by the GNU people.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<string.h>
#include	<localmisc.h>


/* external subroutines */


/* forward references */


/* exported subroutines */

uint hash_elf(cchar *sp,int sl) noex {
	unsigned	h = 0 ;
	unsigned	g ;
	unsigned	v ;
	if (sl < 0) sl = strlen(sp) ;
	while (sl-- > 0) {
	    v = (uchar) *sp++ ;
	    h <<= 4 ;
	    h += v ;
	    g = (h & 0xF0000000) ;
	    if (g != 0) {
	        h ^= (g >> 24) ;
	        h &= (~ g) ;
	    }
	} /* end while */
	return h ;
}
/* end subroutine (hash_elf) */


