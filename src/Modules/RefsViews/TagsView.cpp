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

#include <QListWidget>
#include <QVBoxLayout>

#include "TagsView.h"

TagsView::TagsView()
{
	mListWidget = new QListWidget();
	mListWidget->setFrameStyle( QFrame::NoFrame );

	QVBoxLayout* l = new QVBoxLayout;
	l->setSpacing( 0 );
	l->setMargin( 0 );
	l->addWidget( mListWidget );

	setLayout( l );

	setViewName( trUtf8( "Tags" ) );
}

void TagsView::repositoryChanged( Git::Repository repo )
{
	mRepo = repo;

	mListWidget->clear();

	if( mRepo.isValid() )
	{
		QStringList sl = mRepo.allTags();

		for( int i = 0; i < sl.count(); i++ )
		{
			new QListWidgetItem( sl[ i ], mListWidget );
		}
	}
}

QSize TagsView::sizeHint() const
{
	return QSize( 100, 100 );
}
