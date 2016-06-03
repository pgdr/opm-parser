/*
 Copyright 2016 Statoil ASA.

 This file is part of the Open Porous Media project (OPM).

 OPM is free software: you can redistribute it and/or modify it under the terms
 of the GNU General Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at your option) any later
 version.

 OPM is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 OPM.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <vector>

extern "C" {

  double std_double_vector_iget(std::vector<double> * vec, int index) {
    return vec->at(index);
  }

  int std_double_vector_size(std::vector<double> * vec) {
    return vec->size();
  }

  int std_int_vector_iget(std::vector<int> * vec, int index) {
    return vec->at(index);
  }

  int std_int_vector_size(std::vector<int> * vec) {
    return vec->size();
  }
}
