/* getserial HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get a (system-wide) serial number from a file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getserial

	Description:
	This subroutine is used to get a unique serial number from
	a specified file.  These numbes are used for sequencing and
	other purposes in general code. An attempt is made to lock
	the SERIAL file and if the lock fails, the subroutine returns
	an error (negative number).  Locking may indeed fail due
	to the very poorly written file locking code on the old
	SunOS 4.xxx version of the UNIX system.  Remote file locking
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
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ptm.h>			/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<lockfile.h>		/* LIBUC */
#include	<estrings.h>		/* LIBUC */
#include	<cfnum.h>		/* LIBUC */
#include	<ctdec.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |DIGBUFLEN| */

#include	"getserial.h"


/* local defines */

#define	DEFSERIAL	"/tmp/serial"
#define	GS_FILEMODE	0666

#define	TO_LOCK		4		/* seconds */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

enum varmgrmems {
    varmgrmem_lockbeg,
    varmgrmem_lockend,
    varmgrmem_overlast
} ; /* end enum (varmgrmems) */

namespace {
    struct varmgr ;
    struct varmgr_co {
	varmgr		*op = nullptr ;
	int		w = -1 ;
	void operator () (varmgr *p,int m) noex {
	    op = p ;
	    w = m ;
	} ; /* end */
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ; /* end */
    } ; /* end struct (varmgr_co) */
    struct varmgr {
	friend		varmgr_co ;
	ptm		mx ;
	varmgr_co	lockbeg ;
	varmgr_co	lockend ;
	bool		finit ;
	operator int () noex ;
	varmgr() noex {
	    lockbeg	(this,varmgrmem_lockbeg) ;
	    lockend	(this,varmgrmem_lockend) ;
	    finit	= false ;
	} /* end ctor */
	destruct varmgr() ;
    private:
	int plockbeg	() noex ;
	int plockend	() noex ;
    } ; /* end struct (varmgr) */
} /* end namespace */


/* forward references */

local int	getserial_open	(cchar *) noex ;
local int	getserial_proc	(int) noex ;
local int	getserial_read	(int,char *,int) noex ;
local int	getserial_write	(int,char *,int,int) noex ;


/* local variables */

static varmgr		var ;
cmode			fm = GS_FILEMODE ;


/* exported variables */


/* exported subroutines */

