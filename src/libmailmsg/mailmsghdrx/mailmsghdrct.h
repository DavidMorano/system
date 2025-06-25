/* mailmsghdrct HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* manage content-types with header field */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MAILMSGHDRCT_INCLUDE
#define	MAILMSGHDRCT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	MAILMSGHDRCT		struct mailmsghdrct_head
#define	MAILMSGHDRCT_PAR	struct mailmsghdrct_param
#define	MAILMSGHDRCT_MAGIC	0x53232857
#define	MAILMSGHDRCT_NPARAMS	10


struct mailmsghdrct_param {
	cchar		*kp ;
	cchar		*vp ;
	int		kl ;
	int		vl ;
} ;

struct mailmsghdrct_head {
	cchar		*mtp ;		/* type-main */
	cchar		*stp ;		/* type-sub */
	uint		magic ;
	int		mtl ;		/* type-main */
	int		stl ;		/* type-sub */
	MAILMSGHDRCT_PAR	p[MAILMSGHDRCT_NPARAMS] ;
} ;

typedef	MAILMSGHDRCT		mailmsghdrct ;
typedef	MAILMSGHDRCT_PAR	mailmsghdrct_par ;

EXTERNC_begin

extern int mailmsghdrct_start(mailmsghdrct *,cchar *,int) noex ;
extern int mailmsghdrct_paramget(mailmsghdrct *,int,
		mailmsghdrct_par *) noex ;
extern int mailmsghdrct_paramfind(mailmsghdrct *,cchar *,
		mailmsghdrct_par *) noex ;
extern int mailmsghdrct_finish(mailmsghdrct *) noex ;

EXTERNC_end


#endif /* MAILMSGHDRCT_INCLUDE */



