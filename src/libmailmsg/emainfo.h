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
#include	<clanguage.h>
#include	<usysbase.h>
#include	<ematypes.h>


/* object defines */
#define	EMAINFO			struct emainfo_head
/* other defines */
#define	EMAINFO_TLOCAL		ematype_local
#define	EMAINFO_TUUCP		ematype_uucp
#define	EMAINFO_TARPA		ematype_arpa
#define	EMAINFO_TARPAROUTE	ematype_arparoute
/* special token */
#define	EMAINFO_LOCALHOST	"*LOCAL*"


struct emainfo_head {
	cchar		*lpart ;
	cchar		*hpart ;
	ematypes	type ;
	int		llen ;
	int		hlen ;
} ; /* end struct (emainfo_head) */

typedef	EMAINFO		emainfo ;

EXTERNC_begin

extern int	emainfo_load(emainfo *,cchar *,int) noex ;
extern int	emainfo_mktype(emainfo *,ematypes,char *,int) noex ;

EXTERNC_end


#endif /* EMAINFO_INCLUDE */


