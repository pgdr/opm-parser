/*
 Copyright 2016 Statoil ASA.

 This file is part of the Open Porous Media project (OPM).

 OPM is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 OPM is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with OPM.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OPM_PARSER_ECLIPSE_APPLICATIONS_OCTAVEOUTPUT_HPP_
#define OPM_PARSER_ECLIPSE_APPLICATIONS_OCTAVEOUTPUT_HPP_

#include <iostream>
#include <memory>

#include <stdio.h>
#include <ctype.h>

#include <opm/parser/eclipse/Parser/Parser.hpp>
#include <opm/parser/eclipse/Parser/ParseContext.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>

using namespace Opm;

class OctaveOutput
{
public:

    void upcase(std::string& s) {
        int i = 0;
        char c;
        while (s[i]) {
            c = s[i];
            s[i] = std::toupper(c);
            i++;
        };
    }

    OctaveOutput(const EclipseState& state) :
            m_state(state)
    {
    }

    /// get the associated data for keyword from 3DProperties, return a vector<double>.
    /// Use is_int=true if it is an int keyword, like actnum
    std::vector<double> getProperty(std::string keyword, bool is_int = false) {
        if (!is_int)
            return m_state.get3DProperties().getDoubleGridProperty(keyword).getData();
        std::vector<double> ret;

        auto data = m_state.get3DProperties().getIntGridProperty(keyword).getData();
        auto data_size = data.size();
        for (size_t i = 0; i < data_size; i++) {
            ret.push_back(data[i]);
        }
        return ret;
    }

    ///  Get the octave output of the grid for the given keyword.
    ///
    void octave(std::string keyword) {

        // we are essentially storing three vectors that form the coordinates, and then a value vector.
        // The coordinate given by xvec[i], yvec[i], zvec[i] has value val_vec[i].

        std::vector<size_t> xvec;
        std::vector<size_t> yvec;
        std::vector<size_t> zvec;
        std::vector<double> val_vec;

        bool is_int = false;
        if (m_state.get3DProperties().supportsGridProperty(keyword,
                Eclipse3DProperties::EnabledTypes::DoubleProperties))
            is_int = false;
        else if (m_state.get3DProperties().supportsGridProperty(keyword,
                Eclipse3DProperties::EnabledTypes::IntProperties))
            is_int = true;
        else {

            // okay, no such keyword, try uppercasing it ...

            upcase(keyword);
            if (m_state.get3DProperties().supportsGridProperty(keyword,
                    Eclipse3DProperties::EnabledTypes::DoubleProperties))
                is_int = false;
            else if (m_state.get3DProperties().supportsGridProperty(keyword,
                    Eclipse3DProperties::EnabledTypes::IntProperties))
                is_int = true;
            else {
                std::cout << "%%%--- NO SUCH KEYWORD " + keyword + ".  Suggestion:  Try with an _actual_ keyword?"
                        << std::endl;
                return;
            }
        }

        std::vector<double> data = getProperty(keyword, is_int);

        if (keyword == "ACTNUM") {
            for (size_t i = 0; i < data.size(); i++) {
                data[i] = (data[i] < 2 ? 1 - data[i] : data[i]); // flip 0 and 1 to display inactive
            }
        }

        size_t globalIndex = 0;
        auto grid = m_state.getInputGrid();
        for (size_t x = 0; x < grid->getNX(); x++) {
            for (size_t y = 0; y < grid->getNY(); y++) {
                for (size_t z = 0; z < grid->getNZ(); z++) {
                    double val = data[globalIndex];
                    if (val == 0)
                        continue;       // don't output empty values
                    if (isnan(val))
                        val = 1000;   // real magic
                    xvec.push_back(x);
                    yvec.push_back(y);
                    zvec.push_back(grid->getNZ() - z); // invert to get Z=0 on top
                    val_vec.push_back(val);
                    ++globalIndex;
                }
            }
        }

        std::cout << "%%%---  OUTPUT |X| = " << xvec.size() << std::endl;
        std::cout << "%%%---         |Y| = " << yvec.size() << std::endl;
        std::cout << "%%%---         |Z| = " << zvec.size() << std::endl;
        std::cout << "%%%---         |V| = " << val_vec.size() << std::endl;
        std::cout << "%%%---         SUM = " << xvec.size() + yvec.size() + zvec.size() + val_vec.size() << std::endl;

        size_t MAX_LIMIT = 40000;
        if (xvec.size() > MAX_LIMIT) {
            std::cout << "%%%---  EXCEEDING LIMIT |X| > " << MAX_LIMIT << std::endl;
            std::cout << "%%%---  TRUNCATING AT VALUE   " << MAX_LIMIT << std::endl;
            std::cerr << "WARN: " << xvec.size() << " > " << MAX_LIMIT << std::endl;
            std::cerr << "WARN: TRUNCATING VALUES AT " << MAX_LIMIT << std::endl;
        }

        size_t counter = 0;

        std::cout << "X = [0 ";
        for (auto xval : xvec) {
            if (++counter > MAX_LIMIT)
                break;
            std::cout << xval << " ";
        }
        std::cout << grid->getNX() << "];" << std::endl;

        counter = 0;
        std::cout << "Y = [0 ";
        for (auto yval : yvec) {
            if (++counter > MAX_LIMIT)
                break;
            std::cout << yval << " ";
        }
        std::cout << grid->getNY() << "];" << std::endl;

        counter = 0;
        std::cout << "Z = [0 ";
        for (auto zval : zvec) {
            if (++counter > MAX_LIMIT)
                break;
            std::cout << zval << " ";
        }
        std::cout << grid->getNZ() << "];" << std::endl;

        counter = 0;
        std::cout << "V = [0 ";
        for (auto val : val_vec) {
            if (++counter > MAX_LIMIT)
                break;
            std::cout << val << " ";
        }

        std::cout << "0];" << std::endl;

        std::cout << "scatter3(X,Y,Z,10,V,\"*\");" << std::endl;
    }
private:
    const EclipseState& m_state;
};

#endif /* OPM_PARSER_ECLIPSE_APPLICATIONS_OCTAVEOUTPUT_HPP_ */
