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

#include <QDebug>
#include <QComboBox>
#include <QStatusBar>
#include <QFile>
#include <QApplication>
#include <QMenuBar>
#include <QDockWidget>
#include <QStringBuilder>

#include "Config/Config.h"

#include "GitWrap/ObjectId.h"
#include "GitWrap/Reference.h"

#include "MacGitver/MacGitver.h"
#include "MacGitver/Modules.h"

#include "MainWindow.h"
#include "ConfigDialog.h"
#include "GeneralConfigPage.h"

#include "Diff/View/DiffView.h"

#include "ui_AboutDlg.h"

MainWindow::MainWindow()
	: Heaven::MainWindow()
	, mRepo()
{
	setupUi();

	connect( &MacGitver::self(), SIGNAL(repositoryChanged(Git::Repository)),
			 SLOT(repositoryChanged(Git::Repository)) );

	QString levelId = Config::self().get( "UserLevel", "Novice" ).toString();

	foreach( UserLevelDefinition::Ptr uld, Config::self().levels() )
	{
		if( uld->id() == levelId )
		{
			activateLevel( uld );
			break;
		}
	}

}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
	QFile styleFile( ":/MacGitver.qss" );
	styleFile.open( QFile::ReadOnly );
	setStyleSheet( styleFile.readAll() );

	setupActions( this );
	setMenuBar( mbMainMenuBar->menuBarFor( this ) );

	mModes = new Heaven::ModeSwitchWidget();
	menuBar()->setCornerWidget( mModes );

	setWindowTitle( trUtf8( "MacGitver" ) );

	statusBar()->addPermanentWidget( mLblCurrentBranch = new QLabel() );
	setHeadLabel();

	mTop = new Heaven::TopLevelWidget();
	setCentralWidget( mTop );

	addToolBar( tbMainBar->toolBarFor( this ) );
}

void MainWindow::activateLevel( UserLevelDefinition::Ptr uld )
{
	QStringList modeNames;

	foreach( UserLevelMode::Ptr mode, uld->userModes() )
	{
		modeNames.append( mode->name() );
	}

//	mModes->setModes( modeNames, QString() );

	mCurrentLevel = uld;

	activateModeForRepo();
}

void MainWindow::activateModeForRepo()
{
	QString preset = mRepo.isValid() ? "Normal" : "Welcome";

	QString configPath = mCurrentLevel->id() % "/" % preset;
	QString modeName = Config::self().get( configPath, mCurrentLevel->preset( preset ) ).toString();

	activateMode( modeName );
}

void MainWindow::createPartialLayout( Heaven::ViewContainer* parent,
									  UserLevelDefaultLayoutEntry::Ptr entry )
{
	switch( entry->type() )
	{
	case UserLevelDefaultLayoutEntry::Tab:
		{
			Heaven::ViewContainer* child = new Heaven::ViewContainer(
											   Heaven::ViewContainer::Tab,
											   Heaven::ViewContainer::SubTabTop,
											   parent );

			parent->addContainer( child );

			foreach( UserLevelDefaultLayoutEntry::Ptr subEntry, entry->children() )
			{
				createPartialLayout( child, subEntry );
			}
		}
		break;

	case UserLevelDefaultLayoutEntry::Split:
		{
			Heaven::ViewContainer* child = new Heaven::ViewContainer(
											   Heaven::ViewContainer::Splitter,
											   entry->isVertical() ?
												   Heaven::ViewContainer::SubSplitVert :
												   Heaven::ViewContainer::SubSplitHorz,
											   parent );

			parent->addContainer( child );

			foreach( UserLevelDefaultLayoutEntry::Ptr subEntry, entry->children() )
			{
				createPartialLayout( child, subEntry );
			}
		}
		break;

	case UserLevelDefaultLayoutEntry::View:
		{
			Heaven::View* v = MacGitver::self().createView( entry->name() );

			if( !v )
			{
				qDebug( "Could not create a view with id '%s'", qPrintable( entry->name() ) );
				break;
			}

			parent->addView( v );
		}
		break;
	}
}

void MainWindow::activateMode( const QString& modeName )
{
	qDebug( "Going to %s mode", qPrintable( modeName ) );

	mTop->clear();

	UserLevelMode::Ptr mode = mCurrentLevel->mode( modeName );

	UserLevelDefaultLayout::Ptr layout = mode->defaultLayout();

	createPartialLayout( mTop->rootContainer(), layout->root() );

//	mModes->setEnabled( mode->isLockingMode() );
//	mModes->setCurrentMode( modeName );
}

void MainWindow::repositoryChanged( const Git::Repository& repo )
{
	mRepo = repo;
	activateModeForRepo();
	setHeadLabel();
}

void MainWindow::setHeadLabel()
{
	if( mRepo.isValid() )
	{
		QString curBranch;
		Git::Reference HEAD = mRepo.HEAD();

		if( HEAD.isValid() )
		{
			if( HEAD.name() != "HEAD" )
			{
				curBranch = trUtf8( "on branch <b>%1</b>" )
							.arg( HEAD.name().mid( 11 ).constData() );
			}
			else
			{
				curBranch = trUtf8( "on detached HEAD at <b>%1</b>" )
							.arg( HEAD.objectId().toString() );
			}
		}
		else
		{
			curBranch = trUtf8( "<b style=\"color: red;\">Branch yet to be born</b>" );
		}

		mLblCurrentBranch->setText( curBranch );
	}
	else
	{
		mLblCurrentBranch->setText( trUtf8( "No repository loaded" ) );
	}
}

void MainWindow::integrateView( Heaven::View* view, Heaven::Positions position )
{
	mTop->addView( view, position );
}

QWidget* MainWindow::widget()
{
	return this;
}

void MainWindow::onHelpAbout()
{
	QDialog d;
	Ui::AboutDlg u;
	u.setupUi( &d );
	d.exec();
}

void MainWindow::onToolsPreferences()
{
	ConfigDialog d;
	d.addPage( new GeneralConfigPage( &d ) );
	MacGitver::self().modules()->setupConfigPages( &d );
	d.exec();
}
