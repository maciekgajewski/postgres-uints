-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION uints" to load this file. \quit


-- UINT 2 definition
CREATE TYPE uint2;

CREATE FUNCTION uint2in(cstring) RETURNS uint2
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint2out(uint2) RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint2recv(internal) RETURNS uint2
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint2send(uint2) RETURNS bytea
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

--CREATE TYPE uint2 (
--        INTERNALLENGTH = 2,
--        INPUT = uint2in,
--        OUTPUT = uint2out,
--        RECEIVE = uint2recv,
--        SEND = uint2send,
--        STORAGE = plain,
--        ALIGNMENT = int2,
--        category = 'N',
--        PASSEDBYVALUE
--);
CREATE TYPE uint2 (
    LIKE = int2,
    INPUT = uint2in,
    OUTPUT = uint2out,
    RECEIVE = uint2recv,
    SEND = uint2send
);

-- UINT 4 definition
CREATE TYPE uint4;

CREATE FUNCTION uint4in(cstring) RETURNS uint4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint4out(uint4) RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint4recv(internal) RETURNS uint4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint4send(uint4) RETURNS bytea
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

--CREATE TYPE uint4 (
--        INTERNALLENGTH = 4,
--        INPUT = uint4in,
--        OUTPUT = uint4out,
--        RECEIVE = uint4recv,
--        SEND = uint4send,
--        STORAGE = plain,
--        alignment = int4,
--        category = 'N',
--        PASSEDBYVALUE
--);
CREATE TYPE uint4 (
    LIKE = int4,
    INPUT = uint4in,
    OUTPUT = uint4out,
    RECEIVE = uint4recv,
    SEND = uint4send
);

-- conversion functions

CREATE FUNCTION u2tou4(uint2) RETURNS uint4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION u4tou2(uint4) RETURNS uint2
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint4_bool(uint4) RETURNS BOOLEAN
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION bool_uint4(BOOLEAN) RETURNS uint4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION i4tou2(int4) RETURNS uint2
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

-- arithmetic and logic functions

CREATE FUNCTION uint4eq(uint4, uint4) RETURNS BOOLEAN
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint4ne(uint4, uint4) RETURNS BOOLEAN
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint4lt(uint4, uint4) RETURNS BOOLEAN
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint4le(uint4, uint4) RETURNS BOOLEAN
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint4gt(uint4, uint4) RETURNS BOOLEAN
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint4ge(uint4, uint4) RETURNS BOOLEAN
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;


