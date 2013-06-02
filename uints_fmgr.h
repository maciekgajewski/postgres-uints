/*-------------------------------------------------------------------------
 *
 * uints_fmgr.h
 *	  Extension of fmgr.h
 *	  Adds support for uinsigned integers.
 *
 * Portions Copyright (c) 1996-2013, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef UINTS_FMGR_H
#define UINTS_FMGR_H

/* Macros for returning results */

#define PG_RETURN_UINT16(x)	 return UInt16GetDatum(x)

#endif
