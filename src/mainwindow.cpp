#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <Cfgmgr32.h>
#include <Regstr.h>
#include <Setupapi.h>

#include <QDebug>
#include <QMainWindow>
#include <QFile>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->treeWidget->setColumnCount(1);
    ui->treeWidget->setSortingEnabled(true);

    EnumerateDeviceTree();
    UpdateTreeView();
}

void MainWindow::EnumerateDeviceTree()
{
    DeviceInfoSet = SetupDiGetClassDevs(NULL, NULL, NULL, SetupDiGetClassDevsFlags);
    SP_DEVINFO_DATA DeviceInfoData;
    ZeroMemory(&DeviceInfoData, sizeof(SP_DEVINFO_DATA));
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    DWORD RequiredSize = 0;

    wchar_t GUID_w[128] = { 0 };
    DWORD GUID_w_size = 0;

    DWORD i = 0;
    while(SetupDiEnumDeviceInfo(DeviceInfoSet, i, &DeviceInfoData)) {
        RequiredSize = 256;

        if(CM_Get_DevNode_Registry_PropertyW(DeviceInfoData.DevInst, CM_DRP_CLASSGUID, NULL, GUID_w, &GUID_w_size, 0) ==
                CR_SUCCESS && GUID_w_size != 0) {
            GUID_w_size = 128;
            QString GUID_s = QString::fromWCharArray(GUID_w, GUID_w_size/2 - 1);
            DeviceInfo temp;
            memcpy(&(temp.DeviceInfoData), &DeviceInfoData, DeviceInfoData.cbSize);

            if(DeviceTree.values(GUID_s).isEmpty()) {
                GUIDList.append(GUID_s);
            }

            DeviceTree.insert(GUID_s, temp);
        }
        i++;
    }
}
void MainWindow::UpdateTreeView()
{
    DWORD RequiredSize = 0;
    wchar_t PropertyBufferW[256] = { 0 };
    for(int i = 0; i < GUIDList.size(); i++) {
        QList<DeviceInfo> dev_info_list = DeviceTree.values(GUIDList.at(i));
        RequiredSize = 256;
        QTreeWidgetItem *ClassSubTree = new QTreeWidgetItem(ui->treeWidget);

        if(SetupDiGetClassDescriptionW(&(dev_info_list.at(0).DeviceInfoData.ClassGuid),
                                       PropertyBufferW,  RequiredSize,  &RequiredSize)) {
            ClassSubTree->setText(0, QString::fromWCharArray(PropertyBufferW, RequiredSize - 1));
        }

        for(int j = 0; j < dev_info_list.size(); j++) {
            QTreeWidgetItem *ClassSubTreeChild = new QTreeWidgetItem();
            QVariant data = QVariant::fromValue(dev_info_list.at(j));
            ClassSubTreeChild->setData(0, Qt::UserRole, data);
            ClassSubTreeChild->setText(0, dev_info_list.at(j).DeviceDescription);
        }
        for(int j = 0; j < dev_info_list.size(); j++) {
            QTreeWidgetItem *ClassSubTreeChild = new QTreeWidgetItem();
            QVariant data = QVariant::fromValue(dev_info_list.at(j));
            ClassSubTreeChild->setData(0, Qt::UserRole, data);
            ClassSubTreeChild->setText(0, dev_info_list.at(j).DeviceDescription);
            ClassSubTree->addChild(ClassSubTreeChild);
        }
    }
}
MainWindow::~MainWindow()
{
    delete ui;
}
