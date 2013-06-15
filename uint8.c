/*-------------------------------------------------------------------------
 *
 * uint8.c
 *	  Unsigned 64-bit integer operations
 *
 * Portions Copyright (c) 1996-2013, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include <ctype.h>
#include <limits.h>
#include <math.h>

#include "funcapi.h"
#include "libpq/pqformat.h"
#include "utils/int8.h"
#include "utils/builtins.h"

#include "declare.h"
#include "uints_numutils.h"
#include "uints_fmgr.h"

typedef struct
{
	uint64		current;
	uint64		finish;
	uint64		step;
} generate_series_fctx;


/***********************************************************************
 **
 **		Routines for 64-bit integers.
 **
 ***********************************************************************/

/*----------------------------------------------------------
 * Formatting and conversion routines.
 *---------------------------------------------------------*/

#define MAXINT8LEN 25

/* uint8in()
 */
DECLARE(uint8in)
Datum
uint8in(PG_FUNCTION_ARGS)
{
	char	   *str = PG_GETARG_CSTRING(0);

	PG_RETURN_UINT64(pg_atou8(str, '\0'));
}

/* uint8out()
 */
DECLARE(uint8out)
Datum
uint8out(PG_FUNCTION_ARGS)
{
	uint64		val = PG_GETARG_UINT64(0);
	char		buf[MAXINT8LEN + 1];
	char	   *result;

	pg_u8toa(val, buf);
	result = pstrdup(buf);
	PG_RETURN_CSTRING(result);
}

/*
 *		uint8recv			- converts external binary format to uint8
 */
DECLARE(uint8recv)
Datum
uint8recv(PG_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) PG_GETARG_POINTER(0);

	PG_RETURN_UINT64((uint64) pq_getmsgint64(buf));
}

/*
 *		uint8send			- converts uint8 to binary format
 */
DECLARE(uint8send)
Datum
uint8send(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendint64(&buf, (uint64)arg1);
	PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}


/*----------------------------------------------------------
 *	Relational operators for int8s, including cross-data-type comparisons.
 *---------------------------------------------------------*/

/* uint8relop()
 * Is val1 relop val2?
 */
DECLARE(uint8eq)
Datum
uint8eq(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 == val2);
}

DECLARE(uint8ne)
Datum
uint8ne(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 != val2);
}

DECLARE(uint8lt)
Datum
uint8lt(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 < val2);
}

DECLARE(uint8gt)
Datum
uint8gt(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 > val2);
}

DECLARE(uint8le)
Datum
uint8le(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 <= val2);
}

DECLARE(uint8ge)
Datum
uint8ge(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 >= val2);
}

/* int84relop()
 * Is 64-bit val1 relop 32-bit val2?
 */
DECLARE(uint84eq)
Datum
uint84eq(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint32		val2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(val1 == val2);
}

DECLARE(uint84ne)
Datum
uint84ne(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint32		val2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(val1 != val2);
}

DECLARE(uint84lt)
Datum
uint84lt(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint32		val2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(val1 < val2);
}

DECLARE(uint84gt)
Datum
uint84gt(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint32		val2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(val1 > val2);
}

DECLARE(uint84le)
Datum
uint84le(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint32		val2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(val1 <= val2);
}

DECLARE(uint84ge)
Datum
uint84ge(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint32		val2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(val1 >= val2);
}

/* int48relop()
 * Is 32-bit val1 relop 64-bit val2?
 */
DECLARE(uint48eq)
Datum
uint48eq(PG_FUNCTION_ARGS)
{
	uint32		val1 = PG_GETARG_UINT32(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 == val2);
}

DECLARE(uint48ne)
Datum
uint48ne(PG_FUNCTION_ARGS)
{
	uint32		val1 = PG_GETARG_UINT32(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 != val2);
}

DECLARE(uint48lt)
Datum
uint48lt(PG_FUNCTION_ARGS)
{
	uint32		val1 = PG_GETARG_UINT32(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 < val2);
}

