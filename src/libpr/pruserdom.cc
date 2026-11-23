/* pruserdom SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get user domain */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	I made up this idea for supporting multiple domains on the
	same machine so that each user could have a different domain
	name. This idea of multiplexing a single machine to appear
	to be multiple different machines is becoming very important
	in the Internet age!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pruserdom

	Description:
	This subroutine looks up the given username and returns the
	domainname for that user.  Per-user domain names are optional
	and are administered through the file 'etc/userdomain' located
	relative to the programroot directory that is optionally
	supplied.  If no programroot is supplied, then '/' is used.
	This subroutine uses two different ways to read the UDOMAIN
	DB based on the file it is in.  This is so that in addition
	to the file being "regular" it can also be (secretly) a
	network file or portal of some kind.

	Synopsis:
	int pruserdom(cchar *pr,char *dbuf,int dlen,cchar *un) noex

	Arguments:
	pr		program-root
	dbuf		buffer to receive results (must be MAXPATHLEN in size)
	dlen		length of supplied buffer
	un		specified username to lookup

	Returns:
	>=0		length of returned domain-name
	<0		error (system-return)

	Notes:
	Format of file entries:
		username	domainname
	Note that this strategy of reading the "userdomain" file
	is obsoleted by the use of the 'id' (Internet Domain)
	key-value pair in the 'user_attr' DB of the system!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<estrings.h>		/* LIBUC */
#include	<snwcpy.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<filemap.h>		/* LIBUC */
#include	<filer.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"pruserdom.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	UDOMASTDFNIN
#define	UDOMASTDFNIN	"/etc/userdomain"
#endif

#define	MAXFILESIZE	(2 * 1024 * 1024)


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct suber {
	cchar		*un = nullptr ;
	char		*dbuf ;
	int		dlen ;
	int		ul = 0 ;
	int		maxfilesz ;
	suber(char *dp,int dl,cchar *u,int mf) noex : dbuf(dp) , dlen(dl) {
	    un = u ;
	    if (un) {
	        ul = lenstr(un) ;
 	    }
	    maxfilesz = mf ;
	} ; /* end ctor */
	int udomer	(cchar *) noex ;
	int udomerm	(cchar *) noex ;
	int udomerf	(cchar *) noex ;
	int parseln	(cchar *,int) noex ;
    } ; /* end struct (suber) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int userdomain(cchar *pr,char *dbuf,int dlen,cchar *un) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if (dbuf && un) ylikely {
	    rs = SR_INVALID ;
	    dbuf[0] = '\0' ;
	    if (un[0]) ylikely {
	        suber	a(dbuf,dlen,un,MAXFILESIZE) ;
	        cchar	*fname = UDOMASTDFNIN ;
	        if (pr && (pr[0] != '\0') && (cmpstr(pr,"/") != 0)) {
		     if (char *fbuf ; (rs = lm_mp(&fbuf)) >= 0) {
	    	         if ((rs = mkpath(fbuf,pr,fname)) >= 0) {
	             	    rs = a.udomer(fbuf) ;
		     	    len = rs ;
		         }
		         rs1 = lm_free(fbuf) ;
		         if (rs >= 0) rs = rs1 ;
		     } /* end if (m-a-f) */
	         } else {
	             rs = a.udomer(fname) ;
		     len = rs ;
	         } /* end */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (userdomain) */


/* local subroutines */

int suber::udomer(cchar *fname) noex {
	int		rs ;
	int		len = 0 ;
	if (ustat sb ; (rs = u_stat(fname,&sb)) >= 0) {
	    if (S_ISREG(sb.st_mode) && (sb.st_size < maxfilesz)) {
		rs = udomerm(fname) ;
		len = rs ;
	    } else {
		rs = udomerf(fname) ;
		len = rs ;
	    } /* end if (type of file) */
	} /* end if (stat) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (suber::udomer) */

int suber::udomerm(cchar *fname) noex {
	csize		mfsize = size_t(maxfilesz) ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (filemap udf ; (rs = udf.open(fname,mfsize)) >= 0) {
	    cchar	*lp ;
            while ((rs = udf.getln(&lp)) > 0) {
                cint	ll = rs ;
                {
                    rs = parseln(lp,ll) ;
                    len = rs ;
                }
                if (len > 0) break ;
                if (rs < 0) break ;
            } /* end while (reading lines) */
            rs1 = udf.close ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (opened file) */
	return (rs >= 0) ? len : rs ;
} /* end method (suber::udomerm) */

int suber::udomerf(cchar *fname) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (char *lbuf ; (rs = lm_ml(&lbuf)) >= 0) ylikely {
	    cint	llen = rs ;
	    if ((rs = uc_open(fname,O_RDONLY,0666)) >= 0) ylikely {
	        cint	fd = rs ;
	        if (filer b ; (rs = b.start(fd,0L,0,0)) >= 0) ylikely {
	            while ((rs = b.readln(lbuf,llen,-1)) > 0) {
			{
	                    rs = parseln(lbuf,rs) ;
	                    len = rs ;
			}
	                if (len > 0) break ;
	                if (rs < 0) break ;
	            } /* end while (reading lines) */
	            rs1 = b.finish ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (filer) */
	        rs1 = uc_close(fd) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (opened file) */
	    rs1 = lm_free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
} /* end method (suber::udomerf) */

int suber::parseln(cchar *lbuf,int llen) noex {
	int		rs = SR_OK ;
	int		len = 0 ; /* return-value */
	cchar		*sp ;
	if (int sl ; (sl = sfcontent(lbuf,llen,&sp)) > 0) {
	    cchar *cp ;
	    if (int cl ; (cl = sfnext(sp,sl,&cp)) > 0) {
	        if ((cl == ul) && (strncmp(un,cp,cl) == 0)) {
	            sl -= intconv((cp + cl) - sp) ;
	            sp = (cp + cl) ;
	            if ((cl = sfnext(sp,sl,&cp)) > 0) {
	                rs = snwcpy(dbuf,dlen,cp,cl) ;
		        len = rs ;
	            }
	        } /* end if (username match) */
	    } /* end if (sfnext) */
	} /* end if (sfcontent) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (suber::parseln) */


