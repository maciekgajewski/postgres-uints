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


-- UINT 2 operators


CREATE CAST (int2 AS uint2) WITHOUT FUNCTION;
CREATE CAST (uint2 AS int2) WITHOUT FUNCTION;
CREATE CAST (uint2 AS uint4) WITH FUNCTION u2tou4(uint2) AS IMPLICIT;
CREATE CAST (int4 AS uint2) WITH FUNCTION i4tou2(int4);


-- UINT 4 operators

CREATE CAST (int4 AS uint4) WITHOUT FUNCTION;
CREATE CAST (uint4 AS int4) WITHOUT FUNCTION;
CREATE CAST (uint4 AS uint2) WITH FUNCTION u4tou2(uint4);
