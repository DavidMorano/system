/* MD5C.H - header file for MD5C.C */
/* charset=ISO8859-1 */
/* lang=C20 */

/* RSA MD5 hash (Message-Digest) */
/* version %I% last-modified %G% */

/****
  Copyright (C) 1990-1992, RSA Data Security, Inc. Created 1991. All
  rights reserved.
****/


/*
 ***********************************************************************
 ** md5.h -- header file for implementation of MD5                    **
 ** RSA Data Security, Inc. MD5 Message-Digest Algorithm              **
 ** Created: 2/17/90 RLR                                              **
 ** Revised: 12/27/90 SRD,AJ,BSK,JT Reference C version               **
 ** Revised (for MD5): RLR 4/27/91                                    **
 **   -- G modified to have y&~z instead of y&z                       **
 **   -- FF, GG, HH modified to add in last register done             **
 **   -- Access pattern: round 2 works mod 5, round 3 works mod 3     **
 **   -- distinct additive constant for each step                     **
 **   -- round 4 added, working mod 7                                 **
 ***********************************************************************
 */

/*
 ***********************************************************************
 ** Copyright (C) 1990, RSA Data Security, Inc. All rights reserved.  **
 **                                                                   **
 ** License to copy and use this software is granted provided that    **
 ** it is identified as the "RSA Data Security, Inc. MD5 Message-     **
 ** Digest Algorithm" in all material mentioning or referencing this  **
 ** software or this function.                                        **
 **                                                                   **
 ** License is also granted to make and use derivative works          **
 ** provided that such works are identified as "derived from the RSA  **
 ** Data Security, Inc. MD5 Message-Digest Algorithm" in all          **
 ** material mentioning or referencing the derived work.              **
 **                                                                   **
 ** RSA Data Security, Inc. makes no representations concerning       **
 ** either the merchantability of this software or the suitability    **
 ** of this software for any particular purpose.  It is provided "as  **
 ** is" without express or implied warranty of any kind.              **
 **                                                                   **
 ** These notices must be retained in any copies of any part of this  **
 ** documentation and/or software.                                    **
 ***********************************************************************
 */

/****

This code was written by: 
Dirk Eddelbuettel

His GitHub repository is:
https://github.com/eddelbuettel

****/

#ifndef _MD5_H
#define _MD5_H

#include	<envstandards.h>	/* ordered first to configure */
#include	<stddef.h>
#include	<stdlib.h>
#include	<stdint.h>		/* fixed sized integers */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


typedef struct {
    uint32_t	total[2] ;
    uint32_t	state[4] ;
    uint8_t	buffer[64] ;
} md5_context ;

EXTERNC_begin

void md5_starts( md5_context *ctx) noex ;
void md5_update( md5_context *ctx,uint8_t *input,uint32_t length) noex ;
void md5_finish( md5_context *ctx,uint8_t digest[16]) noex ;

EXTERNC_end


#endif /* _MD5_H */

/*
 ***********************************************************************
 ** End of md5.h                                                      **
 ******************************** (cut) ********************************
 */


