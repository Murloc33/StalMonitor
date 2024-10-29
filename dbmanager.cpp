#include "dbmanager.h"

#include <QNetworkAccessManager>
#include <QStringBuilder>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSqlQuery>
#include <QDebug>
#include <QFile>
#include <QDir>

#include "requestsmanager.h"
#include "settingsmanager.h"

DBManager::DBManager()
{
	m_path = "./Items.sqlite";
	m_db = QSqlDatabase::addDatabase("QSQLITE");
	m_db.setDatabaseName(m_path);
	m_db.open();
	m_query = new QSqlQuery();

}

bool DBManager::checkIntegrity()
{
	return m_db.tables().size() == 4;
}

void DBManager::checkLastUpdateRep()
{
	RequestsManager::instance()->getLastUpdate();
}

void DBManager::createTables()
{
	if (!m_query->exec("CREATE TABLE category"
			   "("
			   "	id INTEGER PRIMARY KEY AUTOINCREMENT,"
			   "	name TEXT"
			   ");")) {
		qDebug() << "Не удалось создать TABLE category";
	}
	m_query->clear();

	if (!m_query->exec("CREATE TABLE subcategory"
			   "("
			   "	id INTEGER PRIMARY KEY AUTOINCREMENT,"
			   "	name TEXT"
			   ");")) {
		qDebug() << "Не удалось создать TABLE subcategory";
	}
	m_query->clear();

	if (!m_query->exec("CREATE TABLE item"
			   "("
			   "	id TEXT PRIMARY KEY,"
			   "	name TEXT,"
			   "	category_id INTEGER,"
			   "	subcategory_id INTEGER,"
			   "	description TEXT,"
			   "	FOREIGN KEY (category_id) REFERENCES catergory (id),"
			   "	FOREIGN KEY (subcategory_id) REFERENCES subcatergory (id)"
			   ");")) {
		qDebug() << "Не удалось создать TABLE item";
	}
	m_query->clear();
}

void DBManager::fillCategory()
{

	m_query->exec("INSERT INTO category (name) VALUES"
			   "('armor'),"
			   "('artefact'),"
			   "('attachment'),"
			   "('bullet'),"
			   "('containers'),"
			   "('drink'),"
			   "('food'),"
			   "('grenade'),"
			   "('medicine'),"
			   "('misc'),"
			   "('other'),"
			   "('weapon');");
	m_query->clear();
}

void DBManager::fillSubCategory()
{
	m_query->exec("INSERT INTO subcategory (name) VALUES"
			   "('clothes'),"
			   "('combat'),"
			   "('combined'),"
			   "('device'),"
			   "('scientist'),"
			   "('biochemical'),"
			   "('electrophysical'),"
			   "('gravity'),"
			   "('other_arts'),"
			   "('thermal'),"
			   "('barrel'),"
			   "('collimator_sights'),"
			   "('forend'),"
			   "('mag'),"
			   "('other'),"
			   "('pistol_handle'),"
			   "('device'),"
			   "('skins'),"
			   "('assault_rifle'),"
			   "('device'),"
			   "('heavy'),"
			   "('machine_gun'),"
			   "('melee'),"
			   "('pistol'),"
			   "('shotgun_rifle'),"
			   "('sniper_rifle'),"
			   "('submachine_gun');");
	m_query->clear();
}

void DBManager::fillItems(QString path)
{
	QDir directory(path);
	directory.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
	QFileInfoList fileInfoList = directory.entryInfoList();
	for (QFileInfo fileInfo : fileInfoList) {
		if (fileInfo.isDir()) {
			if (fileInfo.baseName() == "_variants") {
				continue;
			}
			fillItems(fileInfo.absoluteFilePath());
		} else {
			QJsonObject jsonObject = readJsonFile(fileInfo.absoluteFilePath());
			itemInfo item = parsJsonObject(jsonObject);
			m_query->addBindValue(item.id);
			m_query->addBindValue(item.name);
			m_query->addBindValue( m_categories.category[item.category_name]);
			m_query->addBindValue(m_categories.subcategory[item.subcategory_name]);
			m_query->addBindValue(item.description);
			if (!m_query->exec()) {
				qDebug() << "Не удалось отправить запрос на заполнение таблицы item";
			}
		}
	}
	m_query->clear();
}

