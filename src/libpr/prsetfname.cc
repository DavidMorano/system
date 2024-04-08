/* prsetfname SUPPORT */
/* lang=C++20 */

/* set program-root (oriented) file-name */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We create a file name according to some rules.

	Synopsis:

	int prsetfname(pr,fname,sp,sl,f_def,dname,name,suf)
	cchar	*pr ;
	char		fname[] ;
	cchar	sp[] ;
	cchar	dname[], name[], suf[] ;
	int		sl ;
	int		f_def ;

	Arguments:

	pr		program-root
	fname		result buffer
	sp		source pointer
	sl		source length
	f_def		use default file-name if none (empty) is given
	dname		directory-name
	name		base-name
	suf		suffix

	Returns:
	>=0		length of result
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<estrings.h>
#include	<mkpathx.h>
#include	<mkfnamesuf.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"prsetfname.h"


/* local defines */

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN	2048
#endif
#endif

#ifndef	VARTMPDNAME
#define	VARTMPDNAME	"TMPDIR"
#endif

#ifndef	TMPDNAME
#define	TMPDNAME	"/tmp"
#endif

#ifndef	TMPVARDNAME
#define	TMPVARDNAME	"/var/tmp"
#endif

#ifndef	VCNAME
#define	VCNAME		"var"
#endif


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* local structures */


/* exported variables */


/* exported subroutines */

int prsetfname(cc *pr,char *fname,cc *ebuf,int el,int f_def,cc *dname,
		cc *name,cc *suf) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	char		*tbuf{} ;
	if (pr && fname) {
	    if ((rs = malloc_mp(&tbuf)) >= 0) {
		cint	tlen = rs ;
	        cchar	*sp ;
	        if ((f_def && (ebuf[0] == '\0')) || (strcmp(ebuf,"+") == 0)) {
	            sp = name ;
	            if ((suf != nullptr) && (suf[0] != '\0')) {
	                sp = tbuf ;
	                mkfnamesuf1(tbuf,name,suf) ;
	            }
	            if (sp[0] != '/') {
	                if ((dname != nullptr) && (dname[0] != '\0')) {
	                    rs = mkpath3(fname,pr,dname,sp) ;
			    len = rs ;
	                } else {
	                    rs = mkpath2(fname,pr,sp) ;
			    len = rs ;
		        }
	            } else {
	                rs = mkpath1(fname,sp) ;
			len = rs ;
	            }
	        } else if (strcmp(ebuf,"-") == 0) {
	            fname[0] = '\0' ;
	        } else if (ebuf[0] != '\0') {
	            sp = ebuf ;
	            if (el >= 0) {
	                cint	ml = min(tlen,el) ;
	                sp = tbuf ;
	                strwcpy(tbuf,ebuf,ml) ;
	            }
	            if (ebuf[0] != '/') {
	                if (strchr(sp,'/') != nullptr) {
	                    rs = mkpath2(fname,pr,sp) ;
			    len = rs ;
	                } else {
	                    if ((dname != nullptr) && (dname[0] != '\0')) {
	                        rs = mkpath3(fname,pr,dname,sp) ;
			        len = rs ;
	                    } else {
	                        rs = mkpath2(fname,pr,sp) ;
			        len = rs ;
		            }
	                } /* end if */
	            } else {
	                rs = mkpath1(fname,sp) ;
			len = rs ;
	            }
	        } /* end if */
	        rs1 = uc_free(tbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (prsetfname) */


