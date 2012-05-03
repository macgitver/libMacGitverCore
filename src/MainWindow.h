
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "Core/Git.h"
#include "Core/Repository.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow();
	~MainWindow();

private slots:
	void onRepositoryCreate();
	void onRepositoryOpen();

private:
	void setupUi();

signals:
	void repositoryChanged( const Git::Repository& repo );

public:
	void addError( const QString& err );
	void switchToRepo( const Git::Repository& repo );

private:
	void closeRepository();

private:
	QMenu*				mmuRepository;
	QAction*			macRepositoryOpen;
	QAction*			macRepositoryClose;
	QAction*			macRepositoryCreate;
	QAction*			macRepositoryClone;
	QAction*			macRepositoryQuit;
	QMenu*				mmuWorkingTree;


	Git::Repository		mRepo;

	static MainWindow* sSelf;
public:
	static MainWindow& self(){ return *sSelf; }
};

#endif
