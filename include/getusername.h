/* getusername HEADER */
/* lang=C20 */

/* get the best approximation of the username of the current user */
/* version %I% last-modified %G% */


/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	GETUSERNAME_INCLUDE
#define	GETUSERNAME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* <- for |uid_t| */
#include	<pwd.h>
#include	<usystem.h>
#include	<vecstr.h>
#include	<localmisc.h>


#ifndef	GETXUSER
#define	GETXUSER	struct getxuser_head
#endif
#define	GETXUSER_TTL	(2*3600)	/* for cache */


struct getxuser_head {
	PASSWD		*pwp ;		/* caller supplied */
	char		*pwbuf ;	/* caller supplied */
	char		*ubuf ;		/* caller supplied */
	vecstr		*nlp ;		/* temporary storage */
	uid_t		uid ;		/* caller supplied */
	int		pwlen ;		/* caller supplied */
	int		ulen ;		/* caller supplied */
	int		unl ;		/* result of lookup */
	int		pwl ;		/* result of PASSWD lookup */
	uint		f_self:1 ;	/* it is us */
	uint		f_tried:1 ;	/* tried cache already */
} ;

typedef GETXUSER	getxuser ;

EXTERNC_begin

extern int getxusername(getxuser *) noex ;
extern int getusername(char *,int,uid_t) noex ;
extern int getpwusername(PASSWD *,char *,int,uid_t) noex ;

EXTERNC_end


#endif /* GETUSERNAME_INCLUDE */


