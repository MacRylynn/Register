#include "QtGuiApplication3.h"
#include <QNetworkInterface>
#include<QProcess>
#include<md5_encode.h>
#include <string>
#include <QDateTime>
#include<iostream>
#include<QInputDialog>
#include<QSettings>
#include<QMessageBox>
using namespace std;

QtGuiApplication3::QtGuiApplication3(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	connect(ui.pushButton, SIGNAL(clicked()), this,SLOT(getEncodeString()));

}
//得到MD5加密之后的密钥
void QtGuiApplication3::getEncodeString() {
	QString CPU_ID = get_cpuId();
	QString MAC = getHostMacAddress();
	QString IP= getHostIpAddress();
	QString DISK_ID = getDiskID();
	string original= CPU_ID.toStdString()+ MAC.toStdString() + IP.toStdString() + DISK_ID.toStdString();
	//给定的salt用于不可逆的MD5加密
	string salt = "acs3@!@$#$@#)(*___*sdfkjf124654wefsdfsjdfh8ew8rywe0r983945wproiweh92385osjsdf2525s4f65";
	Md5Encode encode_obj;
	string ret_1 = encode_obj.Encode(original+salt);//得到了原始加了salt之后的加密字符串
	string ret_2 = encode_obj.Encode(ret_1);
	string ret_3 = encode_obj.Encode(ret_2);
	string ret_4 = encode_obj.Encode(ret_3);
	string ret_5 = encode_obj.Encode(ret_4);
	string ret_6 = encode_obj.Encode(ret_5);
	string ret;
	transform(ret_6.begin(), ret_6.end(), back_inserter(ret), ::toupper);//全部都转为大写
	QString encodePassword = QString::fromStdString(ret);//转为QString方便显示
	ui.textEdit->setText(encodePassword);
	//从注册表中找到键值为activeCode的值
	QSettings *reg = new QSettings("HKEY_CURRENT_USER\\Software\\moemil", QSettings::NativeFormat);
	reg->setValue("Version","1.0");
	QString activeCode = reg->value("activeCode").toString();
	//根据找到的值和加密之后的本机信息对比
	if (!activeCode.isNull() && activeCode == encodePassword) {
		QMessageBox::warning(NULL, QObject::tr("success"), QObject::tr("Activie successfuly"));
	}
	else {
		QString itemText2 = QInputDialog::getText(0, tr("ActivieCode"), tr("please input:"));
		if (itemText2.toStdString() == ret) {
			//如果输入的密钥和本机的信息匹配，则在注册表中写入这个值
			reg->setValue("activeCode", itemText2);
			delete reg;
			QMessageBox::warning(NULL, QObject::tr("success"), QObject::tr("Activie successfuly"));
		}
		else {
			QMessageBox::warning(NULL, QObject::tr("error"), QObject::tr("ActivieCode error"));
			
		}

	}

	//得到注册的时间
	QDateTime curDateTime = QDateTime::currentDateTime();
	QString curDateTimeStr = curDateTime.toString("yyyy-MM-dd  hh:mm:ss");
	std::cout << "current time  is " << curDateTimeStr.toStdString()<<endl<< ret<<endl;

	////保存到注册表
	//QSettings *reg = new QSettings("HKEY_CURRENT_USER\\Software\\qCloudSecChk", QSettings::NativeFormat);
	//reg->setValue("activeCode",encodePassword);
	//delete reg;
	//activationJudge(encodePassword);
}
//得到电脑的CPU的ID
QString QtGuiApplication3::get_cpuId() {
	QString cpu_id = "";
	unsigned int dwBuf[4] = { 0 };
	unsigned long long ret = 0;
	getcpuid(dwBuf, 1);
	ret = dwBuf[3];
	ret = ret << 32;

	QString str0 = QString::number(dwBuf[3], 16).toUpper();
	QString str0_1 = str0.rightJustified(8, '0');//这一句的意思是前面补0，但是我遇到的情况是这里都填满了
	QString str1 = QString::number(dwBuf[0], 16).toUpper();
	QString str1_1 = str1.rightJustified(8, '0');//这里必须在前面补0，否则不会填满数据
												 //cpu_id = cpu_id + QString::number(dwBuf[0], 16).toUpper();
	cpu_id = str0_1 + str1_1;
	return cpu_id;
}
void QtGuiApplication3::getcpuid(unsigned int CPUInfo[4], unsigned int InfoType)
{
#if defined(__GNUC__)// GCC
	__cpuid(InfoType, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
#elif defined(_MSC_VER)// MSVC
#if _MSC_VER >= 1400 //VC2005才支持__cpuid
	__cpuid((int*)(void*)CPUInfo, (int)(InfoType));
#else //其他使用getcpuidex
	getcpuidex(CPUInfo, InfoType, 0);
#endif
#endif
}
void QtGuiApplication3::getcpuidex(unsigned int CPUInfo[4], unsigned int InfoType, unsigned int ECXValue)
{
#if defined(_MSC_VER) // MSVC
#if defined(_WIN64) // 64位下不支持内联汇编. 1600: VS2010, 据说VC2008 SP1之后才支持__cpuidex.
	__cpuidex((int*)(void*)CPUInfo, (int)InfoType, (int)ECXValue);
#else
	if (NULL == CPUInfo)  return;
	_asm {
		// load. 读取参数到寄存器.
		mov edi, CPUInfo;
		mov eax, InfoType;
		mov ecx, ECXValue;
		// CPUID
		cpuid;
		// save. 将寄存器保存到CPUInfo
		mov[edi], eax;
		mov[edi + 4], ebx;
		mov[edi + 8], ecx;
		mov[edi + 12], edx;
	}
#endif
#endif
}
//得到电脑的MAC地址
QString QtGuiApplication3::getHostMacAddress() {
	QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();// 获取所有网络接口列表
	int nCnt = nets.count();
	QString strMacAddr = "";
	for (int i = 0; i < nCnt; i++)
	{
		// 如果此网络接口被激活并且正在运行并且不是回环地址，则就是我们需要找的Mac地址
		if (nets[i].flags().testFlag(QNetworkInterface::IsUp) && nets[i].flags().testFlag(QNetworkInterface::IsRunning) && !nets[i].flags().testFlag(QNetworkInterface::IsLoopBack))
		{
			strMacAddr = nets[i].hardwareAddress();
			break;
		}
	}

	return strMacAddr;
}
//得到电脑的IP地址
QString QtGuiApplication3::getHostIpAddress()
{
	QString strIpAddress;
	QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	// 获取第一个本主机的IPv4地址
	int nListSize = ipAddressesList.size();
	for (int i = 0; i < nListSize; ++i)
	{
		if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
			ipAddressesList.at(i).toIPv4Address()) {
			strIpAddress = ipAddressesList.at(i).toString();
			break;
		}
	}
	// 如果没有找到，则以本地IP地址为IP
	if (strIpAddress.isEmpty())
		strIpAddress = QHostAddress(QHostAddress::LocalHost).toString();
	return strIpAddress;
}
//得到电脑硬盘ID
QString QtGuiApplication3::getDiskID()
{
	QProcess p;
	p.start("diskpart");
	p.waitForStarted();
	p.write("select disk 0\n detail disk\n");
	p.closeWriteChannel();
	p.waitForFinished();
	QString info = QString::fromLocal8Bit(p.readAllStandardOutput().data());
	p.kill();

	int nIdx = info.indexOf(QStringLiteral("磁盘 ID"));
	int nIdx2 = info.indexOf("\n", nIdx);

	QString strHDD = info.mid(nIdx + 6, nIdx2 - nIdx - 6);
	strHDD = strHDD.trimmed();

	return strHDD;

}
//WINDOWS下读写注册表函数

bool QtGuiApplication3::activationJudge(QString machineCode) {

	QSettings *reg = new QSettings("HKEY_CURRENT_USER\\Software\\moemil", QSettings::NativeFormat);
	QString activeCode = reg->value("activeCode").toString();

	qDebug() << "activeCode is " << activeCode;
	// QDateTime curDateTime=QDateTime::currentDateTime();
	// QString curDateTimeStr=curDateTime.toString("yyyy-MM-dd");
	//qDebug()<<"current time  is "<<curDateTimeStr;
	if (machineCode.trimmed() != activeCode) {
		QString itemText2 = QInputDialog::getText(0, tr("ActivieCode"), tr("please input:"));

		if (itemText2 == activeCode) {
			reg->setValue("activeCode", itemText2);
			return true;
		}
		else {
			QMessageBox::warning(NULL, QObject::tr("error"), QObject::tr("ActivieCode error"));
			return false;
		}


	}
}

