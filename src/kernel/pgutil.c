/*
 * $Id$
 * 
 *                This source code is part of
 * 
 *                 G   R   O   M   A   C   S
 * 
 *          GROningen MAchine for Chemical Simulations
 * 
 *                        VERSION 3.3.2
 * Written by David van der Spoel, Erik Lindahl, Berk Hess, and others.
 * Copyright (c) 1991-2000, University of Groningen, The Netherlands.
 * Copyright (c) 2001-2007, The GROMACS development team,
 * check out http://www.gromacs.org for more information.

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * If you want to redistribute modifications, please consider that
 * scientific software is very special. Version control is crucial -
 * bugs must be traceable. We will be happy to consider code for
 * inclusion in the official distribution, but derived work must not
 * be called official GROMACS. Details are found in the README & COPYING
 * files - if they are missing, get the official version at www.gromacs.org.
 * 
 * To help us fund GROMACS development, we humbly ask that you cite
 * the papers on the package - you can find them in the top README file.
 * 
 * For more info, check our website at http://www.gromacs.org
 * 
 * And Hey:
 * Groningen Machine for Chemical Simulation
 */
/* This file is completely threadsafe - keep it that way! */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "string2.h"
#include "pgutil.h"
#include "string.h"

void atom_not_found(int fatal_errno,const char *file,int line,
		    char *atomname,int resnr,
		    char *bondtype,bool bDontQuit)
{
  if (strcmp(bondtype,"check") != 0) {
    if (bDontQuit) {
      fprintf(stderr,
	      "WARNING: atom %s not found in residue %d while adding %s\n",
	      atomname,resnr,bondtype);
    } else {
      gmx_fatal(fatal_errno,file,line,
		"Atom %s not found in residue %d while adding %s\n",
		atomname,resnr,bondtype);
    }
  }
}
	
atom_id search_atom(char *type,int start,int natoms,t_atom at[],char **anm[],
		    char *bondtype,bool bDontQuit)
{
  int     i,resnr=-1;
  bool    bPrevious,bNext;

  bPrevious = (strchr(type,'-') != NULL);
  bNext     = (strchr(type,'+') != NULL);

  if (!bPrevious) {
    resnr = at[start].resnr;
    if (bNext) {
      /* The next residue */
      type++;
      while ((start<natoms) && (at[start].resnr == resnr))
	start++;
      if (start < natoms)
	resnr = at[start].resnr;
    }
    
    for(i=start; (i<natoms) && (bNext || (at[i].resnr == resnr)); i++) {
      if (strcasecmp(type,*(anm[i]))==0)
	return (atom_id) i;
    }
    if (!(bNext && at[start].resnr==at[natoms-1].resnr))
      atom_not_found(FARGS,type,at[start].resnr+1,bondtype,bDontQuit);
  }
  else {
    /* The previous residue */
    type++;
    if (start > 0)
      resnr = at[start-1].resnr;
    for(i=start-1; (i>=0) /*&& (at[i].resnr == resnr)*/; i--)
      if (strcasecmp(type,*(anm[i]))==0)
	return (atom_id) i;
    if (start > 0)
      atom_not_found(FARGS,type,at[start].resnr+1,bondtype,bDontQuit);
  }
  return NO_ATID;
}

void set_at(t_atom *at,real m,real q,int type,int resnr)
{
  at->m=m;
  at->q=q;
  at->type=type;
  at->resnr=resnr;
}
