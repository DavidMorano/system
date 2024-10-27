/* filelines SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* calculate the number of lines in a file */
/* get the size of an open file */


/* revision history:

	= 2023-12-17, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2023 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	filelines

	Description:
	This is the (now) famous 'filelines' subroutine.  It is the
	fastest version of this kind of cuntion (counting lines in
	a text file).  Note that unlike the |fileliner(3dam)|
	subroutine, no comment character can be specified.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- for |memchr(3c)| */
#include	<usystem.h>
#include	<intsat.h>
#include	<localmisc.h>

#include	"filelines.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	liner(int,size_t) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int filelines(cchar *fn) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		lines = 0 ;
	if (fn) {
	    rs = SR_INVALID ;
	    if (fn[0]) {
		cint	of = O_RDONLY ;
		if ((rs = uc_open(fn,of,0)) >= 0) {
		    cint	fd = rs ;
		    if (USTAT sb ; (rs = uc_fstat(fd,&sb)) >= 0) {
			csize	fsz = size_t(sb.st_size) ;
			rs = SR_NOTSUP ;
		        if (S_ISREG(sb.st_mode)) {
			    rs = SR_OK ;
			    if (fsz > 0) {
			        rs = liner(fd,fsz) ;
			        lines = rs ;
			    }
			} /* end if (regular file) */
		    } /* end if (stat) */
		    rs1 = uc_close(fd) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (open) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? lines : rs ;
}
/* end subroutine (filelines) */


/* local subroutines */

static int liner(int fd,size_t ms) noex {
	cint		mp = PROT_READ ;
	cint		mf = MAP_SHARED ;
	int		rs ;
	int		rs1 ;
	int		lines = 0 ;
	cnullptr	np{} ;
	void		*md{} ;
	if ((rs = u_mmapbegin(np,ms,mp,mf,fd,0z,&md)) >= 0) {
	    cint	cmd = MADV_SEQUENTIAL ;
	    if ((rs = u_madvise(md,ms,cmd)) >= 0) {
		size_t	ll = ms ;
		cchar	*lp = charp(md) ;
		cchar	*tp ;
		while ((tp = charp(memchr(lp,'\n',ll))) != np) {
		    csize	si = ((tp + 1) - lp) ;
		    lines += 1 ;
		    ll -= si ;
		    lp += si ;
		} /* end while */
		if (ll > 0) lines += 1 ;
	    } /* end if (memory-advise) */
	    rs1 = u_mmapend(md,ms) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (map-file) */
	return (rs >= 0) ? lines : rs ;
}
/* end subroutine (liner) */


