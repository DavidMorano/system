/* dbmake SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* database-make */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-06-11, David A­D­ Morano
	I snarfed this file from some hardware research I was doing
	since it seemed be a pretty good match for the present
	algorithm needs.  We will see how it works out!

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	dbmake

	Description:
	This module is used to manage a DBMAKE object.  We always
	check if your alias key is the string "Postmater" (in a
	case-insensitive way) and if it is, we convert it to LOWER
	case before doing the 'mailalias_fetch()'.  Also, unlike
	other fetching (with other DBs), there is no concept of
	"prefix" match fetching!  We use TWO string tables in this
	DB (unlike some other similar DBs). We use one string table
	for keys and another for values. I sort of did this for fun
	(so far). This is actually potentially less space-efficient
	than using just one string table since strings that are
	both a key and a value are not combined in one table but
	rather appear separately in each of the two tables. However,
	the up side is that:

	a. making a record table of only keys is done by just taking
	advantage of the strtab method for that

	b. we easily can provide an interface to transverse or
	otherwise just query the keys if we want to later

	c. we can do more space-efficient building of the DB file
	since we only need to keep the smaller key string table
	around for the main indexing!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<bit>			/* |rotl(3c++)| + |rotr(3c++)| */
#include	<strings.h>		/* |strncasecmp(3c)| */
#include	<usystem.h>
#include	<sysval.hh>
#include	<bufsizevar.hh>
#include	<endian.h>
#include	<ids.h>
#include	<vecobj.h>
#include	<linebuffer.h>
#include	<bfile.h>
#include	<field.h>
#include	<fieldterminit.hh>
#include	<kvsfile.h>
#include	<strtab.h>
#include	<strn.h>
#include	<sfx.h>
#include	<snx.h>
#include	<sncpyx.h>
#include	<strwcpy.h>
#include	<mkx.h>			/* |mkmagic(3uc)| */
#include	<nextpowtwo.h>
#include	<hash.h>
#include	<hashindex.h>
#include	<randlc.h>
#include	<matxstr.h>
#include	<mailvalues.hh>		/* |mailaliaslen| */
#include	<char.h>
#include	<isnot.h>
#include	<iserror.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"dbmake.hh"
#include	"mailaliashdr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlibentr(3u)| */

/* local defines */


/* imported namespaces */

using namespace	mailutils ;		/* namespace */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::rotl ;			/* subroutine-template */
using std::rotr ;			/* subroutine-template */
using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		mailaliaslen ;
	operator int () noex ;
    } ; /* end struct (vars) */
}


/* forward references */


/* local variables */

static bufsizevar		maxnamelen(getbufsize_mn) ;
static bufsizevar		maxpathlen(getbufsize_mp) ;
static vars			var ;

/* all white space plus colon (':') */
constexpr fieldterminit		keys("\b\t\n\f\v :") ;

/* all white space plus comma (',') */
constexpr fieldterminit		vals("\b\t\n\f\v ,") ;


/* exported variables */


/* exported subroutines */

