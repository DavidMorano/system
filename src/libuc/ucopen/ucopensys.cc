/* ucopensys SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* open some system related resource */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_opensys

	Description:
	This subroutine attempts to open a special system-wide
	resource of some kind.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>		/* |strnrchr(3c)| */
#include	<usystem.h>
#include	<opensysfs.h>
#include	<ipasswd.h>
#include	<strn.h>
#include	<sfx.h>
#include	<matstr.h>
#include	<strwcmp.h>
#include	<strx.h>
#include	<localmisc.h>

import libutil ;

/* local defines */


/* imported namespaces */

using libuc::opensysfs ;


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	opensys_banner(cchar *,int,int) noex ;
}


/* local structures */


/* forward references */

static int isRealName(cchar *,int) noex ;


/* local variables */

enum sysnames {
	sysname_userhomes,
	sysname_usernames,
	sysname_groupnames,
	sysname_projectnames,
	sysname_users,
	sysname_groups,
	sysname_projects,
	sysname_passwd,
	sysname_group,
	sysname_project,
	sysname_realname,
	sysname_shells,
	sysname_shadow,
	sysname_userattr,
	sysname_banner,
	sysname_bandate,
	sysname_overlast
} ; /* end enum (sysnames) */

static constexpr cpcchar	sysname[] = {
	"userhomes",
	"usernames",
	"groupnames",
	"projectnames",
	"users",
	"groups",
	"projects",
	"passwd",
	"group",
	"project",
	"realname",
	"shells",
	"shadow",
	"userattr",
	"banner",
	"bandate",
	nullptr
} ;

static const opensysdbs		whiches[] = {
	opensysdb_userhomes,
	opensysdb_usernames,
	opensysdb_groupnames,
	opensysdb_projectnames,
	opensysdb_usernames,
	opensysdb_groupnames,
	opensysdb_projectnames,
	opensysdb_passwd,
	opensysdb_group,
	opensysdb_project,
	opensysdb_realname,
	opensysdb_shells,
	opensysdb_shadow,
	opensysdb_userattr
} ; /* end array (whiches) */


/* exported variables */


/* exported subroutines */

int uc_opensys(cc *fname,int of,mode_t om,mainv envv,int to,int oo) noex {
	int		rs = SR_FAULT ;
	(void) envv ;
	(void) to ;
	(void) oo ;
	if (fname) {
	    int		fl = -1 ;
	    /* take off any leading slashes */
	    while (fname[0] == '/') fname += 1 ;
	    /* take off the '/sys' componenent */
	    if (cchar *tp ; (tp = strbrk(fname,"/­")) != nullptr) {
	        fl = intconv(tp - fname) ;
	    }
	    {
	        if (int len ; (len = isRealName(fname,fl)) > 0) {
		    fl = len ;
	        }
	    }
	    if (int fi ; (fi = matstr(sysname,fname,fl)) >= 0) {
	        switch (fi) {
	        case sysname_userhomes:
	        case sysname_usernames:
	        case sysname_groupnames:
	        case sysname_projectnames:
	        case sysname_users:
	        case sysname_groups:
	        case sysname_projects:
	        case sysname_passwd:
	        case sysname_group:
	        case sysname_project:
	        case sysname_realname:
	        case sysname_shells:
	        case sysname_shadow:
	        case sysname_userattr:
		    {
	                opensysdbs	w = whiches[fi] ;
	                rs = opensysfs(w,of,-1) ;
		    }
		    break ;
	        case sysname_banner:
	        case sysname_bandate:
		    rs = opensys_banner(fname,of,om) ;
		    break ;
	        default:
		    rs = SR_NOENT ;
		    break ;
	        } /* end switch */
	    } else {
	        rs = SR_NOENT ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_opensys) */


/* local subroutines */

static int isRealName(cchar *fname,int fl) noex {
	int		len = 0 ;
	cchar		*cp ;
	if (int cl ; (cl = sfbasename(fname,fl,&cp)) > 0) {
	    cchar	*suf = IPASSWD_SUF ;
	    if (cchar *tp ; (tp = strnrchr(cp,cl,'.')) != nullptr) {
		sysnames	w = sysname_realname ;
		cint		suflen = xstrlen(suf) ;
		if (strncmp((tp + 1),suf,suflen) == 0) {
		    cint	tl = intconv(tp - cp) ;
		    cchar	*rn = sysname[w] ;
		    if (strwcmp(rn,cp,tl) == 0) {
			len = intconv(tp - fname) ;
		    }
		}
	    }
	}
	return len ;
}
/* end subroutine (isRealName) */


