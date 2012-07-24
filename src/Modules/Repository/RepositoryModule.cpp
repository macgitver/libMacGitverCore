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

#include <QtPlugin>
#include <QFileDialog>

#include "MacGitver/MacGitver.h"

#include "RepositoryModule.h"
#include "RepositoryCore.h"
#include "CloneRepositoryDlg.h"
#include "CreateRepositoryDlg.h"

RepositoryModule::RepositoryModule()
{
	mCore = new RepositoryCore;
}

RepositoryModule::~RepositoryModule()
{
	delete mCore;
}

void RepositoryModule::repositoryChanged( Git::Repository newRepository )
{
	mRepo = newRepository;

	Q_ASSERT( mCore );
	mCore->setRepository( newRepository );

	bool isValid = mRepo.isValid();

	actRepositoryClose->setEnabled( isValid );
}

void RepositoryModule::setupConfigPages( IConfigDialog* dialog )
{
}

Module::Types RepositoryModule::providesModuleTypes() const
{
	return Repository;
}

void RepositoryModule::initialize()
{
	setupActions( this );
	acRepositoryMenuAC->mergeInto( "RepositoryMenuMP" );
	acRepositoryToolBarAC->mergeInto( "RepositoryToolBarMP" );
}

void RepositoryModule::deinitialize()
{
}

void RepositoryModule::onRepositoryClose()
{
	if( mRepo.isValid() )
	{
		MacGitver::self().setRepository( Git::Repository() );
	}
}

void RepositoryModule::onRepositoryCreate()
{
	CreateRepositoryDlg().exec();
}

void RepositoryModule::onRepositoryOpen()
{
	QWidget* main = MacGitver::self().mainWindow()->widget();

	QString fn = QFileDialog::getExistingDirectory( main, trUtf8( "Open repository" ) );
	if( fn.isEmpty() )
	{
		return;
	}

	Git::Repository repo = Git::Repository::open( fn );
	if( repo.isValid() )
	{
		MacGitver::self().setRepository( repo );
	}
}

void RepositoryModule::onRepositoryClone()
{
	CloneRepositoryDlg().exec();
}

Q_EXPORT_PLUGIN2( Repository, RepositoryModule )
