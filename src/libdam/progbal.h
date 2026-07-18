/* progbal HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* language totken balance checker support */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

#ifndef	PROGBAL_INCLUDE
#define	PROGBAL_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */


#define	PROGBAL		struct progbal_head
#define	PROGBAL_NCH	3
#define	PROGBAL_MAGIC	0x13f3c203


struct progbal_head {
	uint		magval ;
	int		counts[PROGBAL_NCH] ;
	int		f_fail ;
} ; /* end struct (progval_head) */

typedef	PROGBAL		progbal ;

EXTERNC_begin

extern int progbal_start	(progbal *) noex ;
extern int progbal_load		(progbal *,cchar *,int) noex ;
extern int progbal_read		(progbal *,char *,int) noex ;
extern int progbal_finish	(progbal *) noex ;

EXTERNC_end


#endif /* PROGBAL_INCLUDE */