DECLARE(uint48gt)
Datum
uint48gt(PG_FUNCTION_ARGS)
{
	uint32		val1 = PG_GETARG_UINT32(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 > val2);
}

DECLARE(uint48le)
Datum
uint48le(PG_FUNCTION_ARGS)
{
	uint32		val1 = PG_GETARG_UINT32(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 <= val2);
}

DECLARE(uint48ge)
Datum
uint48ge(PG_FUNCTION_ARGS)
{
	uint32		val1 = PG_GETARG_UINT32(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 >= val2);
}


/* int82relop()
 * Is 64-bit val1 relop 16-bit val2?
 */
DECLARE(uint82eq)
Datum
uint82eq(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint16		val2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(val1 == val2);
}

DECLARE(uint82ne)
Datum
uint82ne(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint16		val2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(val1 != val2);
}

DECLARE(uint82lt)
Datum
uint82lt(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint16		val2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(val1 < val2);
}

DECLARE(uint82gt)
Datum
uint82gt(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint16		val2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(val1 > val2);
}

DECLARE(uint82le)
Datum
uint82le(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint16		val2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(val1 <= val2);
}

DECLARE(uint82ge)
Datum
uint82ge(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint16		val2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(val1 >= val2);
}

/* int28relop()
 * Is 16-bit val1 relop 64-bit val2?
 */
DECLARE(uint28eq)
Datum
uint28eq(PG_FUNCTION_ARGS)
{
	uint16		val1 = PG_GETARG_UINT16(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 == val2);
}

DECLARE(uint28ne)
Datum
uint28ne(PG_FUNCTION_ARGS)
{
	uint16		val1 = PG_GETARG_UINT16(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 != val2);
}

DECLARE(uint28lt)
Datum
uint28lt(PG_FUNCTION_ARGS)
{
	uint16		val1 = PG_GETARG_UINT16(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 < val2);
}

DECLARE(uint28gt)
Datum
uint28gt(PG_FUNCTION_ARGS)
{
	uint16		val1 = PG_GETARG_UINT16(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 > val2);
}

DECLARE(uint28le)
Datum
uint28le(PG_FUNCTION_ARGS)
{
	uint16		val1 = PG_GETARG_UINT16(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 <= val2);
}

DECLARE(uint28ge)
Datum
uint28ge(PG_FUNCTION_ARGS)
{
	uint16		val1 = PG_GETARG_UINT16(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 >= val2);
}


/*----------------------------------------------------------
 *	Arithmetic operators on 64-bit integers.
 *---------------------------------------------------------*/

DECLARE(uint8up)
Datum
uint8up(PG_FUNCTION_ARGS)
{
	uint64		arg = PG_GETARG_UINT64(0);

	PG_RETURN_UINT64(arg);
}

DECLARE(uint8pl)
Datum
uint8pl(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint64		arg2 = PG_GETARG_UINT64(1);

	if (arg1 > ULONG_LONG_MAX - arg2)
		report_out_of_range();

	PG_RETURN_UINT64(arg1 + arg2);
}

DECLARE(uint8mi)
Datum
uint8mi(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint64		arg2 = PG_GETARG_UINT64(1);

	if (arg2 > arg1)
		report_out_of_range();

	PG_RETURN_UINT64(arg1 - arg2);
}

DECLARE(uint8mul)
Datum
uint8mul(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint64		arg2 = PG_GETARG_UINT64(1);
	uint64		result;

	result = arg1 * arg2;

	/*
	 * Overflow check.	We basically check to see if result / arg2 gives arg1
	 * again.  This may fail when arg2 is 0, but thenm ther can't be an overflow.
	 *
	 * Since the division is likely much more expensive than the actual
	 * multiplication, we'd like to skip it where possible.  The best bang for
	 * the buck seems to be to check whether both inputs are in the uint32
	 * range; if so, no overflow is possible.
	 */
	if ( arg2 != 0 && (arg1 > UINT_MAX || arg2 > UINT_MAX) && result / arg2 != arg1)
		report_out_of_range();

	PG_RETURN_UINT64(result);
}

