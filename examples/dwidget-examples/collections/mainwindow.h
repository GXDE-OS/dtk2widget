/*
 * Copyright (C) 2015 ~ 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTabWidget>

#include "dmainwindow.h"

#include "bartab.h"
#include "buttontab.h"
#include "inputtab.h"
#include "linetab.h"
#include "slidertab.h"
#include "indicatortab.h"
#include "widgetstab.h"
#include "containertab.h"
#include "palettetab.h"


class QAction;
class QActionGroup;
class QMenu;

class MainWindow : public Dtk::Widget::DMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected Q_SLOTS:
    void menuItemInvoked(QAction *action);

private:
    void initTitlebarMenu();
    void initTabWidget();
    void loadSection(int index);
    void applyDemoBackground(bool enabled);
    void setDemoBackgroundTransparent(bool transparent);
    void updateThemeActions();

private:
    QTabWidget *m_mainTab = NULL;
    QMenu *m_titleMenu = NULL;
    QActionGroup *m_themeGroup = NULL;
    QAction *m_lightAction = NULL;
    QAction *m_darkAction = NULL;
    QAction *m_systemThemeAction = NULL;
    QAction *m_backgroundAction = NULL;
    QAction *m_transparentAction = NULL;
    QAction *m_blurWindowAction = NULL;
};

#endif // MAINWINDOW_H
