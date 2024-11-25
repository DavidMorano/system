/* codebal HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* HEX decoder */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

#ifndef	CODEBAL_INCLUDE
#define	CODEBAL_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	CODEBAL_MAGIC	0x13f3c203
#define	CODEBAL		struct codebal_head
#define	CODEBAL_NCH	3


struct codebal_head {
	uint		magic ;
	int		counts[CODEBAL_NCH] ;
	int		f_fail ;
} ;

typedef	CODEBAL		codebal ;

EXTERNC_begin

extern int codebal_start(codebal *) noex ;
extern int codebal_load(codebal *,cchar *,int) noex ;
extern int codebal_read(codebal *,char *,int) noex ;
extern int codebal_finish(codebal *) noex ;

EXTERNC_end


#endif /* CODEBAL_INCLUDE */