DECLARE(uint8div)
Datum
uint8div(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint64		arg2 = PG_GETARG_UINT64(1);

	if (arg2 == 0)
		report_division_by_zero();

	/* No overflow is possible */
	PG_RETURN_UINT64(arg1 / arg2);
}


/* int8mod()
 * Modulo operation.
 */
DECLARE(uint8mod)
Datum
uint8mod(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint64		arg2 = PG_GETARG_UINT64(1);

	if (arg2 == 0)
		report_division_by_zero();

	/* No overflow is possible */
	PG_RETURN_UINT64(arg1 % arg2);
}


DECLARE(uint8inc)
Datum
uint8inc(PG_FUNCTION_ARGS)
{
	/*
	 * When int8 is pass-by-reference, we provide this special case to avoid
	 * palloc overhead for COUNT(): when called as an aggregate, we know that
	 * the argument is modifiable local storage, so just update it in-place.
	 * (If int8 is pass-by-value, then of course this is useless as well as
	 * incorrect, so just ifdef it out.)
	 */
#ifndef USE_FLOAT8_BYVAL		/* controls int8 too */
	if (AggCheckCallContext(fcinfo, NULL))
	{
		uint64	   *arg = (uint64 *) PG_GETARG_POINTER(0);
		uint64		result;

		result = *arg + 1;
		/* Overflow check */
		if (result < 0 && *arg > 0)
			ereport(ERROR,
					(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
					 errmsg("bigint out of range")));

		*arg = result;
		PG_RETURN_POINTER(arg);
	}
	else
#endif
	{
		/* Not called as an aggregate, so just do it the dumb way */
		uint64		arg = PG_GETARG_UINT64(0);
		uint64		result;

		result = arg + 1;
		/* Overflow check */
		if (result < arg)
			report_out_of_range();

		PG_RETURN_UINT64(result);
	}
}

/*
 * These functions are exactly like int8inc but are used for aggregates that
 * count only non-null values.	Since the functions are declared strict,
 * the null checks happen before we ever get here, and all we need do is
 * increment the state value.  We could actually make these pg_proc entries
 * point right at int8inc, but then the opr_sanity regression test would
 * complain about mismatched entries for a built-in function.
 */

DECLARE(uint8inc_any)
Datum
uint8inc_any(PG_FUNCTION_ARGS)
{
	return int8inc(fcinfo);
}

DECLARE(uint8inc_float8_float8)
Datum
uint8inc_float8_float8(PG_FUNCTION_ARGS)
{
	return int8inc(fcinfo);
}


DECLARE(uint8larger)
Datum
uint8larger(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint64		arg2 = PG_GETARG_UINT64(1);
	uint64		result;

	result = ((arg1 > arg2) ? arg1 : arg2);

	PG_RETURN_UINT64(result);
}

DECLARE(uint8smaller)
Datum
uint8smaller(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint64		arg2 = PG_GETARG_UINT64(1);
	uint64		result;

	result = ((arg1 < arg2) ? arg1 : arg2);

	PG_RETURN_UINT64(result);
}

DECLARE(uint84pl)
Datum
uint84pl(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	if (arg1 > ULONG_LONG_MAX - arg2)
		report_out_of_range();

	PG_RETURN_UINT64(arg1 + arg2);
}

DECLARE(uint84mi)
Datum
uint84mi(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	if (arg2 > arg1)
		report_out_of_range();

	PG_RETURN_UINT64(arg1 - arg2);
}

DECLARE(uint84mul)
Datum
uint84mul(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint32		arg2 = PG_GETARG_UINT32(1);
	uint64		result;

	result = arg1 * arg2;

	if (arg2 != 0 && (arg1 > UINT_MAX || arg2 > UINT_MAX) && result / arg2 != arg1)
		report_out_of_range();

	PG_RETURN_UINT64(result);
}

