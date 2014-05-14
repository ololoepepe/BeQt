#include "license.hunspell"
#include "license.myspell"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "filemgr.hxx"

//BeQt patch ----------------------------------------------------------------------------------------------------------
#include <QByteArray>
#include <QBuffer>
//BeQt patch end ------------------------------------------------------------------------------------------------------

int FileMgr::fail(const char * err, const char * par) {
    fprintf(stderr, err, par);
    return -1;
}

//BeQt patch ----------------------------------------------------------------------------------------------------------
FileMgr::FileMgr(const char *file, const char *key, bool fnIsData) {
    mfnIsData = fnIsData;
    mbuffer.setBuffer(&mdata);
//BeQt patch end ------------------------------------------------------------------------------------------------------
    linenum = 0;
    hin = NULL;
//BeQt patch ----------------------------------------------------------------------------------------------------------
    if (fnIsData)
    {
        mdata = QByteArray(file);
        mbuffer.open(QIODevice::ReadOnly);
        fin = 0;
    }
    else
    {
//BeQt patch end ------------------------------------------------------------------------------------------------------
        fin = fopen(file, "r");
//BeQt patch ----------------------------------------------------------------------------------------------------------
    }
    if (!fin && !fnIsData) {
//BeQt patch end ------------------------------------------------------------------------------------------------------
        // check hzipped file
        char * st = (char *) malloc(strlen(file) + strlen(HZIP_EXTENSION) + 1);
        if (st) {
            strcpy(st, file);
            strcat(st, HZIP_EXTENSION);
            hin = new Hunzip(st, key);
            free(st);
        }
    }
//BeQt patch ----------------------------------------------------------------------------------------------------------
    if (!fin && !hin && !fnIsData) fail(MSG_OPEN, file);
//BeQt patch end ------------------------------------------------------------------------------------------------------
}

FileMgr::~FileMgr()
{
//BeQt patch ----------------------------------------------------------------------------------------------------------
    if (mfnIsData)
    {
        mbuffer.close();
        return;
    }
//BeQt patch end-------------------------------------------------------------------------------------------------------
    if (fin) fclose(fin);
    if (hin) delete hin;
}

char * FileMgr::getline() {
    const char * l;
    linenum++;
//BeQt patch ----------------------------------------------------------------------------------------------------------
    if (mfnIsData)
    {
        if (mbuffer.atEnd())
            return 0;
        strcpy(in, mbuffer.readLine().data());
        return in;
    }
//BeQt patch end-------------------------------------------------------------------------------------------------------
    if (fin) return fgets(in, BUFSIZE - 1, fin);
    if (hin && (l = hin->getline())) return strcpy(in, l);
    linenum--;
    return NULL;
}

int FileMgr::getlinenum() {
    return linenum;
}
