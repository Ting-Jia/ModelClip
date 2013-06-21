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

// Qt includes
#include <QtPlugin>

// ModelClip Logic includes
#include <vtkSlicerModelClipLogic.h>

// ModelClip includes
#include "qSlicerModelClipModule.h"
#include "qSlicerModelClipModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerModelClipModule, qSlicerModelClipModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerModelClipModulePrivate
{
public:
  qSlicerModelClipModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerModelClipModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerModelClipModulePrivate
::qSlicerModelClipModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerModelClipModule methods

//-----------------------------------------------------------------------------
qSlicerModelClipModule
::qSlicerModelClipModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerModelClipModulePrivate)
{
  this->hlpText = tr("This is a loadable module bundled in an extension."
	  "it allows you to set the osteotomy trajectory with multiple planes, "
	  "and then clip the model with just one click. "
	  "It is designed for osteotomy simulation. ");
  this->acknowText = tr("This work was a part of thesis of Mr.Jun Lin's bachelor degree "
    "from Shanghai Jiao Tong University (SJTU) under the supervision of Dr.Xiaojun Chen. "
	"It was also partially funded by the National Natural Science Foundation of China (Gr"
	"ant No.51005156 and 81171429).");
}

////-----------------------------------------------------------------------------
//qSlicerModelClipModule
//::qSlicerModelClipModule(QObject* _parent)
//  : Superclass(_parent)
//  , d_ptr(new qSlicerModelClipModulePrivate)
//{
//  this->hlpText = tr("This is a loadable module bundled in an extension. The m"
//    "ajor feature of this extension is to clip a model with a surface that con"
//    "sists of multiple planes. The planes devide the space into positive part "
//    "and negative part, according to the direction of the normal axes of the p"
//    "lanes. The negative part of the model is clipped. An optional function is"
//    "to clip with a clipping depth. The depth is defined with another plane. I"
//    "f the clipping depth is defined, the intersection of the negative parts o"
//    "f the path planes and the depth plane is clipped.");
//  this->acknowText = tr("This work was a part of thesis of my bachelor degree f"
//    "rom Shanghai Jiao Tong University (SJTU). It was partially funded by the "
//    "grant of National Natural Science Foundation of China (No. 81171429).");
//}

//-----------------------------------------------------------------------------
qSlicerModelClipModule::~qSlicerModelClipModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerModelClipModule::helpText()const
{
  return this->hlpText;
}

//-----------------------------------------------------------------------------
QString qSlicerModelClipModule::acknowledgementText()const
{
  return this->acknowText;
}

//-----------------------------------------------------------------------------
QStringList qSlicerModelClipModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Jun LIN(SJTU) and Xiaojun CHEN(SJTU)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerModelClipModule::icon()const
{
  return QIcon(":/Icons/ModelClip.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerModelClipModule::categories() const
{
  return QStringList() << "Surface Models";
}

//-----------------------------------------------------------------------------
QStringList qSlicerModelClipModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerModelClipModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerModelClipModule
::createWidgetRepresentation()
{
  return new qSlicerModelClipModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerModelClipModule::createLogic()
{
  return vtkSlicerModelClipLogic::New();
}