QJsonObject DBManager::readJsonFile(const QString &path)
{
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			qWarning() << "Не удалось открыть файл:" << file.errorString();
	}

	QByteArray jsonData = file.readAll();
	file.close();

	QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData);
	return jsonDocument.object();
}

DBManager::itemInfo DBManager::parsJsonObject(const QJsonObject &object)
{
	itemInfo result;
	result.id = object.value("id").toString();
	result.name = object.value("name").toObject().value("lines").toObject().value("ru").toString();
	parsCategory(result, object);
	result.description = object.value("infoBlocks").toArray()[7].toObject().value("text").toObject()
			.value("lines").toObject().value("ru").toString();

	return result;
}

void DBManager::parsCategory(itemInfo &item, const QJsonObject &object)
{
	auto categories = object.value("category").toString().split('/');
	item.subcategory_name = "";
	if (categories.size() == 2) {
		item.subcategory_name = categories[1];
	}

	item.category_name = categories[0];
}

void DBManager::getCategory()
{
	m_query->exec("SELECT * FROM category");
	while (m_query->next()) {
		m_categories.category.insert(m_query->value("name").toString(),
						  m_query->value("id").toInt());
		m_categories.rcategory.insert(m_query->value("id").toInt(),
									 m_query->value("name").toString());
	}

	m_query->clear();
}

void DBManager::getSubCategory()
{
	QSqlQuery query;
	query.exec("SELECT * FROM subcategory");
	while (query.next()) {
		m_categories.subcategory.insert(query.value("name").toString(),
						  query.value("id").toInt());
		m_categories.rsubcategory.insert(query.value("id").toInt(),
						  query.value("name").toString());
	}

	m_query->clear();

	m_categories.category.insert("", -1);
	m_categories.rcategory.insert(-1, "");
}

DBManager::~DBManager()
{
	m_db.close();
}

void DBManager::slotLustUpdateRequestCompleted()
{
	QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());
	if (reply->error() == QNetworkReply::NoError) {
		QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
		qDebug() << (document.object().value("updated_at").toString() == SettingsManager::instance()->getLastUpdateRep());

	}
}

DBManager* DBManager::instance()
{
	static DBManager manager;
	return &manager;
}

void DBManager::updateDB()
{
	if (!checkIntegrity()) {
		createTables();
		fillCategory();
		fillSubCategory();
		getCategory();
		getSubCategory();
		m_db.transaction();
		m_query->prepare("INSERT INTO item (id, name, category_id, subcategory_id, description) VALUES"
					  " (?, ?, ?, ?, ?)");

		fillItems("./items");
		m_db.commit();

	}

	QSqlQuery query;
	query.exec("SELECT * FROM item LIMIT 10");
	while (query.next()) {
		qDebug() << query.value("name");
		qDebug() << query.value("description");
	}
}

void DBManager::fillItemsModel(QVector<MainCatalogModel::item> &items)
{
	m_query->prepare("SELECT * FROM item");
	m_query->exec();

	while (m_query->next()) {
		MainCatalogModel::item currentItem;
		currentItem.id = m_query->value("id").toString();
		currentItem.name = m_query->value("name").toString();
		currentItem.description = m_query->value("description").toString();
		currentItem.category_id = m_query->value("category_id").toInt();
		currentItem.icon = "./icons/" + m_categories.rcategory[m_query->value("id").toInt()];
		if (m_query->value("subcategory_id").toInt() != -1) {
			currentItem.icon += ("/" + m_categories.rsubcategory[m_query->value("subcategory_id").toInt()]);
		}

		items.push_back(currentItem);

	}
}
