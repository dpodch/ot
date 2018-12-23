#pragma once

#include <QString>
#include <QStringList>

#include <config/abstract_config.h>

/** @class EPortSrvConfig
	@brief Настройки
*/
class EPortSrvConfig : public AbstractConfig
{
	Q_OBJECT

public:
	static EPortSrvConfig &getInstance();

public Q_SLOTS:

	int getRestPort() const;

private:
	EPortSrvConfig(const QString &fileName);

	static EPortSrvConfig *instance; ///< Синглтон
};

#define EPortCfg EPortSrvConfig::getInstance()
