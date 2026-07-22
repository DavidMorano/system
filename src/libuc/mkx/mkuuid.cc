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
#include	<sys/time.h>		/* POSIX® |TIMEVAL| */
#include	<climits>		/* CSTD |UINT_MAX| + |CHAR_BIT| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucsysmisc.h>		/* LIBUC */
#include	<ucgetx.h>		/* LIBUC |uc_gettimeofday(3uc)| */
#include	<getnodename.h>		/* LIBUC */
#include	<getrand.h>		/* LIBUC */
#include	<mkuuid.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"mkuuid.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

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

local ulong		loadbytes(cc *,int) noex ;


/* local variables */

constexpr mkuu_m	makes[] = {
	&mkuu::mkuutime,
	&mkuu::mkuuclk,
	&mkuu::mkuunode
} ; /* end array (makes) */


/* exported variables */


/* exported subroutines */

int mkuuid(uuid_dat *up,int ver) noex {
	int		rs = SR_FAULT ;
	if (up) ylikely {
    	    mkuu uu(up) ;
	    rs = uu(ver) ;
	} /* end if (non-null) */
    	return rs ;
} /* end subroutine (mkuuid) */


/* local subroutines */

int mkuu::operator () (int ver) noex {
	cint		rsz = (NWORDS * szof(uint)) ;
	int		rs ;
	if (ver <= 0) ver = UUID_VERSION ;
	memclear(up) ;
	if ((rs = getrand(rwords,rsz)) >= 0) ylikely {
	    up->version = uchar(ver & 0x0F) ; /* <- four (4) bits */
	    for (cauto &m : makes) {
		rs = (this->*m)() ;
		if (rs < 0) break ;
	    } /* end for */
	} /* end if (reading random) */
	return rs ;
} /* end method (mkuu::operator) */

int mkuu::mkuutime() noex {
    	int		rs ;
    	if (timeval tv ; (rs = uc_gettimeofday(&tv,nullptr)) >= 0) ylikely {
	    ulong	rt = 0 ;	/* resulting-time */
	    ulong	v ;
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
} /* end method (mkuu::mkuutime) */

int mkuu::mkuuclk() noex {
	ulong	v ;
	{
	    v = rwords[rwi++] ;
	    v >>= 16 ;
	}
	up->clk = ushort(v) ;
	return 0 ;
} /* end method (mkuu::mkuuclk) */

/* contributes six bytes */
int mkuu::mkuunode() noex {
    	int		rs ;
	int		rs1 ;
	if (char *nbuf ; (rs = lm_mn(&nbuf)) >= 0) ylikely {
	    if ((rs = getnodename(nbuf,rs)) >= 0) ylikely {
	        ulong	nv = loadbytes(nbuf,rs) ;
	        ulong	v ;
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
	    rs1 = lm_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end method (mkuu::mkuunode) */

/* only need six significant bytes */
local ulong loadbytes(cc *nbuf,int nl) noex {
    	cint		six = 6 ; /* <- six bytes */
    	ulong	rv = 0 ;
	ulong	v ;
	for (int i = 0 ; i < nl ; i += 1) {
	    cint	ch = mkchar(nbuf[i]) ;
	    v = ulong(ch) ;
	    v <<= ((i % six) * CHAR_BIT) ;
	    rv |= v ;
	}
	return rv ;
} /* end subroutine (loadbytes) */


