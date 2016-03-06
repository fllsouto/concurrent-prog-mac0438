#include <iostream>
#include <thread>

static const int num_threads = 10;

void call_from_thread(int tid) {
	std::cout << "Lauched by thread " << tid << std::endl;
}

int main() {
	std::thread t[num_threads];

	//Launch a group of threads
	for (int i = 0; i < num_threads; ++i) {
		t[i] = std::thread(call_from_thread, i);
	}

	//Join the threads with the main thread
	for (int i = 0; i < num_threads; ++i) {
		t[i].join();
	}

	return 0;
}