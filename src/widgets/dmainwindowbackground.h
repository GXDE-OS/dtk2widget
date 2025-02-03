#ifndef DMainWindowBackground_H
#define DMainWindowBackground_H

#include <QImage>
#include <QList>
#include <QPainter>
#include <QMainWindow>
#include "dtkwidget_global.h"

DWIDGET_BEGIN_NAMESPACE

class DMainWindowBackground
{
public:
    enum BackgroundPlace {
        TopLeft = 0,
        TopCenter = 1,
        TopRight = 2,
        CenterLeft = 3,
        Center = 4,
        CenterRight = 5,
        BottomLeft = 6,
        BottomCenter = 7,
        BottomRight = 8,
        // 特殊的一类
        FullWindow = 9
    };

    enum ThemesType {
        light = 0,
        dark = 1
    };

    QStringList m_imageList = {"TopLeft", "TopCenter", "TopRight",
                            "CenterLeft", "Center", "CenterRight",
                            "BottomLeft", "BottomCenter", "BottomRight",
                             // 特殊的一类
                            "FullWindow"};
    QStringList m_imageTranslateList = {"TopLeft", "TopCenter", "TopRight",
                                        "CenterLeft", "Center", "CenterRight",
                                        "BottomLeft", "BottomCenter", "BottomRight",
                                         // 特殊的一类
                                        "FullWindow"};

    DMainWindowBackground(QMainWindow *window);
    void refresh();
    void resizeImage();
    void setMainWindow(QMainWindow *window);
    void setAppName(QString name);
    void setUseGlobalBackground(bool use);
    void setCustomBackgroundPath(QStringList pathList);
    void setUserBackground(ThemesType themetype, QString imagePath);

    QImage getImage(BackgroundPlace place);
    QList<int> getImageXY(BackgroundPlace place);
    QList<int> getImageSize(BackgroundPlace place);
    QStringList customBackgroundPath();

    void drawInWidget(QPainter *painter);

    bool isSetBackground();

private:
    QList<QImage> m_imageVar;
    QList<bool> m_imageVarIsGlobal;
    QMainWindow *m_dmainWindow;
    bool m_showFMLogo = true;
    bool m_useGlobalBackground = true;
    bool m_isSetBackground = false;
    QImage m_fmLogo;
    QImage m_backgroundResized;
    QString m_appName;
    QStringList m_customBackgroundPath;
};

DWIDGET_END_NAMESPACE

#endif // DMainWindowBackground_H
