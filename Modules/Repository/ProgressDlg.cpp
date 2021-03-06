
#include <QStringBuilder>
#include <QCloseEvent>
#include <QDebug>
#include <QPushButton>
#include <QString>

#include "ProgressDlg.hpp"


ProgressDlg::ProgressDlg()
    : BlueSky::Dialog()
    , mDone( false )
{
    setupUi( this );

    QPushButton* close = buttonBox->button( QDialogButtonBox::Close );
    close->setEnabled( false );
    connect( close, SIGNAL(clicked()), this, SLOT(close ()) );

    QPalette p;
    p.setColor( QPalette::Base, p.color( QPalette::Window ) );
    p.setColor( QPalette::Text, p.color( QPalette::WindowText ) );
    txtLog->setPalette( p );

}

void ProgressDlg::setAction( const QString& action,
                             const QStringList& open,
                             const QStringList& current,
                             const QStringList& done )
{
    QString act = action;

    foreach( QString s, done )
    {
        act += QStringLiteral( " (<font color=\"green\">" ) % s % QStringLiteral( "</font>)" );
    }

    foreach( QString s, current )
    {
        act += QStringLiteral( " (<font color=\"blue\">" ) % s % QStringLiteral( "</font>)" );
    }

    foreach( QString s, open )
    {
        act += QStringLiteral( " (<font color=\"red\">" ) % s % QStringLiteral( "</font>)" );
    }

    lblAction->setText( act );
}

void ProgressDlg::setCurrent(QObject* current)
{
    mCurrent = current;

    connect( mCurrent, SIGNAL(remoteMessage(QString)),
             this, SLOT(remoteMessage(QString)) );
    connect( mCurrent, SIGNAL(transportProgress(quint32, quint32, quint32, quint64)),
             this, SLOT(transportProgress(quint32, quint32, quint32, quint64)) );
}

void ProgressDlg::closeEvent( QCloseEvent* ev )
{
    if( !mDone )
    {
        ev->ignore();
        return;
    }

    QDialog::closeEvent( ev );
}

void ProgressDlg::transportProgress( quint32 totalObjects,
                                     quint32 indexedObjects,
                                     quint32 receivedObjects,
                                     quint64 receivedBytes )
{
    QString recv;
    if( receivedBytes > 1024 * 1024 * 950 ) /* 950 is so we get 0.9 gb */
    {
        recv = QString::number( receivedBytes / (1024*1024*1024.0), 'f', 2 ) % QStringLiteral( " Gb" );
    }
    else if( receivedBytes > 1024 * 950 )
    {
        recv = QString::number( receivedBytes / (1024*1024.0), 'f', 2 ) % QStringLiteral( " Mb" );
    }
    else if( receivedBytes > 950 )
    {
        recv = QString::number( receivedBytes / 1024.0, 'f', 2 ) % QStringLiteral( " Kb" );
    }
    else
    {
        recv = QString::number( receivedBytes );
    }
    lblTransferSize->setText( recv );

    progressBar->setRange( 0, totalObjects * 2 );
    progressBar->setValue( indexedObjects + receivedObjects );
}

void ProgressDlg::remoteMessage( const QString& msg )
{
    mRawRemoteMessage += msg;

    QString output;
    QChar outputBuffer[ 256 ];
    int outBufPos = 0, outBufLen = 0;

    for( int i = 0; i < mRawRemoteMessage.length(); ++i )
    {
        if( mRawRemoteMessage[ i ] == QChar( L'\r' ) )
        {
            outBufPos = 0;
        }
        else if( mRawRemoteMessage[ i ] == QChar( L'\n' ) )
        {
            if( outBufLen )
                output += QString( outputBuffer, outBufLen );
            output += QChar( L'\n' );
            outBufPos = outBufLen = 0;
        }
        else
        {
            outputBuffer[ outBufPos++] = mRawRemoteMessage[ i ];
            outBufLen = qMax( outBufLen, outBufPos );
        }
    }

    if( outBufLen )
        output += QString( outputBuffer, outBufLen );

    QString log = mBaseLog % QStringLiteral( "<br/>" ) %
            output.replace( QChar( L'\n' ), QStringLiteral("<br/>") ).simplified();

    txtLog->setHtml( log );
}

void ProgressDlg::beginStep( const QString& step )
{
    mBaseLog += tr( "<font color=\"blue\">%1</font><br/>" ).arg( step );
    txtLog->setHtml( mBaseLog );
}

void ProgressDlg::finalizeStep()
{
    mBaseLog = txtLog->toHtml() % QStringLiteral( "<br/>" );
    mRawRemoteMessage = QString();

    txtLog->setHtml( mBaseLog );
}

void ProgressDlg::setDone()
{
    mDone = true;
    buttonBox->button( QDialogButtonBox::Close )->setEnabled( true );
}
