#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QMdiArea>

#include "terminalgui.h"

/*
 * Handles document views, that are editors or usually do something for files.
 * */
class DocumentManager : public QMdiArea
{
public:
    explicit DocumentManager(QWidget *parent = nullptr);
};

#endif // VIEWMANAGER_H
