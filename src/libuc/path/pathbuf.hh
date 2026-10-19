/* pathbuf HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* buffer object for path manipulation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This was written as part of the FILECHECK utility.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	pathbuf

	Description:
	This object is used for path manipulation.

	Arguments:
	rbuf		buffer to receive cleaned up path string
	pbuf		supplied path to clean up
	plen		length of supplied path to clean up

	Returns:
	>=0		length of cleaned up path
	<0		error code (system-return)

*******************************************************************************/

#ifndef	PATHBUF_INCLUDE
#define	PATHBUF_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */


enum struct pathbufmems {
	pathbufmem_remslash,
	pathbufmem_finish,
	pathbufmem_overlast
} ; /* end enum (pathbufmems) */
struct pathbuf ;
struct pathbuf_co {
    	pathbuf		*op ;
	pathbufmems	w ;
	void operator () (pathbuf *p,pathbufmems m) noex {
	    op = p ;
	    w = m ;
	} ; /* end methodd */
	operator int () noex ;
	int operator () () noex {
	    return operator int () ;
	} ; /* end */
} ; /* end struct (pathbuf_co) */
struct pathbuf {
	friend		pathbuf_co ;
    	using enum	pathbufmems ;
	charp		pbuf{} ;
	int		plen{} ;
	int		idx{} ;
	pathbuf_co	remslash ;
	pathbuf_co	finish ;
	pathbuf() noex {
	    remslash	(this,pathbufmem_remslash) ;
	    finish	(this,pathbufmem_remslash) ;
	} /* end ctor */
	int start	(char *,int) noex ;
	int chr		(int) noex ;
	int str		(cchar *,int = -1) noex ;
    private:
	int premslash	() noex ;
	int pfinish	() noex ;
} ; /* end struct (pathbuf) */


#endif /* PATHBUF_INCLUDE */


