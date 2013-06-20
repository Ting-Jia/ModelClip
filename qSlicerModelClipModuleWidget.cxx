/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

//	MRML includes
#include <vtkMRMLModelDisplayNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLModelStorageNode.h>
#include <vtkMRMLNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLViewNode.h>

// Qt includes
#include <QDebug>
#include <QList>
#include <QMessageBox>
#include "QPlane.h"
#include <QString>

// SlicerQt includes
#include <qMRMLThreeDView.h>
#include <qMRMLThreeDWidget.h>
#include <qMRMLThreeDViewControllerWidget.h>
#include <qSlicerApplication.h>
#include <qSlicerIOManager.h>
#include <qSlicerLayoutManager.h>
#include "qSlicerModelClipModuleWidget.h"
#include "ui_qSlicerModelClipModuleWidget.h"

//	vtk includes
#include <vtkActor.h>
#include <vtkClipPolyData.h>
#include <vtkImplicitBoolean.h>
#include <vtkMath.h>
#include <vtkPlane.h>
#include <vtkPlaneCollection.h>
#include "vtkPlaneExtend.h"
#include <vtkPlaneSource.h>
#include <vtkPlaneWidget.h>
#include <vtkPlaneWidgetSub.h>
#include <vtkPolyData.h>
#include <vtkPolyLine.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerModelClipModuleWidgetPrivate: public Ui_qSlicerModelClipModuleWidget
{
public:
  qSlicerModelClipModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerModelClipModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerModelClipModuleWidgetPrivate::qSlicerModelClipModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerModelClipModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerModelClipModuleWidget::qSlicerModelClipModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerModelClipModuleWidgetPrivate )
{
  //  return the reference to the renderWindow of the Slicer application singleton 
  //  and those to its renderer and renderWindowInteractor
  qSlicerApplication *app = qSlicerApplication::application();
  qSlicerLayoutManager *layoutManager = app->layoutManager();
  qMRMLThreeDWidget *threeDWidget = layoutManager->threeDWidget(0);
  qMRMLThreeDView *threeDView = threeDWidget->threeDView();
  this->renderWindow = threeDView->renderWindow();
  this->renderer = this->renderWindow->GetRenderers()->GetFirstRenderer();
  this->iren = this->renderWindow->GetInteractor();

  this->planeWidget = vtkSmartPointer<vtkPlaneWidgetSub>::New();
  this->planeWidget->SetInteractor(this->iren);
  this->planeWidget->Off();

  this->planeExtend = vtkSmartPointer<vtkPlaneExtend>::New();
  this->planeExtend->SetInteractor(this->iren);
  this->planeExtend->Off();

  this->depthPlane = vtkSmartPointer<vtkPlaneWidget>::New();
  this->depthPlane->SetInteractor(this->iren);
  this->depthPlane->Off();

	this->clipper = vtkSmartPointer<vtkClipPolyData>::New();
	this->clipper->GenerateClippedOutputOn();
	this->sourcePD = vtkSmartPointer<vtkPolyData>::New();

	this->numOfPlanes = 0;
	this->timeOfClip = 0;
  this->isClipped = false;
}

//-----------------------------------------------------------------------------
qSlicerModelClipModuleWidget::~qSlicerModelClipModuleWidget()
{
	for (int i = 0; i < this->numOfPlanes; i++) {
		delete this->planeList.at(i);
	}
}

void qSlicerModelClipModuleWidget::createPlane()
{
	Q_D(qSlicerModelClipModuleWidget);

	this->planeWidget->SetOrigin(100, -100, 0);
	this->planeWidget->SetPoint1(100, 100, 0);
	this->planeWidget->SetPoint2(50, -100, 0);
	this->planeWidget->SetRepresentationToSurface();
  this->planeWidget->SetHandleSize(0.01);
	//this->renderer->ResetCamera();
	this->planeWidget->PlaceWidget();

  this->iren->Initialize();
	this->planeWidget->On();
  this->planeWidget->SetHandleHint();
	this->renderWindow->Render();
	this->iren->Start();

	d->createButton->setEnabled(false);
	d->appendButton->setEnabled(true);
	d->removeButton->setEnabled(true);
	d->hideButton->setEnabled(true);
  d->deleteButton->setEnabled(true);
  d->depthButton->setEnabled(true);
  d->clipButton->setEnabled(!this->isClipped);
}

