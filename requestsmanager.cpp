#include "requestsmanager.h"

#include <QNetworkAccessManager>

#include "dbmanager.h"

#define LAST_UPDATE_URL "https://api.github.com/repos/EXBO-Studio/stalcraft-database"

RequestsManager::RequestsManager()
{


}

RequestsManager *RequestsManager::instance()
{
	static RequestsManager manager;
	return &manager;
}

void RequestsManager::getLastUpdate()
{
	QNetworkRequest request;
	request.setUrl(QUrl(LAST_UPDATE_URL));


	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	QNetworkReply* reply = manager->get(request);
	connect(reply, &QNetworkReply::finished,
			DBManager::instance(), &DBManager::slotLustUpdateRequestCompleted);
}
