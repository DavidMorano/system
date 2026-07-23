/* ucdescmanip HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® File-Descriptor Manipulation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	uc_duper
	uc_moveup

	Descrptor:
	I duplication file-descriptors.

*******************************************************************************/

#ifndef	UCDESCMANIP_INCLUDE
#define	UCDESCMANIP_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int uc_duper(int,int) noex ;
extern int uc_moveup(int,int) noex ;

local inline int dupup(int fd,int min) noex {
	return uc_duper(fd,min) ;
}

EXTERNC_end


#endif /* UCDESCMANIP_INCLUDE */