void qSlicerModelClipModuleWidget::appendPlane()
{
	QPlane *qPlane = new QPlane();

  //  choose which plane widget to use
	if (this->numOfPlanes == 0) {
		qPlane->setPlane(this->planeWidget);
		this->planeWidget->Off();
	} else {
		qPlane->setPlane(this->planeExtend);
	}
	this->planeList.append(qPlane);

  //  take the plane into the renderer
	this->renderer->AddActor(this->planeList.last()->getActor());

  //  set the properties of the next plane
	this->planeExtend->SetOrigin(this->planeList.last()->getPlaneSource()->GetPoint2());
	this->planeExtend->SetPoint1(this->getPoint3(this->numOfPlanes));
	this->planeExtend->SetPoint2(this->makePoint2(this->numOfPlanes));
	this->planeExtend->SetHandleSize(0.01);
	this->planeExtend->SetRepresentationToSurface();
	this->planeExtend->PlaceWidget();

	this->iren->Initialize();
	this->planeExtend->On();
	//this->renderer->ResetCamera();
	this->renderWindow->Render();
	this->iren->Start();
	this->numOfPlanes++;  //  add the planes count by one
}

void qSlicerModelClipModuleWidget::removePlane()
{
	Q_D(qSlicerModelClipModuleWidget);

  //  clear the bodyList if we just operated a clip 
  if (this->isClipped) {
    this->clearBodyList();
    this->isClipped = false;
    d->clipButton->setEnabled(!this->isClipped);
  }

  //  decide the way to remove the last plane according to its position
  switch (this->numOfPlanes) {
    case 0:
      this->planeWidget->Off();
      d->createButton->setEnabled(true);
      d->appendButton->setEnabled(false);
      d->removeButton->setEnabled(false);
      d->hideButton->setEnabled(false);
      d->deleteButton->setEnabled(false);
      d->depthButton->setEnabled(false);
      d->clipButton->setEnabled(false);
      break;
    case 1:
      this->planeWidget->On();
      this->planeWidget->SetHandleHint();
      this->planeExtend->Off();
      this->renderer->RemoveActor(this->planeList.last()->getActor());
      delete this->planeList.last();
      this->planeList.removeLast();
      this->numOfPlanes--;
      d->appendButton->setEnabled(true);
      break;
    default:
      this->planeWidget->Off();
      this->renderer->RemoveActor(this->planeList.last()->getActor());
      double *o = new double[3];
      double *pt1 = new double[3];
      double *pt2 = new double[3];
      o = this->planeList.last()->getPlaneSource()->GetOrigin();
      pt1 = this->planeList.last()->getPlaneSource()->GetPoint1();
      pt2 = this->planeList.last()->getPlaneSource()->GetPoint2();
      delete this->planeList.last();
      this->planeList.removeLast();
      int index = this->planeList.indexOf(this->planeList.last());
      this->planeExtend->SetOrigin(o);
      this->planeExtend->SetPoint1(pt1);
      this->planeExtend->SetPoint2(pt2);
      this->planeExtend->On();
      this->numOfPlanes--;
      d->appendButton->setEnabled(true);
  }
  this->renderWindow->Render();
}

