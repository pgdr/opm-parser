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

void loadDeck(const char * deck_file, bool verbose, bool octave, bool is_int, char* _keyword) {
    Opm::ParseContext parseContext;
    parseContext.update(Opm::ParseContext::PARSE_MISSING_DIMS_KEYWORD, Opm::InputError::IGNORE);
    parseContext.update(Opm::ParseContext::PARSE_RANDOM_SLASH, Opm::InputError::IGNORE);
    parseContext.update(Opm::ParseContext::PARSE_RANDOM_TEXT, Opm::InputError::IGNORE);
    parseContext.update(Opm::ParseContext::PARSE_UNKNOWN_KEYWORD, Opm::InputError::IGNORE);
    parseContext.update(Opm::ParseContext::PARSE_EXTRA_DATA, Opm::InputError::IGNORE);
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

    if (octave) {

        std::vector<size_t> xvec;
        std::vector<size_t> yvec;
        std::vector<size_t> zvec;
        std::vector<int>    i_val_vec;
        std::vector<double> d_val_vec;

        std::string keyword;
        if (_keyword != nullptr)
            keyword = std::string(_keyword);
        else
            keyword = "ACTNUM";

        std::vector<double> d_data;
        std::vector<int> i_data;
        if (is_int)
            i_data = state->get3DProperties().getIntGridProperty(keyword).getData();
        else
            d_data = state->get3DProperties().getDoubleGridProperty(keyword).getData();

        if (keyword == "ACTNUM") {
            for (auto i =0; i < i_data.size(); i++) {
                i_data[i] = (i_data[i] < 2 ? 1- i_data[i]: i_data[i]); // flip 0 and 1 to not display active
            }
        }

        size_t globalIndex = 0;
        auto grid = state->getEclipseGrid();
        for (size_t x = 0; x < grid->getNX(); x++) {
            for (size_t y = 0; y < grid->getNY(); y++) {
                for (size_t z = 0; z < grid->getNZ(); z++) {
                    if ((is_int && i_data[globalIndex] != 0) || (!is_int && d_data[globalIndex] != 0)) {
                        xvec.push_back(x);
                        yvec.push_back(y);
                        zvec.push_back(grid->getNZ() - z); // invert to get Z=0 on top
                        if (is_int)
                            i_val_vec.push_back(i_data[globalIndex]);
                        else
                            d_val_vec.push_back(d_data[globalIndex]);
                    }
                    ++globalIndex;
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

        std::cout << "V = [0 ";
        if (is_int)
            for (auto val : i_val_vec)
                std::cout << val << " ";
        else
            for (auto val : d_val_vec)
                std::cout << val << " ";

        std::cout << "0];" << std::endl;

        std::cout << "scatter3(X,Y,Z,10,V,\"*\");" << std::endl;
    }
}



char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option) {
    return std::find(begin, end, option) != end;
}

void printHelp() {
    std::cout << "Usage: opmi [options] datafile..." << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -v         verbose" << std::endl;
    std::cout << "  -o         octave output" << std::endl;
    std::cout << "  -ik        int keyword info" << std::endl;
    std::cout << "  -dk        double keyword info" << std::endl;

    std::cout << "Examples: opmi -v ECL.DATA" << std::endl;
    std::cout << "          opmi -o -a ECL.DATA" << std::endl;
    std::cout << "          opmi -o -ik SATNUM ECL.DATA" << std::endl;
    std::cout << "          opmi -o -ik ACTNUM ECL.DATA" << std::endl;
    std::cout << "          opmi -o -dk PORO ECL.DATA" << std::endl;
}

int main(int argc, char** argv) {
    if (cmdOptionExists(argv, argv + argc, "-h")) {
        printHelp();
        return 0;
    }

    int offset = 0;
    bool verbose = false;
    if (cmdOptionExists(argv, argv + argc, "-v")) {
        verbose = true;
        offset++;
    }
    bool octave = false;
    if (cmdOptionExists(argv, argv + argc, "-o")) {
        octave = true;
        offset++;
    }
    char* keyword = nullptr;
    bool is_int = false;
    if (cmdOptionExists(argv, argv + argc, "-ik")) {
        keyword = getCmdOption(argv, argv + argc, "-ik");
        offset += 2;
        is_int = true;
    } else if (cmdOptionExists(argv, argv + argc, "-dk")) {
        keyword = getCmdOption(argv, argv + argc, "-dk");
        offset += 2;
    }

    loadDeck(argv[1 + offset], verbose, octave, is_int, keyword);

    return 0;
}

