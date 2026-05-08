#include "dapplicationsettings.h"

#include "dsettingsdialog.h"

#include <DSettings>
#include <dsettingsbackend.h>

#include <QFile>
#include <QDebug>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

DApplicationSettings::DApplicationSettings()
{

}

static int indexOfUnescapedChar(const QByteArray &data, char ch, int from)
{
    for (; from < data.size(); ++from) {
        if (data.at(from) == '\\') {
            ++from;
            continue;
        }

        if (data.at(from) == ch)
            return from;
    }

    return from;
}

static void cleanQtTranslateCall(QByteArray &data, int &from)
{
    const QByteArray qsTr = QByteArrayLiteral("qsTr");
    const QByteArray qsTranslate = QByteArrayLiteral("anslate(");

    if (qsTr != QByteArray(data.constData() + from, qsTr.size()))
        return;

    int index = from + qsTr.size();
    if (index >= data.size())
        return;

    if (data.at(index) == '(') {
        data.remove(from, index - from + 1);
    } else if (qsTranslate == QByteArray(data.constData() + index, qsTranslate.size())) {
        index += qsTranslate.size();
        if (index >= data.size() || (data.at(index) != '"' && data.at(index) != '\''))
            return;

        index = indexOfUnescapedChar(data, data.at(index), index + 1);
        if (index >= data.size())
            return;

        int quote1 = data.indexOf('"', index + 1);
        int quote2 = data.indexOf('\'', index + 1);
        index = quote1 > 0 ? quote1 : quote2;
        if (quote2 > 0 && quote2 < index)
            index = quote2;
        if (index <= 0)
            return;

        data.remove(from, index - from);
    } else {
        return;
    }

    if (from >= data.size() || (data.at(from) != '"' && data.at(from) != '\''))
        return;

    from = indexOfUnescapedChar(data, data.at(from), from + 1);
    if (from >= data.size())
        return;

    from = indexOfUnescapedChar(data, ')', from + 1);
    if (from < data.size()) {
        data.remove(from, 1);
        --from;
    }
}

DSettings *DApplicationSettings::fromJsJsonFile(const QString &fileName, QObject *parent)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        qWarning() << "Failed to open settings template" << fileName << file.errorString();
        return nullptr;
    }

    QByteArray data = file.readAll();

    for (int i = 0; i < data.size(); ++i) {
        switch (data.at(i)) {
        case '\\':
            ++i;
            break;
        case '\'':
        case '"':
            i = indexOfUnescapedChar(data, data.at(i), i + 1);
            break;
        case 'q':
            cleanQtTranslateCall(data, i);
            break;
        default:
            break;
        }
    }

    DSettings *settings = DSettings::fromJson(data);
    if (settings)
        settings->setParent(parent);

    return settings;
}

DSettingsDialog *DApplicationSettings::createDialog(const QString &fileName,
                                                    DSettingsBackend *backend,
                                                    const QByteArray &translateContext,
                                                    QWidget *parent)
{
    DSettingsDialog *dialog = new DSettingsDialog(parent);
    DSettings *settings = fromJsJsonFile(fileName, dialog);
    if (!settings) {
        dialog->deleteLater();
        return nullptr;
    }

    if (backend) {
        backend->setParent(dialog);
        settings->setBackend(backend);
    }

    dialog->updateSettings(translateContext, settings);
    return dialog;
}
