#ifndef REQUESTSMANAGER_H
#define REQUESTSMANAGER_H

#include <QNetworkReply>

class RequestsManager : public QObject
{
	Q_OBJECT

private:
	RequestsManager();

public:

	static RequestsManager *instance();
	void getLastUpdate();
};

#endif // REQUESTSMANAGER_H