CREATE FUNCTION uint2eq(uint2, uint2) RETURNS BOOLEAN
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint2ne(uint2, uint2) RETURNS BOOLEAN
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint2lt(uint2, uint2) RETURNS BOOLEAN
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint2le(uint2, uint2) RETURNS BOOLEAN
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint2gt(uint2, uint2) RETURNS BOOLEAN
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint2ge(uint2, uint2) RETURNS BOOLEAN
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint24eq(uint2, uint4) RETURNS BOOLEAN
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint24ne(uint2, uint4) RETURNS BOOLEAN
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint24lt(uint2, uint4) RETURNS BOOLEAN
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint24le(uint2, uint4) RETURNS BOOLEAN
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint24gt(uint2, uint4) RETURNS BOOLEAN
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint24ge(uint2, uint4) RETURNS BOOLEAN
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint42eq(uint4, uint2) RETURNS BOOLEAN
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint42ne(uint4, uint2) RETURNS BOOLEAN
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint42lt(uint4, uint2) RETURNS BOOLEAN
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint42le(uint4, uint2) RETURNS BOOLEAN
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint42gt(uint4, uint2) RETURNS BOOLEAN
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint42ge(uint4, uint2) RETURNS BOOLEAN
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint4pl(uint4, uint4) RETURNS uint4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint4mi(uint4, uint4) RETURNS uint4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint4mul(uint4, uint4) RETURNS uint4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint4div(uint4, uint4) RETURNS uint4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint4inc(uint4) RETURNS uint4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint2pl(uint2, uint2) RETURNS uint2
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint2mi(uint2, uint2) RETURNS uint2
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint2mul(uint2, uint2) RETURNS uint2
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint2div(uint2, uint2) RETURNS uint2
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint24pl(uint2, uint4) RETURNS uint4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint24mi(uint2, uint4) RETURNS uint4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint24mul(uint2, uint4) RETURNS uint4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint24div(uint2, uint4) RETURNS uint4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint42pl(uint4, uint2) RETURNS uint4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint42mi(uint4, uint2) RETURNS uint4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint42mul(uint4, uint2) RETURNS uint4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint42div(uint4, uint2) RETURNS uint4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint4mod(uint4, uint4) RETURNS uint4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint2mod(uint2, uint2) RETURNS uint2
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint2larger(uint2, uint2) RETURNS uint2
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint2smaller(uint2, uint2) RETURNS uint2
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint4larger(uint4, uint4) RETURNS uint4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint4smaller(uint4, uint4) RETURNS uint4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint4and(uint4, uint4) RETURNS uint4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint4or(uint4, uint4) RETURNS uint4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint4xor(uint4, uint4) RETURNS uint4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint4shl(uint4, int4) RETURNS uint4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint4shr(uint4, int4) RETURNS uint4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint4not(uint4) RETURNS uint4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint2and(uint2, uint2) RETURNS uint2
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint2or(uint2, uint2) RETURNS uint2
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint2xor(uint2, uint2) RETURNS uint2
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint2not(uint2) RETURNS uint2
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint2shl(uint2, int4) RETURNS uint2
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint2shr(uint2, int4) RETURNS uint2
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;


-- TODO how to use these?
--CREATE FUNCTION generate_series_uint4(uint4, uint4) RETURNS uint4
--AS 'MODULE_PATHNAME'
--LANGUAGE C STRICT IMMUTABLE;

--CREATE FUNCTION generate_series_step_uint4(uint4, uint4, int4) RETURNS uint4
--AS 'MODULE_PATHNAME'
--LANGUAGE C STRICT IMMUTABLE;




-- UINT 2 operators


CREATE CAST (int2 AS uint2) WITHOUT FUNCTION;
CREATE CAST (uint2 AS int2) WITHOUT FUNCTION;
CREATE CAST (int4 AS uint2) WITH FUNCTION i4tou2(int4);

CREATE OPERATOR + (
    leftarg = uint2,
    rightarg = uint2,
    procedure = uint2pl,
    commutator = +
);

CREATE OPERATOR - (
    leftarg = uint2,
    rightarg = uint2,
    procedure = uint2mi
);

CREATE OPERATOR * (
    leftarg = uint2,
    rightarg = uint2,
    procedure = uint2mul,
    commutator = *
);

CREATE OPERATOR / (
    leftarg = uint2,
    rightarg = uint2,
    procedure = uint2div
);

CREATE OPERATOR % (
    leftarg = uint2,
    rightarg = uint2,
    procedure = uint2mod
);

CREATE OPERATOR & (
    leftarg = uint2,
    rightarg = uint2,
    procedure = uint2and,
    commutator = &
);

CREATE OPERATOR | (
    leftarg = uint2,
    rightarg = uint2,
    procedure = uint2or,
    commutator = |
);

CREATE OPERATOR # (
    leftarg = uint2,
    rightarg = uint2,
    procedure = uint2xor,
    commutator = #
);

CREATE OPERATOR ~ (
    rightarg = uint2,
    procedure = uint2not
);

CREATE OPERATOR << (
    rightarg = int4,
    leftarg = uint2,
    procedure = uint2shl
);

CREATE OPERATOR >> (
    rightarg = int4,
    leftarg = uint2,
    procedure = uint2shr
);

CREATE OPERATOR > (
    rightarg = uint2,
    leftarg = uint2,
    procedure = uint2gt,
    commutator = <,
    negator = <=
);

CREATE OPERATOR < (
    rightarg = uint2,
    leftarg = uint2,
    procedure = uint2lt,
    commutator = >,
    negator = >=
);

