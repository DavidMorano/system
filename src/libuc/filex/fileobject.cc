/* fileobject SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* determine if a named file is an object file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	fileobject

	Description:
	Given a file-name we determine if it is an object file (or
	core file).

	Synopsis:
	int fileobject(cchar *fname) noex

	Arguments:
	fname		file-path to check

	Returns:
	>0		is an object file
	==0		not an object file
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |CHAR_BIT| | |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |memcmp(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<uclibmem.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"fileobject.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* external subroutines */

extern "C" {
    extern int uc_open(cchar *,int,mode_t) noex ;
    extern int uc_close(int) noex ;
    extern int uc_fstat(int,ustat *) noex ;
    extern int uc_read(int,void *,int) noex ;
}


/* external variables */


/* local structures */

namespace {
    struct matcher {
	cchar	*mbuf ;
	int	mlen ;
	matcher(cchar *b,int l) noex : mbuf(b), mlen(l) { } ;
	operator int () noex ;
	int mat_elf() noex ;
	int mat_coff() noex ;
	int mat_fat() noex ;
    } ; /* end struct (matcher) */
    typedef int (matcher::*matcher_m)() noex ;
} /* end namespace */


/* forward references */


/* local variables */

constexpr matcher_m	mems[] = {
	&matcher::mat_fat,
	&matcher::mat_elf,
	&matcher::mat_coff
} ; /* end array */

constexpr ushort	mag_coffs[] = {
    	0x014C,		/* x86 */
	0x8664,		/* x86_64 */
	0x01C0,		/* ARM */
	0xAA64,		/* ARM_64 */
	0x01F0,		/* Power-PC */
	0
} ; /* end array */


/* exported variables */


/* exported subroutines */

int fileobject(cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		f = false ;
	if (fname) ylikely {
	    rs = SR_INVALID ;
	    if (fname[0]) ylikely {
		if (char *mbuf ; (rs = lm_sn(&mbuf)) >= 0) ylikely {
		    cint	mlen = rs ;
		    cint	of = O_RDONLY ;
		    cmode	om = 0 ;
		    if ((rs = uc_open(fname,of,om)) >= 0) ylikely {
	                cint	fd = rs ;
	                if (ustat sb ; (rs = uc_fstat(fd,&sb)) >= 0) ylikely {
	                    if (S_ISREG(sb.st_mode)) ylikely {
	                        if ((rs = uc_read(fd,mbuf,mlen)) > 0) {
				    matcher mo(mbuf,rs) ;
	                            rs = mo ;
				    f = rs ;
		                } /* end if (uc_read) */
			    } /* end if (regular file) */
	                } /* end if (stat) */
	                rs1 = uc_close(fd) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (open) */
		    rs1 = lm_free(mbuf) ;
	            if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (fileobject) */


/* local subroutines */

matcher::operator int () noex {
    	int		rs = SR_OK ;
	for (cauto &m : mems) {
	    rs = (this->*m)() ;
	    if (! rs) break ;
	} /* end for */
	return rs ;
} /* end method (matcher::operator) */
 
int matcher::mat_elf() noex {
	constexpr char	magstr[] = "\177ELF" ;
    	int		f = false ;
	{
	    constexpr int maglen = clenstr(magstr) ;
	    if (maglen <= mlen) ylikely {
	        f = (memcmp(mbuf,magstr,maglen) == 0) ;
	    }
	} /* end block */
    	return f ;
} /* end method (matcher::mat_elf) */

int matcher::mat_coff() noex {
    	int		f = false ;
	if (mlen >= szof(ushort)) {
	    cushort	ch0 = ushort(mbuf[0] & UCHAR_MAX) ;
	    cushort	ch1 = ushort(mbuf[1] & UCHAR_MAX) ;
	    {
	        cushort mag = ushort((ch1 << CHAR_BIT) | ch0) ;
	        for (int i = 0 ; mag_coffs[i] ; i += 1) {
		    f = (mag_coffs[i] == mag) ;
		    if (f) break ;
	        } /* end for */
	    } /* end block */
	} /* end if (possible) */
    	return f ;
} /* end method (matcher::mat_coff) */

int matcher::mat_fat() noex {
    	constexpr uint	mag = 0xFEEDFACF ; /* stored little-endian */
    	int		f = false ;
	if (cint n = szof(uint) ; mlen >= n) {
	    uint	val = 0 ;
	    for (int i = (n - 1) ; i >= 0 ; i += 1) {
		int	ch = mkchar(mbuf[i]) ;
		val <<= CHAR_BIT ;
		val |= uint(ch) ;
	    } /* end for */
	    f = (val == mag) ;
	} /* end if (possible) */
    	return f ;
} /* end method (matcher::mat_fat) */


