/* configvarsobj HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Configuration-Variables */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CONFIGVARSOBJ_INCLUDE
#define	CONFIGVARSOBJ_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<vecobj.h>

#include	"configvars.h"


enum configvarsws {
	configvarsw_sets,
	configvarsw_vars,
	configvarsw_unsets,
	configvarsw_unvars,
	configvarsw_overlast
} ;

enum keytypes {
	keytype_define,
	keytype_export,
	keytype_set,
	keytype_unset,
	keytype_var,
	keytype_overlast
} ;

#define	CONFIGVARS_WSETS	0
#define	CONFIGVARS_WVARS	1

#define	CV		configvars
#define	CV_VAR		configvars_var
#define	CV_FILE		configvars_file
#define	CV_CUR		configvars_cur
#define	CV_ERR		configvars_err


template<typename ... Args>
static inline int configvars_magic(configvars *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == CONFIGVARS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (configvars_magic) */

namespace configvars_obj {
    extern int configvars_parse(CV *,int,vecobj *) noex ;
    extern int configvars_finvars(CV *) noex ;
    extern int configvars_finfiles(CV *) noex ;
    extern int configvars_addvar(CV *,int,int,cc *,int,cc *,int) noex ;
}

namespace configvars_obj {
    extern int file_start(CV_FILE *,cchar *) noex ;
    extern int file_addvar(CV_FILE *,int,int,cc *,int,cc *,int) noex ;
    extern int file_finish(CV_FILE *) noex ;
    extern int file_finone(CV_FILE *,int) noex ;
}

namespace configvars_obj {
    extern int var_start(CV_VAR *,int,cc *,int,cc *,int) noex ;
    extern int var_finish(CV_VAR *) noex ;
}

namespace configvars_obj {
    extern void	badline(vecobj *,char *,int) noex ;
}

#endif /* __cplusplus */
#endif /* CONFIGVARSOBJ_INCLUDE */


