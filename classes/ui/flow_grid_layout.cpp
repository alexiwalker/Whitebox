#include "flow_grid_layout.h"
#include <QDebug>
#include <QGridLayout>
#include <QEvent>


flow_grid_layout::flow_grid_layout()
{

}


bool flow_grid_layout::event (QEvent* event){
	qDebug() << event << endl;
	if(event->type() == QEvent::Resize){
	 resize (event);
	 return true;
	}

	return false;
}

void flow_grid_layout::addItem(QWidget *widget){

}


void flow_grid_layout::resize(QEvent *event){

}
