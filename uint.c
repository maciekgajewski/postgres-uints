/*-------------------------------------------------------------------------
 *
 * int.c
 *	  Functions for the built-in integer types (except int8).
 *
 * Portions Copyright (c) 1996-2013, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 *-------------------------------------------------------------------------
 */
/*
 * OLD COMMENTS
 *		I/O routines:
 *		 uint2in, uint2out, uint2recv, uint2send
 *		 uint4in, uint4out, uint4recv, uint4send
 *		 uint2vectorin, uint2vectorout, uint2vectorrecv, uint2vectorsend
 *		Boolean operators:
 *		 uinteq, uintne, uintlt, uintle, uintgt, uintge
 *		Arithmetic operators:
 *		 uintpl, uintmi, uint4mul, uintdiv
 *
 *		Arithmetic operators:
 *		 uintmod
 */
#include "postgres.h"

#include <ctype.h>
#include <limits.h>

#include "catalog/pg_type.h"
#include "funcapi.h"
#include "libpq/pqformat.h"
#include "utils/array.h"
#include "utils/builtins.h"
#include "access/hash.h"

#include "uints_numutils.h"
#include "uints_fmgr.h"


#define SAMESIGN(a,b)	(((a) < 0) == ((b) < 0))

typedef struct
{
	uint32		current;
	uint32		finish;
	uint32		step;
} generate_series_fctx;


#define DECLARE(fun) \
	PG_FUNCTION_INFO_V1(fun);\
	Datum fun(PG_FUNCTION_ARGS);

/*****************************************************************************
 *	 USER I/O ROUTINES														 *
 *****************************************************************************/

/*
 *		uint2in			- converts "num" to short
 */
DECLARE(uint2in)
Datum
uint2in(PG_FUNCTION_ARGS)
{
	char	   *num = PG_GETARG_CSTRING(0);

	PG_RETURN_UINT16(pg_atou2(num, '\0'));
}

/*
 *		uint2out			- converts short to "num"
 */
DECLARE(uint2out)
Datum
uint2out(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	char	   *result = (char *) palloc(6);	/* 5 digits, '\0' */

	pg_u2toa(arg1, result);
	PG_RETURN_CSTRING(result);
}

/*
 *		uint2recv			- converts external binary format to uint2
 */
DECLARE(uint2recv)
Datum
uint2recv(PG_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) PG_GETARG_POINTER(0);

	PG_RETURN_UINT16((uint16) pq_getmsgint(buf, sizeof(uint16)));
}

/*
 *		uint2send			- converts uint2 to binary format
 */
DECLARE(uint2send)
Datum
uint2send(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendint(&buf, (int)arg1, sizeof(uint16));
	PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/*
 *		uint4in			- converts "num" to uint4
 */
DECLARE(uint4in)
Datum
uint4in(PG_FUNCTION_ARGS)
{
	char	   *num = PG_GETARG_CSTRING(0);

	PG_RETURN_UINT32(pg_atou4(num, '\0'));
}

/*
 *		uint4out			- converts uint4 to "num"
 */
DECLARE(uint4out)
Datum
uint4out(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	char	   *result = (char *) palloc(11);	/* 10 digits, '\0' */

	pg_u4toa(arg1, result);
	PG_RETURN_CSTRING(result);
}

/*
 *		uint4recv			- converts external binary format to uint4
 */
DECLARE(uint4recv)
Datum
uint4recv(PG_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) PG_GETARG_POINTER(0);

	PG_RETURN_UINT32((uint32) pq_getmsgint(buf, sizeof(uint32)));
}

/*
 *		uint4send			- converts uint4 to binary format
 */
DECLARE(uint4send)
Datum
uint4send(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendint(&buf, (int)arg1, sizeof(uint32));
	PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}


/*
 *		===================
 *		CONVERSION ROUTINES
 *		===================
 */

DECLARE(u2tou4)
Datum
u2tou4(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);

	PG_RETURN_UINT32((uint32) arg1);
}

DECLARE(u4tou2)
Datum
u4tou2(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);

	if (arg1 > USHRT_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("uint2 out of range")));

	PG_RETURN_UINT16((uint16) arg1);
}

/* Cast uint4 -> bool */
DECLARE(uint4_bool)
Datum
uint4_bool(PG_FUNCTION_ARGS)
{
	if (PG_GETARG_UINT32(0) == 0)
		PG_RETURN_BOOL(false);
	else
		PG_RETURN_BOOL(true);
}

