/* nettime_out SUPPORT (Program-Output) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* handle program output */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* switchable debug print-outs */

/* revision history:

	= 1999-08-17, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	We manage optional program output.

	Synopsis:
	int progout_begin(PROGINFO *pip)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<csysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<estrings.h>		/* LIBUC */
#include	<bits.h>		/* LIBUC */
#include	<netfile.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<exitcodes.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */
#include	<bfile.h>		/* LIBB */
#include	<proginfo.hh>		/* LIBPROG */

#include	"nettime_config.h"


/* local defines */

#ifdef	PI
#define	PI	proginfo
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	progout_check(PI *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int progout_begin(PI *pip,cchar *ofn) noex {
	int		rs = SR_OK ;
	if (ofn) {
	    cchar **vpp = &pip->ofname ;
	    rs = proginfo_setentry(pip,vpp,ofn,-1) ;
	} /* end if */
	return rs ;
} /* end subroutine (progout_begin) */

int progout_end(PI *pip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (pip->open.outfile) {
	    pip->open.outfile = false ;
	    if (pip->outfile) {
		{
	            rs1 = bclose(pip->outfile) ;
	            if (rs >= 0) rs = rs1 ;
		} /* end */
		{
		    rs1 = mem.free(pip->outfile) ;
	            if (rs >= 0) rs = rs1 ;
		} /* end if (memory-release) */
		pip->outfile = nullptr ;
	    } /* end if */
	} /* end if */
	return rs ;
} /* end subroutine (progout_end) */

int progout_println(PI *pip,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	if (pip->verboselevel > 0) {
	    if ((rs = progout_check(pip)) >= 0) {
	        rs = bprintln(pip->outfile,sp,sl) ;
	        wlen = rs ;
	    } /* end if */
	} /* end if (verboselevel) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (progout_println) */

int progout_printf(PI *pip,cchar *fmt,...) noex {
	va_list		ap ;
	int		rs = SR_OK ;
	int		wlen = 0 ;
	if (pip->verboselevel > 0) {
	    if ((rs = progout_check(pip)) >= 0) {
	        va_begin(ap,fmt) ;
	        rs = bvprintf(pip->outfile,fmt,ap) ;
	        wlen = rs ;
	        va_end(ap) ;
	    } /* end */
	} /* end if (verboselevel) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (progout_printf) */


/* local subroutines */

local int progout_check(PI *pip) noex {
	int		rs = SR_OK ;
	if (pip->outfile == nullptr) {
	    int		sz = szof(bfile) ;
	    cchar	*ofn = pip->ofname ;
	    if ((ofn == nullptr) || (ofn[0] == '\0') || (ofn[0] == '-')) {
		ofn = BFILE_STDOUT ;
	    } /* end */
	    if (voidp p ; (rs = mem.mall(sz,&p)) >= 0) {
		pip->outfile = p ;
	        if ((rs = bopen(pip->outfile,ofn,"wct",0666)) >= 0) {
		    pip->open.outfile = true ;
		} /* end */
		if (rs < 0) {
		    mem.free(pip->outfile) ;
		    pip->outfile = nullptr ;
		} /* end if (error) */
	    } /* end if (m-a) */
	} /* end if (initialization) */
	return rs ;
} /* end subroutine (progout_check) */


