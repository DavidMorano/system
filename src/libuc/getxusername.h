/* getxusername HEADER */
/* lang=C20 */

/* get the best approximation of the user's username */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETXUSERNAME_INCLUDE
#define	GETXUSERNAME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<pwd.h>
#include	<usystem.h>
#include	<vecstr.h>
#include	<localmisc.h>


#ifndef	GETXUSERNAME
#define	GETXUSERNAME		struct getxusername
#endif
#define	GETXUSERNAME_TTL	(2*3600)	/* for cache */


struct getxusername {
	PASSWD		*pwp ;		/* caller supplied */
	char		*pwbuf ;	/* caller supplied */
	char		*ubuf ;		/* caller supplied */
	vecstr		names ;		/* temporary storage */
	uid_t		uid ;		/* caller supplied */
	int		pwlen ;		/* caller supplied */
	int		ulen ;		/* caller supplied */
	int		unl ;		/* result of lookup */
	int		pwl ;		/* result of PASSWD lookup */
	uint		f_self:1 ;	/* it is us */
	uint		f_tried:1 ;	/* tried cache already */
} ;


#ifdef	__cplusplus
extern "C" {
#endif

extern int getxusername(GETXUSERNAME *) noex ;
extern int getusername(char *,int,uid_t) noex ;
extern int getpwusername(PASSWD *,char *,int,uid_t) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* GETXUSERNAME_INCLUDE */


