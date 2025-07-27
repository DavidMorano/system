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
} ;

typedef	LINEFILTER	linefilter ;
typedef	LINEFILTER_FL	linefilter_fl ;

EXTERNC_begin

extern int linefilter_start(linefilter *,cchar *,cchar *) noex ;
extern int linefilter_check(linefilter *,cchar *,int) noex ;
extern int linefilter_finish(linefilter *) noex ;

EXTERNC_end


#endif /* LINEFILTER_INCLUDE */


