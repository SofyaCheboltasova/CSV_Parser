#pragma once
#include <sstream>
#include <fstream>
#include "Tuple.h"

template<class Head, class... Tail>
struct ReadTuple {
    static std::tuple<Head, Tail...> read(std::istringstream &inputLine, size_t columnNumber = 1, size_t lineNumber = 1) {
        if (inputLine.eof())
            throw std::invalid_argument("Wrong delimeter was given!"); // ',' -> '.'

        Head string;
        inputLine >> string;
        return std::tuple_cat(std::make_tuple(string), ReadTuple<Tail...>::read(inputLine, columnNumber + 1, lineNumber));
    }
};

template<class Head>
struct ReadTuple<Head> {
    static std::tuple<Head> read(std::istringstream &inputStream, size_t columnNumber, size_t lineNumber) {
        if (inputStream.eof())
            throw std::invalid_argument("Not enough data in line: " + std::to_string(lineNumber) + " in column: " + std::to_string(columnNumber));

        Head string;
        inputStream >> string;
        if (inputStream.fail())
            throw std::invalid_argument("Wrong argument type in line: " + std::to_string(lineNumber) + " in column: " + std::to_string(columnNumber));

        return std::make_tuple(string);
    }
};

template<class... Args>
class CSVParser {
private:
    int args;
    size_t skip;
    std::ifstream& input;
    char columnDelim;
    char lineDelim;
    char Shielding;
    size_t lineNumber;
    std::streampos fileStart;

public:
    CSVParser(std::ifstream& file, size_t skip_, char cDelim, char lDelim, char shielding) : input(file) {
        skip = skip_;
        args = sizeof...(Args);
        columnDelim = cDelim;
        lineDelim = lDelim;
        Shielding = shielding;
        lineNumber = 0;
        for (size_t i = 0; i < skip; ++i) {
            std::string skippedLine;
            std::getline(input, skippedLine, lineDelim);

            if(skippedLine.empty())
                throw std::runtime_error("Wrong number of lines to skip!");

            skippedLine.clear();
        }
        fileStart = input.tellg();
    }

    class Iterator {
    private:
        CSVParser CSVparser;
        std::tuple<Args...> tupleIterator;
        bool lastIterator;
        std::streampos currentPos;

        void ConvertLine() {
            if (CSVparser.input.eof()) {
                lastIterator = true;
                return;
            }
            CSVparser.input.clear();
            CSVparser.input.seekg(currentPos);
            std::string currentLine;

            CSVparser.lineNumber++;
            bool hasShielding = false;
            char symb;
            size_t columnCounter = 0;

            while(CSVparser.input.get(symb)) {
                bool ignoreSymbol = false;
                if (symb == ' ') {
                    currentLine.push_back('\0');
                    ignoreSymbol = true;
                }
                if (symb == CSVparser.Shielding) {
                    ignoreSymbol = true;
                    hasShielding = !hasShielding;
                }
                if (symb == CSVparser.columnDelim && !hasShielding) {
                    currentLine.push_back(' ');
                    columnCounter++;

                    if(columnCounter > CSVparser.args - 1)
                        throw std::invalid_argument("In line: " + std::to_string(CSVparser.lineNumber) +
                        " more than " +  std::to_string(CSVparser.args) + " elements");

                    ignoreSymbol = true;
                }
                if(symb == CSVparser.lineDelim && !hasShielding) {
                    break;
                }
                if(!ignoreSymbol)
                    currentLine.push_back(symb);
            }

            if(hasShielding)
                throw std::invalid_argument("In line: " + std::to_string(CSVparser.lineNumber) + " shielding '" + CSVparser.Shielding + "' doesn't close");

            if(currentLine.empty())
                throw std::invalid_argument("Line: " + std::to_string(CSVparser.lineNumber) + " is an empty line");

            currentPos = CSVparser.input.tellg();
            std::istringstream lineParser{currentLine};

            tupleIterator = ReadTuple<Args...>::read(lineParser, 1, CSVparser.lineNumber);
        }

    public:
        Iterator(CSVParser parser, bool isLastIterator) : CSVparser(parser) {
            lastIterator = isLastIterator;
            currentPos = parser.fileStart;
            if (isLastIterator)
                return;
            else ConvertLine();
        }

        bool operator!=(const Iterator& other) const {
            return lastIterator != other.lastIterator;
        }

        Iterator operator++(){
            ConvertLine();
            return *this;
        }

        std::tuple<Args...> operator*() const {
            return tupleIterator;
        }

        ~Iterator() = default;
    };

    Iterator begin() {
        input.clear();
        input.seekg(fileStart);
        lineNumber = skip;
        return Iterator(*this, false);
    }

    Iterator end() {
        return Iterator(*this, true);
    }
};