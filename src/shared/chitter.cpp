//
// Created by zeke on 8/22/18.
//

#include <iostream>
#include "chitter.h"

int main(int argc, char **argv) {
    try {
        pqxx::connection c;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
