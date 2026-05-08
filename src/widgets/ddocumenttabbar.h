#ifndef DDOCUMENTTABBAR_H
#define DDOCUMENTTABBAR_H

#include "dtabbar.h"

DWIDGET_BEGIN_NAMESPACE

class LIBDTKWIDGETSHARED_EXPORT DDocumentTabBar : public DTabBar
{
    Q_OBJECT
    Q_PROPERTY(bool modifiedMarkerEnabled READ modifiedMarkerEnabled WRITE setModifiedMarkerEnabled)
    Q_PROPERTY(QString modifiedMarker READ modifiedMarker WRITE setModifiedMarker)

public:
    explicit DDocumentTabBar(QWidget *parent = nullptr);

    int addDocument(const QString &documentId, const QString &title);
    int addDocument(const QString &documentId, const QIcon &icon, const QString &title);
    int insertDocument(int index, const QString &documentId, const QString &title);
    int insertDocument(int index, const QString &documentId, const QIcon &icon, const QString &title);
    void updateDocument(int index, const QString &documentId, const QString &title);
    void closeDocument(int index);
    void closeCurrentDocument();
    void closeOtherDocuments(int index);
    void closeOtherDocumentsExcept(const QString &documentId);

    int indexOfDocument(const QString &documentId) const;
    QString documentIdAt(int index) const;
    QString currentDocumentId() const;
    QString documentTitleAt(int index) const;
    QString currentDocumentTitle() const;

    bool isDocumentModified(int index) const;
    bool modifiedMarkerEnabled() const;
    QString modifiedMarker() const;

public Q_SLOTS:
    void setDocumentModified(int index, bool modified);
    void setModifiedMarkerEnabled(bool enabled);
    void setModifiedMarker(const QString &marker);
    void previousDocument();
    void nextDocument();

Q_SIGNALS:
    void documentCloseRequested(int index, const QString &documentId);
    void documentsCloseRequested(const QStringList &documentIds);
    void documentActivated(int index, const QString &documentId);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QVariantMap documentData(int index) const;
    void setDocumentData(int index, const QVariantMap &data);
    void refreshTabText(int index);
    QString displayTitle(const QVariantMap &data) const;
    QStringList documentIdsExcept(const QString &documentId) const;

    bool m_modifiedMarkerEnabled = true;
    QString m_modifiedMarker = QStringLiteral("*");
};

DWIDGET_END_NAMESPACE

#endif // DDOCUMENTTABBAR_H
