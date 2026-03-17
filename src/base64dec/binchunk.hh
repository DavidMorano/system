/* binchunk HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Binary-Chunk */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This was made from scratch, although it is (very) similar to
	many others that I have made like it.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	binchunk

	Names:
	binchunk_start
	binchunk_load
	binchunk_read
	binchunk_finish

	Description:
	This object facilitates the management of a binary chunk
	of data.

*******************************************************************************/

#ifndef	BINCHUNK
#define	BINCHUNK
#ifdef	__cplusplus /* (C++ only) */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>


#define	BINCHUNK_NENTS		64	/* starting number entries */


enum binchunkmems {
    	binchunkmem_start,
	binchunkmem_finish,
	binchunkmem_count,
	binchunkmem_len,
	binchunkmem_extent,
	binchunkmem_reset,
	binchunkmem_overlast
} ; /* end enum (binchunkmems) */

struct binchunk ;

struct binchunk_co {
	binchunk	*op = nullptr ;
	int		w = -1 ;
	void operator () (binchunk *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (binchunk_co) */

class binchunk {
    	friend		binchunk_co ;
	int8_t		*binbuf ;
	int		ql ;		/* queue length */
	int		qe ;		/* queue extent */
	int istart() noex ;
	int ifinish() noex ;
	int ireset() noex ;
	int iextend(int) noex ;
	void dtor() noex ;
    public:
	binchunk() noex ;
	binchunk(const binchunk &) = delete ;
	binchunk &operator = (const binchunk &) = delete ;
	binchunk_co	start ;
	binchunk_co	finish ;
	binchunk_co	count ;
	binchunk_co	len ;
	binchunk_co	extent ;
	binchunk_co	reset ;
	int add		(int8_t) noex ;
	int add		(int) noex ;
	int add		(const int8_t *,int = -1) noex ;
	int get		(const int8_t **) noex ;
	int readat	(int,int8_t *,int) noex ;
	int read	(int8_t *,int) noex ;
	int8_t at	(int) const noex ;
	int8_t operator [] (int) const noex ;
	operator int () noex ;
	destruct binchunk() {
	    if (binbuf) dtor() ;
	} ;
} ; /* end structure (binchunk) */


#endif /* __cplusplus (C++ only) */
#endif /* BINCHUNK */


