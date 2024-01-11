#ifndef WEATHER_H
#define WEATHER_H

#include "token.h"
#include "QtCore/qstring.h"
#include "QtCore/qurl.h"

class Weather
{
public:
	Weather();

	Weather(QString unit);

	Weather(QString unit, QString lang);

	void changeLanguage(QString lang);

	void changeUnit(QString unit);

	QString getTempUnit();

	int getFromCity(QString city);

	int getFromGeo(double lat, double lon);

	void city2geo(QString city);

	QString geo2city(double lat, double lon);

private:
	QString _api = "api.openweathermap.org";

	QString token = API_TOKEN;
	
	int sendAndDecode(QUrl url);

	QByteArray get(QUrl url);

	QString _unitSystem, _language;

public:
	struct {
		struct {
			double lat;
			double lon;
		}coord;

		struct {
			int id;
			QString main;
			QString description;
			QString icon;
		}weather;

		QString base;

		double temp;
		double feels_like;
		double temp_min;
		double temp_max;
		int pressure;
		int humidity;
		int sea_level;
		int grnd_level;

		int visibility;

		struct {
			double speed;
			int deg;
			double gust;
		}wind;

		struct {
			int all;
		}clouds;

		struct {
			double oneHour;
			double threeHours;
		}rain;

		struct {
			double oneHour;
			double threeHours;
		}snow;

		double dt;

		struct {
			int type;
			int id;
			QString message;
			QString country;
			int sunrise;
			int sunset;
		}sys;

		int timezone;
		int id;
		QString name;
		}data;

};


#endif // WEATHER_H