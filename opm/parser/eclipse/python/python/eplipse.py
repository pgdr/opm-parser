#!/usr/bin/env python
import sys
sys.path += ["/private/pgdr/ert/ert/devel/python/python"]

import ctypes
from ert.cwrap.clib import lib_name
from ert.cwrap.metacwrap import Prototype
from opm import OPMPrototype, IntVector, DoubleVector
from opm.ecl_state import EclipseState


if len(sys.argv) != 3:
    print "usage: python eplipse.py NORNE_ATW2013.DATA NTG"
    exit(127)


fname = sys.argv[1] #'/private/pgdr/opm/opm-data/norne/NORNE_ATW2013.DATA'
state = EclipseState.parseDeck(fname)
print state

keyword = sys.argv[2]

if keyword not in state:
    print "Does not have keyword %s" % keyword
    exit(1)
else:
    print 'Keyword %s has %d entries' %(keyword, len(state[keyword]))
