/* rebuf HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* this is a resuable (fixed-sized) buffer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This module was changed to serve in the REFERM program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	rebuf

	Description:
	This object implements a reusuable fixed-sized buffer.

*******************************************************************************/

#ifndef	REBUF_INCLUDE
#define	REBUF_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<sbuf.h>		/* LIBUC */


#define	REBUF_MAGIC	0x98879657


enum rebufmems {
        rebufmem_bufbeg,
        rebufmem_bufend,
        rebufmem_finish,
        rebufmem_overlast
} ; /* end enum (rebufmems) */
struct rebuf ;
struct rebuf_co {
        rebuf		*op = nullptr ;
        int             w = -1 ;
        void operator () (rebuf *p,int m) noex {
            op = p ;
            w = m ;
        } ;
        operator int () noex ;
} ; /* end struct (rebuf_co) */
struct rebuf : sbuf {
    	uint		magval ;
	rebuf_co	bufbeg ;
	rebuf_co	bufend ;
	int start	(char *,int) noex ;
	rebuf() noex {
	    bufbeg	(this,rebufmem_bufbeg) ;
	    bufend	(this,rebufmem_bufend) ;
	    finish	(this,rebufmem_finish) ;
	    magval	= 0 ;
	} ; /* end ctor */
	void dtor() noex ;
    	destruct rebuf() {
	    if (magval) dtor() ;
	} ; /* end destruct */
} ; /* end struct (rebuf) */


#endif /* __cplusplus */
#endif /* REBUF_INCLUDE */


