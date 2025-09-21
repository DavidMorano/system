/* hash_elf SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* perform the hash algorithm done in processing ELF files (GNU version) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written by the famous Peter Jay
	Weinberger of AT&T Bell Laboratories (see notes below).  It
	was adapted into its present form by me.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	hash_elf

	Description:
	This subroutine implements the ELF object file hashing
	function procedure.  This is the famous ELF hash function
	created by Peter Jay Weinberger for ELF files in the AT&T
	UNIX® System V Release 4 operating system.

	Synopsis:
	uint hash_elf(cvoid *sp,int sl) noex

	Arguments:
	sp		buffer to be hashed
	sl		len of buffer data to be hashed

	Returns:
	value		the hash value (unsigned)

	Notes:
	1. For the record, the GNU people came up with what they feel
	is a better hashing function for ELF file symbol tables.
	A modern ELF symbol table now contains the symbol table
	hashed by this hash (by Peter Jay Weinberger: the original)
	and the new one created by the GNU people.
	2. As as aside, for those who do not already know, Peter
	Weinberger was the 'W' in the AWK programming language,
	along with Alfred Vaino Aho (author of EGREP and of "Aho
	and Ullman" fame; think "Dragon Book" among others), and
	Brian Kernighan (coauthor of the original C-language book
	along with Dennis Ritchie).

*******************************************************************************/

#include	<envstandards.h>	/* first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdint>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"hash.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

uint hash_elf(cvoid *vp,int sl) noex {
    	char		*sp = charp(vp) ;
	unsigned	h = 0 ;
	unsigned	g ;
	unsigned	v ;
	if (sl < 0) sl = lenstr(sp) ;
	while (sl-- > 0) {
	    v = uchar(*sp++) ;
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


