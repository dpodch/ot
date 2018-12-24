#include <qtsingleapplication/qtsinglecoreapplication.h>
#include <QDebug>
#include <QTimer>
#include <QFileInfo>
#include <QDir>
#include <core/app_runtime_data.h>

#include <rest/rest_service.h>
#include <rest/handlers/default_http_handler.h>

#include <excel2xml_handler.h>
#include <config/excelport_srv_config.h>

#include <excelport_srv_ctl.h>

EPSrvCtl* EPSrvCtl::instance = NULL;

/** @brief Контруктор
	@param parent - родитель в системе Qt
*/
EPSrvCtl::EPSrvCtl() : FsDaemon()
{
	qsrand(time(NULL) + qApp->applicationPid());
}

/** @brief Деструктор */
EPSrvCtl::~EPSrvCtl()
{
}

/** Инициализация демона

	@param startParams - параметры запуска

	@return указатель на текущий инстанс демона
 */
EPSrvCtl* EPSrvCtl::initDaemon()
{
	if (instance != NULL)
	{
		qFatal("EPSrvCtl::initDaemon instance exist!");
		exit(100);
	}

	instance = new EPSrvCtl();
	return instance;
}

/** Возвращает инстанс демона

	@return указатель на текущий инстанс демона
 */
EPSrvCtl* EPSrvCtl::getInstance()
{
	if (instance == NULL)
	{
		qFatal("No EPSrvCtl instance!");
		exit(100);
	}

	return instance;
}

/** Получено сообщение из другой запускаемого демона

	@param message - сообщение
 */
void EPSrvCtl::runDaemonMesReceived(QString msg)
{
	QString message = msg.trimmed();

	qWarning() << "EPSrvCtl running receive message" << message;

	if (message == QString(DAEMON_RUNNING_DEBUG_ON_MSG))
	{
		qWarning() << "EPSrvCtl debug log ON";
		AppRuntimeData::getInstance().setAppParam(FSQtDebugLevel, QtDebugMsg);
	}
	else if (message == QString(DAEMON_RUNNING_DEBUG_OFF_MSG))
	{
		qWarning() << "EPSrvCtl debug log OFF";
		AppRuntimeData::getInstance().setAppParam(FSQtDebugLevel, QtWarningMsg);
	}
	else
	{
		qCritical() << "EPSrvCtl running receive unknown MSG" << message;
	}
}

/** @brief Инициализация параметров командной строки*/
void EPSrvCtl::localInitCmdParams()
{
	cmdOptions.addSection(trUtf8("Основные"));
	cmdOptions.add(CMD_PARAM_DEBUG,
				   trUtf8("Показывать полную отладочную информацию"));
	cmdOptions.add(CMD_PARAM_HELP, trUtf8("Показать помощь и выйти"));
	cmdOptions.alias(CMD_PARAM_HELP, "h");
	cmdOptions.add(CMD_PARAM_SKIPSELFTEST,
				   trUtf8("Пропустить самотестирование"));
	cmdOptions.add(CMD_PARAM_CONFIG,
				   trUtf8("Файл конфигурации"),
				   QxtCommandOptions::ValueRequired);
}

/** @brief Первичная инициализация и проверка
	@return результат инициализации и проверки*/
bool
EPSrvCtl::localInitAndCheck()
{
	qDebug() << Q_FUNC_INFO << "...";
	qDebug() << Q_FUNC_INFO << "OK";
	return true;
}

/** Потеря БД
 */
void
EPSrvCtl::lostDbConn()
{
	qCritical() << Q_FUNC_INFO << "SHUTDOWN Server";
	qApp->exit(5);
}

/** @brief Запуск самотестирвоания */
void
EPSrvCtl::localStartSelfTest()
{
	startDaemon();
}

/** @brief Запуск обратчика */
void
EPSrvCtl::localStartDaemon()
{
	qDebug() << "EPSrvCtl::localStartDaemon()";

	// ==== http server
	RestService* rest = RestService::getInstance();
	rest->setListenPort(EPortCfg.getRestPort());
	rest->setServiceName("EXCEL_PORT");
	rest->registerHandler(new Excel2XmlHandler(), "xls", "test");
	QTimer::singleShot(500, rest, SLOT(start()));
}

/** @brief Первичная проверка параметров командной строки
	@return результат проверки*/
bool
EPSrvCtl::localCheckCmdParams()
{
	extern QString EPortSrvConfigFile;
	if (getStartCmdParam(CMD_PARAM_CONFIG).isValid())
	{
		EPortSrvConfigFile = getStartCmdParam(CMD_PARAM_CONFIG).toString();
	}

	if (getStartCmdParam(CMD_PARAM_DEBUG).toBool())
	{
		AppRuntimeData::getInstance().setAppParam(FSQtDebugLevel, QtDebugMsg);
	}

	return true;
}

