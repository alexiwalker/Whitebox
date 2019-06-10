#ifndef FLOW_GRID_LAYOUT_H
#define FLOW_GRID_LAYOUT_H

#include <QObject>
#include <QWidget>
#include <QGridLayout>

class flow_grid_layout : public QGridLayout
	{
public:
	flow_grid_layout();
	void addItem(QWidget* widget);

private:
	bool event(QEvent* event);
	void resize(QEvent* event);
	QWidgetList itemList();

	};

#endif // FLOW_GRID_LAYOUT_H