DECLARE(uint84div)
Datum
uint84div(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	if (arg2 == 0)
		report_division_by_zero();

	PG_RETURN_UINT64(arg1 / arg2);
}

DECLARE(uint48pl)
Datum
uint48pl(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint64		arg2 = PG_GETARG_UINT64(1);

	if (arg2 > ULONG_LONG_MAX - arg1)
		report_out_of_range();

	PG_RETURN_UINT64(arg1 + arg2);
}

DECLARE(uint48mi)
Datum
uint48mi(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint64		arg2 = PG_GETARG_UINT64(1);

	if (arg2 > arg1)
		report_out_of_range();

	PG_RETURN_UINT64(arg1 - arg2);
}

DECLARE(uint48mul)
Datum
uint48mul(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint64		arg2 = PG_GETARG_UINT64(1);
	uint64		result;

	result = arg1 * arg2;

	if (arg2 != 0 && arg2 > UINT_MAX && result / arg2 != arg1)
		report_out_of_range();

	PG_RETURN_UINT64(result);
}

DECLARE(uint48div)
Datum
uint48div(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint64		arg2 = PG_GETARG_UINT64(1);

	if (arg2 == 0)
		report_division_by_zero();

	/* No overflow is possible */
	PG_RETURN_UINT64((uint64) arg1 / arg2);
}

DECLARE(uint82pl)
Datum
uint82pl(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	if (arg1 > ULONG_LONG_MAX - arg2)
		report_out_of_range();

	PG_RETURN_UINT64(arg1 + arg2);
}

DECLARE(uint82mi)
Datum
uint82mi(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	if (arg2 - arg1)
		report_out_of_range();

	PG_RETURN_UINT64(arg1 - arg2);
}

DECLARE(uint82mul)
Datum
uint82mul(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint16		arg2 = PG_GETARG_UINT16(1);
	uint64		result;

	result = arg1 * arg2;

	if (arg2 != 0 && arg1 > UINT_MAX && result / arg2 != arg1)
		report_out_of_range();

	PG_RETURN_UINT64(result);
}

DECLARE(uint82div)
Datum
uint82div(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	if (arg2 == 0)
		report_division_by_zero();

	PG_RETURN_UINT64(arg1 / arg2);
}
# if 0

DECLARE(xxx)
Datum
uint28pl(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint64		arg2 = PG_GETARG_UINT64(1);
	uint64		result;

	result = arg1 + arg2;

	/*
	 * Overflow check.	If the inputs are of different signs then their sum
	 * cannot overflow.  If the inputs are of the same sign, their sum had
	 * better be that sign too.
	 */
	if (SAMESIGN(arg1, arg2) && !SAMESIGN(result, arg1))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	PG_RETURN_UINT64(result);
}

DECLARE(xxx)
Datum
uint28mi(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint64		arg2 = PG_GETARG_UINT64(1);
	uint64		result;

	result = arg1 - arg2;

	/*
	 * Overflow check.	If the inputs are of the same sign then their
	 * difference cannot overflow.	If they are of different signs then the
	 * result should be of the same sign as the first input.
	 */
	if (!SAMESIGN(arg1, arg2) && !SAMESIGN(result, arg1))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	PG_RETURN_UINT64(result);
}

DECLARE(xxx)
Datum
uint28mul(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint64		arg2 = PG_GETARG_UINT64(1);
	uint64		result;

	result = arg1 * arg2;

	/*
	 * Overflow check.	We basically check to see if result / arg2 gives arg1
	 * again.  There is one case where this fails: arg2 = 0 (which cannot
	 * overflow).
	 *
	 * Since the division is likely much more expensive than the actual
	 * multiplication, we'd like to skip it where possible.  The best bang for
	 * the buck seems to be to check whether both inputs are in the uint32
	 * range; if so, no overflow is possible.
	 */
	if (arg2 != (uint64) ((uint32) arg2) &&
		result / arg2 != arg1)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	PG_RETURN_UINT64(result);
}

