/* ucfdmanip INCLUDE */
/* lang=C20 */

/* UNIX� File-Descriptor Manipulation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A�D� Morano
	This module was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	uc_duper
	uc_moveup

	Descrptor:
	I duplication file-descriptors.

*******************************************************************************/

#ifndef	UCFDMANIP_INCLUDE
#define	UCFDMANIP_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int uc_duper(int,int) noex ;
extern int uc_moveup(int,int) noex ;

static inline int dupup(int fd,int min) noex {
	return uc_duper(fd,min) ;
}

#ifdef	__cplusplus
}
#endif


#endif /* UCFDMANIP_INCLUDE */


