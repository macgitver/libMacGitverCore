/*
 * MacGitver
 * Copyright (C) 2012 Sascha Cunz <sascha@babbelbox.org>
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License (Version 2) as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "libMacGitverCore/Config/Config.h"
#include "libMacGitverCore/Widgets/HeaderView.h"

#include "libGitWrap/Index.hpp"
#include "libGitWrap/Result.hpp"

#include "IndexTreeItemView.h"
#include "WorkingTreeAbstractItem.h"
#include "WorkingTreeModel.h"

#include <QAbstractProxyModel>
#include <QMessageBox>


IndexTreeItemView::IndexTreeItemView(QWidget *parent)
    : TreeViewCtxMenu( parent )
    , mHeader( new HeaderView( Qt::Horizontal ) )
    , mModel( 0 )
{
#ifdef Q_OS_MACX
    setAttribute( Qt::WA_MacShowFocusRect, false );
#endif

    setHeader( mHeader );
    mHeader->setConfigName( QStringLiteral( "Indextree/Columns" ) );

    StageViewCtxMenu::setupActions( this );

    connect( this, SIGNAL(contextMenu(QModelIndex, QPoint)),
             this, SLOT(contextMenu(QModelIndex, QPoint)) );
}

void IndexTreeItemView::setModel(QAbstractItemModel *model)
{
    TreeViewCtxMenu::setModel( model );

    QAbstractProxyModel *apm = qobject_cast< QAbstractProxyModel* >( model );
    while ( apm )
    {
        model = apm->sourceModel();
        Q_ASSERT( model );
        apm = qobject_cast< QAbstractProxyModel* >( model );
    }

    mModel = qobject_cast< WorkingTreeModel * >( model );
}

void IndexTreeItemView::onWtCtxUnstage()
{
    Heaven::Action* action = qobject_cast< Heaven::Action* >( sender() );
    if ( !action )
        return;

    QModelIndex srcIndex = deeplyMapToSource( currentIndex() );
    if ( !srcIndex.isValid() )
        return;

    WorkingTreeAbstractItem* item = mModel->indexToItem( srcIndex );
    Q_ASSERT( item );

    Git::Repository repo = mModel->repository();

    Git::Result r;
    Git::Index i = repo.index( r );

    i.read( r );
    const QString itemPath = item->path();
    if ( repo.status(r, itemPath).testFlag(Git::FileIndexNew) )
    {
        i.removeFile( r, itemPath );
    }
    else
    {
        i.resetFiles( r, QStringList( itemPath ) );
    }
    i.write( r );

    // TODO: workaround to update the model
    mModel->setRepository(repo);

    if ( !r )
    {
        QMessageBox::warning( this, trUtf8("Error while unstaging file."),
                              trUtf8("Error while unstaging file. Git message:\n%1").arg(r.errorText()) );
    }
}

void IndexTreeItemView::contextMenu( const QModelIndex& index, const QPoint& globalPos )
{
    Q_ASSERT( mModel );

    QModelIndex srcIndex = deeplyMapToSource( index );
    if ( !srcIndex.isValid() )
        return;

    WorkingTreeAbstractItem* item = mModel->indexToItem( srcIndex );

    Heaven::Menu* menu = 0;
    if ( item && !item->isDirectory() )
    {
        menu = menuCtxMenuStageView;
        //menu->setActivationContext( item );
    }

    if ( menu )
        menu->showPopup( globalPos );
}

QModelIndex IndexTreeItemView::deeplyMapToSource( QModelIndex current ) const
{
    while( current.isValid() )
    {
        const QAbstractProxyModel* apm = qobject_cast< const QAbstractProxyModel* >( current.model() );
        if( !apm )
            return current;

        current = apm->mapToSource( current );
    }

    return QModelIndex();
}
