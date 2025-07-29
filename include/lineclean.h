/* lineclean HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* clean up a line of text w/ options */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	LINECLEAN_INCLUDE
#define	LINECLEAN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<linecleanopt.h>	/* line-cleanint options */


#define	LINECLEAN		struct lineclean_head
#define	LINECLEAN_FL		struct lineclean_flags
#define	LINECLEAN_MAGIC		0x97868574

struct lineclean_flags {
	uint		nonsub:1 ;
	uint		nulsub:1 ;
	uint		nulign:1 ;
} ; /* end struct (lineclean_flags) */

struct lineclean_head {
	char		*cbuf ;
	int		clen ;
	LINECLEAN_FL	fl ;
	uint		magic ;
} ; /* end struct (lineclean_head) */

#ifdef	__cplusplus
enum linecleanmems {
	linecleanmem_finish,
	linecleanmem_overlast
} ;
struct lineclean ;
struct lineclean_co {
	lineclean	*op = nullptr ;
	int		w = -1 ;
	void operator () (lineclean *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (lineclean_co) */
struct lineclean : lineclean_head {
    	linecleanoptms	om ;
	lineclean_co	finish ;
	lineclean() noex {
	    finish	(this,linecleanmem_finish) ;
	    cbuf = nullptr ;
	    magic = 0 ;
	} ; /* end ctor */
	int start(cchar *,int,int,cchar **) noex ;
	void dtor() noex ;
	destruct lineclean() {
	    if (magic) dtor() ;
	} ;
} ; /* end struct (lineclean) */
#else
typedef	LINECLEAN		lineclean ;
#endif /* __cplusplus */

typedef	LINECLEAN_FL		lineclean_fl ;

EXTERNC_begin

extern int lineclean_start(lineclean *,cchar *,int,int,cchar **) noex ;
extern int lineclean_finish(lineclean *) noex ;

EXTERNC_end


#endif /* LINECLEAN_INCLUDE */