DECLARE(xxx)
Datum
uint28div(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint64		arg2 = PG_GETARG_UINT64(1);

	if (arg2 == 0)
	{
		ereport(ERROR,
				(errcode(ERRCODE_DIVISION_BY_ZERO),
				 errmsg("division by zero")));
		/* ensure compiler realizes we mustn't reach the division (gcc bug) */
		PG_RETURN_NULL();
	}

	/* No overflow is possible */
	PG_RETURN_UINT64((uint64) arg1 / arg2);
}

/* Binary arithmetics
 *
 *		int8and		- returns arg1 & arg2
 *		int8or		- returns arg1 | arg2
 *		int8xor		- returns arg1 # arg2
 *		int8not		- returns ~arg1
 *		int8shl		- returns arg1 << arg2
 *		int8shr		- returns arg1 >> arg2
 */

DECLARE(xxx)
Datum
uint8and(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint64		arg2 = PG_GETARG_UINT64(1);

	PG_RETURN_UINT64(arg1 & arg2);
}

DECLARE(xxx)
Datum
uint8or(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint64		arg2 = PG_GETARG_UINT64(1);

	PG_RETURN_UINT64(arg1 | arg2);
}

DECLARE(xxx)
Datum
uint8xor(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint64		arg2 = PG_GETARG_UINT64(1);

	PG_RETURN_UINT64(arg1 ^ arg2);
}

DECLARE(xxx)
Datum
uint8not(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);

	PG_RETURN_UINT64(~arg1);
}

DECLARE(xxx)
Datum
uint8shl(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_UINT64(arg1 << arg2);
}

DECLARE(xxx)
Datum
uint8shr(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_UINT64(arg1 >> arg2);
}

/*----------------------------------------------------------
 *	Conversion operators.
 *---------------------------------------------------------*/

DECLARE(xxx)
Datum
uint48(PG_FUNCTION_ARGS)
{
	uint32		arg = PG_GETARG_UINT32(0);

	PG_RETURN_UINT64((uint64) arg);
}

DECLARE(xxx)
Datum
uint84(PG_FUNCTION_ARGS)
{
	uint64		arg = PG_GETARG_UINT64(0);
	uint32		result;

	result = (uint32) arg;

	/* Test for overflow by reverse-conversion. */
	if ((uint64) result != arg)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));

	PG_RETURN_INT32(result);
}

DECLARE(xxx)
Datum
uint28(PG_FUNCTION_ARGS)
{
	uint16		arg = PG_GETARG_UINT16(0);

	PG_RETURN_UINT64((uint64) arg);
}

DECLARE(xxx)
Datum
uint82(PG_FUNCTION_ARGS)
{
	uint64		arg = PG_GETARG_UINT64(0);
	uint16		result;

	result = (uint16) arg;

	/* Test for overflow by reverse-conversion. */
	if ((uint64) result != arg)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("smallint out of range")));

	PG_RETURN_INT16(result);
}

DECLARE(xxx)
Datum
i8tod(PG_FUNCTION_ARGS)
{
	uint64		arg = PG_GETARG_UINT64(0);
	float8		result;

	result = arg;

	PG_RETURN_FLOAT8(result);
}

/* dtoi8()
 * Convert float8 to 8-byte integer.
 */
DECLARE(xxx)
Datum
dtoi8(PG_FUNCTION_ARGS)
{
	float8		arg = PG_GETARG_FLOAT8(0);
	uint64		result;

	/* Round arg to nearest integer (but it's still in float form) */
	arg = rint(arg);

	/*
	 * Does it fit in an uint64?  Avoid assuming that we have handy constants
	 * defined for the range boundaries, instead test for overflow by
	 * reverse-conversion.
	 */
	result = (uint64) arg;

	if ((float8) result != arg)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));

	PG_RETURN_UINT64(result);
}

