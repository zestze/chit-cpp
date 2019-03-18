/*
 * notifier.cpp
 *
 * Zeke Reyna
 */

#include "Notifier.h"

void Notifier::update(std::string channel)
{
    if (auto iter = _map.find(channel); iter != _map.end()) {
        iter->second->store(true);
    } else {
        auto atom_ptr = std::make_shared<std::atomic<bool>>(true);
        _map[channel] = atom_ptr;
    }
}

std::shared_ptr<std::atomic<bool>> Notifier::pass_ptr(std::string channel)
{
	return _map[channel];
}

void Notifier::give_state()
{
	for (auto& it : _map) {
		std::cout << it.first << "\n";
		std::cout << *it.second << "\n";
	}
}
