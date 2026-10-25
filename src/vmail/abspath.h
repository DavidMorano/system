/* abspath HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* matters relating to absolute file-names */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	ABSPATH_INCLUDE
#define	ABSPATH_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<stddef.h>		/* POSIX® */
#include	<stdlib.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */


EXTERNC_begin
extern int	initabspath()			noex ;
extern int	abspath(cchar *,char *)		noex ;
extern int	nchdir(char *)			noex ;
extern char	*getwd(char *)			noex ;
extern char	*SaveAbs(char *)		noex ;
extern void	getcurwd(char *)		noex ;
EXTERNC_end


#endif /* ABSPATH_INCLUDE */