int getserial(cchar *sfname) noex {
	int		rs ;
	int		rs1 ;
	int		serial = 0 ; /* return-value */
	if ((sfname == nullptr) || (sfname[0] == '\0')) {
	    sfname = DEFSERIAL ;
	} /* end */
	if (static cint rsv = var ; (rs = rsv) >= 0) ylikely {
	    if ((rs = getserial_open(sfname)) >= 0) ylikely {
	        cint	fd = rs ;
		if ((rs = var.lockbeg) >= 0) {
	            cint	cmd = F_LOCK ;
	            cint	to = TO_LOCK ;
	            if ((rs = lockfile(fd,cmd,0z,0z,to)) >= 0) ylikely {
			rs = getserial_proc(fd) ;
			serial = rs ;
		    } /* end if (lockfile) */
		    rs1 = var.lockend ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (var-lock) */
	        rs1 = uc_close(fd) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (opened successfully) */
	} /* end if (varmgr) */
	return (rs >= 0) ? serial : rs ;
} /* end subroutine (getserial) */


/* local subroutines */

local int getserial_openc(cchar *) noex ;

local int getserial_open(cchar *sfname) noex {
	int		rs ;
	int		fd = -1 ;
	rs = uc_open(sfname,O_RDWR,fm) ;
	fd = rs ;
	if (rs == SR_ACCESS) {
	    if ((rs = uc_unlink(sfname)) >= 0) {
	        rs = SR_NOENT ;
	    }
	} /* end */
	if (rs == SR_NOENT) {
	    rs = getserial_openc(sfname) ;
	    fd = rs ;
	} /* end if */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (getserial_open) */

local int getserial_openc(cchar *sfname) noex {
    	cnullptr	np{} ;
	cint		of = (O_RDWR|O_CREAT) ;
	int		rs ;
	int		rs1 ;
	int		fd = -1 ; /* return-value */
	cmode		om = 0666 ;
        if ((rs = uc_open(sfname,of,om)) >= 0) ylikely {
            fd = rs ;
            if ((rs = uc_fminmod(fd,om)) >= 0) ylikely {
                cint        cmd = _PC_CHOWN_RESTRICTED ;
                if ((rs = uc_fpathconf(fd,cmd,np)) == 0) {
                    cchar   *cp{} ;
                    if (int cl ; (cl = sfdirname(sfname,-1,&cp)) > 0) {
                        if (char *pbuf ; (rs = lm_mp(&pbuf)) >= 0) {
                            if ((rs = mkpath1w(pbuf,cp,cl)) >= 0) {
                                if (ustat sb ; (rs = uc_stat(pbuf,&sb)) >= 0) {
                                    const uid_t     u = sb.st_uid ;
                                    const gid_t     g = sb.st_gid ;
                                    rs = uc_fchown(fd,u,g) ;
                                } /* end if */
                            } /* end if (mkpath) */
                            rs1 = lm_free(pbuf) ;
                            if (rs >= 0) rs = rs1 ;
                        } /* end if (m-a-f) */
                    } /* end if (sfdirname) */
                } /* end if (u_pathconf) */
            } /* end if (uc_fminmod) */
        } /* end if (uc_open) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (getserial_copenc) */

local int getserial_proc(int fd) noex {
	cint		dlen = DIGBUFLEN ;
    	int		rs ;
	int		serial = 0 ; /* return-value */
	char		dbuf[DIGBUFLEN+1] ;
	if ((rs = getserial_read(fd,dbuf,dlen)) >= 0) ylikely {
	    serial = rs ; /* result for return */
	    rs = getserial_write(fd,dbuf,dlen,serial) ;
	} /* end */
	return (rs >= 0) ? serial : rs ;
} /* end subroutine (getserial_proc) */

local int getserial_read(int fd,char *dbuf,int dlen) noex {
	int		rs ;
	int		serial = 0 ; /* return-value */
	if ((rs = uc_read(fd,dbuf,dlen)) > 0) ylikely {
	    cint	dl = rs ;
	    cchar	*cp{} ;
	    if (int cl ; (cl = sfnext(dbuf,dl,&cp)) > 0) ylikely {
	        if ((rs = cfnum(cp,cl,&serial)) >= 0) {
	            rs = uc_rewind(fd) ;
	        } else if (isNotValid(rs)) {
	            serial = 0 ;
	            rs = SR_OK ;
	        } /* end */
	    } /* end if (sfnext) */
	} /* end if (u_read) */
	return (rs >= 0) ? serial : rs ;
} /* end subroutine (getserial_read) */

local int getserial_write(int fd,char *dbuf,int dlen,int serial) noex {
	cint		nserial = ((serial+1) & INT_MAX) ;
	int		rs ;
	if ((rs = ctdec(dbuf,dlen,nserial)) >= 0) ylikely {
	    dbuf[rs++] = '\n' ;
	    if ((rs = uc_write(fd,dbuf,rs)) >= 0) ylikely {
	        coff	uoff = off_t(rs) ;
	        rs = uc_ftruncate(fd,uoff) ;
	    } /* end */
	} /* end if (ctdec) */
	return rs ;
} /* end subroutine (getserial_write) */

varmgr_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case varmgrmem_lockbeg:
	        rs = op->plockbeg() ;
	        break ;
	    case varmgrmem_lockend:
	        rs = op->plockend() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (varmgr_co::operator) */

int varmgr::plockbeg() noex {
    	return mx.lockbeg ;
} /* end method (varmgr::plockbeg) */

int varmgr::plockend() noex {
    	return mx.lockend ;
} /* end method (varmgr::plockend) */

varmgr::operator int () noex {
    	int		rs = SR_OK ;
	if (! finit) {
	    if ((rs = mx.create) >= 0) {
	        finit = true ;
	    }
	} /* end if */
	return rs ;
} /* end subroutine (varmgr::operator) */

varmgr::destruct varmgr() {
	if (finit) {
	    mx.destroy() ;
	    finit = false ;
	} 
} /* end dtor (varmgr::destruct) */


