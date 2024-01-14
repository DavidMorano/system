/* filetype HEADER */
/* lang=C20 */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	FILETYPE_INCLUDE
#define	FILETYPE_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<dirent.h>
#include	<clanguage.h>


enum filetypes {
	filetype_unknown,
	filetype_fifo,
	filetype_chr,
	filetype_dir = 4,
	filetype_blk = 6,
	filetype_reg = 8,
	filetype_lnk = 10,
	filetype_sock = 12,
	filetype_wht = 14,
	filetype_name,
	filetype_overlast
} ;


#ifndef	DT_UNKNOWN
#define	DT_UNKNOWN	filetype_reg
#endif

#ifndef	DT_FIFO
#define	DT_FIFO		filetype_fifo
#endif

#ifndef	DT_CHR
#define	DT_CHR		filetype_chr
#endif

#ifndef	DT_DIR
#define	DT_DIR		filetype_dir
#endif

#ifndef	DT_BLK
#define	DT_BLK		filetype_blk
#endif

#ifndef	DT_REG
#define	DT_REG		filetype_reg
#endif

#ifndef	DT_LNK
#define	DT_LNK		filetype_lnk
#endif

#ifndef	DT_SOCK
#define	DT_SOCK		filetype_sock
#endif

#ifndef	DT_WHT
#define	DT_WHT		filetype_wht
#endif

#ifndef	DT_NAME
#define	DT_NAME		filetype_name
#endif


EXTERNC_begin

static inline int filetype(mode_t m) noex {
	return ((m & 0170000) >> 12) ;
}

EXTERNC_end


#endif /* FILETYPE_INCLUDE */


