#include "dmainwindowbackground.h"
#include <QFile>
#include <DThemeManager>
#include <QDebug>
#include <QDir>
#include "dapplication.h"

DWIDGET_USE_NAMESPACE

DMainWindowBackground::DMainWindowBackground(QMainWindow *window)
{
    setMainWindow(window);
    refresh();
    m_appName = DApplication::applicationName();
}

void DMainWindowBackground::drawInWidget(QPainter *painter)
{
    if (!m_dmainWindow || !painter) {
        // 防止空指针导致崩溃
        return;
    }
    // 只有在设置了壁纸才会渲染
    if (!m_isSetBackground) {
        return;
    }
    QString theme = DThemeManager::instance()->theme(m_dmainWindow);

    QList<BackgroundPlace> list = {
        BackgroundPlace::FullWindow,
        BackgroundPlace::Center,
        BackgroundPlace::TopCenter,
        BackgroundPlace::BottomCenter,
        BackgroundPlace::CenterLeft,
        BackgroundPlace::CenterRight,
        BackgroundPlace::TopLeft,
        BackgroundPlace::TopRight,
        BackgroundPlace::BottomLeft,
        BackgroundPlace::BottomRight
    };
    for (BackgroundPlace i: list) {
        QList<int> xy = getImageXY(DMainWindowBackground::BackgroundPlace(i));
        int x = xy[0];
        int y = xy[1];
        painter->drawImage(x, y, getImage(DMainWindowBackground::BackgroundPlace(i)));
    }
    //// 绘制半透明图层
    // 备份原来的 QPen 和 QBrush
    QPen oldPen = painter->pen();
    QBrush oldBrush = painter->brush();
    if (theme == "dark") {
        painter->setPen(QPen(QColor("#99252525")));
        painter->setBrush(QBrush(QColor("#99252525")));
    }
    else {
        painter->setPen(QPen(QColor("#99ffffff")));
        painter->setBrush(QBrush(QColor("#99ffffff")));
    }
    int windowWidth = m_dmainWindow->size().width();
    int windowHeight = m_dmainWindow->size().height();
    painter->drawRect(0, 0, windowWidth, windowHeight);
    // 还原原来的 QPen 和 QBrush
    painter->setPen(oldPen);
    painter->setBrush(oldBrush);

}

void DMainWindowBackground::setMainWindow(QMainWindow *window)
{
    m_dmainWindow = window;
}

void DMainWindowBackground::resizeImage()
{
    if (!m_dmainWindow) {
        // 防止空指针导致崩溃
        return;
    }
    m_backgroundResized = m_imageVar[BackgroundPlace::FullWindow].scaled(m_dmainWindow->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
}

QImage DMainWindowBackground::getImage(BackgroundPlace place)
{
    if (place == BackgroundPlace::FullWindow) {
        // 拉伸图片
        return m_backgroundResized;
    }
    return m_imageVar[place];
}

QList<int> DMainWindowBackground::getImageSize(BackgroundPlace place)
{
    QList<int> size;
    QImage image = getImage(place);
    size << image.size().width() << image.height();
    return size;
}

QList<int> DMainWindowBackground::getImageXY(BackgroundPlace place)
{
    QList<int> xy;
    if (!m_dmainWindow) {
        // 防止空指针导致崩溃
        return xy;
    }
    QList<int> image = getImageSize(place);
    int imageWidth = image[0];
    int imageHeight = image[1];
    int windowWidth = 0;
    int windowHeight = 0;
    if (m_dmainWindow) {
        windowWidth = m_dmainWindow->width();
        windowHeight = m_dmainWindow->height();
    }
    int x = 0, y = 0;

    //// 确定 X 轴
    // 如果为 Left（0,3,6）,则可以直接确定 X = 0
    if (place % 3 == 0) {
        x = 0;
    }
    // 如果为 Center（1,4,7）,则可以直接确定 X = (width - image.width) / 2
    if (place % 3 == 1 || place == BackgroundPlace::FullWindow) {
        x = (windowWidth - imageWidth) / 2;
    }
    // 如果为 Bottom（2,5,8）,则可以直接确定 X = width - image.width
    if (place % 3 == 2) {
        x = windowWidth - imageWidth;
    }
    //// 确定 Y 轴
    // 如果为 Top（0<=place<=2）,则可以直接确定 Y = 0
    if (place >= BackgroundPlace::TopLeft && place <= BackgroundPlace::TopRight) {
        y = 0;
    }
    // 如果为 Center（3<=place<=5）,则可以直接确定 Y = (height - image.height) / 2
    if ((place >= BackgroundPlace::CenterLeft && place <= BackgroundPlace::CenterRight) || place == 9) {
        y = (windowHeight - imageHeight) / 2;
    }
    // 如果为 Bottom（6<=place<=8）,则可以直接确定 Y = height - image.width
    if (place >= BackgroundPlace::BottomLeft && place <= BackgroundPlace::BottomRight) {
        y = windowHeight - imageHeight;
    }
    xy << x << y;
    return xy;
}

void DMainWindowBackground::setAppName(QString name)
{
    m_appName = name;
}

void DMainWindowBackground::setUseGlobalBackground(bool use)
{
    m_useGlobalBackground = use;
}

void DMainWindowBackground::setCustomBackgroundPath(QStringList pathList)
{
    m_customBackgroundPath = pathList;
}

QStringList DMainWindowBackground::customBackgroundPath()
{
    return m_customBackgroundPath;
}

bool DMainWindowBackground::isSetBackground()
{
    return m_isSetBackground;
}

void DMainWindowBackground::refresh()
{
    if (!m_dmainWindow) {
        // 防止空指针导致崩溃
        return;
    }
    m_isSetBackground = false;
    QString theme = DThemeManager::instance()->theme(m_dmainWindow);

    QStringList imageName = m_imageList;
    QStringList imagePath = {};
    // 清空列表
    m_imageVar.clear();
    // 获取图片路径
    //QString basePath = QDir::homePath() + "/.config/GXDE/" + m_appName + "/background-" + theme;
    QStringList basePathList;
    if (m_useGlobalBackground) {
        basePathList << "/usr/share/backgrounds/GXDE/dtk2/global/background-" + theme
                     << QDir::homePath() + "/.config/GXDE/dtk2/global/background-" + theme;
    }
    basePathList << "/usr/share/backgrounds/GXDE/dtk2/" + m_appName + "/background-" + theme
                 << QDir::homePath() + "/.config/GXDE/" + m_appName + "/background-" + theme;
    for (QString i: m_customBackgroundPath) {
        basePathList.append(i);
    }
    for (QString i: imageName) {
        QString image = basePathList[0] + "-" + i + ".png";
        for (QString j: basePathList) {
            QString tempPath = j + "-" + i + ".png";
            if (QFile::exists(tempPath)) {
                image = tempPath;
            }
        }
        imagePath.append(image);
    }
    // 加载 QImage 对象
    for (int i = 0; i < imageName.count(); ++i) {
        QImage image;
        if (QFile::exists(imagePath[i])) {
            m_isSetBackground = true;
            image.load(imagePath[i]);
            if (i == 8) {
                // 如果已经设置了右下角 logo,则不重复显示
                m_showFMLogo = false;
            }
        }
        m_imageVar.append(image);
    }
    resizeImage();
    // 读取默认 logo
    //m_fmLogo = QImage(":/images/images/fm-logo.png");
}
