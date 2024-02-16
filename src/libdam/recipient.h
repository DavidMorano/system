/* recipient HEADER */
/* lang=C20 */

/* recipient processing structures */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	RECIPIENT_INCLUDE
#define	RECIPIENT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<hdb.h>
#include	<vecstr.h>
#include	<localmisc.h>


#define	RECIPIENT		struct recipient_head
#define	RECIPIENT_VAL		struct recipient_value
#define	RECIPIENT_HCUR		int
#define	RECIPIENT_VCUR		HDB_CUR

#define	RECIPIENT_NOHOST	"*nohost*"


struct recipient_head {
	HDB		hash ;		/* hold entire entries */
	VECSTR		names ;		/* hold host names */
} ;

struct recipient_value {
	cchar		*a ;		/* memory-allocation */
	cchar		*hostpart ;
	cchar		*localpart ;
	int		type ;
} ;

typedef RECIPIENT	recipient ;

EXTERNC_begin

extern int recipient_start(RECIPIENT *,int) noex ;
extern int recipient_finish(RECIPIENT *) noex ;
extern int recipient_add(RECIPIENT *,cchar *,cchar *,int) noex ;
extern int recipient_counthosts(RECIPIENT *) noex ;
extern int recipient_count(RECIPIENT *) noex ;
extern int recipient_hcurbegin(RECIPIENT *,RECIPIENT_HCUR *) noex ;
extern int recipient_hcurend(RECIPIENT *,RECIPIENT_HCUR *) noex ;
extern int recipient_enumhost(RECIPIENT *,RECIPIENT_HCUR *,
			cchar **) noex ;
extern int recipient_vcurbegin(RECIPIENT *,RECIPIENT_VCUR *) noex ;
extern int recipient_vcurend(RECIPIENT *,RECIPIENT_VCUR *) noex ;
extern int recipient_fetchvalue(RECIPIENT *,cchar *,
		RECIPIENT_VCUR *, RECIPIENT_VAL **) noex ;
extern int recipient_already(RECIPIENT *,cchar *,cchar *,int) noex ;

EXTERNC_end


#endif /* RECIPIENT_INCLUDE */


