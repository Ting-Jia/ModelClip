#ifndef __QPLANE_H
#define __QPLANE_H

#include <QObject>

#include <vtkActor.h>
#include <vtkImplicitBoolean.h>
#include <vtkMapper.h>
#include <vtkPlane.h>
#include "vtkPlaneExtend.h"
#include <vtkPlaneSource.h>
#include <vtkPlaneWidget.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>

class QPlane : public QObject
{
	Q_OBJECT

public:
	QPlane(QObject *parent = 0);
	virtual ~QPlane();

	void setPlane(vtkPlaneWidget*);
	void setPlane(vtkPlaneExtend*);
	vtkSmartPointer<vtkPlaneSource> getPlaneSource();
	vtkSmartPointer<vtkPlane> getPlane();
	vtkSmartPointer<vtkActor> getActor();

protected:
	vtkSmartPointer<vtkPlaneSource> planeSource;
	vtkSmartPointer<vtkPolyDataMapper> mapper;
	vtkSmartPointer<vtkActor> actor;
	vtkSmartPointer<vtkPlane> plane;
};

#endif