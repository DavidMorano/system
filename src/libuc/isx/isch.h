/* isch HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* test a character for a property */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	ISCHARX_INCLUDE
#define	ISCHARX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<limits.h>		/* CSTD |UCHAR_MAX| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern bool	ischalpha	(int) noex attrpure ;
extern bool	ischalnum	(int) noex attrpure ;
extern bool	ischdigit	(int) noex attrpure ;
extern bool	ischdigex	(int) noex attrpure ;
extern bool	ischwhite	(int) noex attrpure ;
extern bool	ischlower	(int) noex attrpure ;
extern bool	ischupper	(int) noex attrpure ;
extern bool	ischprint	(int) noex attrpure ;
extern bool	ischterm	(int) noex attrpure ;
extern bool	ischprintterm	(int) noex attrpure ;
extern bool	ischprintbad	(int) noex attrpure ;
extern bool	ischdict	(int) noex attrpure ;
extern bool	ischcmdstart	(int) noex attrpure ;
extern bool	ischhdrkey	(int) noex attrpure ;

local inline bool	ischpl	(int ch) noex attrconst {
	return (ch == '+') ;
} /* end */
local inline bool	ischmi	(int ch) noex attrconst {
	return (ch == '-') ;
} /* end */
local inline bool	ischpm	(int ch) noex attrconst {
	return (ch == '+') || (ch == '-') ;
} /* end */
local inline bool	ischme	(int ch) noex attrconst {
	return (ch == '+') || (ch == '-') || (ch == '!') ;
} /* end */
local inline bool	ischbinar	(int ch) noex attrconst {
	return (ch >= '0') && (ch <= '1') ;
} /* end */
local inline bool	ischoctal	(int ch) noex attrconst {
	return (ch >= '0') && (ch <= '7') ;
} /* end */
local inline bool	ischblank	(int ch) noex attrconst {
	return (ch == ' ') || (ch == '\t') ;
} /* end */
local inline bool	ischdig		(int ch) noex attrconst {
	return (ch >= '0') && (ch <= '9') ;
} /* end */
local inline bool	ischbin		(int ch) noex attrconst {
	return (ch >= '0') && (ch <= '1') ;
} /* end */
local inline bool	ischoct		(int ch) noex attrconst {
	return (ch >= '0') && (ch <= '7') ;
} /* end */
local inline bool	ischdec		(int ch) noex attrconst {
	return (ch >= '0') && (ch <= '9') ;
} /* end */
local inline bool	ischhex		(int ch) noex attrpure {
    	return ischdigex(ch) ;
} /* end */
local inline bool	ischwht		(int ch) noex attrpure {
    	return ischwhite(ch) ;
} /* end */
local inline bool	ischnum		(int ch) noex attrpure {
    	return ischdigex(ch) || (ch == '\\') || (ch == 'x') ;
} /* end */
local inline bool	ischlc		(int ch) noex attrpure {
        return ischlower(ch) ;
} /* end */
local inline bool	ischuc		(int ch) noex attrpure {
        return ischupper(ch) ;
} /* end */
local inline bool	ischeol		(int ch) noex attrconst {
	return (ch == '\n') || (ch == '\r') ;
} /* end */
local inline bool	ischzero	(int ch) noex attrconst {
	return (ch == '0') ;
} /* end */
local inline bool	ischabbr	(int ch) noex attrconst {
	ch &= UCHAR_MAX ;
	return (ch == '.') || (ch == ('­' & UCHAR_MAX)) || (ch == '-') ;
} /* end */
local inline bool	ischblk		(int ch) noex attrpure {
	return ischblank(ch) ;
} /* end */
local inline bool	ischspacetab	(int ch) noex attrpure {
	return ischblank(ch) ;
} /* end */
local inline bool	ischAbbr	(int ch) noex attrpure {
    	return ischabbr(ch) ;
} /* end */

EXTERNC_end


#endif /* ISCHARX_INCLUDE */


