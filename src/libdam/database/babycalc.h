/* babycalc */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BABYCALC_INCLUDE
#define	BABYCALC_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<time.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<modload.h>
#include	<babycalcs.h>


#define	BABYCALC_MAGIC		0x97147229
#define	BABYCALC		struct babycalc_head
#define	BABYCALC_CA		struct babycalc_calls
#define	BABYCALC_INFO		struct babycalc_information


struct babycalc_i {
	time_t		wtime ;
	time_t		atime ;
	uint		acount ;
} ;

EXTERNC_begin
struct babycalc_calls {
	int		(*open)(void *,cchar *,cchar *) noex ;
	int		(*check)(void *,time_t) noex ;
	int		(*lookup)(void *,time_t,uint *) noex ;
	int		(*info)(void *,babycalcs_info *) noex ;
	int		(*close)(void *) noex ;
} ;
EXTERNC_end

struct babycalc_head {
	modload		*lop ;
	void		*obj ;		/* object pointer */
	BABYCALC_CA	call ;
	uint		magic ;
} ;

typedef	BABYCALC		babycalc ;
typedef	BABYCALC_CA		babycalc_ca ;
typedef	BABYCALC_INFO		babycalc_info ;

EXTERNC_begin

extern int	babycalc_open(babycalc *,cchar *,cchar *) noex ;
extern int	babycalc_check(babycalc *,time_t) noex ;
extern int	babycalc_lookup(babycalc *,time_t,uint *) noex ;
extern int	babycalc_info(babycalc *,babycalc_info *) noex ;
extern int	babycalc_close(babycalc *) noex ;

EXTERNC_end


#endif /* BABYCALC_INCLUDE */


