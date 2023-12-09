/* sysval */
/* lang=C++20 */

/* retrieve operating-system values */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Type:
	sysval

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<time.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"sysval.hh"


/* local defines */

#define	TO_CACHE	5


/* external subroutines */


/* external variables */


/* local strutures */

namespace {
    struct sysvalcmds {
	int		cmd[sysval_overlast] ;
	constexpr sysvalcmds() noex {
	    for (int i = 0 ; i < sysval_overlast ; i += 1) cmd[i] = -1 ;
	    cmd[sysval_ps] = _SC_PAGESIZE ;
	    cmd[sysval_mc] = _SC_CHILD_MAX ;
	    cmd[sysval_mi] = _SC_IOV_MAX ;
	    cmd[sysval_mg] = _SC_NGROUPS_MAX ;
	    cmd[sysval_mo] = _SC_OPEN_MAX ;
	    cmd[sysval_mp] = _SC_PHYS_PAGES ;
	    cmd[sysval_ct] = _SC_CLK_TCK ;
	    cmd[sysval_pc] = _SC_NPROCESSORS_CONF ;
	    cmd[sysval_po] = _SC_NPROCESSORS_ONLN ;
	} ;
    } ; /* end struct (sysvalcmds) */
    struct sysvaldata {
	time_t		last = 0 ;
	int		vals[sysval_overlast] = {} ;
	constexpr void cktimeout(int name) noex ;
	constexpr int operator [] (int) noex ;
    } ; /* end struct (sysvaldata) */
}


/* forward references */


/* local variables */

static constexpr sysvalcmds	cmds ;

static constinit sysvaldata	data ;


/* exported subroutines */

sysval::operator int () noex {
	int		rs = SR_INVALID ;
	if ((name >= 0) && (name < sysval_overlast)) {
	    switch (name) {
	    case sysval_po:
		data.cktimeout(name) ;
	    } /* end switch */
	    rs = data[name] ;
	} /* end if (valid) */
	return rs ;
}
/* end method (sysval::operator) */


/* local subroutines */

constexpr void sysvaldata::cktimeout(int name) noex {
	static clong	to = TO_CACHE ;
	if (vals[name] > 0) {
	    const time_t	now = time(nullptr) ;
	    if ((now - last) >= to) {
	        vals[name] = 0 ;
	        last = now ;
	    }
	} /* end if (active value) */
}
/* end method (sysvaldata::cktimeout) */

constexpr int sysvaldata::operator [] (int name) noex {
	int		rs = vals[name] ;
	if (vals[name] == 0) {
	    cint	cmd = cmds.cmd[name] ;
	    rs = SR_NOSYS ;
	    if (cmd >= 0) {
		if ((rs = uc_confsys(cmd,nullptr)) >= 0) {
		    vals[name] = rs ;
		}
	    } /* end if (valid command) */
	} /* end if (value needed) */
	return rs ;
}
/* end method (sysvaldata::operator) */


