/* babycalc HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* object load management for the BABYCALCS object */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BABYCALC_INCLUDE
#define	BABYCALC_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<time.h>		/* |time_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<modload.h>
#include	<babycalcs.h>


#define	BABYCALC_MAGIC		0x97147229
#define	BABYCALC		struct babycalc_head
#define	BABYCALC_FL		struct babycalc_flags
#define	BABYCALC_INFO		struct babycalc_information
#define	BABYCALC_MODBNAME	"babycalcs"
#define	BABYCALC_OBJNAME	"babycalcs"


struct babycalc_information {
	time_t		wtime ;
	time_t		atime ;
	uint		acount ;
} ;

struct babycalc_flags {
    	uint		modload:1 ;
} ;

struct babycalc_head {
	modload		*mlp ;		/* module-load-pointer */
	void		*obj ;		/* object pointer */
	void		*callp ;	/* calls-structure pointer */
	BABYCALC_FL	fl ;
	uint		magic ;
	int		objsz ;
	int		cursz ;
} ;

typedef	BABYCALC		babycalc ;
typedef	BABYCALC_FL		babycalc_fl ;
typedef	BABYCALC_INFO		babycalc_info ;

EXTERNC_begin

extern int	babycalc_open(babycalc *,cchar *,cchar *) noex ;
extern int	babycalc_check(babycalc *,time_t) noex ;
extern int	babycalc_lookup(babycalc *,time_t,uint *) noex ;
extern int	babycalc_getinfo(babycalc *,babycalc_info *) noex ;
extern int	babycalc_close(babycalc *) noex ;

EXTERNC_end


#endif /* BABYCALC_INCLUDE */


