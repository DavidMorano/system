/* prmkfname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* set program-root (oriented) file-name */
/* version %I% last-modified %G% */


/* revision history:

	= 1989-03-01, David A­D­ Morano
	This subroutine was originally written.  

	= 1998-06-01, David A­D­ Morano
	I enhanced the program a little to print out some other
	information.

	= 1999-03-01, David A­D­ Morano
	I enhanced the program a little to to do something (I forget
	what).

	= 2004-01-10, David A­D­ Morano
	The KSH program switched to using a fakey "large file"
	(64-bit fake-out mode) compilation mode on Solaris.  This
	required some checking to see if any references to |u_stat(3u)|
	had to be updated to work with the new KSH.  Although we
	call |u_stat(3u)| here, its structure is not passed to other
	subroutines expecting the regular 32-bit structure.

	= 2005-04-20, David A­D­ Morano
	I changed the program so that the configuration file is
	consulted even if the program is not run in daemon-mode.
	Previously, the configuration file was only consulted when
	run in daemon-mode.  The thinking was that running the
	program in regular (non-daemon) mode should be quick.  The
	problem is that the MS file had to be guessed without the
	aid of consulting the configuration file.  Although not a
	problem in most practice, it was not aesthetically appealing.
	It meant that if the administrator changed the MS file in
	the configuration file, it also had to be changed by
	specifying it explicitly at invocation in non-daemon-mode
	of the program.  This is the source of some confusion (which
	the world really doesn't need).  So now the configuration
	is always consulted.  The single one-time invocation is
	still fast enough for the non-smoker aged under 40! :-)

*/

/* Copyright © 1989,1998,1999,2004,2005 David A­D­ Morano.  */
/* All rights reserved. */

/*******************************************************************************

	Name:
	prmkfname

	Description:
	We create a file name according to some rules.

	Synopsis:
	int prmkfname(cc *pr,cc *fn,cc *sp,int sl,int f_def,
		cc *dname,cc *name,cc *suf) noex

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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<estrings.h>
#include	<mkpathx.h>
#include	<mkfnamesuf.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"prmkfname.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |lenstr(3u)| */
import uconstants ;			/* |varname(3u)| */

/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* local structures */


/* exported variables */


/* exported subroutines */

int prmkfname(cc *pr,char *fname,cc *ebuf,int el,int f_def,cc *dname,
		cc *name,cc *suf) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (pr && fname) {
	    if (char *tbuf ; (rs = lm_mp(&tbuf)) >= 0) {
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
	        rs1 = lm_free(tbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (prmkfname) */


