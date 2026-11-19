/* usupport_cmpstrv HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* c-string comparisons w/ case variations */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTCMPSTRV_INCLUDE
#define	USUPPORTCMPSTRV_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


namespace libu {
extern int	cmpstrval	(cchar *,cchar *) noex ;
extern int	cmpstrvalø	(cchar *,cchar *) noex ;
} /* end namespace (libu) */


#endif /* USUPPORTCMPSTRV_INCLUDE */


