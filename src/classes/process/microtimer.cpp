#include "microtimer.h"
#include <chrono>
#include <QDebug>

using namespace std::chrono;

/**
 * @brief microtimer::microtimer
 * creates a microtimer object and starts the internal microtimer clock, setting timepoint 1 to current internal time
 * this class is mainly intended for debugging use, to find which functions/parts of take longest to run
 */
microtimer::microtimer()
{
	t1 = high_resolution_clock::now();

}
/**
 * @brief microtimer::start
 * starts the internal microtimer clock, setting timepoint 1 to current internal time
 * not required on every usage; constructor also initialises clock
 */
void microtimer::start(){
	t1 = high_resolution_clock::now();

}

/**
 * @brief microtimer::end
 * ends the internal microtimer clock, setting timepoint 2 to current internal time
 * outputs the time since t1 was last set (microseconds)
 * microtimer::start is not required to be used again before doing ::end, it will simply output time since first start
 */
void microtimer::end(){
	t2 = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>( t2 - t1 ).count();

	qDebug() << "microtimer end: " << duration << endl;
}

qint64 microtimer::runninginterval(){
	t2 = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>( t2 - t1 ).count();
	return duration;
}
