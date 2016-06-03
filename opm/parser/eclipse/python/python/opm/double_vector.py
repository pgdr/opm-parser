# Copyright 2016 Statoil ASA.
#
# This file is part of the Open Porous Media project (OPM).
#
# OPM is free software: you can redistribute it and/or modify it under the terms
# of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# OPM is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# OPM.  If not, see <http://www.gnu.org/licenses/>.

import  sys
from    types import IntType, SliceType
from    ert.cwrap import BaseCClass
from    opm import OPMPrototype

class DoubleVector(BaseCClass):
    """
    Super thin (reader) wrapper around a std::vector<double>.
    """
    TYPE_NAME = "opm_double_vector"

    _iget = OPMPrototype("double std_double_vector_iget(opm_double_vector, int)")
    _size = OPMPrototype("int    std_double_vector_size(opm_double_vector)")

    def __init__(self):
        raise NotImplementedError("Can not instantiate std::vector < double >")

    def __len__(self):
        return self._size()

    def __getitem__(self, index):
        if not 0 <= index < len(self):
            raise IndexError("vector index out of range")
        return self._iget(index)

    def __iter__(self):
        i = 0
        for i in range(len(self)):
            yield self[i]
            i += 1


    def __bool__(self):
        """
        Will evaluate to False for empty vector.
        """
        if len(self) == 0:
            return False
        else:
            return True

    def __nonzero__(self):
        return self.__bool__( )

    def __contains__(self , value):
        return self._contains(  value)
