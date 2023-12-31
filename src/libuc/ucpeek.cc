/* ucpeek */
/* lang=C++20 */

/* "peek" at any waiting input */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-20, David A�D� Morano
	This is new code.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_peek

	Description:
	We peek ahead into the input buffer.

	Synosis:
	int uc_peek(int fd,char *dbuf,int dlen) noex

	Arguments:
	fd		file-descriptor
	dbuf		data buffer
	dlen		data buffer size provided

	Return:
	<0		error
	>=0		length of characters peeked at

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<ustropts.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"ucpeek.h"


/* local defines */

#ifndef	CBUFLEN
#define	CBUFLEN		LINEBUFLEN
#endif


/* external subroutines */


/* local structures */


/* forward references */

static int	peek_socket(int,void *,int) noex ;
static int	peek_stream(int,void *,int) noex ;
static int	peek_regular(int,void *,int) noex ;


/* exported subroutines */

int uc_peek(int fd,void *dbuf,int dlen) noex {
	int		rs = SR_FAULT ;
	if (dbuf) {
	    rs = SR_INVALID ;
	    char	*tbuf = (char *) dbuf ;
	    tbuf[0] = '\0' ;
	    if (dlen > 0) {
		rs = SR_BADFD ;
	        if (fd >= 0) {
	            USTAT	sb ;
	            if ((rs = u_fstat(fd,&sb)) >= 0) {
	                if (S_ISSOCK(sb.st_mode)) {
		            rs = peek_socket(fd,dbuf,dlen) ;
	                } else if (S_ISCHR(sb.st_mode)) {
		            rs = peek_stream(fd,dbuf,dlen) ;
	                } else {
		            rs = peek_regular(fd,dbuf,dlen) ;
	                }
	            } /* end if (stat) */
		} /* end if (bad-fd) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_peek) */


/* local subroutines */

static int peek_socket(int fd,void *dbuf,int dlen) noex {
	cint	mopts = MSG_PEEK ;
	return u_recv(fd,dbuf,dlen,mopts) ;
}
/* end subroutine (peek_socket) */

static int peek_stream(int fd,void *dbuf,int dlen) noex {
	cint		clen = CBUFLEN ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	char		*cbuf{} ;
	if ((rs = uc_libmalloc((clen+1),&cbuf)) >= 0) {
	    cbuf[0] = '\0' ;
	    {
	        STRPEEK		pd{} ;
	        pd.flags = 0 ;
	        pd.ctlbuf.buf = cbuf ;
	        pd.ctlbuf.maxlen = clen ;
	        pd.databuf.buf = (char *) dbuf ;
	        pd.databuf.maxlen = dlen ;
	        rs = u_ioctl(fd,I_PEEK,&pd) ;
	        len = pd.databuf.len ;
	    }
	    rs1 = uc_libfree(cbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs > 0) ? len : rs ;
}
/* end subroutine (peek_stream) */

static int peek_regular(int fd,void *dbuf,int dlen) noex {
	offset_t	fo ;
	int		rs ;
	if ((rs = u_tell(fd,&fo)) >= 0) {
	    rs = u_pread(fd,dbuf,dlen,fo) ;
	} /* end if (u_tell) */
	return rs ;
}
/* end subroutine (peek_regular) */


