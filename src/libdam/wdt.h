/* wdt HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* walk a directory tree */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	WDT_INCLUDE
#define	WDT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/stat.h>		/* |USTAT(2)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


/* mode values for call */
#define	WDT_MFOLLOW	1
/* return values */
#define	WDT_ROK		0
#define	WDT_RBADTMP	SR_ACCESS
#define	WDT_RBADTMPOPEN	SR_NOENT
#define	WDT_RBADWRITE	SR_DQUOT


EXTERNC_begin

typedef	int	(*wdt_f)(cchar *,USTAT *,void *) noex ;
extern int	wdt(cchar *,int,wdt_f,void *) noex ;

EXTERNC_end



#endif /* WDT_INCLUDE */


