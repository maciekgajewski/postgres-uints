#!/bin/bash
# Simple test of the extension

DATADIR=testdata

mkdir $DATADIR || exit 1

cd $DATADIR

initdb . && postgres --single -D. postgres < ../test.sql

cd ..

rm -rf $DATADIR

