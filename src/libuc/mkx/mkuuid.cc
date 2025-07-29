/* mkuuid SUPPORT */
/* charset=ISO8859-1 */
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
	I create a UUID.  I try to follow the forumla for creating
	it according to the rules set out by Micro$oft, but it not
	a perfect algorithm.  But hopefully, it is more than adquate
	for making very unique strings.

	Synpsis:
	int mkuuid(uuid_dat *up,int ver) noex

	Arguments:
	up		pointer to UUID_DAT object
	ver		version desired (default == 0)

	Returns:
	>=0		OK
	<0		error (system-return)

	Notes:
	There are bugs in all compilers.  I try to stay calm (as
	mush as possible) when I come across one (bug).  The GCC
	compiler when invoked to flag narrowing conversions
	(conversions that may lose precision or otherwise change a
	value) falsely does not allow (flags as a porlbme) any
	attempt to assign a value to an unsigned bit field to the
	structure 'uuid_dat' member field 'version'.  I had to
	actually change the bit-field member variable to type 'uchar'
	in order to get a clean compile.  F*ck GCC!  Why this happend
	here in the previously present circumstances is anybody's
	guess.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/time.h>		/* |TIMEVAL| */
#include	<stdint.h>
#include	<climits>		/* |UINT_MAX| + |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<getnodename.h>
#include	<getrand.h>
#include	<mkuuid.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"mkuuid.h"

import libutil ;

/* local defines */

#define	NWORDS		4		/* number of random words needed */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct mkuu {
	uuid_dat	*up ;
	uint		rwords[NWORDS+2] ;	/* random-words */
	int		rwi{} ;
	mkuu(uuid_dat *p) noex : up(p) { } ;
	int operator () (int) noex ;
	int mkuutime() noex ;
	int mkuuclk() noex ;
	int mkuunode() noex ;
    } ; /* end struct (mkuu) */
    typedef int (mkuu::*mkuu_m)() noex ;
} /* end namespace */


/* forward references */

static uint64_t		loadbytes(cc *,int) noex ;


/* local variables */

constexpr mkuu_m	mems[] = {
	&mkuu::mkuutime,
	&mkuu::mkuuclk,
	&mkuu::mkuunode
} ;


/* exported variables */


/* exported subroutines */

int mkuuid(uuid_dat *up,int ver) noex {
	int		rs = SR_FAULT ;
	if (up) ylikely {
    	    mkuu	uu(up) ;
	    rs = uu(ver) ;
	}
    	return rs ;
}
/* end subroutine (mkuuid) */


/* local subroutines */

int mkuu::operator () (int ver) noex {
	cint		rsz = (NWORDS * szof(uint)) ;
	int		rs ;
	if (ver <= 0) ver = UUID_VERSION ;
	memclear(up) ;
	if ((rs = getrand(rwords,rsz)) >= 0) ylikely {
	    up->version = uchar(ver & 0x0F) ; /* <- four (4) bits */
	    for (cauto &m : mems) {
		rs = (this->*m)() ;
		if (rs < 0) break ;
	    }
	} /* end if (reading random) */
	return rs ;
}
/* end method (mkuu::operator) */

int mkuu::mkuutime() noex {
    	int		rs ;
    	if (timeval tv ; (rs = uc_gettimeofday(&tv,nullptr)) >= 0) ylikely {
	    uint64_t	rt = 0 ;	/* resulting-time */
	    uint64_t	v ;
	    {
	        v = tv.tv_sec ;
	        rt |= (v << 32) ;
	    }
	    {
		v = (rwords[rwi++] >> 12) ; /* mask off high end */
	        rt |= (v << 20) ;
	        v = tv.tv_usec ;
		rt |= v ;
	    }
	    up->time = rt ;
	} /* end if (uc_gettimeofday) */
	return rs ;
}
/* end method (mkuu::mkuutime) */

int mkuu::mkuuclk() noex {
	uint64_t	v ;
	{
	    v = rwords[rwi++] ;
	    v >>= 16 ;
	}
	up->clk = ushort(v) ;
	return 0 ;
}
/* end method (mkuu::mkuuclk) */

/* contributes six bytes */
int mkuu::mkuunode() noex {
    	int		rs ;
	if (char *nbuf ; (rs = malloc_mn(&nbuf)) >= 0) ylikely {
	    if ((rs = getnodename(nbuf,rs)) >= 0) ylikely {
	        uint64_t	nv = loadbytes(nbuf,rs) ;
	        uint64_t	v ;
	        {
	            v = rwords[rwi++] ;
	            v &= USHORT_MAX ;
	            nv |= (v << 32) ;
	        }
	        {
	            v = rwords[rwi++] ;
	            nv |= v ;
	        }
	        up->node = nv ;
	    } /* end if (getnodename) */
	    rs = rsfree(rs,nbuf) ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end method (mkuu::mkuunode) */

/* only need six significant bytes */
static uint64_t loadbytes(cc *nbuf,int nl) noex {
    	cint		six = 6 ; /* <- six bytes */
    	uint64_t	rv = 0 ;
	uint64_t	v ;
	for (int i = 0 ; i < nl ; i += 1) {
	    cint	ch = mkchar(nbuf[i]) ;
	    v = uint64_t(ch) ;
	    v <<= ((i % six) * CHAR_BIT) ;
	    rv |= v ;
	}
	return rv ;
}
/* end subroutine (loadbytes) */


