/* filegrp HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* group-name cache */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-01-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

#ifndef	FILEGRP_INCLUDE
#define	FILEGRP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types |gid_t| */
#include	<usystem.h>
#include	<vechand.h>
#include	<cq.h>


#define	FILEGRP			struct filegrp_head
#define	FILEGRP_ST		struct filegrp_statistics
#define	FILEGRP_MAGIC		0x98643163
#define	FILEGRP_DEFENTS		10
#define	FILEGRP_DEFMAX		20	/* default maximum entries */
#define	FILEGRP_DEFTTL		600	/* default time-to-live */
#define	FILEGRP_MAXFREE		4


struct filegrp_statistics {
	uint		nentries ;
	uint		total ;			/* access */
	uint		refreshes ;
	uint		phits, pmisses ;	/* positive */
	uint		nhits, nmisses ;	/* negative */
} ;

struct filegrp_head {
	cq		*flp ;
	vechand		*alp ;
	time_t		ti_check ;
	FILEGRP_ST	s ;
	uint		magic ;
	int		ttl ;		/* time-to-live in seconds */
	int		nmax ;		/* maximum entries */
} ;

typedef FILEGRP		filegrp ;
typedef FILEGRP_ST	filegrp_st ;

EXTERNC_begin

extern int filegrp_start	(filegrp *,int,int) noex ;
extern int filegrp_add		(filegrp *,gid_t,cchar *) noex ;
extern int filegrp_lookname	(filegrp *,cchar *,int) noex ;
extern int filegrp_lookgid	(filegrp *,char *,int,gid_t) noex ;
extern int filegrp_check	(filegrp *,time_t) noex ;
extern int filegrp_stats	(filegrp *,filegrp_st *) noex ;
extern int filegrp_finish	(filegrp *) noex ;

EXTERNC_end


#endif /* FILEGRP_INCLUDE */


