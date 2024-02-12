/* hdbstr_loadpairs SUPPORT */
/* lang=C++20 */

/* load strings from a file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*****************************************************************************

	This subroutine will read (process) a file and put all of
	the strings found into the string (supplied) list, consisting
	of an HDBSTR object.

	Note: 
	This is NOT the same as 'vecstr_loadfile(3dam)'! This
	subroutine only reads the first TWO strings on the line and
	puts them into the hash DB as key-value pairs! If there is
	only one string on a line, it is a key without a value.

*****************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<bfile.h>
#include	<field.h>
#include	<hdbstr.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	loadline(hdbstr *,cchar *,int) noex ;


/* local variables */

static constexpr char	fterms[32] = {
	0x00, 0x04, 0x00, 0x00,
	0x08, 0x10, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;


/* exported variables */


/* exported subroutines */

int hdbstr_loadpairs(hdbstr *hsp,cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (hsp && fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		char	*lbuf{} ;
	        if ((rs = malloc_ml(&lbuf)) >= 0) {
	            bfile	loadfile, *lfp = &loadfile ;
	            cint	llen = rs ;
	            if ((rs = bopen(lfp,fname,"r",0666)) >= 0) {
	                while ((rs = breadln(lfp,lbuf,llen)) > 0) {
			    rs = loadline(hsp,lbuf,rs) ;
			    c += rs ;
	                    if (rs < 0) break ;
	                } /* end while (reading lines) */
	                rs1 = bclose(lfp) ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (file opened) */
		    rs1 = uc_free(lbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (hdbstr_loadpairs) */


/* local subroutines */

static int loadline(hdbstr *hsp,cchar *lbuf,int ll) noex {
	field		fsb ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = field_start(&fsb,lbuf,ll)) >= 0) {
	    cchar	*kp{} ;
	    if (int kl ; (kl = field_get(&fsb,fterms,&kp)) > 0) {
		int	vl = 0 ;
		cchar	*vp{} ;
		bool	f = true ;
		f = f && (fsb.term != '#') ;
		f = f && ((vl = field_get(&fsb,fterms,&vp)) >= 0) ;
		if (f) {
		    c += 1 ;
		    rs = hdbstr_add(hsp,kp,kl,vp,vl) ;
		}
	    } /* end if (got one) */
	    rs1 = field_finish(&fsb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (fields) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (loadline) */