CREATE OPERATOR <= (
    rightarg = uint2,
    leftarg = uint2,
    procedure = uint2le,
    commutator = >=,
    negator = >
);

CREATE OPERATOR >= (
    rightarg = uint2,
    leftarg = uint2,
    procedure = uint2ge,
    commutator = <=,
    negator = <
);

CREATE OPERATOR = (
    rightarg = uint2,
    leftarg = uint2,
    procedure = uint2eq,
    commutator = =,
    negator = <>
);

CREATE OPERATOR <> (
    rightarg = uint2,
    leftarg = uint2,
    procedure = uint2ne,
    commutator = <>,
    negator = =
);

-- UINT 4 operators

CREATE CAST (int4 AS uint4) WITHOUT FUNCTION;
CREATE CAST (uint4 AS int4) WITHOUT FUNCTION;

CREATE OPERATOR + (
    leftarg = uint4,
    rightarg = uint4,
    procedure = uint4pl,
    commutator = +
);

CREATE OPERATOR - (
    leftarg = uint4,
    rightarg = uint4,
    procedure = uint4mi
);

CREATE OPERATOR * (
    leftarg = uint4,
    rightarg = uint4,
    procedure = uint4mul,
    commutator = *
);

CREATE OPERATOR / (
    leftarg = uint4,
    rightarg = uint4,
    procedure = uint4div
);

CREATE OPERATOR % (
    leftarg = uint4,
    rightarg = uint4,
    procedure = uint4mod
);

CREATE OPERATOR & (
    leftarg = uint4,
    rightarg = uint4,
    procedure = uint4and,
    commutator = &
);

CREATE OPERATOR | (
    leftarg = uint4,
    rightarg = uint4,
    procedure = uint4or,
    commutator = |
);

CREATE OPERATOR # (
    leftarg = uint4,
    rightarg = uint4,
    procedure = uint4xor,
    commutator = #
);

CREATE OPERATOR ~ (
    rightarg = uint4,
    procedure = uint4not
);

CREATE OPERATOR << (
    rightarg = int4,
    leftarg = uint4,
    procedure = uint4shl
);

CREATE OPERATOR >> (
    rightarg = int4,
    leftarg = uint4,
    procedure = uint4shr
);

CREATE OPERATOR > (
    rightarg = uint4,
    leftarg = uint4,
    procedure = uint4gt,
    commutator = <,
    negator = <=
);

CREATE OPERATOR < (
    rightarg = uint4,
    leftarg = uint4,
    procedure = uint4lt,
    commutator = >,
    negator = >=
);

CREATE OPERATOR <= (
    rightarg = uint4,
    leftarg = uint4,
    procedure = uint4le,
    commutator = >=,
    negator = >
);

CREATE OPERATOR >= (
    rightarg = uint4,
    leftarg = uint4,
    procedure = uint4ge,
    commutator = <=,
    negator = <
);

CREATE OPERATOR = (
    rightarg = uint4,
    leftarg = uint4,
    procedure = uint4eq,
    commutator = =,
    negator = <>
);

CREATE OPERATOR <> (
    rightarg = uint4,
    leftarg = uint4,
    procedure = uint4ne,
    commutator = <>,
    negator = =
);

-- UINT 2/4 operators

CREATE CAST (uint2 AS uint4) WITH FUNCTION u2tou4(uint2) AS IMPLICIT;

CREATE OPERATOR > (
    rightarg = uint4,
    leftarg = uint2,
    procedure = uint24gt,
    commutator = <,
    negator = <=
);

CREATE OPERATOR < (
    rightarg = uint4,
    leftarg = uint2,
    procedure = uint24lt,
    commutator = >,
    negator = >=
);

CREATE OPERATOR <= (
    rightarg = uint4,
    leftarg = uint2,
    procedure = uint24le,
    commutator = >=,
    negator = >
);

CREATE OPERATOR >= (
    rightarg = uint4,
    leftarg = uint2,
    procedure = uint24ge,
    commutator = <=,
    negator = <
);

