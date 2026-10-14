/* termstore HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* store terminal attributes */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	TERMSTORE_INCLUDE
#define	TERMSTORE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<termios.h>		/* POSIX® */
#include	<stddef.h>		/* POSIX® */
#include	<stdlib.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	TERMSTORE	struct termstore

struct termstore {
	TERMIOS		ta ;
	int		fd ;
	mode_t		mode ;
	bool		f_stored ;
} ; /* end struct */

typedef TERMSTORE	termstore ;

EXTERNC_begin

extern int termstore_save	(termstore *,int) noex ;
extern int termstore_restore	(termstore *) noex ;

EXTERNC_end


#endif /* TERMSTORE_INCLUDE */


