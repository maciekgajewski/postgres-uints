/*-------------------------------------------------------------------------
 *
 * uint_numutils.h
 *	  Definitions for unsigned integer string conversion.
 *
 *
 * Portions Copyright (c) 1996-2013, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 *-------------------------------------------------------------------------
 */

#include "c.h"
#include "utils/elog.h"

#include "uints_numutils.h"

#include <stdlib.h>
#include <limits.h>

/* Conveniently, never returns */
static inline
void
report_invalid(char* s)
{
	ereport(ERROR,
			(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
			 errmsg("invalid input syntax for unsgined integer: \"%s\"",
					s)));
}

static inline
void
report_range_error(char* s, const char* type)
{
	ereport(ERROR,
		(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
		errmsg("value \"%s\" is out of range for type %s", s, type)));
}

static inline
void
validate_string(char* s)
{
	/*
	 * Some versions of strtol treat the empty string as an error, but some
	 * seem not to.  Make an explicit test to be sure we catch it.
	 */
	if (s == NULL)
		elog(ERROR, "NULL pointer");
	if (*s == 0)
		report_invalid(s);
}

/*
 * Skip any trailing whitespace; if anything but whitespace remains before
 * the terminating character, bail out
 */
static inline
void
skip_trailing_whitespace(char* s, char* end, int c)
{
	while (*end && *end != c && isspace((unsigned char) *end))
		end++;

	if (*end && *end != c)
		report_invalid(s);
}
/*
 * pg_atouX: convert string to unsigned integer
 *
 * allows any number of leading or trailing whitespace characters.
 *
 * c, if not 0, is a terminator character that may appear after the
 * integer (plus whitespace).  If 0, the string must end after the integer.
 *
 * Unlike plain atoi(), this will throw ereport() upon bad input format or
 * overflow.
 */
uint8 pg_atou1(char *s, int c)
{
	unsigned long int   l;
	char                *badp;

	validate_string(s);
	errno = 0;
	l = strtoul(s, &badp, 10);

	if (s == badp)
		report_invalid(s);

	if (errno == ERANGE || l > UCHAR_MAX)
		report_range_error(s, "uint1");

	skip_trailing_whitespace(s, badp, c);

	return (uint8)l;
}

uint16 pg_atou2(char *s, int c)
{
	unsigned long int   l;
	char                *badp;

	validate_string(s);
	errno = 0;
	l = strtoul(s, &badp, 10);

	if (s == badp)
		report_invalid(s);

	if (errno == ERANGE || l > USHRT_MAX)
		report_range_error(s, "uint2");

	skip_trailing_whitespace(s, badp, c);

	return (uint16)l;
}

uint32 pg_atou4(char *s, int c)
{
	unsigned long int   l;
	char                *badp;

	validate_string(s);
	errno = 0;
	l = strtoul(s, &badp, 10);

	if (s == badp)
		report_invalid(s);

	if (errno == ERANGE)
		report_range_error(s, "uint4");

	skip_trailing_whitespace(s, badp, c);

	return l;
}

uint64 pg_atou8(char *s, int c)
{
	uint8	l;
	char	*badp;

	validate_string(s);
	errno = 0;
	l = strtoull(s, &badp, 10);

	if (s == badp)
		report_invalid(s);

	if (errno == ERANGE)
		report_range_error(s, "uint8");

	skip_trailing_whitespace(s, badp, c);

	return l;
}


void
pg_u1toa(uint8 i, char* a)
{
	pg_u4toa((uint32)i, a);
}

void
pg_u2toa(uint16 i, char* a)
{
	pg_u4toa((uint32)i, a);
}

void
pg_u4toa(uint32 i, char* a)
{
	pg_u8toa((uint64)i, a);

}

void
pg_u8toa(uint64 value, char* a)
{
	char	   *start = a;

	/* Compute the result string backwards. */
	do
	{
		uint64		remainder;
		uint64		oldval = value;

		value /= 10;
		remainder = oldval - value * 10;
		*a++ = '0' + remainder;
	} while (value != 0);

	/* Add trailing NUL byte, and back up 'a' to the last character. */
	*a-- = '\0';

	/* Reverse string. */
	while (start < a)
	{
		char		swap = *start;

		*start++ = *a;
		*a-- = swap;
	}

}
