/* mkhashfile */
/* lang=C20 */

/* write out the hash file */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-03-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1999 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkhashfile

	Description:
	This subroutine writes out the hash file.

	Synopsis:
	int mkhashfile(PI *pip,vecint *table,int tablen,KI *kip,cc *fn) noex

	Arguments:
	- pip		program information pointer
	- table		array of VECINTs
	- tablen	table length
	- tfp		pointer to tag file
	- fn		file to process

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<limits.h>
#include	<unistd.h>
#include	<time.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<endian.h>
#include	<bfile.h>
#include	<vecint.h>
#include	<localmisc.h>
#include	<intceil.h>
#include	<strwcpy.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#define	HASHFILE_MAGIC		"MKINVHASH"
#define	HASHFILE_VERSION	0
#define	HASHFILE_TYPE		0

#ifndef	ITEMLEN
#define	ITEMLEN		100
#endif

#define	PI		struct proginfo
#define	KI		struct keyinfo


/* external subroutines */


/* external variables */


/* local structures */

enum headers {
	header_wtime,
	header_sdnoff,
	header_sfnoff,
	header_taboff,
	header_tablen,
	header_minlen,
	header_maxlen,
	header_maxtags,
	header_overlast
} ;


/* forward references */

static int showtable(cchar *,uint,int) noex ;


/* local variables */


/* exported subroutines */

int mkhashfile(PI *pip,vecint *table,int tablen,KI *kip,cc *fname) noex {
	bfile		hashfile ;
	int		rs ;
	int		rs1 ;
	int		c_entries = 0 ;

	if (fname == NULL) return SR_FAULT ;

	pip->daytime = time(NULL) ;

	if ((rs = bopen(&hashfile,fname,"rwct",0664)) >= 0) {
	    offset_t	boff, uoff ;
	    uint	header[header_overlast] ;
	    uint	fto ;
	    uint	*ptab = NULL ;
	    int		cl ;
	    int		size, headsize, sdnsize, sfnsize, tabsize ;
	    int		headoff, sdnoff, sfnoff, taboff, listoff ;
	    int		maxtags = 0 ;
	    int		ropts = 0 ;
	    char	vetu[16] ;
	    char	*cp ;

/* prepare and write the file magic */

	cp = strwcpy(vetu,HASHFILE_MAGIC,15) ;

	*cp++ = '\n' ;
	cl = (vetu + 16) - cp ;
	memset(cp,0,cl) ;

	rs = bwrite(&hashfile,vetu,16) ;
	fto = 16 ;

/* prepare and write the version and encoding (VETU) */

	vetu[0] = HASHFILE_VERSION ;
	vetu[1] = ENDIAN ;
	vetu[2] = ropts ;
	vetu[3] = 0 ;

	if (rs >= 0)
	rs = bwrite(&hashfile,vetu,4) ;
	fto += 4 ;

/* OK, we're ready to start writing stuff out! */

	headsize = header_overlast * sizeof(uint) ;

	sdnsize = (pip->sdn != NULL) ? uceil((strlen(pip->sdn) + 1),4) : 4 ;

	sfnsize = (pip->sfn != NULL) ? uceil((strlen(pip->sfn) + 1),4) : 4 ;

	tabsize = tablen * sizeof(uint) ;

	headoff = fto ;
	fto += headsize ;

	sdnoff = fto ;
	fto += sdnsize ;

	sfnoff = fto ;
	fto += sfnsize ;

	taboff = fto ;
	fto += tabsize ;

	listoff = fto ;

/* skip up to the start of the tag lists */

	uoff = listoff ;
	if (rs >= 0)
	    rs = bseek(&hashfile,uoff,SEEK_SET) ;

/* write out the lists */

	if ((rs >= 0) && ((rs = uc_malloc(tabsize,&ptab)) >= 0)) {
	    int		*va ;
	    int		c ;
	    int		i ;

	    for (i = 0 ; i < tablen ; i += 1) {

	        ptab[i] = fto ;

	        rs = vecint_getvec((table + i),&va) ;
	        if (rs > maxtags)
	            maxtags = rs ;

	        c = (rs > 0) ? rs : 0 ;
	        if (c > 0)
	            c_entries += 1 ;

	        rs = bwrite(&hashfile,&c,sizeof(uint)) ;

	        fto += ((rs > 0) ? rs : 0) ;

	        if ((rs >= 0) && (c > 0)) {
	            size = (c * sizeof(uint)) ;
	            rs = bwrite(&hashfile,va,size) ;
	            fto += ((rs > 0) ? rs : 0) ;
	        }

	        if (rs < 0) break ;
	    } /* end for (table lists) */

/* now write out the header itself (must seek back) */

	    boff = headoff ;
	    if (rs >= 0)
	    bseek(&hashfile,boff,SEEK_SET) ;

	    memset(header,0,headsize) ;
	    header[header_wtime] = (uint) pip->daytime ;
	    header[header_sdnoff] = sdnoff ;
	    header[header_sfnoff] = sfnoff ;
	    header[header_taboff] = taboff ;
	    header[header_tablen] = tablen ;
	    header[header_minlen] = kip->minlen ;
	    header[header_maxlen] = kip->maxlen ;
	    header[header_maxtags] = maxtags ;

	    if (rs >= 0)
	        rs = bwrite(&hashfile,header,headsize) ;

	    if (rs >= 0) {
	        char	zbuf[4] ;

/* write out the base directory name */

	        if (rs >= 0) {
	            cp = pip->sdn ;
	            if (pip->sdn == NULL) {
	                cp = zbuf ;
	                memset(zbuf,0,4) ;
	            }
	            rs = bwrite(&hashfile,(void *) cp,sdnsize) ;
	        } /* end if (ok) */

/* write out the default filename */

	        if (rs >= 0) {
	            cp = pip->sfn ;
	            if (pip->sfn == NULL) {
	                cp = zbuf ;
	                memset(zbuf,0,4) ;
	            }
	            rs = bwrite(&hashfile,(void *) cp,sfnsize) ;
	        } /* end if (ok) */

	    } /* end block (writing file path/name information) */

/* write the hash table */

	    if (rs >= 0) {
	        size = tablen * sizeof(uint) ;
	        rs = bwrite(&hashfile,ptab,size) ;
	    }

	    uc_free(ptab) ;
	} /* end block (memory allocation) */

/* we're out of here */

	rs1 = bclose(&hashfile) ;
	if (rs >= 0) rs = rs1 ;
	} /* end if (file) */

	if ((rs < 0) && (fname[0] != '\0')) {
	    u_unlink(fname) ;
	}

	return (rs >= 0) ? c_entries : rs ;
}
/* end subroutine (mkhashfile) */