void qSlicerModelClipModuleWidget::hideSurface()
{
  Q_D(qSlicerModelClipModuleWidget);

  this->renderWindow->Render();
  this->iren->Initialize();
  if (d->hideButton->text() == tr("Hide the Planes")) {
    for (int i = 0; i < this->numOfPlanes; i++) {
      this->renderer->RemoveActor(this->planeList.at(i)->getActor());
    }
    if (d->depthButton->text() == tr("Remove Clip Depth")) {
      this->depthPlane->Off();
    }
    this->planeWidget->Off();
    this->planeExtend->Off();
    d->appendButton->setEnabled(false);
    d->removeButton->setEnabled(false);
    d->hideButton->setText(tr("Show the Planes"));
    d->deleteButton->setEnabled(false);
    d->clipButton->setEnabled(false);
    d->depthButton->setEnabled(false);
  } else {
    for (int i = 0; i < this->numOfPlanes; i++) {
      this->renderer->AddActor(this->planeList.at(i)->getActor());
    }
    if (d->depthButton->text() == tr("Remove Clip Depth")) {
      this->depthPlane->On();
    }
    if (!this->isClipped) {
      if (this->numOfPlanes == 0) {
        this->planeWidget->On();
        this->planeWidget->SetHandleHint();
      } else {
        this->planeExtend->On();
      }
    }
    d->appendButton->setEnabled(!this->isClipped);
    d->removeButton->setEnabled(true);
    d->hideButton->setText(tr("Hide the Planes"));
    d->deleteButton->setEnabled(true);
    d->clipButton->setEnabled(!this->isClipped);
    d->depthButton->setEnabled(true);
  }
  //this->renderer->ResetCamera();
  this->renderWindow->Render();
  this->iren->Start();
}

void qSlicerModelClipModuleWidget::deleteSurface()
{
	Q_D(qSlicerModelClipModuleWidget);

	for (int i = 0; i < this->numOfPlanes; i++) {
		this->renderer->RemoveActor(this->planeList.at(i)->getActor());
		delete this->planeList.at(i);
	}
	for (int i = 0; i < this->numOfPlanes; i++) {
		this->planeList.removeFirst();
	}
  this->clearBodyList();

	this->planeWidget->Off();
	this->planeExtend->Off();
  this->depthPlane->Off();
	this->renderWindow->Render();
	this->numOfPlanes = 0;

  this->isClipped = false;
	d->createButton->setEnabled(true);
	d->appendButton->setEnabled(false);
	d->removeButton->setEnabled(false);
	d->hideButton->setEnabled(false);
	d->deleteButton->setEnabled(false);
  d->depthButton->setEnabled(false);
	d->clipButton->setEnabled(false);
}

void qSlicerModelClipModuleWidget::setDepth()
{
  Q_D(qSlicerModelClipModuleWidget);

  if (d->depthButton->text() == tr("Set Clip Depth")) {
    double *oP1 = new double[3];
    double *oP2 = new double[3];
    double *pt1 = new double[3];
    double *axis = new double[3];
    double *axisNorm = new double[3];
    vtkMath::Subtract(this->planeWidget->GetPoint2(), 
      this->planeWidget->GetOrigin(), oP2);
    vtkMath::Subtract(this->planeWidget->GetPoint1(), 
      this->planeWidget->GetOrigin(), axis);
    vtkMath::Normalize(axis);
    vtkMath::Cross(axis, oP2, oP1);
    vtkMath::Add(this->planeWidget->GetOrigin(), oP1, pt1);
    this->depthPlane->SetOrigin(this->planeWidget->GetOrigin());
    this->depthPlane->SetPoint1(pt1);
    this->depthPlane->SetPoint2(this->planeWidget->GetPoint2());
    this->depthPlane->On();
    this->depthPlane->SetHandleSize(0.05);
    //this->depthPlane->SetPlaceFactor(1.0f);
    this->depthPlane->SetRepresentationToSurface();
    this->depthPlane->PlaceWidget();
    d->depthButton->setText(tr("Remove Clip Depth"));
    this->iren->Initialize();
	  this->renderWindow->Render();
	  this->iren->Start();
  } else {
    this->depthPlane->Off();
    d->depthButton->setText(tr("Set Clip Depth"));
    this->renderWindow->Render();
  }
}

