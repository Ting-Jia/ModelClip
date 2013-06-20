/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// FooBar Widgets includes
#include "qSlicerModelClipFooBarWidget.h"
#include "ui_qSlicerModelClipFooBarWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ModelClip
class qSlicerModelClipFooBarWidgetPrivate
  : public Ui_qSlicerModelClipFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerModelClipFooBarWidget);
protected:
  qSlicerModelClipFooBarWidget* const q_ptr;

public:
  qSlicerModelClipFooBarWidgetPrivate(
    qSlicerModelClipFooBarWidget& object);
  virtual void setupUi(qSlicerModelClipFooBarWidget*);
};

// --------------------------------------------------------------------------
qSlicerModelClipFooBarWidgetPrivate
::qSlicerModelClipFooBarWidgetPrivate(
  qSlicerModelClipFooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerModelClipFooBarWidgetPrivate
::setupUi(qSlicerModelClipFooBarWidget* widget)
{
  this->Ui_qSlicerModelClipFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerModelClipFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerModelClipFooBarWidget
::qSlicerModelClipFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerModelClipFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerModelClipFooBarWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerModelClipFooBarWidget
::~qSlicerModelClipFooBarWidget()
{
}