CREATE OPERATOR = (
    rightarg = uint4,
    leftarg = uint2,
    procedure = uint24eq,
    commutator = =,
    negator = <>
);

CREATE OPERATOR <> (
    rightarg = uint4,
    leftarg = uint2,
    procedure = uint24ne,
    commutator = <>,
    negator = =
);

CREATE OPERATOR + (
    rightarg = uint4,
    leftarg = uint2,
    procedure = uint24pl,
    commutator = +
);

CREATE OPERATOR - (
    rightarg = uint4,
    leftarg = uint2,
    procedure = uint24mi
);

CREATE OPERATOR * (
    rightarg = uint4,
    leftarg = uint2,
    procedure = uint24mul,
    commutator = *
);

CREATE OPERATOR / (
    rightarg = uint4,
    leftarg = uint2,
    procedure = uint24div
);


-- UINT 4/2 operators

CREATE CAST (uint4 AS uint2) WITH FUNCTION u4tou2(uint4);

CREATE OPERATOR > (
    rightarg = uint2,
    leftarg= uint4,
    procedure = uint42gt,
    commutator = <,
    negator = <=
);

CREATE OPERATOR < (
    rightarg = uint2,
    leftarg= uint4,
    procedure = uint42lt,
    commutator = >,
    negator = >=
);

CREATE OPERATOR <= (
    rightarg = uint2,
    leftarg= uint4,
    procedure = uint42le,
    commutator = >=,
    negator = >
);

CREATE OPERATOR >= (
    rightarg = uint2,
    leftarg= uint4,
    procedure = uint42ge,
    commutator = <=,
    negator = <
);

CREATE OPERATOR = (
    rightarg = uint2,
    leftarg= uint4,
    procedure = uint42eq,
    commutator = =,
    negator = <>
);

CREATE OPERATOR <> (
    rightarg = uint2,
    leftarg= uint4,
    procedure = uint42ne,
    commutator = <>,
    negator = =
);

CREATE OPERATOR + (
    rightarg = uint2,
    leftarg= uint4,
    procedure = uint42pl,
    commutator = +
);

CREATE OPERATOR - (
    rightarg = uint2,
    leftarg= uint4,
    procedure = uint42mi
);

CREATE OPERATOR * (
    rightarg = uint2,
    leftarg= uint4,
    procedure = uint42mul,
    commutator = *
);

CREATE OPERATOR / (
    rightarg = uint2,
    leftarg= uint4,
    procedure = uint42div
);

-- UINT 2 operator classes

CREATE FUNCTION uint2_cmp(uint2, uint2) RETURNS INTEGER
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint2_hash(uint2) RETURNS INTEGER
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;


CREATE OPERATOR CLASS uint2_ops_btree
    DEFAULT FOR TYPE uint2 USING btree AS
        OPERATOR        1       < ,
        OPERATOR        2       <= ,
        OPERATOR        3       = ,
        OPERATOR        4       >= ,
        OPERATOR        5       > ,
        FUNCTION        1       uint2_cmp(uint2, uint2);


CREATE OPERATOR CLASS uint2_ops_hash
    DEFAULT FOR TYPE uint2 USING hash AS
        OPERATOR        1       = ,
        FUNCTION        1       uint2_hash(uint2);

-- UINT 4 operator classes

CREATE FUNCTION uint4_cmp(uint4, uint4) RETURNS INTEGER
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION uint4_hash(uint4) RETURNS INTEGER
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OPERATOR CLASS uint4_ops_btree
    DEFAULT FOR TYPE uint4 USING btree AS
        OPERATOR        1       < ,
        OPERATOR        2       <= ,
        OPERATOR        3       = ,
        OPERATOR        4       >= ,
        OPERATOR        5       > ,
        FUNCTION        1       uint4_cmp(uint4, uint4);

CREATE OPERATOR CLASS uint4_ops_hash
    DEFAULT FOR TYPE uint4 USING hash AS
        OPERATOR        1       = ,
        FUNCTION        1       uint4_hash(uint4);
