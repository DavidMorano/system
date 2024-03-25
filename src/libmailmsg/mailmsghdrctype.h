/* mailmsghdrctype HEADER */
/* lang=C20 */

/* manage content-types with header field */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */


#ifndef	MAILMSGHDRCTYPE_INCLUDE
#define	MAILMSGHDRCTYPE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>


/* object defines */
#define	MAILMSGHDRCTYPE		struct mailmsghdrctype_head
#define	MAILMSGHDRCTYPE_PAR	struct mailmsghdrctype_param
#define	MAILMSGHDRCTYPE_NPARAMS	10


struct mailmsghdrctype_param {
	cchar		*kp, *vp ;
	int		kl, vl ;
} ;

struct mailmsghdrctype_head {
	cchar		*mtp ;		/* main-type */
	cchar		*stp ;		/* sub-type */
	int		mtl ;		/* main-type */
	int		stl ;		/* sub-type */
	MAILMSGHDRCTYPE_PAR	p[MAILMSGHDRCTYPE_NPARAMS] ;
} ;

typedef	MAILMSGHDRCTYPE		mailmsghdrctype ;
typedef	MAILMSGHDRCTYPE_PARAM	mailmsghdrctype_par ;


EXTERNC_begin

extern int mailmsghdrctype_start(MAILMSGHDRCTYPE *,cchar *,int) noex ;
extern int mailmsghdrctype_paramget(MAILMSGHDRCTYPE *,int,
		mailmsghdrctype_param *) noex ;
extern int mailmsghdrctype_paramfind(MAILMSGHDRCTYPE *,cchar *,
		mailmsghdrctype_param *) noex ;
extern int mailmsghdrctype_finish(MAILMSGHDRCTYPE *) noex ;

EXTERNC_end


#endif /* MAILMSGHDRCTYPE_INCLUDE */



