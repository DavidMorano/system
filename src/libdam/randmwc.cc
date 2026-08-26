/* randmwc SUPPORT (RNG Multiply-With-Carry) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* random number generator (Multiply-With-Carry) */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debugging */

/* revision history:

	= 1998-04-23, David A­D­ Morano
	I made this subroutine to make a random number (scramble
	up one really) for 64-bit integers. I am not aware of any
	existing random number generator (scrambler) for 64-bit
	integers so far. I suppose that once all machines are 64
	bits (and they adhere to the LP64 model) all 'long's will
	be 64 bits without us even knowing about it! Whatever.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	randmwc

	Description:
	This object calculates random numbers using a multiply-with-carry
	algorithm.  This algorithm became popular just before
	subtract-with-borrow started to gain popularity.  Obviously
	subtracting with a borrow is cheaper than a multiply with
	a carry, but hey, this is still is use.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"randmwc.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import bitmanip ;			/* |nbits(3u)| */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* compile-time debugging */
#endif


/* external subroutines */

extern "C" {
    extern int	randlc(int) noex ;
} /* end extern (C) */


/* forward references */

local uint	single(uint,uint,uint,uint *) noex ;


/* local variables */

constexpr uint		aes[] = {
	1967773755,
	1517746329,
	1447497129,
	1655692410,
	1606218150,
	2051013963,
	1075433238,
	1557985959,
	1781943330,
	1893513180
} ; /* end array */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int randmwc_start(randmwc *op,int ai,uint seed) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_NOTSUP ;
	    if (ai < nelem(aes)) {
	        ulong	xhi, xlo ;
	        ulong	chi, clo ;
	        uint	oxlo ;
	        uint	oclo ;
	        uint	oxhi ;
	        uint	ochi ;
	        uint	nxhi, nxlo ;
	        uint	nchi, nclo ;
		rs = SR_OK ;
	        if (ai < 0) ai = 0 ;
	        op->a = aes[ai] ;
	        if (seed == 0) {
	            seed = 31415926 ;
	        }
	        {
	            oxlo = (uint) randlc(seed) ;
	            oclo = (uint) randlc(oxlo) ;
	        }
	        DEBUGPRINTF("oxlo=%08x oclo=%08x\n",oxlo,oclo) ;
	        nxlo = single(op->a,oxlo,oclo,&nclo) ;
	        {
	            oxhi = (uint) randlc(nxlo) ;
	            ochi = (uint) randlc(oxhi) ;
	        }
	        DEBUGPRINTF("oxhi=%08x ochi=%08x\n",oxhi,ochi) ;
	        nxhi = single(op->a,oxhi,ochi,&nchi) ;
	        {
	            xlo = ulong(nxlo) ;
	            xhi = ulong(nxhi) ;
	        }
	        {
	            clo = ulong(nclo) ;
	            chi = ulong(nchi) ;
	        }
	        {
	            op->x = (xhi << nbits<uint>) | xlo ;
	            op->c = (chi << nbits<uint>) | clo ;
	        }
	    } /* end if (supported) */
	} /* end if (non-null) */
	DEBUGPRINTF("ret x=%016llx o=%016llx\n",op->x,op->c) ;
	return rs ;
} /* end subroutine (randmwc_start) */

int randmwc_finish(randmwc *op) noex {
    	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (randmwc_finish) */

int randmwc_getulong(randmwc *op,ulong *ulp) noex {
    	int		rs = SR_FAULT ;
	if (op && ulp) ylikely {
	    ulong	xhi, xlo ;
	    ulong	chi, clo ;
	    uint	oxhi, oxlo ;
	    uint	ochi, oclo ;
	    uint	nxhi, nxlo ;
	    uint	nchi, nclo ;
	    rs = SR_OK ;
    	    {
	        oxhi = uint(op->x >> nbits<uint>) ;
	        ochi = uint(op->c >> nbits<uint>) ;
	    }
    	    {
	        oxlo = uint(op->x) ;
	        oclo = uint(op->c) ;
	    }
    	    {
	        nxhi = single(op->a,oxhi,ochi,&nchi) ;
	        nxlo = single(op->a,oxlo,oclo,&nclo) ;
	    }
	    {
	        xhi = ulong(nxhi) ;
	        xlo = ulong(nxlo) ;
	    }
	    {
	        chi = ulong(nchi) ;
	        clo = ulong(nclo) ;
	    }
	    DEBUGPRINTF("xhi=%016llx xlo=%016llx\n",xhi,xlo) ;
	    DEBUGPRINTF("chi=%016llx clo=%016llx\n",chi,clo) ;
    	    {
	        op->x = (xhi << nbits<uint>) | xlo ;
	        op->c = (chi << nbits<uint>) | clo ;
	    }
	    *ulp = op->x ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (randmwc_getulong) */


/* private subroutines */

local uint single(uint a,uint ox,uint oc,uint *ncp) noex {
	ulong		xox = ulong(ox) ;
	ulong		xoc = ulong(oc) ;
	ulong		ss ;
	ulong		aa = a ;
	uint		nx ; /* return =value */
	{
	    ss = ((aa * xox) + xoc) ;
	    *ncp = uint(ss >> nbits<uint>) ;
	    nx = (uint) ss ;
	}
	return nx ;
} /* end subroutine (single) */


