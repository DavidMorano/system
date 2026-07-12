/* charnames SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* names (c-strings) associated with ISO-Latin-1 character codes */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-15, David A­D­ Morano
	This code is newly written but modeled after some code that
	was originally written to get some standardized variable
	serialization for the Ethernet-Controller (ETHCON) development
	work.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	ascii

	Description:
	This contains miscellaneious support subroutines.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"charnames.h"


/* local defines */

#define	NCHS	4			/* number of bytes in string */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

cint		tablen = (UCHAR_MAX + 1) ;

namespace {
    struct charpair {
	int	idx ;
	ccharp	name ;
    } ;
} /* end namespace */

constexpr charpair	pairs[] = {
	{ 0x00,		"NUL"	},
	{ 0x01,		"SOH"	},
	{ 0x02,		"STX"	},
	{ 0x03,		"ETX"	},
	{ 0x04,		"EOT"	},
	{ 0x05,		"ENQ"	},
	{ 0x06,		"ACK"	},
	{ 0x07,		"BEL"	},
	{ 0x08,		"BS"	},
	{ 0x09,		"TAB"	},
	{ 0x0A,		"LF"	},
	{ 0x0B,		"VT"	},
	{ 0x0C,		"FF"	},
	{ 0x0D,		"CR"	},
	{ 0x0E,		"SO"	},
	{ 0x0F,		"SI"	},
	{ 0x10,		"DLE"	},
	{ 0x11,		"DC1"	},
	{ 0x12,		"DC2"	},
	{ 0x13,		"DC3"	},
	{ 0x14,		"DC4"	},
	{ 0x15,		"NAK"	},
	{ 0x16,		"SYN"	},
	{ 0x17,		"ETB"	},
	{ 0x18,		"CAN"	},
	{ 0x19,		"EM"	},
	{ 0x1A,		"SUB"	},
	{ 0x1B,		"ESC"	},
	{ 0x1C,		"FS"	},
	{ 0x1D,		"GS"	},
	{ 0x1E,		"RS"	},
	{ 0x1F,		"US"	},
	{ CH_SP,	"SP"	},
	{ CH_DEL,	"DEL"	},
	{ CH_B0,	"B0"	},
	{ CH_B1,	"B1"	},
	{ CH_B2,	"B2"	},
	{ CH_B3,	"B3"	},
	{ CH_IND,	"IND"	},
	{ CH_NEL,	"NEL"	},
	{ CH_SSA,	"SSA"	},
	{ CH_ESA,	"ESA"	},
	{ CH_HTS,	"HTS"	},
	{ CH_HTJ,	"HTJ"	},
	{ CH_VTS,	"VTS"	},
	{ CH_PLD,	"PLD"	},
	{ CH_PLU,	"PLU"	},
	{ CH_RI,	"RI"	},
	{ CH_SS2,	"SS2"	},
	{ CH_SS3,	"SS3"	},
	{ CH_DCS,	"DCS"	},
	{ CH_PU1,	"PU1"	},
	{ CH_PU2,	"PU2"	},
	{ CH_STS,	"STS"	},
	{ CH_CRH,	"CRH"	},
	{ CH_MW,	"MW"	},
	{ CH_SPA,	"SPA"	},
	{ CH_EPA,	"EPA"	},
	{ CH_SOS,	"SOS"	},
	{ CH_99,	"99"	},
	{ CH_DID,	"DID"	},
	{ CH_CSI,	"CSI"	},
	{ CH_ST,	"ST"	},
	{ CH_OSC,	"OSC"	},
	{ CH_PM,	"PM"	},
	{ CH_APC,	"APC"	},
	{ CH_NBSP,	"NSP"	}
} ; /* end array */

namespace {
    struct charnames {
	char	tab[tablen][NCHS] = {} ;
	consteval void mktab() noex ;
	consteval charnames() noex {
	    mktab() ;
	} ; /* end ctor */
    } ; /* end struct (charnames) */
} /* end namespace */

consteval void charnames::mktab() noex {
    	for (int ch = 0 ; ch < tablen ; ch += 1) {
	    tab[ch][0] = char(ch) ;
	} /* end for */
    	for (cauto &e : pairs) {
	    for (int j = 0 ; j < (NCHS - 1) ; j += 1) {
	        tab[e.idx][j] = e.name[j] ;
	    } /* end for */
	} /* end for */
} /* end method (charnames::mktab) */


/* forward references */

local int mkinit() noex ;


/* local variables */

constexpr charnames	charname_data ;
static cint		dummy = mkinit() ;


/* exported variables */

cchar		(*charname)[NCHS] ;


/* exported subroutines */


/* local subroutines */

local int mkinit() noex {
    	charname = charname_data.tab ;
	return 0 ;
} /* end subroutine */


