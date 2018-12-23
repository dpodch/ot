#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>

#include "excelport_srv_config.h"

// Путь до конфиг файла
#ifdef QT_DEBUG
	QString EPortSrvConfigFile = "excelport_srv.xml";
#else
	QString EPortSrvConfigFile = "";
#endif

/** Глобальная сущность конфигурации приложения. */
EPortSrvConfig *EPortSrvConfig::instance = 0;

/** @return Возвращает текущую конфигурацию приложения. */
EPortSrvConfig &
EPortSrvConfig::getInstance()
{
	if (!instance)
	{
		instance = new EPortSrvConfig(EPortSrvConfigFile);
	}
	return *instance;
}

/** Загружает настройки конфигурации. */
EPortSrvConfig::EPortSrvConfig(const QString &fileName)
	: AbstractConfig(fileName)
{
	qDebug() << "EPortSrvConfig read config from: " << fileName;
}

int EPortSrvConfig::getRestPort() const
{
	return getValue("rest", "port").toString().toInt();
}
