/*
 * notifier.h
 *
 * Zeke Reyna
 */
#include <map>
#include <atomic>
#include <memory>
#include <iostream>

class Notifier {
	public:
		void update(std::string channel);

		std::shared_ptr<std::atomic<bool>> pass_ptr(std::string channel);

		void give_state();

	private:
		std::map<std::string, std::shared_ptr<std::atomic<bool>>> _map;

};
