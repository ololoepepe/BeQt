/* file manager class - read lines of files [filename] OR [filename.hz] */
#ifndef _FILEMGR_HXX_
#define _FILEMGR_HXX_

#include "hunvisapi.h"

#include "hunzip.hxx"
#include <stdio.h>

//BeQt patch ----------------------------------------------------------------------------------------------------------
#include <QByteArray>
#include <QBuffer>
//BeQt patch end ------------------------------------------------------------------------------------------------------

class LIBHUNSPELL_DLL_EXPORTED FileMgr
{
protected:
    FILE * fin;
    Hunzip * hin;
    char in[BUFSIZE + 50]; // input buffer
    int fail(const char * err, const char * par);
    int linenum;

public:
//BeQt patch ----------------------------------------------------------------------------------------------------------
    FileMgr(const char * filename, const char * key = NULL, bool fnIsData = false);
private:
    bool mfnIsData;
    QByteArray mdata;
    QBuffer mbuffer;
public:
//BeQt patch end ------------------------------------------------------------------------------------------------------
    ~FileMgr();
    char * getline();
    int getlinenum();
};
#endif
