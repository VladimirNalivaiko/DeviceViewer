#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>

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

private:
    Ui::MainWindow *ui;

    QMultiMap<QString, DeviceInfo> DeviceTree;
    QList<QString> GUIDList;

    DWORD SetupDiGetClassDevsFlags = DIGCF_PRESENT | DIGCF_ALLCLASSES;
    HDEVINFO DeviceInfoSet;

private:
    void EnumerateDeviceTree();
    void UpdateTreeView();
};

#endif // MAINWINDOW_H
