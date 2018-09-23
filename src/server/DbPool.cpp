//
// Created by zeke on 9/22/18.
//

#include "DbPool.h"
#include <chitter.h>

DbPool::DbPool() {
    //@TODO: choose a more reasonable number?
    const std::size_t NUM_CONNECTIONS = 6;
    for (std::size_t i = 0; i < NUM_CONNECTIONS; i++) {
        //pqxx::connection connection = chitter::initiate("../shared/config");
        //ConnectPtr connectPtr = std::make_unique<pqxx::connection>(std::move(connection));
        ConnectPtr connectPtr = std::make_unique<pqxx::connection>(chitter::initiate("../shared/config"));
        //_queue.emplace_back(std::move(connectPtr));
        //_queue.push_back(std::move(std::make_unique<pqxx::connection>(
         //       std::move(chitter::initiate("../shared/config")))));
    }
}

DbPool::DbPool(const std::size_t numConnections) {
    for (std::size_t i = 0; i < numConnections; i++) {
        //_queue.push_back(std::make_unique<pqxx::connection>(
         //       chitter::initiate("../shared/config")));
    }
}

DbPool::~DbPool() {
    selfDestruct();
}

ConnectPtr DbPool::pop() {
    // wait() expects a unique_lock rather than lock_guard
    std::unique_lock<std::mutex> lock(_mutex);
    //@TODO: give optional time out, that if runs out, need to throw
    // error or something.
    _cond.wait(lock, [&q = _queue]{ return !q.empty();});
    ConnectPtr connectPtr (std::move(_queue.back()));
    _queue.pop_back();
    return connectPtr;
}

void DbPool::push(ConnectPtr&& connectPtr) {
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.emplace_back(std::move(connectPtr));
    }
    _cond.notify_one();
}

void DbPool::selfDestruct() {
    // put null ptrs into queue to let threads now they should self destruct
    std::lock_guard<std::mutex> lock(_mutex);
    _queue.clear();

    //@TODO: somehow need to get list of threads that are using the queue...
}

int main() {
    DbPool dbPool;
    ConnectPtr connectPtr = dbPool.pop();
    std::cout << chitter::getBio("blah", *connectPtr) << std::endl;
    return 0;
}
