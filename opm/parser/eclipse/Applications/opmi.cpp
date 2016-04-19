/*
  Copyright 2013 Statoil ASA.

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

#include <iostream>
#include <memory>

#include <opm/parser/eclipse/Parser/Parser.hpp>
#include <opm/parser/eclipse/Parser/ParseContext.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>

void loadDeck(const char * deck_file, bool verbose, bool actoct) {
    Opm::ParseContext parseContext;
    Opm::ParserPtr parser(new Opm::Parser());
    std::shared_ptr<const Opm::Deck> deck;
    std::shared_ptr<Opm::EclipseState> state;

    if (verbose)
        std::cout << "Loading deck: " << deck_file << " ..... ";
    std::cout.flush();
    deck = parser->parseFile(deck_file, parseContext);
    if (verbose)
        std::cout << "parse complete - creating EclipseState .... ";
    std::cout.flush();
    state = std::make_shared<Opm::EclipseState>(deck, parseContext);
    if (verbose)
        std::cout << "complete." << std::endl;

    if (actoct) {

        std::vector<size_t> xvec;
        std::vector<size_t> yvec;
        std::vector<size_t> zvec;

        auto grid = state->getEclipseGrid();
        for (size_t x = 0; x < grid->getNX(); x++) {
            for (size_t y = 0; y < grid->getNY(); y++) {
                for (size_t z = 0; z < grid->getNZ(); z++) {
                    if (!grid->cellActive(x, y, z)) {
                        xvec.push_back( x);
                        yvec.push_back( y);
                        zvec.push_back(grid->getNZ() - z); // invert to get Z=0 on top
                    }
                }
            }
        }

        std::cout << "X = [0 ";
        for (auto xval : xvec) {
            std::cout << xval << " ";
        }
        std::cout << grid->getNX() << "];" << std::endl;

        std::cout << "Y = [0 ";
        for (auto yval : yvec) {
            std::cout << yval << " ";
        }
        std::cout << grid->getNY() << "];" << std::endl;

        std::cout << "Z = [0 ";
        for (auto zval : zvec) {
            std::cout << zval << " ";
        }
        std::cout << grid->getNZ() << "];" << std::endl;

        std::cout << "scatter3(X,Y,Z,10,\"b\",\"*\");" << std::endl;
    }
}

bool cmdOptionExists(char** begin, char** end, const std::string& option) {
    return std::find(begin, end, option) != end;
}

int main(int argc, char** argv) {
    if (cmdOptionExists(argv, argv + argc, "-h")) {
        std::cout << "Usage: opmi [options] datafile..." << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  -v         verbose" << std::endl;
        std::cout << "  -o         octave output" << std::endl;
        //std::cout << "  -a         actnum info" << std::endl;

        std::cout << "Examples: opmi -v ECL.DATA" << std::endl;
        std::cout << "          opmi -o -a ECL.DATA" << std::endl;

        return 0;
    }

    int offset = 0;
    bool verbose = false;
    if (cmdOptionExists(argv, argv + argc, "-v")) {
        verbose = true;
        offset++;
    }
    bool actoct = false;
    if (cmdOptionExists(argv, argv + argc, "-o")) {
        actoct = true;
        offset++;
    }
    //for (int iarg = 1; iarg < argc; iarg++)
    loadDeck(argv[1 + offset], verbose, actoct);

    return 0;
}

