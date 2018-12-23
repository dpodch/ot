#pragma once

#include <daemon/fs_daemon.h>

const QString CMD_PARAM_DEBUG = "debug";
const QString CMD_PARAM_SKIPSELFTEST = "skipselftest";
const QString CMD_PARAM_HELP = "help";
const QString CMD_PARAM_CONFIG = "config";

#define DAEMON_RUNNING_DEBUG_ON_MSG		"debug"
#define DAEMON_RUNNING_DEBUG_OFF_MSG	"nodebug"

/** @class EPSrvCtl
	@brief
*/
class EPSrvCtl: public FsDaemon
{
	Q_OBJECT
	static EPSrvCtl *instance;

public slots:

public:
	static EPSrvCtl* initDaemon();
	static EPSrvCtl* getInstance();
	virtual ~EPSrvCtl();

private:
	EPSrvCtl();

	void localInitCmdParams();
	bool localCheckCmdParams();
	bool localInitAndCheck();
	void localStartSelfTest();
	void localStartDaemon();

private Q_SLOTS:
	void lostDbConn();
	void runDaemonMesReceived(QString message);

private:
};
