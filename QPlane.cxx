#include "QPlane.h"

#include <vtkActor.h>
#include <vtkCollectionIterator.h>
#include <vtkImplicitBoolean.h>
#include <vtkImplicitFunctionCollection.h>
#include <vtkMapper.h>
#include <vtkMath.h>
#include <vtkObject.h>
#include <vtkPlane.h>
#include "vtkPlaneExtend.h"
#include <vtkPlaneSource.h>
#include <vtkPlaneWidget.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>

QPlane::QPlane(QObject *parent) : QObject(parent)
{
	this->planeSource = vtkSmartPointer<vtkPlaneSource>::New();
	this->mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	this->actor = vtkSmartPointer<vtkActor>::New();
	this->plane = vtkSmartPointer<vtkPlane>::New();
}

QPlane::~QPlane()
{
}

void QPlane::setPlane(vtkPlaneWidget *planeWidget)
{
	//QString sFRE;
	//sFRE.sprintf("FRE=%.3f, %.3f, %.3f", planeWidget->GetOrigin()[0], planeWidget->GetOrigin()[1], planeWidget->GetOrigin()[2]);
	//QMessageBox::information(0, tr("Information"), sFRE);
	this->planeSource->SetOrigin(planeWidget->GetOrigin());
	this->planeSource->SetPoint1(planeWidget->GetPoint1());
	this->planeSource->SetPoint2(planeWidget->GetPoint2());
}

void QPlane::setPlane(vtkPlaneExtend *planeExtend)
{
	this->planeSource->SetOrigin(planeExtend->GetOrigin());
	this->planeSource->SetPoint1(planeExtend->GetPoint1());
	this->planeSource->SetPoint2(planeExtend->GetPoint2());
}

vtkSmartPointer<vtkPlaneSource> QPlane::getPlaneSource()
{
	return this->planeSource;
}

vtkSmartPointer<vtkPlane> QPlane::getPlane()
{
	this->plane->SetNormal(this->planeSource->GetNormal());
	this->plane->SetOrigin(this->planeSource->GetOrigin());
	return this->plane;
}

vtkSmartPointer<vtkActor> QPlane::getActor()
{
	this->mapper->SetInputConnection(this->planeSource->GetOutputPort());
	this->actor->SetMapper(this->mapper);
	this->actor->GetProperty()->SetColor(0, 0, 1);
	return this->actor;
}
