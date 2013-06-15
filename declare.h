#ifndef POSTGRES_UINTS_DECLARE_H
#define POSTGRES_UINTS_DECLARE_H

#define DECLARE(fun) \
	PG_FUNCTION_INFO_V1(fun);\
	Datum fun(PG_FUNCTION_ARGS);

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

#endif
