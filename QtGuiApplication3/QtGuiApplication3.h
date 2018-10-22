#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtGuiApplication3.h"

class QtGuiApplication3 : public QMainWindow
{
	Q_OBJECT


public:
	QtGuiApplication3(QWidget *parent = Q_NULLPTR);
	

private:
	Ui::QtGuiApplication3Class ui;
	QString get_cpuId();
	void getcpuid(unsigned int CPUInfo[4], unsigned int InfoType);
	void getcpuidex(unsigned int CPUInfo[4], unsigned int InfoType, unsigned int ECXValue);
	QString getHostMacAddress();
	QString getHostIpAddress();
	QString getDiskID();
	bool activationJudge(QString machineCode);


 	 private slots:
	 void getEncodeString();
};
