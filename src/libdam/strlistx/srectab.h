/* srectab HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* record-table */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SRECTAB_INCLUDE
#define	SRECTAB_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* systems IDs */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	SRECTAB		struct srectab_head


struct srectab_head {
	uint		*rt ;
	int		i ;		/* highest index */
	int		n ;		/* extent */
} ;

typedef	SRECTAB		srectab ;

EXTERNC_begin

extern int	srectab_start(srectab *,int) noex ;
extern int	srectab_add(srectab *,uint) noex ;
extern int	srectab_done(srectab *) noex ;
extern int	srectab_getvec(srectab *,uint **) noex ;
extern int	srectab_extend(srectab *) noex ;
extern int	srectab_finish(srectab *) noex ;
extern int	srectab_count(srectab *) noex ;

EXTERNC_end


#endif /* SRECTAB_INCLUDE */


