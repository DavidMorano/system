/* snfilemode */
/* lang=C20 */

/* make string version of the file-mode flags */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snfilemode

	Description:
	Ths subroutine creates in the result string a symbolic
	representation of file-mode settings.

	Synopsis:
	int snfilemode(char *dbuf,int dlen,mode_t fm) noex

	Arguments:
	dbuf		destination string buffer
	dlen		destination string buffer length
	fm		file-mode

	Returns:
	>=0		number of bytes in result
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<fcntl.h>
#include	<string.h>
#include	<usystem.h>
#include	<ctoct.h>
#include	<localmisc.h>

#include	"snflags.h"


/* local defines */

#ifndef	OCTBUFLEN
#define	OCTBUFLEN	47
#endif


/* external subroutines */


/* external variables */


/* local structures */

struct flagstrs {
	int		f ;
	cchar		*s ;
} ;


/* foward references */


/* local variables */

static const struct flagstrs	fileperms[] = {
	{ S_ISUID, "SUID" },
	{ S_ISGID, "SGID" },
	{ S_ISVTX, "SAVETXT" },
	{ 0, NULL }
} ;


/* exported subroutines */

int snfilemode(char *dbuf,int dlen,mode_t fm) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (dbuf) {
	    SNFLAGS	ss ;
	    if ((rs = snflags_start(&ss,dbuf,dlen)) >= 0) {
	        cint	ft = (fm & S_IFMT) ;
	        cchar	*ms = NULL ;
	        switch (ft) {
	        case S_IFIFO:
	            ms = "FIFO" ;
		    break ;
	        case S_IFCHR:
	            ms = "CHAR" ;
		    break ;
	        case S_IFDIR:
	            ms = "DIR" ;
		    break ;
	        case S_IFNAM:
	            ms = "NAME" ;
		    break ;
	        case S_IFBLK:
	            ms = "BLOCK" ;
		    break ;
	        case S_IFREG:
	            ms = "REG" ;
		    break ;
	        case S_IFLNK:
	            ms = "LINK" ;
		    break ;
	        case S_IFSOCK:
	            ms = "SOCK" ;
		    break ;
	        case S_IFDOOR:
	            ms = "DOOR" ;
		    break ;
	        default:
		    ms = "UNKNOWN" ;
		    break ;
	        } /* end switch */
	        if (ms) {
		    rs = snflags_addstr(&ss,ms) ;
	        }
	        for (int i = 0 ; (rs >= 0) && fileperms[i].f ; i += 1) {
	            if (fm & fileperms[i].f) {
	                rs = snflags_addstr(&ss,fileperms[i].s) ;
		    }
	        } /* end for */
	        if (rs >= 0) {
		    cint	n = 4 ; /* last characters */
		    cint	plen = OCTBUFLEN ;
		    char	pbuf[OCTBUFLEN+1] ;
		    if ((rs = ctocti(pbuf,plen,(fm&S_IAMB))) >= 0) {
		        cchar	*cp = ((rs > n) ? (pbuf+(rs-n)) : pbuf) ;
	                rs = snflags_addstr(&ss,cp) ;
		    }
	        } /* end if (ok) */
	        rs1 = snflags_finish(&ss) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (snflags) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (snfilemode) */


