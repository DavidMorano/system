/* emainfo HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* parse mail route-address host and local parts */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-12, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	EMAINFO_INCLUDE
#define	EMAINFO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ema.h>			/* LIBUC |ematypes| */
#include	<mailaddrtypes.h>	/* LIBMAILMSG */


/* object defines */
#define	EMAINFO			struct emainfo_head
/* other defines */
#define	EMAINFO_TLOCAL		mailaddrtype_local
#define	EMAINFO_TUUCP		mailaddrtype_uucp
#define	EMAINFO_TARPA		mailaddrtype_arpa
#define	EMAINFO_TARPAROUTE	mailaddrtype_arparoute
/* special token */
#define	EMAINFO_LOCALHOST	"*LOCAL*"


struct emainfo_head {
	cchar		*lpart ;
	cchar		*hpart ;
	mailaddrtypes	type ;
	int		llen ;
	int		hlen ;
} ; /* end struct (emainfo_head) */

typedef	EMAINFO		emainfo ;

EXTERNC_begin

extern int	emainfo_load(emainfo *,cchar *,int) noex ;
extern int	emainfo_mktype(emainfo *,mailaddrtypes,char *,int) noex ;

EXTERNC_end


#endif /* EMAINFO_INCLUDE */


