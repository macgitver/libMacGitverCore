/*
 * MacGitver
 * Copyright (C) 2012-2015 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Sascha Cunz <sascha@cunz-rad.com>
 * (C) Cunz RaD Ltd.
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

#include "GitPatchConsumer.hpp"

#include "libDiffViews/Model/BinaryFilePatch.hpp"
#include "libDiffViews/Model/TextFilePatch.hpp"

#define DBG 0

GitPatchConsumer::GitPatchConsumer()
    : mPatch( new DiffViews::Patch() )
{
}

GitPatchConsumer::~GitPatchConsumer()
{
}

bool GitPatchConsumer::startFileChange( const Git::ChangeListEntry &entry )
{
    #if DBG
    qDebug( "%s => %s; t=%i, sim=%i, %s",
            qPrintable( entry.oldPath ),
            qPrintable( entry.newPath ),
            int(entry.type),
            entry.similarity,
            entry.isBinary ? "Bin" : "Txt" );
    #endif

    if( entry.isBinary )
    {
        mCurFile = NULL;
        DiffViews::FilePatch::Ptr bfp;
        bfp = new DiffViews::BinaryFilePatch( QStringList() << entry.oldPath << entry.newPath );
        mPatch->addPath( bfp );
    }
    else
    {
        mCurFile = new DiffViews::TextFilePatch( QStringList() << entry.oldPath << entry.newPath );
        mPatch->addPath( mCurFile );
    }

    return true;
}

bool GitPatchConsumer::startHunkChange( int newStart, int newLines, int oldStart, int oldLines,
                                        const QString& header )
{
    if( mCurFile )
    {
        #if DBG
        qDebug( "    Hunk: %i,%i %i,%i (%s)",
                newStart, newLines,
                oldStart, oldLines,
                qPrintable( header ) );
        #endif

        mRemainsNew = newLines;
        mRemainsOld = oldLines;

        mCurNew = newStart;
        mCurOld = oldStart;

        mCurType = None;
        mCurHunk = new DiffViews::Hunk();
        mCurFile->asTextFilePatch()->addHunk( mCurHunk );
    }

    return true;
}

bool GitPatchConsumer::appendContext( const QString& content )
{
    if( mCurFile )
    {
        #if DBG
        qDebug( "        Ctx:'%s'",
                qPrintable( content ) );
        #endif

        if( mCurType != Context )
        {
            mCurDiff = new DiffViews::HunkPart( 2, DiffViews::HunkPart::Context );
            mCurHunk->addPart( mCurDiff );
            mCurDiff->sideLines( 0 )->setFirstLine( mCurOld );
            mCurDiff->sideLines( 1 )->setFirstLine( mCurNew );
            mCurType = Context;
        }

        mCurDiff->sideLines( 0 )->addLine( content );
        mCurDiff->sideLines( 1 )->addLine( content );

        ++mCurNew;
        ++mCurOld;
        --mRemainsNew;
        --mRemainsOld;
    }

    return true;
}

bool GitPatchConsumer::appendAddition( const QString& content )
{
    if( mCurFile )
    {
        #if DBG
        qDebug( "        Add:'%s'",
                qPrintable( content ) );
        #endif

        if( mCurType == None || mCurType == Context || mCurType == Change )
        {
            mCurDiff = new DiffViews::HunkPart( 2, DiffViews::HunkPart::Insert );
            mCurHunk->addPart( mCurDiff );
            mCurDiff->sideLines( 0 )->setFirstLine( mCurOld );
            mCurDiff->sideLines( 1 )->setFirstLine( mCurNew );
            mCurType = Add;
        }
        else if( mCurType == Del )
        {
            mCurType = Change;
            mCurDiff->setType( DiffViews::HunkPart::Change );
        }
        //else mCurType == Add

        ++mCurNew;
        --mRemainsNew;
        mCurDiff->sideLines( 1 )->addLine( content );
    }

    return true;
}

bool GitPatchConsumer::appendDeletion( const QString& content )
{
    if( mCurFile )
    {
        #if DBG
        qDebug( "        Del:'%s'",
                qPrintable( content ) );
        #endif

        if( mCurType == None || mCurType == Context || mCurType == Add )
        {
            mCurDiff = new DiffViews::HunkPart( 2, DiffViews::HunkPart::Delete );
            mCurHunk->addPart( mCurDiff );
            mCurDiff->sideLines( 0 )->setFirstLine( mCurOld );
            mCurDiff->sideLines( 1 )->setFirstLine( mCurNew );
            mCurType = Del;
        }
        // Else we're 'Change' or 'Del', and can just append

        ++mCurOld;
        --mRemainsOld;
        mCurDiff->sideLines( 0 )->addLine( content );
    }

    return true;
}

DiffViews::Patch::Ptr GitPatchConsumer::patch() const
{
    return mPatch;
}
