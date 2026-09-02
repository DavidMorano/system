/* modsys4 MODULE (implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* get system configuration information */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:

	Description:

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/utsname.h>		/* POSIX |uname(2)| */
#include	<unistd.h>		/* POSIX |_SC_{x}| */
#include	<utmpx.h>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<algorithm>		/* C++STD |sort(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<utimeout.h>		/* LIBU |uto_{x}| */
#include	<sysconfcmds.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"modsys.hh"

module modsys ;


/* local defines */


/* imported namespaces */

using std::sort ;			/* subroutine-template */
using std::partition_point ;		/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

namespace {
    struct cacher {
	int		n[dataitem_overlast] = {} ;
	uchar		s[dataitem_overlast] = {} ;
	consteval void mktab() noex {
	    n[dataitem_pagesz]		= _SC_PAGESIZE ;
	    n[dataitem_maxpid]		= _SC_PID_MAX ;
	    n[dataitem_maxarg]		= _SC_ARG_MAX ;
	    n[dataitem_maxline]		= _SC_LINE_MAX ;
	    n[dataitem_maxlink]		= _SC_LINK_MAX ;
	    n[dataitem_maxlogin]	= _SC_LOGIN_NAME_MAX ;
	    n[dataitem_maxgroups]	= _SC_NGROUPS_MAX ;
	    n[dataitem_symlinks]	= _SC_SYMLOOP_MAX ;
	    n[dataitem_maxsymbol]	= _SC_SYMBOL_MAX ;
	    n[dataitem_maxnamelen]	= _SC_NAME_MAX ;
	    n[dataitem_maxpathlen]	= _SC_PATH_MAX ;
	    n[dataitem_maxnodename]	= _SC_NODENAME_MAX ;
	    n[dataitem_maxusername]	= _SC_USERNAME_MAX ;
	    n[dataitem_maxgroupname]	= _SC_GROUPNAME_MAX ;
	    n[dataitem_maxprojectname]	= _SC_PROJECTNAME_MAX ;
	    n[dataitem_maxprot]		= _SC_PROTNAME_MAX ;
	    n[dataitem_maxnetw]		= _SC_NETWNAME_MAX ;
	    n[dataitem_maxhost]		= _SC_HOSTNAME_MAX ;
	    n[dataitem_maxserv]		= _SC_SERVNAME_MAX ;
	    n[dataitem_maxentut]	= _SC_UTMPENT_SIZE_MAX ;
	    n[dataitem_maxentpw]	= _SC_GETPW_R_SIZE_MAX ;
	    n[dataitem_maxentsp]	= _SC_GETSP_R_SIZE_MAX ;
	    n[dataitem_maxentua]	= _SC_GETUA_R_SIZE_MAX ;
	    n[dataitem_maxentgr]	= _SC_GETGR_R_SIZE_MAX ;
	    n[dataitem_maxentpj]	= _SC_GETPJ_R_SIZE_MAX ;
	    n[dataitem_maxentpr]	= _SC_GETPR_R_SIZE_MAX ;
	    n[dataitem_maxentnw]	= _SC_GETNW_R_SIZE_MAX ;
	    n[dataitem_maxentho]	= _SC_GETHO_R_SIZE_MAX ;
	    n[dataitem_maxentsv]	= _SC_GETSV_R_SIZE_MAX ;
	    n[dataitem_maxmsg]		= _SC_MSG_MAX ;
	    n[dataitem_maxfstype]	= _SC_FSTYPE ;
	    n[dataitem_maxtzname]	= _SC_TZNAME_MAX ;
	    n[dataitem_maxtzabbr]	= _SC_TZABBR_MAX ;
	    n[dataitem_clk]		= _SC_CLK_TCK ;
        } ; /* end struct (mktab) */
	consteval void tabload_x() noex {
	    cint ne = dataitem_overlast ;
	    for (int i = 0 ; i < ne ; i += 1) {
		s[i] = uchar(i) ;
	    } /* end for */
	} ; /* end method (tabload_x) */
	consteval void tabinit() noex {
	    cint ne = dataitem_overlast ;
	    cauto cmpf = [this] (con uchar &ia,con uchar &ib) noex -> bool {
    		return (n[ia] < n[ib]) ;
	    } ; /* end lambda */
	    tabload_x() ;
	    sort(s,(s + ne),cmpf) ;
	} ; /* end method (tabinit) */
	consteval cacher() noex {
	    tabinit() ;
	} /* end ctor (cacher) */
    } ; /* end struct (cacher) */
} /* end namespace */

constexpr cacher	cdata ;
const getter		getci(cdata.n,cdata.s,dataitem_overlast) ;


/* exported variables */


/* exported subroutines */


/* local subroutines */


