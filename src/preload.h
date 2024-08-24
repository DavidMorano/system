/* preload HEADER */
/* lang=C20 */

/* UNIX® program-data cache */
/* version %I% last-modified %G% */


/* revision history:

	- 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PRELOAD_INCLUDE
#define	PRELOAD_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>


/* registry */
#define	PRELOAD_DCSPATH		0	/* CSPATH */
#define	PRELOAD_DHWSERIAL	1	/* HW-SERIAL */
#define	PRELOAD_DSYSTAT		2	/* sys-stat */


EXTERNC_begin

extern int preload_init() ;
extern int preload_set(int,const char *,int,int) ;
extern int preload_get(int,char *,int) ;
extern void preload_fini() ;

EXTERNC_end


#endif /* PRELOAD_INCLUDE */


