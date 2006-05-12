#! /usr/bin/env bash
LIST=`find . -name \*.ui -o -name \*.kcfg -o -name \*.rc` 
if test -n "$LIST"; then 
	$EXTRACTRC $LIST >> rc.cpp 
fi
LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`
if test -n "$LIST"; then
	$XGETTEXT $LIST -o $podir/klinkstatus.pot
fi

