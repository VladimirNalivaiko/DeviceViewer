#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QToolButton>

#include <windows.h>
#include <setupapi.h>
#include <QDebug>

namespace Ui
{
    class MainWindow;
}

class DeviceInfo
{
public:
    QString DeviceDescription;
    QString DeviceClass;
    SP_DEVINFO_DATA DeviceInfoData;

    bool isDisabled;
    bool isPlugged;
    bool isDisableable;
    bool isGood;
};
Q_DECLARE_METATYPE(DeviceInfo)


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


public slots:
    void on_refreshToolButtonClicked();
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

private:
    Ui::MainWindow *ui;
    QToolButton *refreshToolButton;

    QMultiMap<QString, DeviceInfo> DeviceTree;
    QList<QString> GUIDList;

    DWORD SetupDiGetClassDevsFlags = DIGCF_PRESENT | DIGCF_ALLCLASSES;
    HDEVINFO DeviceInfoSet;

private:
    void EnumerateDeviceTree();
    void UpdateTreeView();
};

#endif // MAINWINDOW_H
