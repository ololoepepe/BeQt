#ifndef BABSTRACTFILETYPE_P_H
#define BABSTRACTFILETYPE_P_H

class BSyntaxHighlighter;

#include "babstractfiletype.h"

#include <BeQtCore/private/bbase_p.h>

/*============================================================================
================================ BAbstractFileTypePrivate ====================
============================================================================*/

class BAbstractFileTypePrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BAbstractFileType)
public:
    explicit BAbstractFileTypePrivate(BAbstractFileType *q);
    ~BAbstractFileTypePrivate();
public:
    void init();
public:
    BSyntaxHighlighter *highlighter;
private:
    Q_DISABLE_COPY(BAbstractFileTypePrivate)
};

#endif // BABSTRACTFILETYPE_P_H
