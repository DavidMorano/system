/* mailenvelope HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MAILENVELOPE_INCLUDE
#define	MAILENVELOPE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<localmisc.h>


#define	MAILENVELOPE		struct mailenvelope

#define	MAILENVELOPE_ADDRESSLEN		((2 * MAXHOSTNAMELEN) + 9)
#define	MAILENVELOPE_DATELEN		255
#define	MAILENVELOPE_REMOTELEN		MAXHOSTNAMELEN


struct mailenvelope {
	int	f_escape ;
	char	address[MAILENVELOPE_ADDRESSLEN + 1] ;
	char	date[MAILENVELOPE_DATELEN + 1] ;
	char	remote[MAILENVELOPE_REMOTELEN + 1] ;
} ;

EXTERNC_begin

extern int mailenvelope_parse(MAILENVELOPE *,char *) noex ;

EXTERNC_end


#endif /* MAILENVELOPE_INCLUDE */


