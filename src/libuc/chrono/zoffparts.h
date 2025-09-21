/* zoffparts HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* manage time-zone offsets */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-08-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************
 
  	Object:
	zoffparts

	Description:
	These two small subroutines manipulate zone-offsets for use
	in time strings.

*******************************************************************************/

#ifndef	ZOFFPARTS_INCLUDE
#define	ZOFFPARTS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	ZOFFPARTS	struct zoffparts_head


struct zoffparts_head {
	uint	hours ;
	uint	mins ;
	int	zoff ;		/* value */
} ;

#ifdef	__cplusplus

struct zoffparts : zoffparts_head {
	zoffparts() = default ;
	zoffparts(const zoffparts &) = delete ;
	zoffparts &operator = (const zoffparts &) = delete ;
	int	set(int) noex ;
	int	get(int *) noex ;
	int	mkstr(char *,int) noex ;
} ; /* end struct (zoffparts) */
#else	/* __cplusplus */
typedef ZOFFPARTS	zoffparts ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	zoffparts_set(zoffparts *,int) noex ;
extern int	zoffparts_get(zoffparts *,int *) noex ;
extern int	zoffparts_mkstr(zoffparts *,char *,int) noex ;

EXTERNC_end


#endif /* ZOFFPARTS_INCLUDE */