/* Cast bool -> int4 */
DECLARE(bool_uint4)
Datum
bool_uint4(PG_FUNCTION_ARGS)
{
	if (PG_GETARG_BOOL(0) == false)
		PG_RETURN_UINT32(0);
	else
		PG_RETURN_UINT32(1);
}

/* Cast int4 -> uint2 */
DECLARE(i4tou2)
Datum
i4tou2(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);

	if (arg1 < 0 || arg1 > USHRT_MAX)
		ereport(ERROR,
			(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
			errmsg("value %d is out of range for type uint2", arg1)));

	PG_RETURN_UINT16((uint16)arg1);

}

/*
 *		============================
 *		COMPARISON OPERATOR ROUTINES
 *		============================
 */

/*
 *		uinteq			- returns 1 if arg1 == arg2
 *		uintne			- returns 1 if arg1 != arg2
 *		uintlt			- returns 1 if arg1 < arg2
 *		uintle			- returns 1 if arg1 <= arg2
 *		uintgt			- returns 1 if arg1 > arg2
 *		uintge			- returns 1 if arg1 >= arg2
 */

DECLARE(uint4eq)
Datum
uint4eq(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

DECLARE(uint4ne)
Datum
uint4ne(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

DECLARE(uint4lt)
Datum
uint4lt(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

DECLARE(uint4le)
Datum
uint4le(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

DECLARE(uint4gt)
Datum
uint4gt(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 > arg2);
}

DECLARE(uint4ge)
Datum
uint4ge(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 >= arg2);
}

DECLARE(uint2eq)
Datum
uint2eq(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

DECLARE(uint2ne)
Datum
uint2ne(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

DECLARE(uint2lt)
Datum
uint2lt(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

DECLARE(uint2le)
Datum
uint2le(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

DECLARE(uint2gt)
Datum
uint2gt(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 > arg2);
}

DECLARE(uint2ge)
Datum
uint2ge(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 >= arg2);
}

DECLARE(uint24eq)
Datum
uint24eq(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

DECLARE(uint24ne)
Datum
uint24ne(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

DECLARE(uint24lt)
Datum
uint24lt(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

DECLARE(uint24le)
Datum
uint24le(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

DECLARE(uint24gt)
Datum
uint24gt(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 > arg2);
}

DECLARE(uint24ge)
Datum
uint24ge(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 >= arg2);
}

DECLARE(uint42eq)
Datum
uint42eq(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

DECLARE(uint42ne)
Datum
uint42ne(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

DECLARE(uint42lt)
Datum
uint42lt(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

DECLARE(uint42le)
Datum
uint42le(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

DECLARE(uint42gt)
Datum
uint42gt(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 > arg2);
}

DECLARE(uint42ge)
Datum
uint42ge(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 >= arg2);
}

/*
 *		uint[24]pl		- returns arg1 + arg2
 *		uint[24]mi		- returns arg1 - arg2
 *		uint[24]mul		- returns arg1 * arg2
 *		uint[24]div		- returns arg1 / arg2
 */

static inline
void
report_out_of_range()
{
	ereport(ERROR,
			(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
			 errmsg("integer out of range")));
}

static inline
void
report_division_by_zero()
{
	ereport(ERROR,
			(errcode(ERRCODE_DIVISION_BY_ZERO),
			 errmsg("division by zero")));
}

DECLARE(uint4pl)
Datum
uint4pl(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	/* Overflow check */
	if (arg1 > (UINT_MAX - arg2))
		report_out_of_range();

	PG_RETURN_UINT32(arg1 + arg2);
}

DECLARE(uint4mi)
Datum
uint4mi(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	/* Overflow check */
	if (arg2 > arg1)
		report_out_of_range();

	PG_RETURN_UINT32(arg1 - arg2);
}

DECLARE(uint4mul)
Datum
uint4mul(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);
	uint32		result;

	result = arg1 * arg2;

	/* Overflow check */
	if (arg1 > USHRT_MAX && arg2 > USHRT_MAX && result / arg1 != arg2)
		report_out_of_range();

	PG_RETURN_UINT32(result);
}

DECLARE(uint4div)
Datum
uint4div(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	if (arg2 == 0)
		report_division_by_zero();

	/* No overflow is possible */

	PG_RETURN_UINT32(arg1 / arg2);
}

DECLARE(uint4inc)
Datum
uint4inc(PG_FUNCTION_ARGS)
{
	uint32		arg = PG_GETARG_UINT32(0);

	/* Overflow check */
	if (arg == UINT_MAX)
		report_out_of_range();

	PG_RETURN_UINT32(arg + 1);
}

DECLARE(uint2pl)
Datum
uint2pl(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	/* Overflow check */
	if (arg1 > USHRT_MAX - arg2)
		report_out_of_range();
	PG_RETURN_UINT16(arg1 + arg2);
}

DECLARE(uint2mi)
Datum
uint2mi(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	/* Overflow check */
	if (arg1 < arg2)
		report_out_of_range();

	PG_RETURN_UINT16(arg1 - arg2);
}

DECLARE(uint2mul)
Datum
uint2mul(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);
	uint16		result;

	result = arg1 * arg2;

	/* Overflow check */
	if (arg1 > UCHAR_MAX && arg2 > UCHAR_MAX && result / arg1 != arg2)
		report_out_of_range();

	PG_RETURN_UINT16(result);

}

DECLARE(uint2div)
Datum
uint2div(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	if (arg2 == 0)
		report_division_by_zero();


	/* No overflow is possible */
	PG_RETURN_UINT16(arg1 / arg2);
}

DECLARE(uint24pl)
Datum
uint24pl(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	/* Overflow check */
	if (arg2 > UINT_MAX - arg1)
		report_out_of_range();

	PG_RETURN_UINT32(arg1 + arg2);
}

DECLARE(uint24mi)
Datum
uint24mi(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_UINT32(arg1 - arg2);
}

DECLARE(uint24mul)
Datum
uint24mul(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint32		arg2 = PG_GETARG_UINT32(1);
	uint32		result;

	result = arg1 * arg2;

	/*
	 * Overflow check.	We basically check to see if result / arg2 gives arg1
	 * again.  There is one case where this fails: arg2 = 0 (which cannot
	 * overflow).
	 *
	 * Since the division is likely much more expensive than the actual
	 * multiplication, we'd like to skip it where possible.  The best bang for
	 * the buck seems to be to check whether both inputs are in the uint16
	 * range; if so, no overflow is possible.
	 */
	if (arg2 > (uint32) USHRT_MAX && result / arg2 != arg1)
		report_out_of_range();

	PG_RETURN_UINT32(result);
}

DECLARE(uint24div)
Datum
uint24div(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	if (arg2 == 0)
		report_division_by_zero();

	/* No overflow is possible */
	PG_RETURN_UINT32((uint32) arg1 / arg2);
}

DECLARE(uint42pl)
Datum
uint42pl(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	if (arg1 > UINT_MAX - arg2)
		report_out_of_range();

	PG_RETURN_UINT32(arg1 + arg2);
}

DECLARE(uint42mi)
Datum
uint42mi(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	if (arg2 > arg1)
		report_out_of_range();

	PG_RETURN_UINT32(arg1 - arg2);
}

DECLARE(uint42mul)
Datum
uint42mul(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint16		arg2 = PG_GETARG_UINT16(1);
	uint32		result;

	result = arg1 * arg2;

	/*
	 * Overflow check.	We basically check to see if result / arg1 gives arg2
	 * again.  There is one case where this fails: arg1 = 0 (which cannot
	 * overflow).
	 *
	 * Since the division is likely much more expensive than the actual
	 * multiplication, we'd like to skip it where possible.  The best bang for
	 * the buck seems to be to check whether both inputs are in the uint16
	 * range; if so, no overflow is possible.
	 */
	if (arg1 >= (uint32) USHRT_MAX && result / arg1 != arg2)
		report_out_of_range();

	PG_RETURN_UINT32(result);
}

DECLARE(uint42div)
Datum
uint42div(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	if (arg2 == 0)
		report_division_by_zero();

	PG_RETURN_UINT32(arg1 / arg2);
}

DECLARE(uint4mod)
Datum
uint4mod(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	if (arg2 == 0)
		report_division_by_zero();

	/* No overflow is possible */

	PG_RETURN_UINT32(arg1 % arg2);
}

DECLARE(uint2mod)
Datum
uint2mod(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	if (arg2 == 0)
	{
		ereport(ERROR,
				(errcode(ERRCODE_DIVISION_BY_ZERO),
				 errmsg("division by zero")));
		/* ensure compiler realizes we mustn't reach the division (gcc bug) */
		PG_RETURN_NULL();
	}

	/*
	 * Some machines throw a floating-point exception for INT_MIN % -1, which
	 * is a bit silly since the correct answer is perfectly well-defined,
	 * namely zero.  (It's not clear this ever happens when dealing with
	 * uint16, but we might as well have the test for safety.)
	 */
	if (arg2 == -1)
		PG_RETURN_UINT16(0);

	/* No overflow is possible */

	PG_RETURN_UINT16(arg1 % arg2);
}

DECLARE(uint2larger)
Datum
uint2larger(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_UINT16((arg1 > arg2) ? arg1 : arg2);
}

DECLARE(uint2smaller)
Datum
uint2smaller(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_UINT16((arg1 < arg2) ? arg1 : arg2);
}

DECLARE(uint4larger)
Datum
uint4larger(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_UINT32((arg1 > arg2) ? arg1 : arg2);
}

DECLARE(uint4smaller)
Datum
uint4smaller(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_UINT32((arg1 < arg2) ? arg1 : arg2);
}

/*
 * Bit-pushing operators
 *
 *		int[24]and		- returns arg1 & arg2
 *		int[24]or		- returns arg1 | arg2
 *		int[24]xor		- returns arg1 # arg2
 *		int[24]not		- returns ~arg1
 *		int[24]shl		- returns arg1 << arg2
 *		int[24]shr		- returns arg1 >> arg2
 */

DECLARE(uint4and)
Datum
uint4and(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_UINT32(arg1 & arg2);
}

DECLARE(uint4or)
Datum
uint4or(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_UINT32(arg1 | arg2);
}

DECLARE(uint4xor)
Datum
uint4xor(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_UINT32(arg1 ^ arg2);
}

DECLARE(uint4shl)
Datum
uint4shl(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int32		arg2 = PG_GETARG_INT32(1);

	PG_RETURN_UINT32(arg1 << arg2);
}

DECLARE(uint4shr)
Datum
uint4shr(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int32		arg2 = PG_GETARG_INT32(1);

	PG_RETURN_UINT32(arg1 >> arg2);
}

DECLARE(uint4not)
Datum
uint4not(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);

	PG_RETURN_UINT32(~arg1);
}

DECLARE(uint2and)
Datum
uint2and(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_UINT16(arg1 & arg2);
}

DECLARE(uint2or)
Datum
uint2or(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_UINT16(arg1 | arg2);
}

DECLARE(uint2xor)
Datum
uint2xor(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_UINT16(arg1 ^ arg2);
}

DECLARE(uint2not)
Datum
uint2not(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);

	PG_RETURN_UINT16(~arg1);
}


DECLARE(uint2shl)
Datum
uint2shl(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int32		arg2 = PG_GETARG_INT32(1);

	PG_RETURN_UINT16(arg1 << arg2);
}

DECLARE(uint2shr)
Datum
uint2shr(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int32		arg2 = PG_GETARG_INT32(1);

	PG_RETURN_UINT16(arg1 >> arg2);
}

/*
 * non-persistent numeric series generator
 */
DECLARE(generate_series_uint4)
DECLARE(generate_series_step_uint4)

Datum
generate_series_uint4(PG_FUNCTION_ARGS)
{
	return generate_series_step_uint4(fcinfo);
}

Datum
generate_series_step_uint4(PG_FUNCTION_ARGS)
{
	FuncCallContext *funcctx;
	generate_series_fctx *fctx;
	uint32		result;
	MemoryContext oldcontext;

	/* stuff done only on the first call of the function */
	if (SRF_IS_FIRSTCALL())
	{
		uint32		start = PG_GETARG_UINT32(0);
		uint32		finish = PG_GETARG_UINT32(1);
		int32		step = 1;

		/* see if we were given an explicit step size */
		if (PG_NARGS() == 3)
			step = PG_GETARG_INT32(2);
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
		if ( (fctx->step > 0 && result > fctx->current) || (fctx->step < 0 && result < fctx->current))
			fctx->step = 0;

		/* do when there is more left to send */
		SRF_RETURN_NEXT(funcctx, UInt32GetDatum(result));
	}
	else
		/* do when there is no more left */
		SRF_RETURN_DONE(funcctx);
}

/* Comparators */

DECLARE(uint2_cmp)
Datum
uint2_cmp(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	if (arg1 > arg2)
		PG_RETURN_INT32(1);
	else if (arg1 < arg2)
		PG_RETURN_INT32(-1);
	else
		PG_RETURN_INT32(0);
}

DECLARE(uint4_cmp)
Datum
uint4_cmp(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	if (arg1 > arg2)
		PG_RETURN_INT32(1);
	else if (arg1 < arg2)
		PG_RETURN_INT32(-1);
	else
		PG_RETURN_INT32(0);
}

/* Hashes */

DECLARE(uint4_hash)
Datum
uint4_hash(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	return DatumGetUInt32(hash_uint32(arg1));
}

DECLARE(uint2_hash)
Datum
uint2_hash(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);

	return DatumGetUInt32(hash_uint32((uint32)arg1));
}
