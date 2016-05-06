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

#include <opm/parser/eclipse/Applications/OctaveOutput.hpp>

using namespace Opm;

const std::string STARTLINE = "%%%---\t";

std::shared_ptr<const EclipseGrid> parseGrid(const char * deck_file, bool verbose = false) {
    try {
        return Parser::parseGrid(deck_file);
    } catch (std::logic_error &) {
        if (verbose)
            std::cout << STARTLINE << "Parsing full grid failed ... will construct lean grid" << std::endl;
        try {
            return Parser::parseGrid(deck_file, ParseContext().withKey(ParseContext::PARSE_MISSING_SECTIONS));
        } catch (std::logic_error &) {
            if (verbose)
                std::cout << STARTLINE << "Parsing lean grid failed ... will construct slim grid" << std::endl;
            Parser p;
            auto deck = p.parseFile(deck_file, ParseContext());
            return std::make_shared<const EclipseGrid>(*deck);
        }
    }
}

EclipseState parse(std::string fileName, bool verbose = false) {

    std::cout << STARTLINE << "READING FILE \"" << fileName << "\"." << std::flush << std::endl;

    ParseContext parseContext;

    {
        parseContext = parseContext.withKey(ParseContext::PARSE_MISSING_DIMS_KEYWORD)
                                   .withKey(ParseContext::PARSE_RANDOM_SLASH)
                                   .withKey(ParseContext::PARSE_RANDOM_TEXT)
                                   .withKey(ParseContext::PARSE_UNKNOWN_KEYWORD)
                                   .withKey(ParseContext::PARSE_EXTRA_DATA)
                                   .withKey(ParseContext::PARSE_MISSING_SECTIONS);
    }

    Parser parser;

    if (verbose) {
        std::cout << STARTLINE << "Loading deck: " << fileName << " ... ";
    }

    DeckPtr deck = parser.parseFile(fileName, ParseContext());

    if (verbose) {
        std::cout << "parse complete" << std::endl;
    }
    if (verbose) {
        std::cout << STARTLINE << "Creating EclipseState ... ";
    }
    EclipseState state(*deck, ParseContext());
    if (verbose)
        std::cout << "done." << std::endl;

    return state;
}

char* getCmdOption(char ** begin, char ** end, const std::string & option) {
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end) {
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
    std::cout << "  -kw        double keyword info" << std::endl;

    std::cout << "Examples: opmi -v ECL.DATA" << std::endl;
    std::cout << "          opmi -o -a ECL.DATA" << std::endl;
    std::cout << "          opmi -o -kw SATNUM ECL.DATA" << std::endl;
    std::cout << "          opmi -o -kw ACTNUM ECL.DATA" << std::endl;
    std::cout << "          opmi -o -kw PORO ECL.DATA" << std::endl;
    std::cout << "          bin/opmi -v -o -kw PERMX ../testdata/integration_tests/IOConfig/SPE1CASE2.DATA > permx.m" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "The latter creates a file permx.m.  Open octave and write \"permx\" [enter]." << std::endl;

}

int main(int argc, char** argv) {
    if (argc == 0) {
        // legacy behavior
        try {
            parseGrid(argv[0]);
        } catch (std::logic_error &) {
            return 1;
        }
        return 0;
    }

    if (cmdOptionExists(argv, argv + argc, "-h") || argc == 0) {
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
    if (!octave) {
        std::cout << STARTLINE << "No octave output.  Goodbye." << std::flush << std::endl;
        return 0;
    }

    char* keyword = nullptr;
    if (cmdOptionExists(argv, argv + argc, "-kw")) {
        keyword = getCmdOption(argv, argv + argc, "-kw");
        offset += 2;
    }

    std::string fileName(argv[1 + offset]);
    try {
        auto state = parse(fileName, verbose);
        OctaveOutput oo(state);
        oo.octave(keyword);
    } catch (std::exception& err) {
        std::cout << STARTLINE << "Error parsing " << fileName << ".  Is it a full deck?" << std::endl;
        std::cout << STARTLINE << err.what() << std::endl;
    } catch (std::string& err) {
        std::cout << STARTLINE << "Error parsing " << fileName << ".  Is it a full deck?" << std::endl;
        std::cout << STARTLINE << err << std::endl;
    } catch (...) {
        std::cout << STARTLINE << "Error parsing " << fileName << ".  Is it a full deck?" << std::endl;
    }

    return 0;
}
