/* langparse HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Quoted-Printable (QP) decoder */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

#ifndef	LANGPARSE_INCLUDE
#define	LANGPARSE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	LANGPARSE_MAGIC		0x13f3c205
#define	LANGPARSE		struct langparse_head
#define	LANGPARSE_FL		struct langparse_flags
#define	LANGPARSE_NSTAGE	2


struct langparse_flags {
	uint		esc:1 ;
	uint		space:1 ;
} ;

struct langparse_head {
	void		*outbuf ;	/* output-buffer */
	LANGPARSE_FL	fl ;
	uint		magic ;
	int		rl ;		/* stage length */
	char		rb[LANGPARSE_NSTAGE + 1] ;	/* stage buffer */
} ;

typedef	LANGPARSE	langparse ;
typedef	LANGPARSE_FL	langparse_fl ;

EXTERNC_begin

extern int langparse_start	(langparse *,int) noex ;
extern int langparse_load	(langparse *,cchar *,int) noex ;
extern int langparse_read	(langparse *,char *,int) noex ;
extern int langparse_finish	(langparse *) noex ;

EXTERNC_end


#endif /* LANGPARSE_INCLUDE */


