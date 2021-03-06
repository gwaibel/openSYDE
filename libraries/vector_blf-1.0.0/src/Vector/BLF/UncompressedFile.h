/*
 * Copyright (C) 2013 Tobias Lorenz.
 * Contact: tobias.lorenz@gmx.net
 *
 * This file is part of Tobias Lorenz's Toolkit.
 *
 * Commercial License Usage
 * Licensees holding valid commercial licenses may use this file in
 * accordance with the commercial license agreement provided with the
 * Software or, alternatively, in accordance with the terms contained in
 * a written agreement between you and Tobias Lorenz.
 *
 * GNU General Public License 3.0 Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl.html.
 */

#pragma once

#include "platform.h"

#include <iostream>
#include <vector>

#include "AbstractFile.h"

#include "vector_blf_export.h"

namespace Vector {
namespace BLF {

/** UncompresesdFile (Input/output memory stream) */
class VECTOR_BLF_EXPORT UncompressedFile : public AbstractFile
{
public:
    UncompressedFile();

    /** Default size of old data to keep until its trashed */
    size_t oldDataSize;

    /** Read block of data */
    virtual void read(char * s, std::streamsize n);

    /** Get position in input sequence */
    virtual std::streampos tellg();

    /** Set position in input sequence */
    virtual void seekg(std::streampos pos);

    /** Set position in input sequence */
    virtual void seekg(std::streamoff off, std::ios_base::seekdir way);

    /** Write block of data */
    virtual void write(const char * s, std::streamsize n);

    /** Get position in input sequence */
    virtual std::streampos tellp();

    /**
     * drop old data (only if dropSize is possible)
     *
     * @param dropSize size of data to drop (used in write case)
     * @param remainingDataSize size of data remaining to go back and read again (used in read case)
     */
    virtual void dropOldData(std::streamsize dropSize, std::streamsize remainingDataSize);

private:
    /** get position */
    std::streampos privateTellg;

    /** put position */
    std::streampos privateTellp;

    /** data start position */
    std::streampos dataTell;

    /** data */
    std::vector<char> data;
};

}
}
