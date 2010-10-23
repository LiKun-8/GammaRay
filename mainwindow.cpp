#include "mainwindow.h"
#include "probe.h"
#include "objectlistmodel.h"
#include "objecttreemodel.h"
#include "objecttypefilterproxymodel.h"
#include "scenemodel.h"
#include "connectionmodel.h"

#include <KLocalizedString>
#include <QCoreApplication>
#include <qgraphicsscene.h>
#include <qdebug.h>
#include <qgraphicsitem.h>
#include <krecursivefilterproxymodel.h>
#include "singlecolumnobjectproxymodel.h"

using namespace Endoscope;

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent)
{
  QWidget *center = new QWidget( this );
  setCentralWidget( center );
  ui.setupUi( centralWidget() );

  QSortFilterProxyModel *objectFilter = new KRecursiveFilterProxyModel( this );
  objectFilter->setSourceModel( Probe::instance()->objectTreeModel() );
  objectFilter->setDynamicSortFilter( true );
  ui.objectTreeView->setModel( objectFilter );
  ui.objectSearchLine->setProxy( objectFilter );
  connect( ui.objectTreeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
	   SLOT(objectSelected(QModelIndex)) );

  ObjectTypeFilterProxyModel<QWidget> *widgetFilterProxy = new ObjectTypeFilterProxyModel<QWidget>( this );
  widgetFilterProxy->setSourceModel( Probe::instance()->objectTreeModel() );
  KRecursiveFilterProxyModel* widgetSearchProxy = new KRecursiveFilterProxyModel( this );
  widgetSearchProxy->setSourceModel( widgetFilterProxy );
  ui.widgetTreeView->setModel( widgetSearchProxy );
  ui.widgetSearchLine->setProxy( widgetSearchProxy );
  connect( ui.widgetTreeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
           SLOT(widgetSelected(QModelIndex)) );

  ObjectTypeFilterProxyModel<QAbstractItemModel> *modelFilterProxy = new ObjectTypeFilterProxyModel<QAbstractItemModel>( this );
  modelFilterProxy->setSourceModel( Probe::instance()->objectListModel() );
  SingleColumnObjectProxyModel *singleColumnProxy = new SingleColumnObjectProxyModel( this );
  singleColumnProxy->setSourceModel( modelFilterProxy );
  ui.modelComboBox->setModel( singleColumnProxy );
  connect( ui.modelComboBox, SIGNAL(currentIndexChanged(int)), SLOT(modelSelected(int)) );

  ObjectTypeFilterProxyModel<QGraphicsScene> *sceneFilterProxy = new ObjectTypeFilterProxyModel<QGraphicsScene>( this );
  sceneFilterProxy->setSourceModel( Probe::instance()->objectListModel() );
  singleColumnProxy = new SingleColumnObjectProxyModel( this ),
  singleColumnProxy->setSourceModel( sceneFilterProxy );
  ui.sceneComboBox->setModel( singleColumnProxy );
  connect( ui.sceneComboBox, SIGNAL(activated(int)), SLOT(sceneSelected(int)) );
  m_sceneModel = new SceneModel( this );
  QSortFilterProxyModel *sceneFilter = new KRecursiveFilterProxyModel( this );
  sceneFilter->setSourceModel( m_sceneModel );
  ui.sceneTreeView->setModel( sceneFilter );
  ui.screneTreeSearchLine->setProxy( sceneFilter );
  connect( ui.sceneTreeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
           SLOT(sceneItemSelected(QModelIndex)) );

  QSortFilterProxyModel *connectionFilterProxy = new QSortFilterProxyModel( this );
  connectionFilterProxy->setSourceModel( Probe::instance()->connectionModel() );
  ui.connectionSearchLine->setProxy( connectionFilterProxy );
  ui.connectionView->setModel( connectionFilterProxy );

  setWindowTitle( i18n( "Endoscope (%1)", qApp->applicationName() ) );
}

void MainWindow::objectSelected( const QModelIndex &index )
{
  if ( index.isValid() ) {
    QObject *obj = index.data( ObjectListModel::ObjectRole ).value<QObject*>();
    ui.objectPropertyWidget->setObject( obj );
  } else {
    ui.objectPropertyWidget->setObject( 0 );
  }
}

void MainWindow::widgetSelected(const QModelIndex& index)
{
  if ( index.isValid() ) {
    QObject *obj = index.data( ObjectListModel::ObjectRole ).value<QObject*>();
    ui.widgetPropertyWidget->setObject( obj );
    ui.widgetPreviewWidget->setWidget( qobject_cast<QWidget*>( obj ) );
  } else {
    ui.widgetPropertyWidget->setObject( 0 );
    ui.widgetPreviewWidget->setWidget( 0 );
  }
}

void MainWindow::modelSelected( int index )
{
  QObject* obj = ui.modelComboBox->itemData( index, ObjectListModel::ObjectRole ).value<QObject*>();
  QAbstractItemModel* model = qobject_cast<QAbstractItemModel*>( obj );
  ui.modelContentView->setModel( model );
}

void MainWindow::sceneSelected(int index)
{
  QObject* obj = ui.sceneComboBox->itemData( index, ObjectListModel::ObjectRole ).value<QObject*>();
  QGraphicsScene* scene = qobject_cast<QGraphicsScene*>( obj );
  qDebug() << Q_FUNC_INFO << scene << obj;

  m_sceneModel->setScene( scene );
}

void MainWindow::sceneItemSelected(const QModelIndex& index)
{
  if ( index.isValid() ) {
    QGraphicsItem* item = index.data( SceneModel::SceneItemRole ).value<QGraphicsItem*>();
    ui.scenePropertyWidget->setObject( item->toGraphicsObject() );
  } else {
    ui.scenePropertyWidget->setObject( 0 );
  }
}

#include "mainwindow.moc"
