/* pingtab SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* object to handle the user's PINGTAB file */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debugging */

/* revision history:

	= 1999-11-13, David A­D­ Morano
        This is a new object modules that collects the code that accesses the
        user currency file in one place.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	pingtab

  	Description:
	This object module accesses a PINGTAB file. The PINGTAB
	file is used to hold the names and PING timeout of hosts
	that are to be pinged by our caller.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIXO */
#include	<sys/param.h>		/* POSIX */
#include	<sys/stat.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<netdb.h>		/* POSIX */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<cfdec.h>		/* LIBUC */
#include	<field.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<strdcpy.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |TIMEBUFLEN| */
#include	<bfile.h>		/* LIBB */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"pingtab.h"


/* local defines */

#define	PT		pingtab
#define	PT_ENT		pingtab_ent

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* compile-time debugging */
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */



/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local inline int pingtab_magic(PT *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == PINGTAB_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (pingtab_magic) */


/* local variables */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int pingtab_open(PT *op,cchar *fname) noex {
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	DEBUGPRINTF("ent\n") ;
	if (op && fname) ylikely {
	    rs = SR_NOENT ;
	    if (fname[0]) ylikely {
		rs = SR_NOMEM ;
		if (bfile *pfp = new(nt) bfile ; pfp) ylikely {
	            /* open the user PINGTAB file (if present) */
		    if ((rs = pfp->open(fname,"r")) >= 0) ylikely {
		        if (ustat sb ; (rs = pfp->stat(&sb)) >= 0) ylikely {
	        	    if (! S_ISDIR(sb.st_mode)) {
				op->pfp = pfp ;
	            	        op->magval = PINGTAB_MAGIC ;
	        	    } else {
	            	        rs = SR_ISDIR ;
			    }
	    	        } /* end if (bstat) */
	                if (rs < 0) {
	                    pfp->close() ;
	                } /* end if (error) */
	            } /* end if (bfile-open) */
		    if (rs < 0) {
		        delete pfp ;
		    } /* end if (error) */
	        } /* end if (new-bfile) */
	   } /* end if (valid) */
	} /* end if (non-null) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (pingtab_open) */

/* close this object */
int pingtab_close(PT * op) noex {
	int	rs ;
	int	rs1 ;
	if ((rs = pingtab_magic(op)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (bfile *pfp = op->pfp ; pfp) {
	        rs1 = pfp->close ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (pingtab_magic) */
	return rs ;
} /* end subroutine (pingtab_close) */

local int pingtab_procln(PT *,PT_ENT *,char *,int,cchar *,int) noex ;

/* read an entry from the file */
int pingtab_read(PT *op,PT_ENT *ep,char *ebuf,int elen) noex {
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	int		hl = 0 ; /* return-value */
	if ((rs = pingtab_magic(op,ep,ebuf)) >= 0) {
	    ebuf[0] = '\0' ;
	    if (char *lbuf ; (rs = mem.ml(&lbuf)) >= 0) {
		cint llen = rs ;
		bfile *pfp = op->pfp ;
	        ep->intminping = -1 ;
	        ep->timeout = -1 ;
	        while ((rs = pfp->readln(lbuf,llen)) > 0) {
		    cchar *sp ;
	            if (cint sl = sfcontent(lbuf,rs,&sp) ; sl > 0) {
			rs = pingtab_procln(op,ep,ebuf,elen,sp,sl) ;
			hl = rs ;
		    } /* end if (sfcontent) */
		    if (rs < 0) break ;
		} /* end while */
		rs1 = mem.free(lbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (pingtab_magic) */
	return (rs >= 0) ? hl : rs ;
} /* end subroutine (pingtab_read) */

local int loadbuf(char *bp,int bl,cc *sp,int sl) noex {
    	return conv<int>(strdcpy(bp,bl,sp,sl) - bp) ;
} /* end subroutine (loadbuf) */

local int pingtab_procln(PT *op,PT_ENT *ep,char *ebuf,int elen,
		cchar *sp,int sl) noex {
    	cnullptr	np{} ;
    	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	int		hl = 0 ; /* return-value */
	if (op) ylikely {
            if (field fb ; (rs = fb.start(sp,sl)) >= 0) {
                int         fl ;
                cchar       *fp ;
                for (int i = 0 ; i < 3 ; i += 1) {
                    if ((fl = fb.get(np,&fp)) >= 0) {
                        if (fl > 0) {
                            switch (i) {
                            case 0:
			        hl = loadbuf(ebuf,elen,fp,fl) ;
                                break ;
                            case 1:
                                if (int iw ; cfdecti(fp,fl,&iw) >= 0) {
                                    ep->intminping = iw ;
			        }
                                break ;
                            case 2:
                                if (int iw ; cfdecti(fp,fl,&iw) >= 0) {
                                    ep->timeout = iw ;
			        }
                                break ;
                            } /* end switch */
                        } /* end if (non-zero positive) */
                    } /* end if (field_get) */
                    if (fb.term == '#') break ;
		    if (hl > 0) break ;
                    if (rs < 0) break ;
                } /* end for */
                rs1 = fb.finish ;
                if (rs >= 0) rs = rs1 ;
            } /* end if (field) */
	} /* end if (bug-check) */
	return (rs >= 0) ? hl : rs ;
} /* end subroutine (pingtab_procln) */

int pingtab_rewind(PT *op) noex {
	int		rs ;
	if ((rs = pingtab_magic(op)) >= 0) {
	    bfile *pfp = op->pfp ;
	    rs = pfp->rewind ;
	} /* end if (pingtab_magic) */
	return rs ;
} /* end subroutine (pingtab_rewind) */


