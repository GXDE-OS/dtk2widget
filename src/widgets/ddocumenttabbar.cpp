#include "ddocumenttabbar.h"

#include <QAction>
#include <QEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QVariantMap>

DWIDGET_BEGIN_NAMESPACE

static const char kDocumentIdKey[] = "documentId";
static const char kTitleKey[] = "title";
static const char kModifiedKey[] = "modified";

DDocumentTabBar::DDocumentTabBar(QWidget *parent)
    : DTabBar(parent)
{
    installEventFilter(this);

    setMovable(true);
    setTabsClosable(true);
    setVisibleAddButton(true);
    setDragable(true);
    setElideMode(Qt::ElideMiddle);
    setFocusPolicy(Qt::NoFocus);
}

int DDocumentTabBar::addDocument(const QString &documentId, const QString &title)
{
    return insertDocument(currentIndex() + 1, documentId, title);
}

int DDocumentTabBar::addDocument(const QString &documentId, const QIcon &icon, const QString &title)
{
    return insertDocument(currentIndex() + 1, documentId, icon, title);
}

int DDocumentTabBar::insertDocument(int index, const QString &documentId, const QString &title)
{
    return insertDocument(index, documentId, QIcon(), title);
}

int DDocumentTabBar::insertDocument(int index, const QString &documentId, const QIcon &icon, const QString &title)
{
    const int existingIndex = indexOfDocument(documentId);
    if (existingIndex >= 0) {
        setCurrentIndex(existingIndex);
        return existingIndex;
    }

    index = qBound(0, index, count());
    const int documentIndex = icon.isNull() ? insertTab(index, title) : insertTab(index, icon, title);

    QVariantMap data;
    data.insert(QLatin1String(kDocumentIdKey), documentId);
    data.insert(QLatin1String(kTitleKey), title);
    data.insert(QLatin1String(kModifiedKey), false);
    setDocumentData(documentIndex, data);
    setTabMaximumSize(documentIndex, QSize(300, 100));
    setCurrentIndex(documentIndex);
    return documentIndex;
}

void DDocumentTabBar::updateDocument(int index, const QString &documentId, const QString &title)
{
    if (index < 0 || index >= count())
        return;

    QVariantMap data = documentData(index);
    data.insert(QLatin1String(kDocumentIdKey), documentId);
    data.insert(QLatin1String(kTitleKey), title);
    setDocumentData(index, data);
    refreshTabText(index);
}

void DDocumentTabBar::closeDocument(int index)
{
    if (index < 0 || index >= count())
        return;

    Q_EMIT documentCloseRequested(index, documentIdAt(index));
    removeTab(index);
}

void DDocumentTabBar::closeCurrentDocument()
{
    closeDocument(currentIndex());
}

void DDocumentTabBar::closeOtherDocuments(int index)
{
    closeOtherDocumentsExcept(documentIdAt(index));
}

void DDocumentTabBar::closeOtherDocumentsExcept(const QString &documentId)
{
    Q_EMIT documentsCloseRequested(documentIdsExcept(documentId));
}

int DDocumentTabBar::indexOfDocument(const QString &documentId) const
{
    for (int i = 0; i < count(); ++i) {
        if (documentIdAt(i) == documentId)
            return i;
    }

    return -1;
}

QString DDocumentTabBar::documentIdAt(int index) const
{
    return documentData(index).value(QLatin1String(kDocumentIdKey)).toString();
}

QString DDocumentTabBar::currentDocumentId() const
{
    return documentIdAt(currentIndex());
}

QString DDocumentTabBar::documentTitleAt(int index) const
{
    return documentData(index).value(QLatin1String(kTitleKey)).toString();
}

QString DDocumentTabBar::currentDocumentTitle() const
{
    return documentTitleAt(currentIndex());
}

bool DDocumentTabBar::isDocumentModified(int index) const
{
    return documentData(index).value(QLatin1String(kModifiedKey)).toBool();
}

bool DDocumentTabBar::modifiedMarkerEnabled() const
{
    return m_modifiedMarkerEnabled;
}

QString DDocumentTabBar::modifiedMarker() const
{
    return m_modifiedMarker;
}

void DDocumentTabBar::setDocumentModified(int index, bool modified)
{
    if (index < 0 || index >= count())
        return;

    QVariantMap data = documentData(index);
    if (data.value(QLatin1String(kModifiedKey)).toBool() == modified)
        return;

    data.insert(QLatin1String(kModifiedKey), modified);
    setDocumentData(index, data);
    refreshTabText(index);
}

void DDocumentTabBar::setModifiedMarkerEnabled(bool enabled)
{
    if (m_modifiedMarkerEnabled == enabled)
        return;

    m_modifiedMarkerEnabled = enabled;
    for (int i = 0; i < count(); ++i)
        refreshTabText(i);
}

void DDocumentTabBar::setModifiedMarker(const QString &marker)
{
    if (m_modifiedMarker == marker)
        return;

    m_modifiedMarker = marker;
    for (int i = 0; i < count(); ++i)
        refreshTabText(i);
}

void DDocumentTabBar::previousDocument()
{
    if (count() == 0)
        return;

    const int index = currentIndex() <= 0 ? count() - 1 : currentIndex() - 1;
    setCurrentIndex(index);
}

void DDocumentTabBar::nextDocument()
{
    if (count() == 0)
        return;

    const int index = currentIndex() >= count() - 1 ? 0 : currentIndex() + 1;
    setCurrentIndex(index);
}

bool DDocumentTabBar::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == this && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::RightButton) {
            const int index = tabAt(mouseEvent->pos());
            if (index >= 0) {
                QMenu menu(this);
                QAction *closeAction = menu.addAction(tr("Close tab"));
                QAction *closeOthersAction = menu.addAction(tr("Close other tabs"));

                if (QAction *action = menu.exec(mapToGlobal(mouseEvent->pos()))) {
                    if (action == closeAction) {
                        Q_EMIT tabCloseRequested(index);
                    } else if (action == closeOthersAction) {
                        closeOtherDocuments(index);
                    }
                }

                return true;
            }
        }
    }

    return DTabBar::eventFilter(watched, event);
}

QVariantMap DDocumentTabBar::documentData(int index) const
{
    if (index < 0 || index >= count())
        return QVariantMap();

    return tabData(index).toMap();
}

void DDocumentTabBar::setDocumentData(int index, const QVariantMap &data)
{
    if (index < 0 || index >= count())
        return;

    setTabData(index, data);
}

void DDocumentTabBar::refreshTabText(int index)
{
    if (index < 0 || index >= count())
        return;

    setTabText(index, displayTitle(documentData(index)));
}

QString DDocumentTabBar::displayTitle(const QVariantMap &data) const
{
    const QString title = data.value(QLatin1String(kTitleKey)).toString();
    if (m_modifiedMarkerEnabled && data.value(QLatin1String(kModifiedKey)).toBool())
        return m_modifiedMarker + title;

    return title;
}

QStringList DDocumentTabBar::documentIdsExcept(const QString &documentId) const
{
    QStringList ids;
    for (int i = 0; i < count(); ++i) {
        const QString id = documentIdAt(i);
        if (id != documentId)
            ids << id;
    }

    return ids;
}

DWIDGET_END_NAMESPACE
