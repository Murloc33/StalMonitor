#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QJsonObject>
#include <QSqlQuery>
#include <QSettings>
#include <QString>
#include <QObject>

#include <maincatalogmodel.h>

class DBManager : public QObject
{
	Q_OBJECT

public:
	struct itemInfo
	{
		QString id;
		QString name;
		QString category_name;
		QString subcategory_name;
		QString icon;
		QString description;
	};

private:
	struct categories{
		QMap<QString, int> category;
		QMap<int, QString> rcategory;
		QMap<QString, int> subcategory;
		QMap<int, QString> rsubcategory;
	};

public slots:
	void slotLustUpdateRequestCompleted();

public:
	static DBManager* instance();
	void updateDB();
	void fillItemsModel(QVector<MainCatalogModel::item> &items);

private:
	DBManager();
	bool checkIntegrity();
	void checkLastUpdateRep();
	void createTables();
	void fillCategory();
	void fillSubCategory();
	void fillItems(QString path);
	QJsonObject readJsonFile(const QString &path);
	itemInfo parsJsonObject(const QJsonObject &object);
	void parsCategory(itemInfo &item, const QJsonObject &object);
	void getCategory();
	void getSubCategory();
	~DBManager();

private:
	QSqlDatabase m_db;
	QString m_path;
	QSettings* m_settings;
	categories m_categories;
	QString m_curLastUpdate;
	QSqlQuery *m_query;
};

#endif // DBMANAGER_H
