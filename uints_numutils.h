/*-------------------------------------------------------------------------
 *
 * uint_numutils.h
 *	  Declarations for unsigned integer string conversion.
 *
 *
 * Portions Copyright (c) 1996-2013, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef UINT_NUMUITLS_H
#define UINT_NUMUITLS_H

extern uint8 pg_atou1(char *s, int c);
extern uint16 pg_atou2(char *s, int c);
extern uint32 pg_atou4(char *s, int c);
extern uint64 pg_atou8(char *s, int c);

extern void pg_u1toa(uint8 i, char* a);
extern void pg_u2toa(uint16 i, char* a);
extern void pg_u4toa(uint32 i, char* a);
extern void pg_u8toa(uint64 i, char* a);


#endif
