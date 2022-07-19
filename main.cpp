#include "CSVParser.h"

int main() {
    std::ifstream file;
    file.open("C:/Users/sofya/CLionProjects/lab4c++/test.csv");
    if (file.fail()) {
        std::cout << "Uploaded file couldn't be opened" << std::endl;
        return 0;
    }

    try {
        CSVParser<std::string, int, std::string, double, std::string> CSVparser(file, 0, ',', '\n', '"');
        for (auto i: CSVparser) {
            std::cout << i << std::endl;
        }
    }
    catch (std::exception &exception) {
        std::cout << exception.what() << std::endl;
    }

    file.close();
    return 0;
}

