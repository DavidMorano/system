/* recipient HEADER */
/* charset=ISO8859-1 */
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
#include	<hdb.h>
#include	<vecstr.h>


#define	RECIPIENT		struct recipient_head
#define	RECIPIENT_VAL		struct recipient_value
#define	RECIPIENT_HCUR		int
#define	RECIPIENT_VCUR		hdb_cur
#define	RECIPIENT_MAGIC		0x87542381

#define	RECIPIENT_NOHOST	"*nohost*"


struct recipient_head {
	hdb		*hlp ;		/* hold entire entries */
	vecstr		*nlp ;		/* hold host names */
	uint		magic ;
} ;

struct recipient_value {
	cchar		*a ;		/* memory-allocation */
	cchar		*hostpart ;
	cchar		*localpart ;
	int		type ;
} ;

typedef RECIPIENT	recipient ;
typedef	RECIPIENT_VAL	recipient_val ;
typedef	RECIPIENT_HCUR	recipient_hcur ;
typedef	RECIPIENT_VCUR	recipient_vcur ;

EXTERNC_begin

extern int recipient_start(recipient *,int) noex ;
extern int recipient_finish(recipient *) noex ;
extern int recipient_add(recipient *,cchar *,cchar *,int) noex ;
extern int recipient_counthosts(recipient *) noex ;
extern int recipient_count(recipient *) noex ;
extern int recipient_hcurbegin(recipient *,recipient_hcur *) noex ;
extern int recipient_hcurend(recipient *,recipient_hcur *) noex ;
extern int recipient_enumhost(recipient *,recipient_hcur *,
			cchar **) noex ;
extern int recipient_vcurbegin(recipient *,recipient_vcur *) noex ;
extern int recipient_vcurend(recipient *,recipient_vcur *) noex ;
extern int recipient_fetchvalue(recipient *,cchar *,
		recipient_vcur *,recipient_val **) noex ;
extern int recipient_already(recipient *,cchar *,cchar *,int) noex ;

EXTERNC_end


#endif /* RECIPIENT_INCLUDE */


