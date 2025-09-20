/* numincr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* number-incrementer */
/* version %I% last-modified %G% */


/* revision history:

	- 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	NUMINCR_INCLUDE
#define	NUMINCR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


/* object defines */
#define	NUMINCR		struct numincr_head
#define	NUMINCR_FL	struct numincr_flags
#define	NUMINCR_DEFENTS	10
#define	NUMINCR_MAXPREC	18


struct numincr_flags {
	uint		uc:1 ;		/* upper case */
	uint		alpha:1 ;	/* alpha numbers */
} ;

struct numincr_head {
	NUMINCR_FL	fl ;
	int		v ;		/* value */
	int		b ;		/* base (not used?) */
	int		prec ;		/* precision */
} ;

typedef	NUMINCR		numincr ;
typedef	NUMINCR_FL	numincr_fl ;

EXTERNC_begin

extern int numincr_start(numincr *,cchar *,int) noex ;
extern int numincr_load(numincr *,cchar *,int) noex ;
extern int numincr_setprec(numincr *,int) noex ;
extern int numincr_incr(numincr *,int) noex ;
extern int numincr_cvtstr(numincr *,char *,int,int) noex ;
extern int numincr_finish(numincr *) noex ;

EXTERNC_end


#endif /* NUMINCR_INCLUDE */


