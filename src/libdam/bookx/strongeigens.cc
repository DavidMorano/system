/* strongeigens SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Strong's Eigen-Words */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-02-07, David A­D­ Morano
	This code was originally written.  

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	strongeigens

	Description:
	These are the Strong's Eigen-Words.

	Notes:
	These words are not likely to change since their publication
	in 1890!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"strongeigens.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */

const ccharp	strongeigens[] = {
	"a", "an", "and", "are", "as", "be", "but", "by", "for",
	"from", "he", "her", "him", "his", "i", "in", "is", "it",
	"me", "my", "not", "o", "of", "our", "out", "shall", "shalt",
	"she", "that", "the", "thee", "their", "them", "they",
	"thou", "thy", "to", "unto", "up", "upon", "us", "was",
	"we", "were", "with", "ye", "you",
	nullptr
} ; /* end array (strongeigens) */


/* exported subroutines */


/* local subroutines */


