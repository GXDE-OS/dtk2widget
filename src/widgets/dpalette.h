#ifndef DPALETTE_H
#define DPALETTE_H

#include <QObject>
#include <QWidget>
#include <QPalette>

class DPalette: public QPalette
{
public:
    enum ColorType {
        NoType,
        ItemBackground,     //列表项的背景色
        TextTitle,          //标题型文本的颜色
        TextTips,           //提示性文本的颜色
        TextWarning,        //警告类型的文本颜色
        TextLively,         //活跃式文本颜色（不受活动色影响）
        LightLively,        //活跃式按钮（recommend button）背景色中的亮色（不受活跃色影响）
        DarkLively,         //活跃式按钮（recommend button）背景色中的暗色，会从亮色渐变到暗色（不受活跃色影响）
        FrameBorder,        //控件边框颜色
        PlaceholderText,    //占位类型的文本颜色，可用于输入框占位内容等提示性文字
        FrameShadowBorder,  //用于跟阴影叠加的边框颜色
        ObviousBackground,  //明显的背景色
        NColorTypes
    };

    DPalette();
    DPalette(const QPalette &palette);
};

#endif // DPALETTE_H
