/* pcsuserdomains_procdname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* process a directory */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* run-time debug print-outs */

/* revision history:

	= 1996-03-01, David A­D­ Morano
	The subroutine was adapted from others programs that
	did similar types of functions.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Description:
	This module traverses the given directory and creates
	the Directory Cache (DIRCACHE) file from the traversal.

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<ctime>
#include	<csignal>
#include	<cstddef>
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<baops.h>
#include	<field.h>
#include	<bfile.h>
#include	<fsdirtree.h>
#include	<localmisc.h>
#include	<libdebug.h>		/* LIBDEBUG */

#include	"config.h"
#include	"defs.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int procdname(PI *pip,bfile *ofp,cchar *newsdname) noex {
	bfile	dcfile ;
	int	rs, rs1, c ;
	int	f_local = FALSE ;
	char	dcfname[MAXPATHLEN + 1] ;
	char	tmpfname[MAXPATHLEN + 1] ;
	char	tempbuf[MAXPATHLEN + 1], *fname = tempbuf ;

	if (newsdname == NULL) return SR_FAULT ;
	if (newsdname[0] == '\0') return SR_INVALID ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("procdname: dname=%s\n",newsdname) ;
#endif

	mkpath2(dcfname,newsdname,DIRCACHEFNAME) ;

/* create the replacement file for the directory cache */

	mkpath2(tempbuf,newsdname,"dircacheXXXXXX") ;

	rs = mktmpfile(tmpfname,0664,tempbuf) ;

	if ((rs < 0) && (pip->uid != pip->euid)) {

	    rs1 = u_seteuid(pip->uid) ;

	    if (rs1 >= 0) {

	        f_local = TRUE ;
	        rs = mktmpfile(tmpfname,0664,tempbuf) ;

	        u_seteuid(pip->euid) ;

	    }
	}

	if (rs < 0)
	    goto ret0 ;

	if ((pip->gid_pcs >= 0) && (! f_local)) {
	    u_chown(tmpfname,-1,pip->gid_pcs) ;
	}

	c = 0 ;
	if ((rs = bopen(&dcfile,tmpfname,"wct",0664)) >= 0) {
	    bprintf(&dcfile,"DIRCACHE\n") ;
	    /* only gets directories, but follow links to find them */
	    fsdirtree	dir ;
	    cint	fdo = = (fsdirtreem.follow | fsdirtreem.dir) ;
	    if ((rs = fsdirtree_open(&dir,newsdname,fdo)) >= 0) {
	        ustat	sb ;
		cint	mpl = MAXPATHLEN ;
	        while ((rs1 = fsdirtree_read(&dir,&sb,fname,mpl)) > 0) {
		    if (pip->verboselevel >= 2) {
			bprintf(ofp,"%s\n",fname,rs1) ;
		    }
	            c += 1 ;
	            bwrite(&dcfile,fname,rs1) ;
	            bputc(&dcfile,'\n') ;
	        } /* end while */
	        fsdirtree_close(&dir) ;
	    } /* end if (fsdirtree) */
	    bclose(&dcfile) ;
	} /* end if (opened replacement file) */

	if (rs >= 0) {

	    rs = u_rename(tmpfname,dcfname) ;

	    if ((rs < 0) && f_local) {

	        rs1 = u_seteuid(pip->uid) ;

	        if (rs1 >= 0) {

	            rs = u_rename(tmpfname,dcfname) ;

	            u_seteuid(pip->euid) ;

	        }
	    }

	} /* end if (renaming attempt) */

	if ((rs < 0) && (tmpfname[0] != '\0')) {

	    rs1 = u_unlink(tmpfname) ;

	    if ((rs1 < 0) && f_local) {

	        rs1 = u_seteuid(pip->uid) ;

	        if (rs1 >= 0) {

	            u_unlink(tmpfname) ;

	            u_seteuid(pip->euid) ;

	        }
	    }

	} /* end if (unlink attempt) */

ret0:

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("procdname: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procdname) */



