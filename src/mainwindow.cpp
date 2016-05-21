#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <Cfgmgr32.h>
#include <Regstr.h>
#include <Setupapi.h>

#include <QDebug>

#include <QLabel>
#include <QMainWindow>
#include <QFile>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->treeWidget->setColumnCount(1);
    ui->treeWidget->setSortingEnabled(true);

    EnumerateDeviceTree();
    UpdateTreeView();
}

void MainWindow::EnumerateDeviceTree()
{
    DeviceInfoSet = SetupDiGetClassDevs(NULL,
                                        NULL,
                                        NULL,
                                        SetupDiGetClassDevsFlags);
    SP_DEVINFO_DATA DeviceInfoData;
    ZeroMemory(&DeviceInfoData, sizeof(SP_DEVINFO_DATA));
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    DWORD RequiredSize = 0;


    wchar_t PropertyBufferW[128] = { 0 };
    wchar_t GUID_w[128] = { 0 };
    DWORD GUID_w_size = 0;

    DWORD i = 0;
    while(SetupDiEnumDeviceInfo(DeviceInfoSet, i, &DeviceInfoData)) {
        RequiredSize = 256;
        if(CM_Get_DevNode_Registry_Property(DeviceInfoData.DevInst,
                                            CM_DRP_FRIENDLYNAME,
                                            NULL,
                                            PropertyBufferW,
                                            &RequiredSize,
                                            0) != CR_SUCCESS || RequiredSize == 0) {
            RequiredSize = 256;
            if(CM_Get_DevNode_Registry_Property(DeviceInfoData.DevInst,
                                                CM_DRP_DEVICEDESC,
                                                NULL,
                                                PropertyBufferW,
                                                &RequiredSize, 0) != CR_SUCCESS || RequiredSize == 0)
            {
                 i++;
                 continue;
            }
        }
        if(CM_Get_DevNode_Registry_Property(DeviceInfoData.DevInst,
                                            CM_DRP_CLASSGUID,
                                            NULL,
                                            GUID_w,
                                            &GUID_w_size,
                                            0) == CR_SUCCESS && GUID_w_size != 0)
        {
            GUID_w_size = 128;
            QString GUID_s = QString::fromWCharArray(GUID_w, GUID_w_size/2 - 1);
            DeviceInfo temp;
            temp.DeviceDescription = QString::fromWCharArray(PropertyBufferW,
                                                             RequiredSize/2 - 1);
            memcpy(&(temp.DeviceInfoData), &DeviceInfoData, DeviceInfoData.cbSize);

            RequiredSize = 256;
            CM_Get_DevNode_Registry_Property(DeviceInfoData.DevInst,
                                             CM_DRP_CLASS,
                                             NULL,
                                             PropertyBufferW,
                                             &RequiredSize,
                                             0);
            temp.DeviceClass = QString::fromWCharArray(PropertyBufferW, RequiredSize/2 - 1);

            if(DeviceTree.values(GUID_s).isEmpty()) {
                GUIDList.append(GUID_s);
            }
            DWORD flags = 0;
            DWORD flags_size = 4;
            if(CM_Get_DevNode_Registry_Property(DeviceInfoData.DevInst,
                                                CM_DRP_CONFIGFLAGS,
                                                NULL,
                                                &flags,
                                                &flags_size,
                                                0) == CR_SUCCESS) {
                temp.isDisabled = flags & CONFIGFLAG_DISABLED;
            }

            ULONG  pulStatus = 0;
            ULONG  pulProblemNumber = 0;
            CONFIGRET CM_RET = CR_SUCCESS;
            CM_RET = CM_Get_DevNode_Status(&pulStatus,
                                           &pulProblemNumber,
                                           DeviceInfoData.DevInst,
                                           0);
            if(CM_RET == CR_NO_SUCH_DEVINST) {
                temp.isPlugged = false;
            }
            if(CM_RET == CR_SUCCESS) {
                temp.isPlugged = true;
            }
            if(pulStatus & DN_HAS_PROBLEM) {
                temp.isGood = false;
            } else {
                temp.isGood = true;
            }
            if(pulStatus & DN_DISABLEABLE){
                temp.isDisableable = true;
            } else {
                temp.isDisableable = false;
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
    for(int i = 0; i < GUIDList.size(); i++)
    {
        QList<DeviceInfo> dev_info_list = DeviceTree.values(GUIDList.at(i));
        RequiredSize = 256;
        QTreeWidgetItem *ClassSubTree = new QTreeWidgetItem(ui->treeWidget);

        if(SetupDiGetClassDescriptionW(&(dev_info_list.at(0).DeviceInfoData.ClassGuid),
                                       PropertyBufferW,
                                       RequiredSize,
                                       &RequiredSize))
        {
            ClassSubTree->setText(0, QString::fromWCharArray(PropertyBufferW, RequiredSize - 1));
        }

        for(int j = 0; j < dev_info_list.size(); j++)
        {
            QTreeWidgetItem *ClassSubTreeChild = new QTreeWidgetItem();
            QVariant data = QVariant::fromValue(dev_info_list.at(j));
            ClassSubTreeChild->setData(0, Qt::UserRole, data);
            ClassSubTreeChild->setText(0, dev_info_list.at(j).DeviceDescription);
        }
        for(int j = 0; j < dev_info_list.size(); j++)
        {
            QTreeWidgetItem *ClassSubTreeChild = new QTreeWidgetItem();
            QVariant data = QVariant::fromValue(dev_info_list.at(j));
            ClassSubTreeChild->setData(0, Qt::UserRole, data);
            ClassSubTreeChild->setText(0, dev_info_list.at(j).DeviceDescription);
            ClassSubTree->addChild(ClassSubTreeChild);
        }
    }
}

void MainWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    QVariant a = item->data(column, Qt::UserRole);
    DeviceInfo dev_info = a.value<DeviceInfo>();
    if(dev_info.DeviceDescription.isEmpty())
    {
        return;
    }

    QLabel *DevDescLabel = new QLabel();
    wchar_t PropertyBufferW[128] = { 0 };

    DWORD RequiredSize = 256;
    if(CM_Get_DevNode_Registry_PropertyW(dev_info.DeviceInfoData.DevInst,
                                         CM_DRP_CLASS,
                                         NULL,
                                         PropertyBufferW,
                                         &RequiredSize,
                                         0) == CR_SUCCESS && RequiredSize != 0)
    {
        DevDescLabel->setText(DevDescLabel->text()
                              + "Class: \t\t\t"
                              + QString::fromWCharArray(PropertyBufferW,
                                                        RequiredSize/2 - 1));
    }

    RequiredSize = 256;
    if(CM_Get_DevNode_Registry_PropertyW(dev_info.DeviceInfoData.DevInst,
                                         CM_DRP_CLASSGUID,
                                         NULL,
                                         PropertyBufferW,
                                         &RequiredSize,
                                         0) == CR_SUCCESS && RequiredSize != 0)
    {
        DevDescLabel->setText(DevDescLabel->text()
                              + "\nClass GUID: \t\t"
                              + QString::fromWCharArray(PropertyBufferW,
                                                        RequiredSize/2 - 1));
    }

    RequiredSize = 256;
    if(CM_Get_DevNode_Registry_PropertyW(dev_info.DeviceInfoData.DevInst,
                                         CM_DRP_ENUMERATOR_NAME,
                                         NULL,
                                         PropertyBufferW,
                                         &RequiredSize,
                                         0) == CR_SUCCESS && RequiredSize != 0)
    {
        DevDescLabel->setText(DevDescLabel->text()
                              + "\nEnumerator: \t\t"
                              + QString::fromWCharArray(PropertyBufferW,
                                                        RequiredSize/2 - 1));
    }

    RequiredSize = 256;
    if(CM_Get_DevNode_Registry_PropertyW(dev_info.DeviceInfoData.DevInst,
                                         CM_DRP_LOCATION_INFORMATION,
                                         NULL,
                                         PropertyBufferW,
                                         &RequiredSize,
                                         0) == CR_SUCCESS && RequiredSize != 0)
    {
        DevDescLabel->setText(DevDescLabel->text()
                              + "\nLocation: \t\t\t"
                              + QString::fromWCharArray(PropertyBufferW,
                                                        RequiredSize/2 - 1));
    }

    RequiredSize = 256;
    if(CM_Get_DevNode_Registry_PropertyW(dev_info.DeviceInfoData.DevInst,
                                         CM_DRP_MFG,
                                         NULL,
                                         PropertyBufferW,
                                         &RequiredSize,
                                         0) == CR_SUCCESS && RequiredSize != 0)
    {
        DevDescLabel->setText(DevDescLabel->text()
                              + "\nManufacturer: \t\t"
                              + QString::fromWCharArray(PropertyBufferW,
                                                        RequiredSize/2 - 1));
    }

    RequiredSize = 256;
    if(CM_Get_DevNode_Registry_PropertyW(dev_info.DeviceInfoData.DevInst,
                                         CM_DRP_PHYSICAL_DEVICE_OBJECT_NAME,
                                         NULL,
                                         PropertyBufferW,
                                         &RequiredSize,
                                         0) == CR_SUCCESS && RequiredSize != 0)
    {
        DevDescLabel->setText(DevDescLabel->text()
                              + "\nPhysical Object Name: \t"
                              + QString::fromWCharArray(PropertyBufferW,
                                                        RequiredSize/2 - 1));
    }

    RequiredSize = 256;
    if(CM_Get_DevNode_Registry_PropertyW(dev_info.DeviceInfoData.DevInst,
                                         CM_DRP_SECURITY_SDS,
                                         NULL,
                                         PropertyBufferW,
                                         &RequiredSize,
                                         0) == CR_SUCCESS && RequiredSize != 0) {
        DevDescLabel->setText(DevDescLabel->text()
                              + "\nSecurity descriptor: \t" +
                              QString::fromWCharArray(PropertyBufferW,
                                                      RequiredSize/2 - 1));
    }

    RequiredSize = 256;
    if(CM_Get_DevNode_Registry_PropertyW(dev_info.DeviceInfoData.DevInst,
                                         CM_DRP_SERVICE,
                                         NULL,
                                         PropertyBufferW,
                                         &RequiredSize,
                                         0) == CR_SUCCESS && RequiredSize != 0)
    {
        DevDescLabel->setText(DevDescLabel->text()
                              + "\nService: \t\t\t" +
                              QString::fromWCharArray(PropertyBufferW,
                                                      RequiredSize/2 - 1));
    }

    RequiredSize = 256;
    if(CM_Get_DevNode_Registry_PropertyW(dev_info.DeviceInfoData.DevInst,
                                         CM_DRP_FRIENDLYNAME,
                                         NULL,
                                         PropertyBufferW,
                                         &RequiredSize,
                                         0) == CR_SUCCESS && RequiredSize != 0)
    {
        DevDescLabel->setText(DevDescLabel->text()
                              + "\nDevice Name: \t\t"
                              + QString::fromWCharArray(PropertyBufferW,
                                                        RequiredSize/2 - 1));
    }

    RequiredSize = 256;
    if(CM_Get_DevNode_Registry_PropertyW(dev_info.DeviceInfoData.DevInst,
                                         CM_DRP_DEVICEDESC,
                                         NULL,
                                         PropertyBufferW,
                                         &RequiredSize,
                                         0) == CR_SUCCESS && RequiredSize != 0)
    {
        DevDescLabel->setText(DevDescLabel->text()
                              + "\nDevice Description: \t\t" +
                              QString::fromWCharArray(PropertyBufferW,
                                                      RequiredSize/2 - 1));
    }

    QWidget *w = new QWidget();
    {
        w->setWindowFlags(Qt::WindowCloseButtonHint);
        w->setWindowTitle(dev_info.DeviceDescription);
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(DevDescLabel);
        w->setLayout(layout);
        w->layout()->addWidget(DevDescLabel);
    }
    w->show();
}
MainWindow::~MainWindow()
{
    delete ui;
}
