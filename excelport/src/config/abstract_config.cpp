/* Copyright (c) 2012 Forward Systems. All rights reserved */
#include <QtCore>
#include <QtXml>

#ifdef Q_OS_WIN
#include "windows.h"
#endif

#include "config/abstract_config.h"

/** Считывает настройки из xml-файла.
  * @param device представляет файл настроек.
  * @param map вмещает считываемый словарь значений.
  * @return Возвращает истину, если файл имеет формат xml. */
bool
readXmlFile(QIODevice& device, QSettings::SettingsMap& map)
{
	QXmlStreamReader xmlReader(&device);

	bool isLeaf = 0;
	QStringList path;
	while (!xmlReader.atEnd())
	{
		xmlReader.readNext();

		//Построить "путь" до элемента
		while (xmlReader.isStartElement())
		{
			isLeaf = 1;
			QString node = xmlReader.name().toString();
			if (node != "settings")
			{
				path << node;
			}
			xmlReader.readNext();
		}
		while (xmlReader.isEndElement())
		{
			if (!path.isEmpty())
			{
				if (isLeaf)
				{
					// Добавить пустое значение
					QString key = path.join("/");
					map[key] = "";
					qDebug() << "key=" << key << "value=\"\"";
					isLeaf = 0;
				}
				path.removeLast();
			}
			xmlReader.readNext();
		}

		//Записать элемент
		if (xmlReader.isCharacters() && !xmlReader.isWhitespace())
		{
			QString key = path.join("/");
			QString value = xmlReader.text().toString();
			map[key] = value;
			qDebug() << "key=" << key << "value=" << value;
			isLeaf = 0;
		}
	}

	bool res = xmlReader.hasError();
	if (res)
	{
		qCritical() << Q_FUNC_INFO << "ERROR" << xmlReader.errorString();
	}

	return !res;
}

/** Записывает настройки в файл.
  * @param device предоставляет файл для записи.
  * @param map вмещает словарь записываемых значений.
  * @return Возвращает истину. */
bool
writeXmlFile(QIODevice& device, const QSettings::SettingsMap& map)
{

	QDomDocument doc("");
	QDomElement root = doc.createElement("settings");
	doc.appendChild(root);

	QMapIterator<QString, QVariant> i(map);
	while (i.hasNext())
	{
		i.next();

		//Создать родительские узлы
		QStringList path = i.key().split('/');

		QDomElement domElement = root;
		Q_FOREACH (QString prefix, path)
		{
			QDomNode groupNode = domElement.namedItem(prefix);
			if (groupNode.isNull())
			{
				QDomElement groupElement = doc.createElement(prefix);
				domElement.appendChild(groupElement);
				domElement = groupElement;
			}
			else
			{
				domElement = groupNode.toElement();
			}
		}

		//Добавить элемент
		domElement.appendChild(doc.createTextNode(i.value().toString()));
	}

	QDomNode xmlNode = doc.createProcessingInstruction(
				"xml", "version=\"1.0\" encoding=\"UTF-8\"");
	doc.insertBefore(xmlNode, doc.firstChild());

	QTextStream out(&device);
	doc.save(out, 4);

	return true;
}

/** Преобразует строковую версию в числовой аналог.
  * @param version Строка с версией формата d[d].d[d].d[d]
  * @return Возвращает вычисленное значение версии. */
int
getNumericVersion(const QString &version)
{
	int verMaj = version.section('.', 0, 0).toInt();
	int verMin = version.section('.', 1, 1).toInt();
	int verPat = version.section('.', 2, 2).toInt();
	int verNum = verPat + 100 * verMin + 100000 * verMaj;
	return verNum;
}

const QSettings::Format FS_SETTINGS_XML_FORMAT =
		QSettings::registerFormat("xml", readXmlFile, writeXmlFile);

/** Загружает настройки конфигурации.
  * @param fileName Содержит имя файла конфигурации.
  * @param autosave Определяет возможность автоматического
  * сохранения изменений в настройках. */
