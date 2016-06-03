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
#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>
#include <opm/parser/eclipse/EclipseState/Eclipse3DProperties.hpp>
#include <opm/parser/eclipse/Parser/Parser.hpp>

extern "C" {

    Opm::EclipseState * eclipse_state_parse(const char * filename) {
        Opm::ParseContext pc;
        pc.update(Opm::ParseContext::PARSE_RANDOM_SLASH, Opm::InputError::IGNORE);

        auto eclipseState = Opm::Parser::parse(filename, pc);

        return new Opm::EclipseState(eclipseState);
    }

    bool eclipse_state_has_double_keyword(const Opm::EclipseState * state, const char * keyword) {
        if (!state->get3DProperties().supportsDoubleGridProperty(keyword))
            return false;
        return state->get3DProperties().hasDeckDoubleGridProperty(keyword);
    }

    bool eclipse_state_has_int_keyword(const Opm::EclipseState * state, const char * keyword) {
        if (!state->get3DProperties().supportsIntGridProperty(keyword))
            return false;
        return state->get3DProperties().hasDeckIntGridProperty(keyword);
    }

    const std::vector<double> * eclipse_state_get_double_keyword(const Opm::EclipseState * state, const char * keyword)
    {
        return &state->get3DProperties().getDoubleGridProperty(keyword).getData();
    }

    const std::vector<int> * eclipse_state_get_int_keyword(const Opm::EclipseState * state, const char * keyword) {
        return &state->get3DProperties().getIntGridProperty(keyword).getData();
    }

    const char * eclipse_state_get_title(const Opm::EclipseState * state) {
        return state->getTitle().data();
    }

}
