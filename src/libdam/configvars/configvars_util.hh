/* configvars_util HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Configuration-Variables */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CONFIGVARS_INCLUDE
#define	CONFIGVARS_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecobj.h>		/* LIBUC */

#include	"configvars.h"

#define	CV		configvars
#define	CV_VAR		configvars_var
#define	CV_FILE		configvars_file
#define	CV_CUR		configvars_cur
#define	CV_ERR		configvars_err


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
#endif /* CONFIGVARS_INCLUDE */