int hashmapverify(pip,hmip)
PROGINFO	*pip ;
struct hashmap	*hmip ;
{
	const uint	*header ;
	uint		sdnoff, sfnoff, taboff ;
	int		cl ;
	const uchar	*vetu ;
	char		*cp ;

	cl = 15 ;
	if ((cp = strchr(hmip->filemap,'\n')) != NULL)
	    cl = cp - hmip->filemap ;

	if (strncmp(hmip->filemap,HASHFILE_MAGIC,cl) != 0)
	    return SR_INVALID ;

	vetu = (uchar *) (hmip->filemap + 16) ;
	if (vetu[0] != HASHFILE_VERSION)
	    return SR_INVALID ;

	if (vetu[1] != ENDIAN)
	    return SR_INVALID ;

	header = (const uint *) (hmip->filemap + 20) ;

	sdnoff = header[header_sdnoff] ;
	sfnoff = header[header_sfnoff] ;
	taboff = header[header_taboff] ;

	hmip->sdn = (char *) (hmip->filemap + sdnoff) ;
	hmip->sfn = (char *) (hmip->filemap + sfnoff) ;
	hmip->table = (uint *) (hmip->filemap + taboff) ;
	hmip->wtime = header[header_wtime] ;
	hmip->tablen = header[header_tablen] ;
	hmip->minlen = header[header_minlen] ;
	hmip->maxlen = header[header_maxlen] ;
	hmip->maxtags = header[header_maxtags] ;

	hmip->listoff = taboff + (header[header_tablen] * sizeof(uint)) ;

	return 0 ;
}
/* end subroutine (hashmap_verify) ;


/* local subroutines */


#ifdef	COMMENT

static int showtable(fname,taboff,tablen)
cchar	fname[] ;
uint		taboff ;
int		tablen ;
{
	const int	size = tablen * sizeof(uint) ;
	offset_t	uoff ;
	uint		*ptab ;
	int		rs ;
	if ((rs = uc_malloc(size,&ptab)) >= 0) {
	    if ((rs = u_open(fname,O_RDONLY,0666)) >= 0) {
	        const int	hfd = rs ;
	        offset_t	uoff = taboff ;
		int		i ;
	        u_seek(hfd,uoff,SEEK_SET) ;
	        u_read(hfd,ptab,size) ;
	        for (i = 0 ; i < tablen ; i += 1) {
	            debugprintf("mkhashfile: table[%u]=%u\n",i,ptab[i]) ;
	        }
	        u_close(hfd) ;
	    } /* end if (open) */
	    uc_free(ptab) ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (showtable) */

#endif /* COMMENT */