int dbmake::wrfileproc(cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c_rec = 0 ; /* ¥ GCC false complaint */
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		if (linebuffer lb ; (rs = lb.start) >= 0) {
		    cint	llen = lb.llen ;
		    char	*lbuf = lb.lbuf ;
		    cmode	om = 0666 ;
	            if (bfile afile ; (rs = bopen(&afile,fname,"r",om)) >= 0) {
	                f_havekey = false ;
	                while ((rs = breadln(&afile,lbuf,llen)) > 0) {
			    cchar	*cp ;
			    if (int cl ; (cl = sfcontent(lbuf,rs,&cp)) > 0) {
	                            if (! CHAR_ISWHITE(cp[0])) {
	                                f_havekey = false ;
	                            }
	                            rs = wrfileline(cp,cl) ;
	                            c_rec += rs ;
	                    } /* end if (sfcontetnt) */
	                    if (rs < 0) break ;
	                } /* end while */
	                rs1 = bclose(&afile) ;
	                if (rs >= 0) rs = rs1 ;
	            } else if (isNotPresent(rs)) {
	                rs = SR_OK ;
	            } /* end if (afile) */
		    rs1 = lb.finish ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (linebuffer) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c_rec : rs ;
}
/* end subroutine (dbmake::wrfileproc) */

/* write out the cache file */
int dbmake::wrfile(time_t dt) noex {
	int		rs ;
	if ((rs = strtab_count(klp)) >= 0) {
	    ktlen = (rs+1) ;
	    ktsize = (ktlen + 1) * szof(int) ;
	    if ((rs = rlp->count) >= 0) {
		reclen = (rs + 1) ;
		recsize = ((reclen+1) * (2 * szof(int))) ;
		rilen = nextpowtwo(reclen) ;
		if (rilen < 4) rilen = 4 ;
		risize = (rilen * 2 * szof(int)) ;
		if ((rs = strtab_strsize(klp)) >= 0) {
		    sksize = rs ;
		    if ((rs = strtab_strsize(vlp)) >= 0) {
			svsize = rs ;
		        rs = wrfiler(dt) ;
		    }
		}
	    }
	}
	return rs ;
}
/* end subroutine (dbmake::wrfile) */

/* OK, we are ready to start writing stuff out! */
int dbmake::wrfiler(time_t dt) noex {
	int		rs ;
	int		header[mailaliashdr_overlast] ;
	int		fto = 0 ;
	int		ml ;
	char		fidbuf[DBMAKE_IDLEN + 1] ;
	char		*bp ;
	/* prepare the file magic */
	{
	    bp = fidbuf ;
	    ml = mkmagic(bp,mags,magp) ;
	    bp += ml ;
	}
	/* prepare the version and encoding (VETU) */
	{
	fidbuf[16] = uchar(fver) ;	/* file-version */
	fidbuf[17] = uchar(ENDIAN) ;
	fidbuf[18] = uchar(ropts) ;
	fidbuf[19] = 0 ;
	}
	/* write magic along with version encoding */
	cint	fidlen = intconv(bp - fidbuf) ;
	if ((rs = u_write(fd,fidbuf,fidlen)) >= 0) {
	    cint	hsize = (mailaliashdr_overlast * szof(int)) ;
	    fto += intconv(bp - fidbuf) ;
	    /* make the header itself (skip over it for FTO) */
	    {
	    fto += hsize ;
	    }
	    /* everything else */
	    {
	    header[mailaliashdr_wtime] = uint(dt) ;
	    header[mailaliashdr_wcount] = 1 ;
	    }
	    {
	    header[mailaliashdr_key] = fto ;
	    header[mailaliashdr_keylen] = ktlen ;
	    fto += ktsize ;
	    }
	    {
	    header[mailaliashdr_rec] = fto ;
	    header[mailaliashdr_reclen] = reclen ;
	    fto += recsize ;
	    }
	    {
	    header[mailaliashdr_ind] = fto ;
	    header[mailaliashdr_indlen] = rilen ;
	    fto += risize ;
	    }
	    {
	    header[mailaliashdr_skey] = fto ;
	    header[mailaliashdr_skeysize] = sksize ;
	    fto += sksize ;
	    }
	    {
	    header[mailaliashdr_sval] = fto ;
	    header[mailaliashdr_svalsize] = svsize ;
	    fto += svsize ;
	    }
	    /* write out the header */
	    if ((rs = u_write(fd,header,hsize)) >= 0) {
		if ((rs = wrfilekeytab()) >= 0) {
	            if ((rs = wrfilerec()) >= 0) {
	        	if ((rs = wrfilekeys()) >= 0) {
	        	    rs = wrfilevals() ;
			}
		    }
		}
	    } /* end if (ok) */
	} /* end if (write of id-section) */
	return rs ;
}
/* end subroutine (dbmake::wrfiler) */

int dbmake::wrfileline(cchar *lbuf,int llen) noex {
    	static cint	rsv = var ;
	int		rs ;
	int		rs1 ;
	int		c_rec = 0 ;
	if ((rs = rsv) >= 0) {
	    cint	klen = var.mailaliaslen ;
	    if (field fsb ; (rs = fsb.start(lbuf,llen)) >= 0) {
	        cint	rsn = SR_NOTFOUND ;
	        char	kbuf[klen +1] = { 0 } ;
	        if (! f_havekey) {
		    cchar	*kt = keys.terms ;
	            cchar	*pm = "Postmaster" ;
	            cchar	*kp ;
	            if (int kl ; (kl = fsb.get(kt,&kp)) >= 0) {
	                if (kl > 0) {
			    bool	f = true ;
	                    f = f && (kl == 10) ;
			    f = f && (strncasecmp(pm,kp,kl) == 0) ;
	                    f_havekey = true ;
	                    if (f) {
	                        strwcpylc(kbuf,kp,min(kl,klen)) ;
	                    } else {
	                        strwcpy(kbuf,kp,min(kl,klen)) ;
	                    }
	                } /* end if (positive) */
	            } /* end if (field_get) */
	        } /* end if (didn't have a key already) */
	        if (f_havekey && (fsb.term != '#')) {
		    int	c = 0 ;
	            int	vl ;
		    cchar	*vt = vals.terms ;
	            cchar	*vp ;
	            while ((vl = fsb.get(vt,&vp)) >= 0) {
	                if (vl > 0) {
	                    int	ival = 0 ;
	                    if (c == 0) { /* enter into key-string table */
	                        if ((rs = strtab_already(klp,kbuf,-1)) == rsn) {
	                            rs = strtab_add(klp,kbuf,-1) ;
	                            ikey = rs ;
	                        } else {
	                            ikey = rs ;
	                        }
	                    } /* end if (entering key) */
			    /* enter value into string table */
	                    if (rs >= 0) { /* enter into val-string table */
	                        if ((rs = strtab_already(vlp,vp,vl)) == rsn) {
	                            rs = strtab_add(vlp,vp,vl) ;
	                            ival = rs ;
	                        } else if (rs >= 0) {
	                            ival = rs ;
	                        }
			        /* enter record */
	                        if ((rs >= 0) && (ival > 0)) {
	                            record	re(ikey,ival) ;
	                            if ((rs = rlp->find(&re)) == rsn) {
	                                rs = rlp->add(&re) ;
	                            }
	                            if (rs >= 0) {
	                                nrecs += 1 ;
	                                c_rec += 1 ;
				        c += 1 ;
	                            }
	                        } /* end if (new entry) */
	                    } /* end if (ok) */
	                    if (fsb.term == '#') break ;
	                    if (rs < 0) break ;
	                } /* end while (fields) */
	            } /* end if (retrieved key) */
	        } /* end if (have key) */
	        rs1 = fsb.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (field) */
	    count = c_rec ;
	} /* end if (vars) */
	return (rs >= 0) ? c_rec : rs ;
}
/* end method (dbmake::wrfileline) */

/* write the key-table */
int dbmake::wrfilekeytab() noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if (int *keytab ; (rs = libmem.mall(ktsize,&keytab)) >= 0) {
	    if ((rs = strtab_recmk(klp,keytab,ktsize)) >= 0) {
		rs = u_write(fd,keytab,ktsize) ;
		wlen += rs ;
	    }
	    rs1 = libmem.free(keytab) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */
	return (rs >= 0) ? wlen : rs ;
}
/* end method (dbmake::wrfilekeytab) */

/* write the record table */
int dbmake::wrfilerec() noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = libmem.mall(recsize,&rectab)) >= 0) {
	    {
	        void	*vp{} ;
	        int	ri = 0 ;
	        rectab[ri][0] = 0 ;
	        rectab[ri][1] = 0 ;
	        ri += 1 ;
	        for (int i = 0 ; rlp->get(i,&vp) >= 0 ; i += 1) {
	    	    record	*rep = recordp(vp) ;
	            rectab[ri][0] = rep->key ;
	            rectab[ri][1] = rep->val ;
	            ri += 1 ;
	        } /* end for */
	        rectab[ri][0] = -1 ;
	        rectab[ri][1] = 0 ;
	        rs = u_write(fd,rectab,recsize) ;
	        wlen = rs ;
	    } /* end block */
	    rs1 = libmem.free(rectab) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (record table) */
	return (rs >= 0) ? wlen : rs ;
}
/* end method (dbmake::wrfilerec) */

