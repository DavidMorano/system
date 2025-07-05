/* outbuf HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* output buffer management (really?) */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	OUTBUF_INCLUDE
#define	OUTBUF_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	OUTBUF		struct outbuf_head


struct outbuf_head {
	char		*obuf ;
	int		olen ;
	int		f_alloc ;
} ;

#ifdef	__cplusplus
enum outbufmems {
	outbufmem_finish,
	outbufmem_overlast
} ;
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
	    finish(this,outbufmem_finish) ;
	    obuf = nullptr ;
	    f_alloc = false ;
	} ;
	outbuf(const outbuf &) = delete ;
	outbuf &operator = (const outbuf &) = delete ;
	int start(cchar *,int = -1) noex ;
	int get(cchar **) noex ;
	void dtor() noex ;
	operator int () noex ;
	destruct outbuf() {
	    if (obuf || f_alloc) dtor() ;
	} ;
} ; /* end struct (outbuf) */
#else	/* __cplusplus */
typedef OUTBUF		outbuf ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	outbuf_start(outbuf *,char *,int) noex ;
extern int	outbuf_get(outbuf *,cchar **) noex ;
extern int	outbuf_finish(outbuf *) noex ;

EXTERNC_end


#endif /* OUTBUF_INCLUDE */


