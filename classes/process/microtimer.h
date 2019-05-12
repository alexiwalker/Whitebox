#ifndef MICROTIMER_H
#define MICROTIMER_H
#include <chrono>
#include <QIntegerForSize>
using namespace std::chrono;
class microtimer
	{
public:
	microtimer();
	void start();
	void end();
	high_resolution_clock::time_point t1;
	high_resolution_clock::time_point t2;
	qint64 runninginterval();

	};




#endif // MICROTIMER_H