/* make the index table and the key-string table */
int dbmake::wrfilekeys() noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if (int (*indtab)[2] ; (rs = libmem.mall(risize,&indtab)) >= 0) {
	    if (char *kstab ; (rs = libmem.mall(sksize,&kstab)) >= 0) {
	        if ((rs = strtab_strmk(klp,kstab,sksize)) >= 0) {
	            int		(*it)[2] = indtab ;
	            cint	ris = risize ;
	            if ((rs = mkind(rlp,kstab,it,ris)) >= 0) {
	                if ((rs = u_write(fd,indtab,risize)) >= 0) {
			    wlen += rs ;
	                    rs = u_write(fd,kstab,sksize) ;
			    wlen += rs ;
	                }
	            }
	        }
	        rs1 = libmem.free(kstab) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (memory allocation) */
	    rs1 = libmem.free(indtab) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (memory allocation) */
	return (rs >= 0) ? wlen : rs ;
}
/* end method (dbmake::wrfilekeys) */

/* make the value-string table */
int dbmake::wrfilevals() noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if (char *vstab ; (rs = libmem.mall(svsize,&vstab)) >= 0) {
	    if ((rs = strtab_strmk(vlp,vstab,svsize)) >= 0) {
	        rs = u_write(fd,vstab,svsize) ;
		wlen += rs ;
	    }
	    rs1 = libmem.free(vstab) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (value string-table) */
	return (rs >= 0) ? wlen : rs ;
}
/* end method (dbmake::wrfilevals) */

/* make the (only) index for this file */
int dbmake::mkind(vecobj *rp,cc *skey,rt_t it,int itsz) noex {
	cint		ns = nshift ;
	int		rs = SR_FAULT ;
	int		n = 0 ; /* ¥ GCC false complaint */
	if (it) {
	    int		sz ;
	    if ((rs = rp->count) >= 0) {
	        n = nextpowtwo(rs) ;
	        if (n < 4) n = 4 ;
	        sz = (n * 2 * szof(uint)) ;
	        if (sz <= itsz) {
		    void	*vp{} ;
	            uint	khash ;
	            int		ri, hi, ki ;
	            int		v ;
	            int		kl ;
	            int		f ;
	            cchar	*kp ;
	            memset(it,0,sz) ;
	            ri = 1 ;
	            for (int i = 0 ; rp->get(i,&vp) >= 0 ; i += 1) {
	                record	*rep = recordp(vp) ;
	                int	c = 0 ;
	                kp = charp(skey + rep->key) ;
	                kl = lenstr(kp) ;
	                khash = hash_elf(kp,kl) ;
	                hi = hashindex(khash,n) ;
	                if ((ropts & DBMAKE_OSEC) && (it[hi][0] != 0)) {
	                    while ((v = it[hi][0]) != 0) {
	                        ki = rectab[v][0] ;
	                        f = (strcmp(kp,(skey + ki)) == 0) ;
	                        if (! f) break ;
	                        if (ropts & DBMAKE_ORANDLC) {
	                            khash = randlc(khash + c) ;
	                        } else {
	                            khash = rotr(khash,ns) ;
				    khash += c ;
	                        }
	                        hi = hashindex(khash,n) ;
	                        c += 1 ;
	                    } /* end while */
	                } /* end if (secondary hash on collision) */
	                if (it[hi][0] != 0) {
	                    int		lhi ;
	                    while (it[hi][1] != 0) {
	                        hi = it[hi][1] ;
	                    }
	                    lhi = hi ;	/* save last hash-index value */
	                    hi = hashindex((hi + 1),n) ;
	                    while (it[hi][0] != 0) {
	                        hi = hashindex((hi + 1),n) ;
	                    } /* end while */
	                    it[lhi][1] = hi ; /* update the previous slot */
	                } /* end if (got a hash collision) */
	                it[hi][0] = ri ;
	                it[hi][1] = 0 ;
	                ri += 1 ;
	            } /* end for (looping through records) */
	        } else {
	            rs = SR_OVERFLOW ;
	        }
	    } /* end if (vecobj_count) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end method (dbmake::mkind) */

vars::operator int () noex {
    	mailaliaslen = mailvalue.mailaliaslen ;
	return SR_OK ;
}
/* end method (vars::operator) */


