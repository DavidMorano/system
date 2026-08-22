/* dialticotsordmux SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* dial to the server listening on USSMUX */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	dialticotsordmux

	Description:
	This object dials out to a UNIX®-Socket-Stream (USS) that
	implements a multiplexor on the server side.

	Synopsis:
	int dialticotsordmux(cc *abuf,int alen,cc *svc,mv sav,
			int to,int opts) noex

	Origin:
	-Dial
	-Transport
	-Interface
	-Connection
	-Oriented
	-Transport
	-Service
	-Orderly
	-Release

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<csignal>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uxti.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucsigset.h>		/* LIBUC */
#include	<sigign.h>		/* LIBUC */
#include	<buffer.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<mkx.h>			/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"dialticotsord.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	DBUFLEN		(8 * 1024)

#ifndef	PORTSPEC_USSMUX
#define	PORTSPEC_USSMUX	"/tmp/ussmux"
#endif


/* imported namespaces */


/* local typedefs */

typedef const mainv	cmv ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	dialer(buffer *,cchar *,int,char *,int,int,int) noex ;


/* local variables */

constexpr int	igns[] = {
	SIGPIPE,
	SIGHUP,
	0
} ; /* end array (igns) */


/* exported variables */


/* exported subroutines */

int dialticotsordmux(cc *abuf,int alen,cc *svc,cmv sav,int to,int opts) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ;
	if (abuf && svc) ylikely {
	    rs = SR_INVALID ;
	    if (svc[0]) ylikely {
	        cchar	*sp{} ;
		if (int sl ; (sl = sfshrink(svc,-1,&sp)) > 0) ylikely {
		    cint bs = 100 ;
	            if (buffer srv ; (rs = srv.start(bs)) >= 0) ylikely {
	                cint	dlen = DBUFLEN ;
	                char *dbuf ; 
		        if ((rs = lm_mall((dlen+1),&dbuf)) >= 0) ylikely {
	                    srv.strw(svc) ;
	                    if (sav) {
		                cauto	mq = mkquoted ;
	                        for (int i = 0 ; sav[i] ; i += 1) {
				    cchar	*sap = sav[i] ;
	                            if ((rs = mq(dbuf,dlen,sap,-1)) >= 0) {
	                                srv.chr(' ') ;
	                                srv.buf(dbuf,rs) ;
	                            } /* end if (mkquoted) */
				    if (rs < 0) break ;
	                        } /* end for */
	                    } /* end if */
	                    if (rs >= 0) {
	                        srv.chr('\n') ;
	                    } /* end if (ok) */
	                    if (rs >= 0) {
	                        rs = dialer(&srv,abuf,alen,dbuf,dlen,to,opts) ;
	                        fd = rs ;
	                    } /* end if (ok) */
	                    rs1 = lm_free(dbuf) ;
		            if (rs >= 0) rs = rs1 ;
	                } /* end if (m-a-f) */
	                rs1 = srv.finish ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (buffer) */
	            if ((rs >= 0) && (fd >= 0)) nlikely {
			u_close(fd) ;
		    } /* end if (error) */
		} /* end if (sfshrink) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (dialticotsordmux) */


/* local subroutines */

local int dialer(buffer *sbp,cchar *abuf,int alen,char *dbuf,int dlen,
		int to,int opts) noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if (cchar *bp ; (rs = sbp->get(&bp)) >= 0) ylikely {
	    cint	blen = rs ;
	    if (sigign sig ; (rs = sig.start(igns)) >= 0) {
	        if ((rs = dialticotsord(abuf,alen,to,opts)) >= 0) ylikely {
	            fd = rs ;
	            if ((rs = uc_writen(fd,bp,blen)) >= 0) ylikely {
	                dbuf[0] = '\0' ;
	                if ((rs = uc_readlnto(fd,dbuf,dlen,to)) >= 0) {
	                    if ((rs == 0) || (dbuf[0] != '+')) {
	                        rs = SR_BADREQUEST ;
	                    }
	                } /* end if (uc_readlnto) */
	            } /* end if (wrote service code) */
	        } /* end if (opened) */
	        rs1 = sig.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sigign) */
	    if ((rs < 0) && (fd >= 0)) nlikely {
		u_close(fd) ;
	    } /* end if (error) */
	} /* end if (buffer_get) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutiner (dialer) */


