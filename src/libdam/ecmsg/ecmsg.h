/* ecmsg HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* email-cover subchannel message */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-06-13, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	ECMSG_INCLUDE
#define	ECMSG_INCLUDE

#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	ECMSG			struct ecmsg_head
#define	ECMSG_MAXBUFLEN		(8 * 1024)


struct ecmsg_head {
	char		*ebuf ;
	int		elen ;
} ; /* end struct */

#ifdef	__cplusplus
enum ecmsgmems {
    	ecmsgmem_start,
	ecmsgmem_already,
	ecmsgmem_finish,
	ecmsgmem_overlast
} ; /* end enum (ecmsgmems) */
struct ecmsg ;
struct ecmsg_co {
	ecmsg		*op = nullptr ;
	int		w = -1 ;
	void operator () (ecmsg *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (ecmsg_co) */
struct ecmsg : ecmsg_head {
	ecmsg_co	start ;
	ecmsg_co	already ;
	ecmsg_co	finish ;
	ecmsg() noex {
	    start	(this,ecmsgmem_start) ;
	    already	(this,ecmsgmem_already) ;
	    finish	(this,ecmsgmem_finish) ;
	} ; /* end ctor */
	ecmsg(const ecmsg &) = delete ;
	ecmsg &operator = (const ecmsg &) = delete ;
	int loadbuf	(cchar *,int = -1) noex ;
	void dtor	() noex ;
	operator int () noex ;
} ; /* end struct (ecmsg) */
#else	/* __cplusplus */
typedef ECMSG		ecmsg ;
#endif /* __cplusplus */

EXTERNC_begin

extern int ecmsg_start		(ecmsg *) noex ;
extern int ecmsg_loadbuf	(ecmsg *,cchar *,int) noex ;
extern int ecmsg_already	(ecmsg *) noex ;
extern int ecmsg_finish		(ecmsg *) noex ;

EXTERNC_end


#endif /* ECMSG_INCLUDE */


