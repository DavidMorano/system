/* cm_try SUPPORT (Communications-Connection Manager) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Connection Manager */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cm

	Description:
	This is a communications connection manager object.  This
	object abstracts the details of a particular connection
	from the calling program.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<getusername.h>		/* LIBUC */
#include	<expcook.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<fieldterms.h>		/* LIBUC */
#include	<field.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<buffer.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |TIMEBUFLEN| */

#include	"cm_subinfo.h"
#include	"cm.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	TI_FILECHECK	3

#define	EC		expcook
#define	SI		cm_subinfo
#define	SY		systems
#define	SY_ENT		systems_ent
#define	ST_CUR		systems_cur
#define	SD		sysdialer
#define	SD_ARGS		sysdialer_args
#define	SD_ENT		sysdialer_ent
#define	VS		vecstr


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */

typedef mainv		mv ;

extern "C" {
    typedef int (*open_f)	(void *) noex ;
    typedef int (*close_f)	(void *) noex ;
    typedef int (*shutdown_f)	(void *,int) noex ;
    typedef int (*reade_f)	(void *,void *,int,int,int) noex ;
    typedef int (*recve_f)	(void *,void *,int,int,int,int) noex ;
    typedef int (*write_f)	(void *,cvoid *,int) noex ;
} /* end extern (C) */


/* external subroutines */


/* external variables */

namespace cm_ns {
    extern char		terms[fieldterms_termsize] ;
} /* end namespace (cm_ns) */


/* local structures */

namespace {
    struct caller {
    	open_f		open ;
    	close_f		close ;
    	shutdown_f	shutdown ;
    	reade_f		reade ;
    	revve_f		recve ;
    	wtite_f		write ;
    } ; /* end struct (caller) */
} /* end namespace */


/* forward references */

namespace cm_ns {
    extern int cm_trysys(CM *,SI *,SD *,SD_ARGS *,
			EC *,SY_ENT *,cc *,cc *,mv) noex ;
    extern int cm_trysysargs(CM *,SI *,vecstr *,cc *,int) noex ;
} /* end namespace */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace {
    struct opener {
	cm		*op ;
	cm_args		*argp ;
	cchar		*hn ;
	cchar		*sn ;
	mainv		av ;
	SD_ARGS		da{} ;
	expcook		cooks ;
	int		arglen ;
	opener(cm *ªop,cm_args *ªap,ccp ªhn,ccp ªsn,mainv ªav) noex {
	    op		= ªop ;
	    argp	= ªap ;
	    hn		= ªhn ;
	    sn		= ªsn ;
	    av		= ªav ;
	} ; /* end ctor */
	operator int () noex ;
	int procargs	() noex ;
	int loadcooks	() noex ;
	int procargs	() noex ;
	int tryfind	() noex ;
	int trysave	() noex ;
    } ; /* end struct (opener) */
} /* end namespace */

namespace cm_ns {
    int cm_trysys(cm *op,SI *sip,SD *dp,SD_ARGS *dap,
	    EC *cookp,SY_ENT *sep,cc *sysname,cc *svcname,mainv av) noex {
	SD_ENT		*dep ;
	vecstr		args ;
	buffer		barg ;
	int		rs ;
	bool		f_loaded = false ;
	bool		f_args = false ;

	op->dobj = nullptr ;
	rs = sysdialer_loadin(dp,sep->dialername,&dep) ;
	f_loaded = (rs >= 0) ;

	/* does this module have our attributes */
	if ((rs >= 0) && (dap->options != 0)) {
	    if ((dap->options & op->dflags) != op->dflags) {
	        rs = SR_NOTSUP ;
	    }
	}

	/* OK, allocate and open the module */
	if (rs >= 0) {
	    op->dsz = dep->osz ;
	    rs = lm_mall(dep->osz,&op->dobj) ;
	}

	if (rs >= 0) {
	    rs = vecstr_start(&args,5,0) ;
	    f_args = (rs >= 0) ? true : false ;
	}

	if ((rs >= 0) && ((rs = buffer_start(&barg,sip->argslen)) >= 0)) {
	    cchar	*abuf = nullptr ;
	    int		alen = 0 ;

	    memclear(op->dobj,op->dsz) ;

	    op->c = dep->c ;

	    rs = vecstr_add(&args,sep->dialername,-1) ;

	/* expand out the dialer arguments */
	    if ((rs >= 0) && (sep->dialerargs != nullptr)) {
		cint	dal = sep->dialerargslen ;
		cchar		*dap = sep->dialerargs ;

	    	if ((rs = expcook_expbuf(cookp,0,&barg,dap,dal)) >= 0) {
		    rs = buffer_get(&barg,&abuf) ;
		    alen = rs ;
		}

	    } /* end if */

	/* separate the individual arguments out */

	    if ((rs >= 0) && (alen > 0)) {
		rs = cm_trysysargs(op,sip,&args,abuf,alen) ;
	    }

	    if (rs >= 0) {
		if (op->c.open != nullptr) {
 	            if (mainv tav ; (rs = vecstr_getvec(&args,&tav)) >= 0) {
	                dap->argv = tav ;
	                rs = (op->c.open)(op->dobj,dap,sysname,svcname,av) ;
		    }
		} else {
	            rs = SR_LIBACC ;
		}
	    }

	    buffer_finish(&barg) ;
	} /* end if (open the module) */

	if (f_args) {
	    vecstr_finish(&args) ;
	}

	if (rs < 0) {
	    if (op->dobj) {
	        lm_free(op->dobj) ;
		op->dobj = nullptr ;
	    } /* end if (memory-release) */
	    if (f_loaded) {
	        sysdialer_loadout(dp,sysname) ;
	    }
	} /* end if */

	return rs ;
    } /* end subroutine (cm_trysys) */
} /* end namespace (cm_ns) */

namespace cm_ns {
    int cm_trysysargs(cm *op,SI *sip,VS *alp,char *abuf,int alen) noex {
	using		cm_ns::terms ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op && sip) ylikely {
	    if (char *fbuf ; (rs = lm_mn(&fbuf)) >= 0) ylikely {
		cint	flen = rs ;
	        if (field fsb ; (rs = fsb.start(abuf,alen)) >= 0) {
	            for (int fl ; (fl = fsb.sharg(terms,fbuf,flen)) > 0 ; ) {
		        rs = alp->add(fbuf,fl) ;
		        if (rs < 0) break ;
	            } /* end for */
	            rs1 = fsb.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (field) */
		{
	            rs1 = lm_free(fbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end (memory-release) */
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (cm_trysysargs) */
} /* end namespace (cm_ns) */


