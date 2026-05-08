#ifndef DAPPLICATIONSETTINGS_H
#define DAPPLICATIONSETTINGS_H

#include "dtkwidget_global.h"

#include <QByteArray>
#include <QString>

QT_BEGIN_NAMESPACE
class QObject;
class QWidget;
QT_END_NAMESPACE

DCORE_BEGIN_NAMESPACE
class DSettings;
class DSettingsBackend;
DCORE_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE

class DSettingsDialog;

class LIBDTKWIDGETSHARED_EXPORT DApplicationSettings
{
public:
    DApplicationSettings();

    static DTK_CORE_NAMESPACE::DSettings *fromJsJsonFile(const QString &fileName, QObject *parent = nullptr);
    static DSettingsDialog *createDialog(const QString &fileName,
                                         DTK_CORE_NAMESPACE::DSettingsBackend *backend = nullptr,
                                         const QByteArray &translateContext = QByteArray(),
                                         QWidget *parent = nullptr);
};

DWIDGET_END_NAMESPACE

#endif // DAPPLICATIONSETTINGS_H
