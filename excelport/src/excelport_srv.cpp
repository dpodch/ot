
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QVariantMap>
#include <QJsonDocument>
#include <xlnt/xlntparser.h>
#include <core/group.h>

#include "excelport_srv.h"

static const int HTTP_OK = 200;
static const int HTTP_BAD = 400;

ExcelportSrv::ExcelportSrv(int port, int threadCount)
{
	this->port = port;
	this->maxThreadCount = threadCount;

	QThread *th = new QThread;
	th->start();

	connect(this, SIGNAL(destroyed(QObject*)), th, SLOT(quit()));
	connect(th, SIGNAL(finished()), th, SLOT(deleteLater()));

	moveToThread(th);
}

ExcelportSrv::~ExcelportSrv()
{
}

void ExcelportSrv::start()
{
	QTimer::singleShot(0, this, SLOT(run()));
}

void ExcelportSrv::run()
{
	qInfo() << "running http server: "
			 << "\nport: " << port
			 << "\nmax threads: " << maxThreadCount;

	served::multiplexer mux;
	mux.handle("/xls")
			.get(std::bind(&ExcelportSrv::getStatQuery,
							 this,
							 std::placeholders::_1,
							 std::placeholders::_2))
			.post(std::bind(&ExcelportSrv::postXls,
							 this,
							 std::placeholders::_1,
							 std::placeholders::_2));

	served::net::server server("0.0.0.0", std::to_string(port), mux);
	server.run(maxThreadCount);
}

void ExcelportSrv::getStatQuery(served::response &resp, const served::request &)
{
	qDebug() << "Request xls stat";
	QString msg("Total count: %1\nError count: %2\nlast query(utc): %3\navg size: %4");
	msg = msg.arg(stat.getQueryCount())
			.arg(stat.getErrorCount())
			.arg(stat.getLastQueryTime().toString(Qt::ISODate))
			.arg(stat.getAvgSize());

	resp.set_header("Content-Type", "text/plain");
	resp.set_header("Content-Length", std::to_string(msg.toUtf8().size()));
	resp.set_body(msg.toStdString());
	resp.set_status(HTTP_OK);
}

void ExcelportSrv::postXls(served::response &resp, const served::request &req)
{
	qDebug() << "Request parse xls";

	QByteArray ba(req.body().c_str(), req.body().size());

	if (ba.size() < 100)
	{
		const QString msg = "post body is empty";
		qWarning() << msg;
		setText(resp, msg.toUtf8(), HTTP_BAD);
		stat.increment(true, ba.size());
		return;
	}
	else if (ba.size() > 100 * 1000 * 1000)
	{
		const QString msg = "file is very large!! Max < 100MB";
		qWarning() << msg;
		setText(resp, msg.toUtf8(), HTTP_BAD);
		stat.increment(true, ba.size());
		return;
	}

	bool isOk = true;
	QString errmsg = "";

	QList<Group *> groups = XlntParser().parse(ba, &isOk, &errmsg);
	QVariantList fList =  Group::toVariantList(groups);
	qDeleteAll(groups);
	groups.clear();

	if (isOk == true)
	{
		QVariantMap map;
		map.insert("version", "1");
		map.insert("firms_list", fList);

		QJsonDocument doc = QJsonDocument::fromVariant(map);
		QByteArray result = doc.toJson();

		setJson(resp, result, HTTP_OK);
		stat.increment(false, result.size());
	}
	else
	{
		const QString msg = "Error parse xls format " + errmsg;
		stat.increment(true, ba.size());
		qWarning() << msg;
		setText(resp, msg.toUtf8(), HTTP_BAD);
	}
}

void ExcelportSrv::setText(served::response &resp,
						   const QByteArray &message,
						   int httpStatus) const
{
	resp.set_status(httpStatus);
	resp.set_header("Content-Type", "text/plain");
	resp.set_header("Content-Length", std::to_string(message.size()));
	resp.set_body(std::string(message.constData(), message.size()));
}

void ExcelportSrv::setJson(served::response &resp,
						   const QByteArray &message,
						   int httpStatus) const
{
	resp.set_status(httpStatus);
	resp.set_header("Content-Type", "text/json");
	resp.set_header("Content-Length", std::to_string(message.size()));
	resp.set_body(std::string(message.data(), message.size()));
}
