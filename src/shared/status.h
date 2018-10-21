//
// Created by zeke on 10/19/18.
//

#ifndef CHIT_CPP_STATUS_H
#define CHIT_CPP_STATUS_H

#include <string>
#include <stdexcept>

enum class Status {
    Admin,
    User,
    Banned,
    Nonexistent
};

template <class T>
inline T& operator << (T& stream, const Status statusEnum) {
    switch (statusEnum) {
        case Status::Admin:  stream << "Admin";  break;
        case Status::User:   stream << "User";   break;
        case Status::Banned: stream << "Banned"; break;
        case Status::Nonexistent: stream << "Nonexistent"; break;
        default: break;
    }
    return stream;
}

inline std::string getStatusString(const Status statusEnum) {
    std::string statusString;
    switch (statusEnum) {
        case Status::Admin:  statusString = "Admin";  break;
        case Status::User:   statusString = "User";   break;
        case Status::Banned: statusString = "Banned"; break;
        case Status::Nonexistent: statusString = "Nonexistent"; break;
        default: break;
    }
    return statusString;
}

inline Status getStatusEnum(const std::string statusString) {
    Status statusEnum;
    if (statusString == "Admin") {
        statusEnum = Status::Admin;
    } else if (statusString == "User") {
        statusEnum = Status::User;
    } else if (statusString == "Banned") {
        statusEnum = Status::Banned;
    } else if (statusString == "Nonexistent") {
        statusEnum = Status::Nonexistent;
    } else {
        throw std::runtime_error("don't recognize status string");
    }
    return statusEnum;
}

#endif //CHIT_CPP_STATUS_H
