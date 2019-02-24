#include <QtDebug>
#include <QDir>
#include <QThread>
#include <QCoreApplication>
#include <QTextCodec>

#include <boost/program_options.hpp>
#include <excelport_srv.h>

static int debugLevel = 1;

namespace po = boost::program_options;

struct CmdParam
{
	int port = 12100;
	int debugLevel = 2;
	int threadCount = 10;
};

po::options_description createCmdResciption()
{
	po::options_description desc("General options");
	desc.add_options()
			("port,p", po::value<int>(), "Select tcp port");
	desc.add_options()
			("thread,t", po::value<int>(), "Max thread count");
	desc.add_options()
			("debug,d", po::value<int>(),
				"Debug level (0 - error|critical, 1 - warning, 2- debug)");

	return desc;
}

CmdParam parseCmd(int argc, char *argv[])
{
	CmdParam res;
	po::options_description desc = createCmdResciption();

	po::variables_map vm;
	try
	{
		po::store(po::parse_command_line(argc,argv,desc), vm);
		if (vm.count("port"))
		{
			res.port = vm["port"].as<int>();
			if (res.port < 0)
			{
				qFatal("Http rest only > 0 possible");
			}
		}
		if (vm.count("debug"))
		{
			res.debugLevel = vm["debug"].as<int>();
			if (res.debugLevel < 0 || res.debugLevel > 2)
			{
				qFatal("DEbug level only >= 0 and <= 2");
			}
			debugLevel = res.debugLevel;
		}
		if (vm.count("thread"))
		{
			res.threadCount = vm["thread"].as<int>();
			if (res.threadCount < 0 || res.threadCount > 20)
			{
				qFatal("Thread count > 0 and < 20");
			}
		}
	}
	catch(std::exception& ex)
	{
		qFatal("Error parse input parameters");
	}


	return res;
}

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	QSet<QtMsgType> types;

	if (debugLevel >= 0)
	{
		types.insert(QtCriticalMsg);
		types.insert(QtFatalMsg);
	}

	if (debugLevel >= 1)
	{
		types.insert(QtWarningMsg);
		types.insert(QtInfoMsg);
	}

	if (debugLevel >= 2)
	{
		types.insert(QtDebugMsg);
	}

	if (!types.contains(type))
	{
		return;
	}

	QByteArray localMsg = msg.toLocal8Bit();
	const char *file = context.file ? context.file : "";
	const char *function = context.function ? context.function : "";
	switch (type)
	{
	case QtDebugMsg:
		fprintf(stderr, "Debug [%s]: %s\n",
				QDateTime::currentDateTime().toString(Qt::ISODate).toUtf8().constData(),
				localMsg.constData());
		break;
	case QtInfoMsg:
		fprintf(stderr, "Info [%s]: %s\n",
				QDateTime::currentDateTime().toString(Qt::ISODate).toUtf8().constData(),
				localMsg.constData());
		break;
	case QtWarningMsg:
		fprintf(stderr, "Warning [%s]: %s\n",
				QDateTime::currentDateTime().toString(Qt::ISODate).toUtf8().constData(),
				localMsg.constData());
		break;
	case QtCriticalMsg:
		fprintf(stderr, "Critical [%s]: %s\n",
				QDateTime::currentDateTime().toString(Qt::ISODate).toUtf8().constData(),
				localMsg.constData());
		break;
	case QtFatalMsg:
		fprintf(stderr, "Fatal [%s]: %s (%s:%u, %s)\n",
				QDateTime::currentDateTime().toString(Qt::ISODate).toUtf8().constData(),
				localMsg.constData(), file, context.line, function);
		break;
	}
}

/** @brief Демон phash-srv
 @param argc - кол-во параметров
 @param argv - строка параметров
*/
int
main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	qInstallMessageHandler(myMessageOutput);

	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));

	CmdParam cmdParam = parseCmd(argc, argv);

	app.thread()->setObjectName("main");
	QDir::setCurrent(qApp->applicationDirPath());

	ExcelportSrv p(cmdParam.port, cmdParam.threadCount);
	p.start();

	return app.exec();
}


