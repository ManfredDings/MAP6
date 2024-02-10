// Copyright 2016 Manfred Dings

#include "mdutils.h"


QString truncateFilename(QString filename, int length)
{
    if (filename.length() < (length-3)) return filename;
    else{
        QString s1, s2;
        s1 = filename.mid(0, length / 2 -2);
        s2 = filename.mid(filename.length() - length /2 - 2, filename.length());
        return s1+"..."+s2;
    }
}
