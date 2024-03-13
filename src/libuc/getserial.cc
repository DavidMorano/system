/* getserial HEADER */
/* lang=C++20 */

/* get the serial number for logging references */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-1, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getserial

	Description:
	This subroutine is used to get a unique serial number from
	a specified file. These numbes are used for sequencing and
	other purposes in general code. An attempt is made to lock
	the SERIAL file and if the lock fails, the subroutine returns
	an error (negative number).  Locking may indeed fail due
	to the very poorly written file locking code on the old
	SunOS 4.xxx version of the UNIX system. Remote file locking
	over NFS on the old SunOS 4.xxx systems **never** worked
	correctly!  Other errors, like "couldn't create the file"
	are reported as such.

	Synopsis:
	int getserial(cchar *sfname) noex

	Arguments:
	sfname		sfname of file containing the serial number

	Returns:
	>0		the serial number
	==0		file was just created
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<lockfile.h>
#include	<estrings.h>
#include	<cfnum.h>
#include	<ctdec.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"getserial.h"


/* local defines */

#ifndef	DIGBUFLEN
#define	DIGBUFLEN	40		/* can hold int128_t in decimal */
#endif

#define	DEFSERIAL	"/tmp/serial"
#define	FILEMODE	0666

#define	TO_LOCK		4		/* seconds */

#if	defined(IRIX)
#ifndef	F_ULOCK
#define F_ULOCK	0	/* Unlock a previously locked region */
#endif
#ifndef	F_LOCK
#define F_LOCK	1	/* Lock a region for exclusive use */
#endif
#ifndef	F_TLOCK
#define F_TLOCK	2	/* Test and lock a region for exclusive use */
#endif
#ifndef	F_TEST
#define F_TEST	3	/* Test a region for other processes locks */
#endif
#endif /* defined(IRIX) */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	getserial_open(cchar *) noex ;
static int	getserial_read(int,char *,int) noex ;
static int	getserial_write(int,char *,int,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int getserial(cchar *sfname) noex {
	int		rs ;
	int		rs1 ;
	int		serial = 0 ;
	if ((sfname == NULL) || (sfname[0] == '\0')) sfname = DEFSERIAL ;
	if ((rs = getserial_open(sfname)) >= 0) {
	    cint	fd = rs ;
	    if ((rs = lockfile(fd,F_LOCK,0L,0L,TO_LOCK)) >= 0) {
		cint	dlen = DIGBUFLEN ;
		char	dbuf[DIGBUFLEN+1] ;
	        if ((rs = getserial_read(fd,dbuf,dlen)) >= 0) {
	            serial = rs ; /* result for return */
	            rs = getserial_write(fd,dbuf,dlen,serial) ;
	        }
	    } /* end if (lockfile) */
	    rs1 = u_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (opened successfully) */

	return (rs >= 0) ? serial : rs ;
}
/* end subroutine (getserial) */


/* local subroutines */

static int getserial_open(cchar *sfname) noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	cmode		m = FILEMODE ;
	rs = uc_open(sfname,O_RDWR,m) ;
	fd = rs ;
	if (rs == SR_ACCESS) {
	    if ((rs = uc_unlink(sfname)) >= 0) {
	        rs = SR_NOENT ;
	    }
	}
	if (rs == SR_NOENT) {
	    cint	of = (O_RDWR|O_CREAT) ;
	    if ((rs = uc_open(sfname,of,m)) >= 0) {
	        fd = rs ;
	        if ((rs = uc_fminmod(fd,m)) >= 0) {
	            cint	cmd = _PC_CHOWN_RESTRICTED ;
	            if ((rs = u_fpathconf(fd,cmd,NULL)) == 0) {
	                USTAT	sb ;
	                cchar	*cp{} ;
	                if (int cl ; (cl = sfdirname(sfname,-1,&cp)) > 0) {
	                    char	*pbuf{} ;
			    if ((rs = malloc_mp(&pbuf)) >= 0) {
	                        if ((rs = mkpath1w(pbuf,cp,cl)) >= 0) {
	                            if ((rs = uc_stat(pbuf,&sb)) >= 0) {
	                                rs = u_fchown(fd,sb.st_uid,sb.st_gid) ;
	                            }
	                        } /* end if (mkpath) */
				rs1 = uc_free(pbuf) ;
				if (rs >= 0) rs = rs1 ;
			    } /* end if (m-a-f) */
	                } /* end if (sfdirname) */
	            } /* end if (u_pathconf) */
	        } /* end if (uc_fminmod) */
	    } /* end if (uc_open) */
	}
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (getserial_open) */

static int getserial_read(int fd,char *dbuf,int dlen) noex {
	int		rs ;
	int		serial = 0 ;
	if ((rs = u_read(fd,dbuf,dlen)) > 0) {
	    cint	dl = rs ;
	    cchar	*cp{} ;
	    if (int cl ; (cl = sfnext(dbuf,dl,&cp)) > 0) {
	        if ((rs = cfnum(cp,cl,&serial)) >= 0) {
	            rs = u_rewind(fd) ;
	        } else if (isNotValid(rs)) {
	            serial = 0 ;
	            rs = SR_OK ;
	        }
	    } /* end if (sfnext) */
	} /* end if (u_read) */
	return (rs >= 0) ? serial : rs ;
}
/* end subroutine (getserial_read) */

static int getserial_write(int fd,char *dbuf,int dlen,int serial) noex {
	cint		nserial = ((serial+1) & INT_MAX) ;
	int		rs ;
	    if ((rs = ctdec(dbuf,dlen,nserial)) >= 0) {
	        dbuf[rs++] = '\n' ;
	        if ((rs = u_write(fd,dbuf,rs)) >= 0) {
	            const off_t	uoff = off_t(rs) ;
	            rs = uc_ftruncate(fd,uoff) ;
	        }
	    } /* end if (ctdeci) */
	return rs ;
}
/* end subroutine (getserial_write) */


