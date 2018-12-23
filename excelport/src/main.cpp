#include <qtsingleapplication/qtsinglecoreapplication.h>
#include <QDir>
#include <QThread>
#include <QMetaType>
#include <QDebug>
#include <QTimer>

#include <core/app_runtime_data.h>

#include <config/excelport_srv_config.h>
#include <excelport_srv_ctl.h>

#include "cmake_config.h"

typedef QPair<QByteArray, QByteArray> HeaderPair;

/** @brief Инициализация libfsqt
*/
void
init_fsqt()
{
	APP_RUN_SET_PARAM(FSQtAppName, "excelport_srv");
	APP_RUN_SET_PARAM(FSQtAppVersionMajor, VERSION_MAJOR);
	APP_RUN_SET_PARAM(FSQtAppVersionMinor, VERSION_MINOR);
	APP_RUN_SET_PARAM(FSQtAppVersionPatch, VERSION_PATCH);
	APP_RUN_SET_PARAM(FSQtAppVersionBuild, VERSION_BUILD);
	APP_RUN_SET_PARAM(FSQtDebugPrintThreadId, true);

#ifdef QT_DEBUG
	APP_RUN_SET_PARAM(FSQtDebugLevel, QtDebugMsg);
#else
	APP_RUN_SET_PARAM(FSQtDebugLevel, QtWarningMsg);
#endif
}

/** @brief Демон phash-srv
 @param argc - кол-во параметров
 @param argv - строка параметров
*/
int
main(int argc, char *argv[])
{
	QtSingleCoreApplication app(argc, argv);
	qApp->thread()->setObjectName("main");

	QDir::setCurrent(qApp->applicationDirPath());
	init_fsqt();

	daemon_main(argc, argv, NULL, "isRun", "--help");
#ifndef QT_DEBUG
	daemonInitCrashHandler();
#endif
	EPSrvCtl *daemon = EPSrvCtl::initDaemon();

	if (! daemon->parseCmdParams())
	{
		qCritical() << "EPSrvCtl cmd parse ERROR";
		exit(1);
	}

	if (daemon->getStartCmdParam(CMD_PARAM_HELP).toBool())
	{
		exit(0);
	}

	// грузим и проверяем конфиг
	if (!EPortCfg.isConsistent())
	{
		printf("Bad config file. Exit....\n");

		exit(2);
	}

	qWarning() << "Starting " << APP_RUN_PARAM_STRING(FSQtAppName)
			   << "version: "
			   << AppRuntimeData::getInstance().getAppFullVersStr()
			   << " libfsqt version: " << APP_RUN_PARAM_STRING(FSQtVersion);

	QTimer::singleShot(0, EPSrvCtl::getInstance(),
					   SLOT(checkAndStartDaemon()));

	return app.exec();
}


