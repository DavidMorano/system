
/* parse an access server file */
static int acctab_fileparse(acctab *op,int fi) noex {
	PARTTYPE	netgroup ;
	bfile		file, *fp = &file ;
	ACCTAB_FI	*fep ;
	acctab_ent	se ;
	USTAT		sb ;
	field		fsb ;
	int		rs ;
	int		c ;
	int		cl, len ;
	int		line = 0 ;
	int		c_added = 0 ;
	cchar	*cp ;
	char		lbuf[LINEBUFLEN + 1] ;

	void		*vp{} ;
	rs = vecobj_get(op->flp,fi,&vp) ;
	fep = filep(vp) ;

	if (rs < 0)
	    goto ret0 ;

	rs = bopen(fp,fep->fname,"r",0664) ;

	if (rs < 0)
	    goto ret0 ;

	rs = bcontrol(fp,BC_STAT,&sb) ;
	if (rs < 0)
	    goto done ;

/* have we already parsed this one? */

	rs = SR_OK ;
	if (fep->mtime >= sb.st_mtime)
	    goto done ;

	fep->dev = sb.st_dev ;
	fep->ino = sb.st_ino ;
	fep->mtime = sb.st_mtime ;
	fep->len = sb.st_size ;

/* start processing the configuration file */

	part_compile(&netgroup,ACCTAB_DEFNETGROUP,-1) ;

	c_added = 0 ;
	while ((rs = breadln(fp,lbuf,LINEBUFLEN)) > 0) {
	    len = rs ;

	    line += 1 ;
	    if (len == 1) continue ;	/* blank line */

	    if (lbuf[len - 1] != '\n') {
	        while ((c = bgetc(fp)) >= 0) {
	            if (c == '\n') break ;
	        }
	        continue ;
	    }

	    cp = lbuf ;
	    cl = len ;
	    lbuf[--cl] = '\0' ;
	    while (CHAR_ISWHITE(*cp)) {
	        cp += 1 ;
	        cl -= 1 ;
	    }

	    if ((*cp == '\0') || (*cp == '#')) continue ;

	    if ((rs = field_start(&fsb,cp,cl)) >= 0) {
	        int	fl ;
	        cchar	*fp ;
	        if ((fl = field_get(&fsb,gterms,&fp)) > 0) {

/* we have something! */

	            entry_start(&se) ;

	            se.fi = fi ;
	            if (fsb.term == ':') {
	                part_finish(&netgroup) ;
	                rs = part_compile(&netgroup,fp,fl) ;
	                fl = field_get(&fsb,aterms,&fp) ;
	            } /* end if */

/* see if there is a machine on this same line */

	            if (fl > 0) {

	                part_copy(&se.netgroup,&netgroup) ;

/* there is something else on this line */

	                rs = part_compile(&se.machine,fp,fl) ;

	                if (rs == SR_NOMEM)
	                    goto badpart ;

	                if ((fl = field_get(&fsb,aterms,&fp)) > 0) {

	                    rs = part_compile(&se.username,fp,fl) ;

	                    if (rs == SR_NOMEM)
	                        goto badpart ;

	                }

	                if ((fl = field_get(&fsb,aterms,&fp)) > 0) {

	                    rs = part_compile(&se.password,fp,fl) ;

	                    if (rs == SR_NOMEM)
	                        break ;

	                }

	                rs = acctab_entadd(op,&se) ;

	                if (rs >= 0)
	                    c_added += 1 ;

	            } /* end if */

	        } /* end if (got something) */

	        field_finish(&fsb) ;
	    } /* end if (field) */

	    if (rs < 0) break ;
	} /* end while (reading lines) */

	part_finish(&netgroup) ;

	if (rs < 0)
	    goto bad4 ;

/* done with configuration file processing */
done:
	bclose(fp) ;

ret0:
	return (rs >= 0) ? c_added : rs ;

/* handle bad things */
bad4:
	acctab_filedump(op,fi) ;

badpart:
	part_finish(&netgroup) ;

	entry_finish(&se) ;

	goto done ;
}
/* end subroutine (acctab_fileparse) */

