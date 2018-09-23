//
// Created by zeke on 9/22/18.
//

#ifndef CHIT_CPP_DBPOOL_H
#define CHIT_CPP_DBPOOL_H

#include <pqxx/pqxx>

#include <mutex>
#include <condition_variable>
#include <deque>

// @TODO: unique ptr or smart ptr?
//using ConnectPtr = std::unique_ptr<pqxx::lazyconnection>;
//using ConnectPtr = std::unique_ptr<pqxx::connection>;

class DbPool {
public:
    DbPool();
    DbPool(const std::size_t numConnections);
    ~DbPool();

    void selfDestruct();

    ConnectPtr pop();
    //pqxx::lazyconnection pop();

    void push(ConnectPtr&& connectPtr);
    //void push(const )

private:
    std::mutex _mutex;
    std::condition_variable _cond;
    std::deque<ConnectPtr> _queue;
};


#endif //CHIT_CPP_DBPOOL_H
