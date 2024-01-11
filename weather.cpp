#include "weather.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>


#include "QtCore/qdebug.h"
#include "QtCore/qjsonobject.h"
#include "QtCore/qjsondocument.h"
#include "QtCore/qjsonarray.h"

#include "tracer_common.h"
#include "opentelemetry/trace/semantic_conventions.h"

namespace beast = boost::beast;
namespace http = beast::http;
using tcp = boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;
using namespace opentelemetry::trace;
namespace http_client = opentelemetry::ext::http::client;


Weather::Weather(){}
Weather::Weather(QString unit) : _unitSystem(unit) {}
Weather::Weather(QString unit, QString lang) : _unitSystem(unit), _language(lang) {}

QString Weather::getTempUnit() {
    if (_unitSystem == "metric") {
        return "°C";
    }
    else if (_unitSystem == "imperial") {
        return "°F";
    }
    return "K";
}

int Weather::getFromCity(QString city) {
    //city2geo(city);
    QUrl url = QString("https://" + _api + "/data/2.5/weather"
        + "?q=" + city
        + "&units=" + _unitSystem
        + "&lang=" + _language
        + "&appid=" + token);
    qDebug() << "[URL] " << url;
    return sendAndDecode(url);
    return 0;
}


int Weather::getFromGeo(double lat, double lon) {
        QUrl url = QString("https://" + _api + "/data/2.5/weather"
            + "?lat=" + QString::number(lat)
            + "&lon=" + QString::number(lon)
            + "&units=" + _unitSystem
            + "&lang=" + _language
            + "&appid=" + token);
        qDebug() << "[URL] " << url;
        return sendAndDecode(url);
}


int Weather::sendAndDecode(QUrl url) {
    QByteArray response = get(url);
    QJsonObject jroot = QJsonDocument::fromJson(get(url)).object();

    StartSpanOptions options;
    options.kind = SpanKind::kClient;

    

    if (jroot.isEmpty())
        return 0;

    //qDebug() << "[JSON]" << jroot;

    int cod = jroot["cod"].toInt();
    switch (cod) {
    case 200: break;
    case 400: return -1;
    default: return -1;
    }

    QJsonObject jcoord = jroot["coord"].toObject();
    data.coord.lat = jcoord["lat"].toDouble();
    data.coord.lon = jcoord["lon"].toDouble();

    QJsonObject jweather = jroot["weather"].toArray().at(0).toObject();
    data.weather.id = jweather["id"].toInt();
    data.weather.description = jweather["description"].toString();
    data.weather.icon = jweather["icon"].toString();

    QJsonObject jmain = jroot["main"].toObject();
    data.temp = jmain["temp"].toDouble();
    data.feels_like = jmain["feels_like"].toDouble();
    data.temp_min = jmain["temp_min"].toDouble();
    data.temp_max = jmain["temp_max"].toDouble();
    data.pressure = jmain["pressure"].toInt();
    data.humidity = jmain["humidity"].toInt();
    data.sea_level = jmain["sea_level"].toInt();
    data.grnd_level = jmain["grnd_level"].toInt();

    QJsonObject jwind = jroot["wind"].toObject();
    data.wind.speed = jwind["speed"].toDouble();
    data.wind.deg = jwind["deg"].toDouble();
    data.wind.gust = jwind["gust"].toDouble();

    QJsonObject jclouds = jroot["clouds"].toObject();
    data.clouds.all = jclouds["all"].toInt();

    QJsonObject jrain = jroot["rain"].toObject();
    data.rain.oneHour = jrain["1h"].toDouble();
    data.rain.threeHours = jrain["3h"].toDouble();

    QJsonObject jsnow = jroot["snow"].toObject();
    data.snow.oneHour = jrain["1h"].toDouble();
    data.snow.threeHours = jrain["3h"].toDouble();

    QJsonObject jsys = jroot["sys"].toObject();
    data.sys.type = jsys["type"].toInt();
    data.sys.id = jsys["id"].toInt();
    data.sys.message = jsys["message"].toString();
    data.sys.country = jsys["country"].toString();
    data.sys.sunrise = jsys["sunrise"].toInt();
    data.sys.sunset = jsys["sunset"].toInt();


    data.base = jroot["base"].toString();
    data.visibility = jroot["visibility"].toInt();
    data.dt = jroot["dt"].toDouble();
    data.timezone = jroot["timezone"].toInt();
    data.id = jroot["id"].toInt();
    data.name = jroot["name"].toString();

    return 1;
}

QByteArray Weather::get(QUrl url) {

    boost::asio::io_context io_context;
    tcp::resolver resolver(io_context);

    ssl::context ctx{ ssl::context::sslv23_client };

    std::string url_string = QString(url.toString()).toStdString();
    //start active span
     opentelemetry::trace::StartSpanOptions options;
     options.kind = SpanKind::kClient; //client
     opentelemetry::ext::http::common::UrlParser url_parser(url_string);
     auto span = get_tracer("weather-client")->StartSpan("getweatherdata",
     			{ {SemanticConventions::kUrlFull, url_parser.url_},
     			{SemanticConventions::kUrlScheme, url_parser.scheme_},
     			{SemanticConventions::kHttpRequestMethod, "GET"},
                {SemanticConventions::kNetHostName, url_parser.host_ } }, options);
     auto scope = get_tracer("weather-client")->WithActiveSpan(span);

     


    //make a request to openweather api
    boost::asio::ssl::stream<tcp::socket> stream{ io_context, ctx };
    auto const results = resolver.resolve(_api.toStdString(), "https");
    //establish a connection to the server
    boost::asio::connect(stream.next_layer(), results.begin(), results.end());

    // Perform the SSL handshake
    stream.handshake(ssl::stream_base::client);

    
    //create an HTTP REQUEST
    http::request<http::string_body> request(http::verb::get, url_string, 11);
    request.set(http::field::host, _api.toStdString());
    request.set(http::field::user_agent, "Opentelemetry cpp ");
    request.prepare_payload();
    http::write(stream, request);

    //recieve the response
    beast::flat_buffer buffer;
    beast::http::response<beast::http::dynamic_body> response;
    http::read(stream, buffer, response);
    
    // inject current context into http header

    auto current_ctx = opentelemetry::context::RuntimeContext::GetCurrent();
    FunctionTextMapCarrier<beast::http::response<beast::http::dynamic_body>> carrier;
    auto prop = opentelemetry::context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();
    prop->Inject(carrier, current_ctx);

    std::string response_body = beast::buffers_to_string(response.body().data());
    
    auto status_code = response.result_int();
    span->SetAttribute(SemanticConventions::kHttpResponseStatusCode, status_code);
    for (auto const& fields : response) {
        std::stringstream name;
        name << fields.name();
     	std::stringstream value;
     	value << fields.value();
     	std::stringstream data;
     	span->SetAttribute("http.headers." + name.str(), value.str());
     	span->SetAttribute("http.response", response_body);
        //span->SetAttribute("http.status_code", status_code);
     	}
     	span->End();

     if (status_code >= 400)
     {
     	span->SetStatus(StatusCode::kError,
     	"response status:" + beast::buffers_to_string(response.body().data()));
     }

   
    QByteArray read = QByteArray::fromStdString(response_body);
    return read;
}

void Weather::city2geo(QString city) {
    QUrl url = QString("https://" + _api + "/geo/1.0/direct?q=" + city + "&appid=" + token);
    qDebug() << "[URL] " << url;
    QJsonObject jroot = QJsonDocument::fromJson(get(url)).object();
    qDebug() << jroot;
}

QString Weather::geo2city(double lat, double lon) {
    return "";
}




