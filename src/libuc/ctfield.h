/* ctfield INCLUDE */
/* lang=C20 */

/* structure definition for ctfield extraction calls */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	File was originally written.
	
*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CTFIELD_INCLUDE
#define	CTFIELD_INCLUDE


#include	<envstandards.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<stdintx.h>


struct ctfield_head {
	char	*lp ;			/* line pointer */
	char	*fp ;			/* ctfield pointer */
	int	rlen ;			/* line length remaining */
	int	flen ;			/* ctfield length */
	int	term ;			/* terminating character */
} ;

#define	CTFIELD		struct ctfield_head

#ifdef	__cplusplus
extern "C" {
#endif

extern int field_start(CTFIELD *,const char *,int) noex ;
extern int field_get(CTFIELD *,const unsigned char *) noex ;
extern int field_term(CTFIELD *,const unsigned char *) noex ;
extern int field_sharg(CTFIELD *,const unsigned char *,char *,int) noex ;
extern int field_finish(CTFIELD *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* CTFIELD_INCLUDE */


