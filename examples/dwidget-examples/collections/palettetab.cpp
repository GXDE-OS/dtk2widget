#include "palettetab.h"

#include "dbackgroundgroup.h"
#include "dblursurface.h"
#include "dcardwidget.h"
#include "dguiapplicationhelper.h"
#include "dswitchbutton.h"
#include "dtypographylabel.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

static QLabel *label(const QString &text, DTypographyLabel::TextRole role, QWidget *parent)
{
    DTypographyLabel *l = new DTypographyLabel(text, role, parent);
    l->setWordWrap(true);
    return l;
}

static QWidget *row(const QString &title, const QString &description, QWidget *rightWidget, QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QHBoxLayout *layout = new QHBoxLayout(w);
    QVBoxLayout *textLayout = new QVBoxLayout;
    textLayout->setSpacing(2);
    textLayout->addWidget(label(title, DTypographyLabel::Body, w));
    DTypographyLabel *caption = new DTypographyLabel(description, DTypographyLabel::Caption, w);
    caption->setSecondary(true);
    textLayout->addWidget(caption);

    layout->setContentsMargins(14, 10, 14, 10);
    layout->addLayout(textLayout);
    layout->addStretch();
    if (rightWidget)
        layout->addWidget(rightWidget);
    return w;
}

static QWidget *createPreview(const QString &title, DGuiApplicationHelper::ColorType type, QWidget *parent)
{
    QWidget *page = new QWidget(parent);
    page->setAutoFillBackground(true);
    page->setPalette(DGuiApplicationHelper::standardPalette(type));

    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(12);

    DCardWidget *card = new DCardWidget(page);
    card->setPalette(page->palette());
    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(18, 14, 18, 14);
    cardLayout->setSpacing(8);
    cardLayout->addWidget(new DSectionTitle(title, card));
    DTypographyLabel *body = new DTypographyLabel("使用 QPalette 角色取色的组件可以在浅色和深色主题下复用同一套代码。", DTypographyLabel::Body, card);
    body->setWordWrap(true);
    cardLayout->addWidget(body);
    cardLayout->addWidget(new QPushButton("操作", card), 0, Qt::AlignLeft);
    layout->addWidget(card);

    DBackgroundGroup *group = new DBackgroundGroup(new QVBoxLayout, page);
    group->setPalette(page->palette());
    group->layout()->setContentsMargins(0, 0, 0, 0);
    group->setItemMargins(QMargins(0, 0, 0, 0));
    group->layout()->addWidget(row("分组行", "统一圆角背景跟随当前预览调色板。", new DSwitchButton(group), group));
    group->layout()->addWidget(row("次要文字", "Caption 文本保持对比度，不需要应用硬编码颜色。", new QPushButton("打开", group), group));
    layout->addWidget(group);

    DBlurSurface *surface = new DBlurSurface(page);
    surface->setPalette(page->palette());
    surface->setMaskAlpha(type == DGuiApplicationHelper::DarkType ? 120 : 82);
    QVBoxLayout *surfaceLayout = new QVBoxLayout(surface);
    surfaceLayout->setContentsMargins(18, 14, 18, 14);
    surfaceLayout->addWidget(label("Blur 面板", DTypographyLabel::Title, surface));
    surfaceLayout->addWidget(label("内置调色板也会影响半透明遮罩和模糊面板的观感。", DTypographyLabel::Caption, surface));
    layout->addWidget(surface);

    layout->addStretch();
    return page;
}

PaletteTab::PaletteTab(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(14);

    DTypographyLabel *heading = new DTypographyLabel("内置调色板主题", DTypographyLabel::Title, this);
    heading->setEmphasis(true);
    mainLayout->addWidget(heading);
    DTypographyLabel *intro = new DTypographyLabel("DTK2 提供浅色和深色 QPalette 预设。这里用同一组组件分别套用两套调色板，验证应用不依赖系统 ddark/dlight 样式也能正确显示。", DTypographyLabel::Body, this);
    intro->setSecondary(true);
    intro->setWordWrap(true);
    mainLayout->addWidget(intro);

    QHBoxLayout *previewLayout = new QHBoxLayout;
    previewLayout->setSpacing(16);
    previewLayout->addWidget(createPreview("浅色调色板", DGuiApplicationHelper::LightType, this));
    previewLayout->addWidget(createPreview("深色调色板", DGuiApplicationHelper::DarkType, this));
    mainLayout->addLayout(previewLayout);
}
