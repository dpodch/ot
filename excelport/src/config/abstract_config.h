/* Copyright (c) 2012 Forward Systems. All rights reserved */
/** @addtogroup Config */
/*@{*/
#ifndef ABSTRACT_CONFIG_H
#define ABSTRACT_CONFIG_H

#include <QSettings>
#include <QObject>
#include <QDate>
#include <QString>

/** @class AbstractConfig
  * @brief Абстрактный класс настроек.
  *
  * Служит для хранения информации о настройках.
  * Новые настройки должны добавляться в результате наследования
  * в виде открытых слотов, начинающихся с глагола get.
  *
  * Способен автоматически осуществлять синхронизацию с файлом,
  * в котором настройки хранятся. Для этого необходимо передать
  * параметр autosave в конструктор. */
class AbstractConfig : public QObject
{
	Q_OBJECT

public:
	static QString getAppName();
	static QString getVersionFileName();

public:
	bool isConsistent() const;

	~AbstractConfig();

	void sync();

	bool syncBeforeRead() const;
	void setSyncBeforeRead(bool syncOn);

protected:
	AbstractConfig(const QString &fileName, bool autosave = 0);

	QVariant getValue(const QString &group, const QString &field,
					  const QVariant &def = QVariant()) const;

	void setValue(const QString &field, const QVariant &value = QVariant());
	QSettings settings; ///< Настройки приложения
	
private:
	
	bool isCorrectable; ///< Исправлять файл настроек, если он некорректен
	mutable bool isValid; ///< Корректность соответствия с файлом
};

int getNumericVersion(const QString &version);

#endif//ABSTRACT_CONFIG_H
/*@}*/