AbstractConfig::AbstractConfig(const QString &fileName, bool autosave)
	: settings(fileName, FS_SETTINGS_XML_FORMAT)
{
	isCorrectable = autosave;
	isValid = 1;
}

/** Служит для корректного удаления потомков. */
AbstractConfig::~AbstractConfig()
{
}

/** @return Возвращает значение переменной конфигурации или значение
  * по умолчанию, переданное в качестве аргумента.
  * @param grp представляет группу, в которую помещена переменная.
  * @param fld вмещает имя переменной.
  * @param def определяет значение по умолчанию в случае, если переменная
  * еще не была проинициализированна. */
QVariant
AbstractConfig::getValue(const QString &grp, const QString &fld,
						 const QVariant &def) const
{
	static QMutex mtx;
	QMutexLocker locker(&mtx);

	AbstractConfig *cfg = const_cast<AbstractConfig*>(this);
	cfg->settings.beginGroup(grp);

	if (!cfg->settings.contains(fld))
	{
		if (isCorrectable)
		{
			cfg->settings.setValue(fld, def);
		}
		else
		{
			isValid = 0;
		}
		qCritical("No config entry %s/%s found",
				  grp.toLocal8Bit().data(),
				  fld.toLocal8Bit().data());
		qCritical("Config: %s", cfg->settings.fileName().toLocal8Bit().data());

	}
	if (syncBeforeRead())
	{
		cfg->settings.sync();
	}
	QVariant val = cfg->settings.value(fld, def);
	cfg->settings.endGroup();
	return val;
}

/** Устанавливает значение переменной конфигурации
 @param field Поле (например, "sdb/name")
 @param value Значение
*/
void
AbstractConfig::setValue(const QString &field, const QVariant &value)
{
	settings.setValue(field, value);
}

/** Выполняет синхронизацию файла настроек */
void
AbstractConfig::sync()
{
	qCritical() << "Unauthorized config sync detected";
	//settings.sync();
}

/** @return Возвращает истину, если формат файла, используемого для
  * чтения настроек, соответствует формату конфига.
*/
bool
AbstractConfig::isConsistent() const
{
	AbstractConfig *editable = const_cast<AbstractConfig *>(this);
	if (!editable)
	{
		return 0;
	}
	const QMetaObject *mo = editable->metaObject();
	if (!mo)
	{
		return 0;
	}

	editable->isValid = 1;
	bool wasCorrectable = isCorrectable;
	editable->isCorrectable = 0;
	int n = mo->methodCount();
	for (int i = 0; i < n; ++i)
	{
		QMetaMethod mm = mo->method(i);
		if (QString(mm.signature()).startsWith("get"))
		{
			mm.invoke(editable, Qt::DirectConnection);
		}

	}
	editable->isCorrectable = wasCorrectable;
	return editable->isValid;
}

/** @return Возвращает имя файла с версией приложения. */
QString
AbstractConfig::getVersionFileName()
{
	QString appName = getAppName();
	QString userName = "default";

#ifdef Q_OS_WIN
	DWORD cchBuff = 256;
	WCHAR tchBuffer[512 + 1];
	GetUserNameW(tchBuffer, &cchBuff);
	userName = QString::fromWCharArray(tchBuffer);
#else
	userName = getenv("USER");
#endif

	QString fileName = appName + "-" + userName;
	return fileName;
}

/**
 * @brief AbstractConfig::syncBeforeRead Возвращает значение флага
 * syncBeforeRead. Если true, то при вызове функции getValue() данные будут
 * считываться из конфигурационного файла с диска.
 * По умолчанию флаг имеет значение true.
 * @return
 */
bool
AbstractConfig::syncBeforeRead() const
{
	QVariant syncBeforeRead = property("syncBeforeRead");
	if (syncBeforeRead.isValid())
	{
		return syncBeforeRead.toBool();
	}
	return true;
}

/**
 * @brief AbstractConfig::setSyncBeforeRead
 * См функцию AbstractConfig::syncBeforeRead().
 * @param syncOn
 */
void
AbstractConfig::setSyncBeforeRead(bool syncOn)
{
	setProperty("syncBeforeRead",syncOn);
}
