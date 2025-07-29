/* bobj SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* "Basic I-O" package */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

	= 1999-01-10, David A­D­ Morano
	I added the little extra code to allow for memory mapped
	I-O.  It is all a waste because it manual memory mapping
	is less performatice than the regular pre-reading done by
	the operating system itself.  This should teach me to leave
	old programs alone!

*/

/* Copyright © 1998,1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	bfile

	Description:
	This code piece provides for the basic "open" and "close"
	functions for the BFILE I-O library.

	The following global functions are made available from this
	code piece:

	- bopen
	- bopene
	- bopenmod
	- bclose

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<sysval.hh>
#include	<stdfnames.h>
#include	<getfdfile.h>
#include	<snx.h>
#include	<cfdec.h>
#include	<conallof.h>
#include	<intsat.h>
#include	<intceil.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int bfile::opene(cchar *fn,cchar *aos,mode_t aom,int to) noex {
	return bopene(this,fn,aos,aom,to) ;
}

int bfile::open(cchar *fn,cchar *aos,mode_t aom) noex {
	return bopene(this,fn,aos,aom,-1) ;
}
/* end subroutine (bopen) */

int bfile::openmod(cchar *fname,cchar *aof,mode_t aom) noex {
	return bopenmod(this,fname,aof,aom) ;
}

int bfile::controlv(int cmd,va_list ap) noex {
    	return bcontrolv(this,cmd,ap) ;
}

int bfile::control(int cmd,...) noex {
	va_list		ap ;
	int		rs ;
	{
	    va_begin(ap,cmd) ;
	    rs = bcontrolv(this,cmd,ap) ;
	    va_end(ap) ;
	}
    	return rs ;
}

int bfile::stat(USTAT *sbp) noex {
	return bstat(this,sbp) ;
}

int bfile::copyfile(bfile *ofp,char *ubuf,int ulen) noex {
	return bcopyfile(this,ofp,ubuf,ulen) ;
}

int bfile::dup(bfile *fnewp) noex {
	return bdup(this,fnewp) ;
}

int bfile::flushn(int n) noex {
	return bflushn(this,n) ;
}

int bfile::minmod(mode_t aom) noex {
	return bminmod(this,aom) ;
}

int bfile::printcleanln(cchar *lp,int ll) noex {
	return bprintcleanln(this,lp,ll) ;
}

int bfile::printcleanlns(int linelen,cchar *lp,int ll) noex {
	return bprintcleanlns(this,linelen,lp,ll) ;
}

int bfile::printf(cchar *fmt,...) noex {
    	va_list		ap ;
    	int		rs ;
	{
	    va_begin(ap,fmt) ;
	    rs = bvprintf(this,fmt,ap) ;
	    va_end(ap) ;
	}
	return rs ;
}

int bfile::vprintf(cchar *fmt,va_list ap) noex {
	return bvprintf(this,fmt,ap) ;
}

int bfile::printlns(int flen,cchar *lbuf,int llen) noex {
	return bprintlns(this,flen,lbuf,llen) ;
}

int bfile::putc(int ch) noex {
	return bputc(this,ch) ;
}

int bfile::reade(void *ubuf,int ulen,int to,int opts) noex {
	return breade(this,ubuf,ulen,to,opts) ;
}

int bfile::read(void *ubuf,int ulen) noex {
	return bread(this,ubuf,ulen) ;
}

int bfile::readlnto(char *ubuf,int ulen,int to) noex {
	return breadlnto(this,ubuf,ulen,to) ;
}

int bfile::readlns(char *lbuf,int llen,int to,int *lcp) noex {
	return breadlns(this,lbuf,llen,to,lcp) ;
}

int bfile::reserve(int n) noex {
	return breserve(this,n) ;
}

int bfile::seek(off_t wo,int w) noex {
	return bseek(this,wo,w) ;
}

int bfile::tell(off_t *rp) noex {
	return btell(this,rp) ;
}

int bfile::truncate(off_t off) noex {
	return btruncate(this,off) ;
}

int bfile::wasteln(char *lbuf,int llen) noex {
	return bwasteln(this,lbuf,llen) ;
}

int bfile::write(cvoid *abuf,int alen) noex {
	return bwrite(this,abuf,alen) ;
}

int bfile::writeblanks(int n) noex {
	return bwriteblanks(this,n) ;
}

int bfile::writechrs(int ch,int n) noex {
	return bwritechrs(this,ch,n) ;
}

int bfile::writeblock(bfile *ifp,int ulen) noex {
	return bwriteblock(this,ifp,ulen) ;
}

int bfile::writefile(cchar *fname) noex {
	return bwritefile(this,fname) ;
}

void bfile::dtor() noex {
	if (cint rs = close ; rs < 0) {
	    ulogerror("bfile",rs,"fini-finish") ;
	}
}

int bfile_pr::operator () (cchar *sp,int sl) noex {
    	return bprintln(op,sp,sl) ;
}

bfile_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case bfilemem_size:
	        rs = bsize(op) ;
	        break ;
	    case bfilemem_getc:
	        rs = bgetc(op) ;
	        break ;
	    case bfilemem_isterm:
	        rs = bisterm(op) ;
	        break ;
	    case bfilemem_flush:
	        rs = bflush(op) ;
	        break ;
	    case bfilemem_rewind:
	        rs = brewind(op) ;
	        break ;
	    case bfilemem_close:
	        rs = bclose(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (bfile_co::operator) */