DECLARE(xxx)
Datum
i8tof(PG_FUNCTION_ARGS)
{
	uint64		arg = PG_GETARG_UINT64(0);
	float4		result;

	result = arg;

	PG_RETURN_FLOAT4(result);
}

/* ftoi8()
 * Convert float4 to 8-byte integer.
 */
DECLARE(xxx)
Datum
ftoi8(PG_FUNCTION_ARGS)
{
	float4		arg = PG_GETARG_FLOAT4(0);
	uint64		result;
	float8		darg;

	/* Round arg to nearest integer (but it's still in float form) */
	darg = rint(arg);

	/*
	 * Does it fit in an uint64?  Avoid assuming that we have handy constants
	 * defined for the range boundaries, instead test for overflow by
	 * reverse-conversion.
	 */
	result = (uint64) darg;

	if ((float8) result != darg)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));

	PG_RETURN_UINT64(result);
}

DECLARE(xxx)
Datum
i8tooid(PG_FUNCTION_ARGS)
{
	uint64		arg = PG_GETARG_UINT64(0);
	Oid			result;

	result = (Oid) arg;

	/* Test for overflow by reverse-conversion. */
	if ((uint64) result != arg)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("OID out of range")));

	PG_RETURN_OID(result);
}

DECLARE(xxx)
Datum
oidtoi8(PG_FUNCTION_ARGS)
{
	Oid			arg = PG_GETARG_OID(0);

	PG_RETURN_UINT64((uint64) arg);
}

/*
 * non-persistent numeric series generator
 */
DECLARE(xxx)
Datum
generate_series_int8(PG_FUNCTION_ARGS)
{
	return generate_series_step_int8(fcinfo);
}

DECLARE(xxx)
Datum
generate_series_step_int8(PG_FUNCTION_ARGS)
{
	FuncCallContext *funcctx;
	generate_series_fctx *fctx;
	uint64		result;
	MemoryContext oldcontext;

	/* stuff done only on the first call of the function */
	if (SRF_IS_FIRSTCALL())
	{
		uint64		start = PG_GETARG_UINT64(0);
		uint64		finish = PG_GETARG_UINT64(1);
		uint64		step = 1;

		/* see if we were given an explicit step size */
		if (PG_NARGS() == 3)
			step = PG_GETARG_UINT64(2);
		if (step == 0)
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					 errmsg("step size cannot equal zero")));

		/* create a function context for cross-call persistence */
		funcctx = SRF_FIRSTCALL_INIT();

		/*
		 * switch to memory context appropriate for multiple function calls
		 */
		oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

		/* allocate memory for user context */
		fctx = (generate_series_fctx *) palloc(sizeof(generate_series_fctx));

		/*
		 * Use fctx to keep state from call to call. Seed current with the
		 * original start value
		 */
		fctx->current = start;
		fctx->finish = finish;
		fctx->step = step;

		funcctx->user_fctx = fctx;
		MemoryContextSwitchTo(oldcontext);
	}

	/* stuff done on every call of the function */
	funcctx = SRF_PERCALL_SETUP();

	/*
	 * get the saved state and use current as the result for this iteration
	 */
	fctx = funcctx->user_fctx;
	result = fctx->current;

	if ((fctx->step > 0 && fctx->current <= fctx->finish) ||
		(fctx->step < 0 && fctx->current >= fctx->finish))
	{
		/* increment current in preparation for next iteration */
		fctx->current += fctx->step;

		/* if next-value computation overflows, this is the final result */
		if (SAMESIGN(result, fctx->step) && !SAMESIGN(result, fctx->current))
			fctx->step = 0;

		/* do when there is more left to send */
		SRF_RETURN_NEXT(funcctx, Int64GetDatum(result));
	}
	else
		/* do when there is no more left */
		SRF_RETURN_DONE(funcctx);
}
#endif
