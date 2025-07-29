/* linefilter HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* filter a string of text against some criteria */
/* version %I% last-modified %G% */


/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

#ifndef	LINEFILTER_INCLUDE
#define	LINEFILTER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<vecstr.h>


/* object defines */
#define	LINEFILTER		struct linefilter_head
#define	LINEFILTER_FL		struct linefilter_flags
/* options */
#define	LINEFILTER_MCARRIAGE	0x0001


struct linefilter_flags {
	uint		sslist:1 ;	/* select */
	uint		sxlist:1 ;	/* exclude */
} ;

struct linefilter_head {
	vecstr		*sslp ;	/* select list pointer */
	vecstr		*sxlp ;	/* exclude list pointer */
	LINEFILTER_FL	fl ;
	uint		magic ;
} ;

#ifdef	__cplusplus
enum linefiltermems {
	linefiltermem_finish,
	linefiltermem_overlast
} ;
struct linefilter ;
struct linefilter_co {
	linefilter	*op = nullptr ;
	int		w = -1 ;
	void operator () (linefilter *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (linefilter_co) */
struct linefilter : linefilter_head {
	linefilter_co	finish ;
	linefilter() noex {
	    finish(this,linefiltermem_finish) ;
	    magic = 0 ;
	} ;
	linefilter(const linefilter &) = delete ;
	linefilter &operator = (const linefilter &) = delete ;
	int start(cchar *,cchar *) noex ;
	int check(cchar *,int) noex ;
	void dtor() noex ;
	destruct linefilter() {
	    if (magic) dtor() ;
	} ;
} ; /* end struct (linefilter) */
#else	/* __cplusplus */
typedef	LINEFILTER	linefilter ;
#endif /* __cplusplus */

typedef	LINEFILTER_FL	linefilter_fl ;

EXTERNC_begin

extern int linefilter_start(linefilter *,cchar *,cchar *) noex ;
extern int linefilter_check(linefilter *,cchar *,int) noex ;
extern int linefilter_finish(linefilter *) noex ;

EXTERNC_end


#endif /* LINEFILTER_INCLUDE */


