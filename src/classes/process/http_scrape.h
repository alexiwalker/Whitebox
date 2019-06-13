#ifndef HTTP_SCRAPE_H
#define HTTP_SCRAPE_H

#include <QString>
#include <vector>


class http_scrape
	{
public:
	http_scrape(int _mode);

private:
	static int IMAGE;
	static int MAGNET;
	static QString magnet_url;
	static QString image_url; //todo, need to figure out best place to scrape the images from

	int mode;


//magnet interface
public:

	struct magnet_results {
		QString showname;
		int season;
		int episode;
		QString url;
		float size_mb;
		};


	std::vector<magnet_results> missing_episodes();
	void set_show_id(int id);
	int find_show_id(QString showname);

private:


	};

#endif // HTTP_SCRAPE_H