void qSlicerModelClipModuleWidget::clip()
{
	Q_D(qSlicerModelClipModuleWidget);

  this->appendPlane();  //  count in the plane which the planeExtend occupies
  this->planeExtend->Off();
	this->planeWidget->Off();
	this->renderWindow->Render();
	this->timeOfClip++; //  add the clip count by one
  this->isClipped = true;
	d->appendButton->setEnabled(false);
	//d->removeButton->setEnabled(false);
	d->deleteButton->setEnabled(true);
  d->clipButton->setEnabled(!this->isClipped);
	this->numOfPlanes = this->planeList.count();

	//	obtain the source model for clipping
  qSlicerApplication *app = qSlicerApplication::application();
	vtkMRMLScene *mrmlScene = app->mrmlScene();
	QString nodeID;
	char *charNodeID;
	nodeID = d->clipNodeComboBox->currentNodeId();
	charNodeID = nodeID.toLatin1().data();
	vtkMRMLNode *sourceNode = mrmlScene->GetNodeByID(charNodeID);
	vtkSmartPointer<vtkMRMLModelNode> sourceModel =
		vtkSmartPointer<vtkMRMLModelNode>::New();
	sourceModel->Copy(sourceNode);
	this->sourcePD->DeepCopy(sourceModel->GetPolyData());
	this->clipper->SetInput(this->sourcePD);

	//	make the clipping surface using bool operation and set it as the clip
  //  function of the clipper - core part
	if (this->numOfPlanes == 1) {
    //  simply use the lone plane if there is exactly only one plane
    this->clipper->SetClipFunction(this->withDepth(this->planeList.at(0)->getPlane()));
	} else {
    //  when clipping using more than one plane
    this->angleList.append(this->makeAngle(1));
		this->bodyList.append(this->angleList.at(0));
		if (this->numOfPlanes == 2) {
      //  just use the triangular space to clip if there are exactly two planes
      this->clipper->SetClipFunction(this->withDepth(this->bodyList.at(0)));  //  this->bodyList.at(0) == this->angleList.at(0)
		} else if (this->numOfPlanes > 2) {
			for (int i = 2; i < this->numOfPlanes; i++) {
        //  make the clip space step by step
        this->bodyList.append(this->makeBody(i));
			}
      this->clipper->SetClipFunction(this->withDepth(this->bodyList.last()));
		}
	}

	//	the most exciting moment
	this->clipper->Update();

	//	get the output and the clipped output
  vtkSmartPointer<vtkPolyData> resultPD = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPolyData> clippedPD = vtkSmartPointer<vtkPolyData>::New();
	resultPD->DeepCopy(this->clipper->GetOutput());
	this->resultList.append(resultPD);
	clippedPD->DeepCopy(this->clipper->GetClippedOutput());
	this->clippedList.append(clippedPD);

	//	display and store the result model
	vtkSmartPointer<vtkMRMLModelNode> resultModel =
		vtkSmartPointer<vtkMRMLModelNode>::New();
	QString resultName = tr("Result Part_");
	resultName.append(QString::number(this->timeOfClip));
	resultModel->SetName(resultName.toLatin1().data());
	resultModel->SetAndObservePolyData(this->resultList.at(this->timeOfClip - 1));
	//mrmlScene->SaveStateForUndo();
	resultModel->SetScene(mrmlScene);

	vtkSmartPointer<vtkMRMLModelDisplayNode> resultDisplay =
		vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
	vtkSmartPointer<vtkMRMLModelStorageNode> resultStorage =
		vtkSmartPointer<vtkMRMLModelStorageNode>::New();
	resultDisplay->SetScene(mrmlScene);
	resultStorage->SetScene(mrmlScene);
	resultDisplay->SetInputPolyData(resultModel->GetPolyData());
	resultDisplay->SetColor(1.0, 0.0, 0.0);
	resultStorage->SetFileName(resultName.toLatin1().data());
	mrmlScene->AddNode(resultDisplay);
	mrmlScene->AddNode(resultStorage);
	resultModel->SetAndObserveDisplayNodeID(resultDisplay->GetID());
	resultModel->SetAndObserveStorageNodeID(resultStorage->GetID());

	//	display and store the clipped model
	vtkSmartPointer<vtkMRMLModelNode> clippedModel =
		vtkSmartPointer<vtkMRMLModelNode>::New();
	QString clippedName = tr("Clipped Part_");
	clippedName.append(QString::number(this->timeOfClip));
	clippedModel->SetName(clippedName.toLatin1().data());
	clippedModel->SetAndObservePolyData(this->clippedList.at(this->timeOfClip - 1));
	//mrmlScene->SaveStateForUndo();
	clippedModel->SetScene(mrmlScene);

	vtkSmartPointer<vtkMRMLModelDisplayNode> clippedDisplay =
		vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
	vtkSmartPointer<vtkMRMLModelStorageNode> clippedStorage =
		vtkSmartPointer<vtkMRMLModelStorageNode>::New();
	clippedDisplay->SetScene(mrmlScene);
	clippedStorage->SetScene(mrmlScene);
	clippedDisplay->SetInputPolyData(clippedModel->GetPolyData());
	clippedDisplay->SetColor(0.0, 1.0, 0.0);
	clippedStorage->SetFileName(clippedName.toLatin1().data());
	mrmlScene->AddNode(clippedDisplay);
	mrmlScene->AddNode(clippedStorage);
	clippedModel->SetAndObserveDisplayNodeID(clippedDisplay->GetID());
	clippedModel->SetAndObserveStorageNodeID(clippedStorage->GetID());

	mrmlScene->AddNode(resultModel);
	mrmlScene->AddNode(clippedModel);
}

