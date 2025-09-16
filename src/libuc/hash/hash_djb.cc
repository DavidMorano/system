/* hash_djb SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* perform the hash algorithm done in processing ELF files */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written by the famous Daniel Julius
	Bernstein (DJB).  This hash algorithm was adopted for use
	in the ELF-type object symbol hashing procedures by the GNU
	organization.  It was adapted into its present form by me
	(I did not really do much here at all; one cannot change
	something that requires precise bit compatibility).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	hash_djb

	Description:
	This subroutine implements DJB's symbol (c-string) hashing
	algorithm.  This is used as the GNU (DT_GNU_HASH) symbol
	32-bit (unsigned) hash for a linker symbols.  Three versions
	are provided for reader completeness.

	Synopsis:
	uint hash_djb(cvoid *sp,int sl) noex

	Arguments:
	sp		buffer to be hashed
	sl		len of buffer data to be hashed

	Returns:
	value		the hash value (unsigned)

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

uint hash_djb(cvoid *vp,int sl) noex {
    	uint		h = 5381 ; /* return-value */
    	cuchar		*sp = ucharp(vp) ;
	for (uchar c = *sp ; sl-- && (c != '\0') ; c = *++sp) {
            h = h * 33 + c ;
        } /* end for */
	return h ;
}
/* end subroutine (hash_djb) */

#ifdef	COMMENT
uint hash_djb1 (cvoid *sp,int sl) {
	uint		h = 5381 ; /* return-value */
	cuchar		*name = (uchar *) sp ;
	for (uchar c ; sl-- && ((c = *name++) != '\0') ; ) {
            h = (h << 5) + h + c ;	/* h * 33 + c */
	} /* end for */
	return h ;
}
#endif /* COMMENT */

#ifdef	COMMENT
uint hash_djb2(cvoid *sp,int sl) {
	uint		h = 5381 ; /* return-value */
	cuchar		*name = (uchar *) sp ;
	for (uchar c ; sl-- && ((c = *name++) != '\0') ; ) {
            h = (h << 5) + h + c ;	/* h * 33 + c */
            if ((c = *name++) == '\0') break ;
            h = (h << 5) + h + c ;
            if ((c = *name++) == '\0') break ;
            h = (h << 5) + h + c ;
            if ((c = *name++) == '\0') break ;
            h = (h << 5) + h + c ;
	} /* end for */
	return h ;
}
#endif /* COMMENT */


