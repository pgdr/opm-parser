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

extern "C" {

    int deck_size( const Opm::Deck * deck ) {
        return static_cast<int>( deck->size() );
    }


    Opm::Deck * deck_alloc() {
        return new Opm::Deck();
    }


    void deck_free( Opm::Deck * deck ) {
        delete deck;
    }


    const Opm::DeckKeyword * deck_iget_keyword( const Opm::Deck * deck , int index) {
        return &deck->getKeyword( index );
    }


    const Opm::DeckKeyword * deck_iget_named_keyword( const Opm::Deck * deck , const char * keyword , int index) {
        return &deck->getKeyword( keyword , index );
    }


    bool deck_has_keyword( const Opm::Deck * deck , const char * keyword) {
        return deck->hasKeyword( keyword );
    }

    int deck_num_keywords( const Opm::Deck * deck , const char * keyword) {
        return static_cast<int>( deck->count(keyword) );
    }


}


