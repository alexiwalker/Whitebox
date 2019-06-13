#include "http_scrape.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>

int http_scrape::IMAGE = 1;
int http_scrape::MAGNET = 2;


/*
	todo, there are other issues that need to be dealt with first
*/

QString http_scrape::magnet_url = "https://eztv.io/api/get-torrents?imdb_id=";

http_scrape::http_scrape(int _mode)
{
	mode = _mode;

}
