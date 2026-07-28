/* outbuf HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* output buffer management (really?) */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	OUTBUF_INCLUDE
#define	OUTBUF_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	OUTBUF		struct outbuf_head


struct outbuf_head {
	char		*obuf ;
	int		olen ;
	int		falloc ;
} ; /* end (outbuf_head) */

#ifdef	__cplusplus
enum outbufmems {
	outbufmem_finish,
	outbufmem_overlast
} ; /* end enum (outbufmems) */
struct outbuf ;
struct outbuf_co {
	outbuf		*op = nullptr ;
	int		w = -1 ;
	void operator () (outbuf *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (outbuf_co) */
struct outbuf : outbuf_head {
	outbuf_co	finish ;
	outbuf() noex {
	    finish	(this,outbufmem_finish) ;
	    obuf = nullptr ;
	    falloc = false ;
	} ; /* end ctor */
	outbuf(const outbuf &) = delete ;
	outbuf &operator = (const outbuf &) = delete ;
	int start(char *,int = -1) noex ;
	int get(cchar **) noex ;
	void dtor() noex ;
	operator int () noex ;
	destruct outbuf() {
	    if (obuf || falloc) dtor() ;
	} ;
} ; /* end struct (outbuf) */
#else	/* __cplusplus */
typedef OUTBUF		outbuf ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	outbuf_start	(outbuf *,char *,int) noex ;
extern int	outbuf_get	(outbuf *,cchar **) noex ;
extern int	outbuf_finish	(outbuf *) noex ;

EXTERNC_end


#endif /* OUTBUF_INCLUDE */


