/*
 * notifier.cpp
 *
 * Zeke Reyna
 */

#include "notifier.h"

void Notifier::update(std::string channel)
{
	if (_map.count(channel)) {
		auto& atom_ptr = _map[channel];
		*atom_ptr = true;
	} else {
		_map[channel] = std::make_shared<std::atomic<bool>>();
		auto& atom_ptr = _map[channel];
		*atom_ptr = true;
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

/*
int main()
{
	std::cout << "It's working?\n";
	Notifier notif;
	std::string channel = "blah";
	notif.update(channel);
	notif.give_state();

	std::shared_ptr<std::atomic<bool>> s_ptr = notif.pass_ptr(channel);
	*s_ptr = false;

	notif.give_state();
	return 0;
}
*/
