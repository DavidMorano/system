/* mkuuid SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* make UUID (also a specialized object) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for calendar operations.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkuuid

	Description:
	We create a UUID.

	Synpsis:
	int mkuuid(uuid_dat *up,int ver) noex

	Arguments:
	up		pointer to uuid_dat object
	ver		version desired

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>		/* <- for |UINT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>		/* <- for |memclear| */
#include	<mkuuid.h>
#include	<localmisc.h>

#include	"mkuuid.h"


/* local defines */

#define	NWORDS	4


/* external subroutines */

extern "C" {
    extern int	getrand(void *,int) noex ;
}


/* forward references */

static int mkuutime(uuid_dat *,uint *) noex ;
static int mkuuclk(uuid_dat *,uint *) noex ;
static int mkuunode(uuid_dat *,uint *) noex ;


/* exported subroutines */

int mkuuid(uuid_dat *up,int) noex {
	cint		rsize = (NWORDS * sizeof(uint)) ;
	uint		rwords[NWORDS+1] ;
	int		rs ;
	memclear(up) ;
	if ((rs = getrand(rwords,rsize)) >= 0) {
	    up->version = 4 ;
	    mkuutime(up,rwords) ;
	    mkuuclk(up,rwords) ;
	    mkuunode(up,rwords) ;
	} /* end if (reading random) */
	return rs ;
}
/* end subroutine (mkuuid) */


/* local subroutines */

static int mkuutime(uuid_dat *up,uint *rwords) noex {
	uint64_t	tv = 0 ;
	uint64_t	v ;
	{
	    v = (rwords[0] & UINT_MAX) ;
	    tv |= v ;
	}
	{
	    v = (rwords[1] & UINT_MAX) ;
	    tv |= (v << 32) ;
	}
	up->time = tv ;
	return 0 ;
}
/* end subroutine (mkuutime) */

static int mkuuclk(uuid_dat *up,uint *rwords) noex {
	uint64_t	v ;
	{
	    v = (rwords[2] & UINT_MAX) ;
	    v >>= 16 ;
	}
	up->clk = v ;
	return 0 ;
}
/* end subroutine (mkuuclk) */

static int mkuunode(uuid_dat *up,uint *rwords) noex {
	uint64_t	nv = 0 ;
	uint64_t	v ;
	{
	    v = (rwords[2] & UINT_MAX) ;
	    v &= USHORT_MAX ;
	    nv |= (v << 32) ;
	}
	{
	    v = (rwords[3] & UINT_MAX) ;
	    nv |= v ;
	}
	up->node = nv ;
	return 0 ;
}
/* end subroutine (mkuunode) */


