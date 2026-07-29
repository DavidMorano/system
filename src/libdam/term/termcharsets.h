/* termcharsets HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* terminal character sets */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/**** Notes:

	The DEC® Line-Drawing character set (a 94-cs) is also known
	as the DEC® Special-Graphics character set.  DEC® now calls
	their old Line-Drawing set their Special-Graphics set.
	We tend to use the old name (Line-Drawing).

****/

#ifndef	TERMCHARSETS_INCLUDE
#define	TERMCHARSETS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


#define	TCS_ASCII	"B"		/* 94-cs */
#define	TCS_UK		"A"		/* 94-cs */
#define	TCS_DECLINEDRAW	"0"		/* DEC® Line-Drawing (94-cs) */
#define	TCS_DECTECH	">"		/* DEC® Technical (94-cs) */
#define	TCS_DECSUP	"%5"		/* DEC® Supplemental (94-cs) */
#define	TCS_DECGREEK	"\"?"
#define	TCS_DECHEBREW	"\"4"
#define	TCS_DECTURKISH	"%0"
#define	TCS_DECCYRILLIC	"&4"
#define	TCS_FRENCH	"R"
#define	TCS_FRENCHCAN	"9" /* or 'Q' */
#define	TCS_NORWEGIAN	"`E" /* or '6' */
#define	TCS_FINISH	"5" /* or 'C' */
#define	TCS_GERMAN	"K"
#define	TCS_ITALIAN	"Y"

#define	TCS_ISOLATIN1	"A" /* 96-cs */
#define	TCS_ISOLATIN2	"B" /* 96-cs */
#define	TCS_PREFSUP	"<" /* 96-cs */
#define	TCS_GREEKSUP	"F" /* 96-cs */
#define	TCS_HEBREWSUP	"H" /* 96-cs */
#define	TCS_LATINCYRIL	"L" /* 96-cs */
#define	TCS_ISOLATIN5	"M" /* 96-cs */


#ifdef	COMMENT

/****

=	Swiss
7	Swedish
H	Swedish
Z	Spanish
%6	Portuguese
">	Greek
%=	Hebrew
%2	Turkish
%3	SCS
&5	Russian

****/

#endif /* COMMENT */


#endif /* TERMCHARSETS_INCLUDE */