//-----------------------------------------------------------------------------
void qSlicerModelClipModuleWidget::setup()
{
  Q_D(qSlicerModelClipModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  QObject::connect(d->createButton, SIGNAL(clicked()), this, SLOT(createPlane()));
  QObject::connect(d->appendButton, SIGNAL(clicked()), this, SLOT(appendPlane()));
  QObject::connect(d->removeButton, SIGNAL(clicked()), this, SLOT(removePlane()));
  QObject::connect(d->hideButton, SIGNAL(clicked()), this, SLOT(hideSurface()));
  QObject::connect(d->deleteButton, SIGNAL(clicked()), this, SLOT(deleteSurface()));
  QObject::connect(d->depthButton, SIGNAL(clicked()), this, SLOT(setDepth()));
  QObject::connect(d->clipButton, SIGNAL(clicked()), this, SLOT(clip()));
}

double* qSlicerModelClipModuleWidget::getPoint3(int i)
{
	vtkPlaneSource *planeSource = this->planeList.at(i)->getPlaneSource();
	double *point1 = planeSource->GetPoint1();
	double *point2 = planeSource->GetPoint2();
	double *origin = planeSource->GetOrigin();
	double *point3 = new double[3];

	double *oP1 = new double[3];
	double *oP2 = new double[3];
	double *oP3 = new double[3];

	vtkMath::Subtract(point1, origin, oP1);
	vtkMath::Subtract(point2, origin, oP2);
	vtkMath::Add(oP1, oP2, oP3);
	vtkMath::Add(origin, oP3, point3);

	return point3;
}

double* qSlicerModelClipModuleWidget::makePoint2(int i)
{
	double *oP1 = new double[3];
	double *oP2 = new double[3];
	double *normal = this->planeList.at(i)->getPlaneSource()->GetNormal();
	double *o = this->planeExtend->GetOrigin();
	double *pt1 = this->planeExtend->GetPoint1();
	double *pt2 = new double[3];

	vtkMath::Subtract(pt1, o, oP1);
	vtkMath::Cross(normal, oP1, oP2);
	vtkMath::MultiplyScalar(oP2, 0.25);
	vtkMath::Add(o, oP2, pt2);

	return pt2;
}

double* qSlicerModelClipModuleWidget::lengthen(int i)
{
	double *pt2 = this->planeList.at(i)->getPlaneSource()->GetPoint2();
	double *o = this->planeList.at(i)->getPlaneSource()->GetOrigin();
	double *output = new double[3];
	double *vector = new double[3];
	if (i ==0) {
		vtkMath::Subtract(o, pt2, vector);
		vtkMath::MultiplyScalar(vector, 100);
		vtkMath::Add(vector, pt2, output);
	} else {
		vtkMath::Subtract(pt2, o, vector);
		vtkMath::MultiplyScalar(vector, 100);
		vtkMath::Add(vector, o, output);
	}
	return output;
}

double qSlicerModelClipModuleWidget::dotProduct(int i)
{
	return this->dotProduct(i, i - 1);
}

double qSlicerModelClipModuleWidget::dotProduct(int i, int j)
{
	double *oP2 = new double[3];
	vtkMath::Subtract(
		this->planeList.at(i)->getPlaneSource()->GetPoint2(),
		this->planeList.at(i)->getPlaneSource()->GetOrigin(),
		oP2);
	double dot = vtkMath::Dot(
		this->planeList.at(j)->getPlane()->GetNormal(),
		oP2);
	return dot;
}

bool qSlicerModelClipModuleWidget::isInter(int i)
{
	if (i == 2) {
		return false;
	}

	//	create a vtkPoints object and store the points in it
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	int numOfLines = i - 2;
	points->InsertNextPoint(this->planeList.at(0)->getPlaneSource()->GetOrigin());
	for (int j = 0; j < numOfLines; j++) {
		points->InsertNextPoint(this->planeList.at(j)->getPlaneSource()->GetPoint2());
	}

	//	create a vtkPolyLine object and store the polyline in it
	vtkSmartPointer<vtkPolyLine> polyLine = vtkSmartPointer<vtkPolyLine>::New();
	polyLine->GetPoints()->DeepCopy(points);
	polyLine->GetPointIds()->SetNumberOfIds(numOfLines + 1);
	for(int j = 0; j <= numOfLines; j++) {
		polyLine->GetPointIds()->SetId(j, j);
	}

	double tolerance = 0.001;
	double t;
	double x[3];
	double pcoords[3];
	int subId;
	int intersection = polyLine->IntersectWithLine(
		this->planeList.at(i - 1)->getPlaneSource()->GetOrigin(),
		this->lengthen(i - 1), tolerance, t, x, pcoords, subId);
	if (intersection != 0) {
		return true;
	} else {
		return false;
	}
}

void qSlicerModelClipModuleWidget::clearBodyList()
{
	int numOfBodies = this->bodyList.count();
	for (int i = 0; i < numOfBodies; i++) {
		this->bodyList.removeFirst();
		this->angleList.removeFirst();
	}
}

vtkSmartPointer<vtkImplicitBoolean> qSlicerModelClipModuleWidget::makeAngle(int i)
{
	vtkSmartPointer<vtkImplicitBoolean> angle = 
    vtkSmartPointer<vtkImplicitBoolean>::New();
	if (i == 1) {
		if (this->dotProduct(1) < 0) {
			angle->SetOperationTypeToIntersection();
		} else if (this->dotProduct(1) > 0)  {
			angle->SetOperationTypeToUnion();
		}
		angle->AddFunction(this->planeList.at(0)->getPlane());
		angle->AddFunction(this->planeList.at(1)->getPlane());
	} else {
		angle->SetOperationTypeToDifference();
		if (this->dotProduct(i) < 0) {
			angle->AddFunction(this->planeList.at(i - 1)->getPlane());
			angle->AddFunction(this->planeList.at(i)->getPlane());
		} else {
			angle->AddFunction(this->planeList.at(i)->getPlane());
			angle->AddFunction(this->planeList.at(i - 1)->getPlane());
		}
	}
	return angle;
}

vtkSmartPointer<vtkImplicitBoolean> qSlicerModelClipModuleWidget::makeAngle(int i, int j)
{
  vtkSmartPointer<vtkImplicitBoolean> angle = 
    vtkSmartPointer<vtkImplicitBoolean>::New();
  angle->SetOperationTypeToDifference();
  if (this->dotProduct(i, j) < 0) {
    angle->AddFunction(this->planeList.at(j)->getPlane());
    angle->AddFunction(this->planeList.at(i)->getPlane());
  } else {
    angle->AddFunction(this->planeList.at(i)->getPlane());
    angle->AddFunction(this->planeList.at(j)->getPlane());
  }
  return angle;
}

vtkSmartPointer<vtkImplicitBoolean> qSlicerModelClipModuleWidget::makeBody(int i)
{
  this->angleList.append(this->makeAngle(i));

	if (this->isInter(i)) {
		int j = i - 1;
		while (this->isInter(j)) {
			j--;
		}

    vtkSmartPointer<vtkImplicitBoolean> superBody = 
      vtkSmartPointer<vtkImplicitBoolean>::New();
    superBody->AddFunction(this->bodyList.at(j - 2));
    superBody->AddFunction(this->makeBody(i, j));
    if (this->dotProduct(j) < 0) {
      superBody->SetOperationTypeToIntersection();
      if (this->dotProduct(i, j - 1) > 0) {
        vtkSmartPointer<vtkImplicitBoolean> finalSuperBody = 
          vtkSmartPointer<vtkImplicitBoolean>::New();
        finalSuperBody->SetOperationTypeToUnion();
        finalSuperBody->AddFunction(superBody);
        finalSuperBody->AddFunction(this->makeAngle(i, j - 1));
        return finalSuperBody;
      } else {
        return superBody;
      }
    } else {
      superBody->SetOperationTypeToUnion();
      if (this->dotProduct(i, j - 1) < 0) {
        vtkSmartPointer<vtkImplicitBoolean> finalSuperBody = 
          vtkSmartPointer<vtkImplicitBoolean>::New();
        finalSuperBody->SetOperationTypeToDifference();
        finalSuperBody->AddFunction(superBody);
        finalSuperBody->AddFunction(this->makeAngle(i, j - 1));
        return finalSuperBody;
      } else {
        return superBody;
      }
    }
  } else {
		vtkSmartPointer<vtkImplicitBoolean> body =
			vtkSmartPointer<vtkImplicitBoolean>::New();
		if (this->dotProduct(i) < 0) {
			body->SetOperationTypeToDifference();
		} else {
			body->SetOperationTypeToUnion();
		}
		body->AddFunction(this->bodyList.at(i - 2));
		body->AddFunction(this->angleList.at(i - 1));
		return body;
	}
}

vtkSmartPointer<vtkImplicitBoolean> qSlicerModelClipModuleWidget::makeBody(int i, int j)
{
  QList<vtkSmartPointer<vtkImplicitBoolean> > bodyListLocal;
  int num = i - j - 1;

  vtkSmartPointer<vtkImplicitBoolean> body = 
    vtkSmartPointer<vtkImplicitBoolean>::New();
  if (this->dotProduct(j + 1) < 0) {
    body->SetOperationTypeToIntersection();
  } else {
    body->SetOperationTypeToUnion();
  }
  body->AddFunction(this->planeList.at(j)->getPlane());
  body->AddFunction(this->planeList.at(j + 1)->getPlane());
  bodyListLocal.append(body);

  for (int k = 0; k < num; k++) {
    vtkSmartPointer<vtkImplicitBoolean> body =
		  vtkSmartPointer<vtkImplicitBoolean>::New();
	  if (this->dotProduct(j + k + 2) < 0) {
		  body->SetOperationTypeToDifference();
	  } else {
		  body->SetOperationTypeToUnion();
	  }
    body->AddFunction(bodyListLocal.at(k));
    body->AddFunction(this->makeAngle(j + k + 2));
	  bodyListLocal.append(body);
  }
  return bodyListLocal.last();
}

vtkSmartPointer<vtkImplicitBoolean> qSlicerModelClipModuleWidget::
  withDepth(vtkSmartPointer<vtkImplicitFunction> clipFunction)
{
  Q_D(qSlicerModelClipModuleWidget);

  vtkSmartPointer<vtkImplicitBoolean> bodyWithDepth = 
    vtkSmartPointer<vtkImplicitBoolean>::New();
  if (d->depthButton->text() == tr("Remove Clip Depth")) {
    vtkSmartPointer<vtkPlane> depthFunction = 
      vtkSmartPointer<vtkPlane>::New();
    this->depthPlane->GetPlane(depthFunction);
    bodyWithDepth->SetOperationTypeToIntersection();
    bodyWithDepth->AddFunction(clipFunction);
    bodyWithDepth->AddFunction(depthFunction);
  } else {
    bodyWithDepth->AddFunction(clipFunction);
  }
  return bodyWithDepth;
}
