#include "tracer_common.h"
#include "mainwindow.h"
#include "QtWidgets/qapplication.h"
#include "QtCore/qplugin.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <opentelemetry/exporters/ostream/span_exporter.h>
#include <opentelemetry/sdk/trace/simple_processor.h>
#include <opentelemetry/trace/provider.h>
#include <nlohmann/json.hpp>
#include "opentelemetry/ext/http/common/url_parser.h"


Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);


using namespace opentelemetry::trace;
namespace http_client = opentelemetry::ext::http::client;
namespace nostd = opentelemetry::nostd;

int main(int argc, char *argv[]) {
	// try {
	// 	InitTracer();
	// 	boost::asio::io_context io_context;
	// 	tcp::resolver resolver(io_context);

	// 	ssl::context ctx{ ssl::context::sslv23_client };

	// 	const std::string url = "https://api.openweathermap.org/data/2.5/weather?q=London&appid=cfd734e0f9c13d81ce6429cc78c995f4";

	// 	//start active span
	// 	opentelemetry::trace::StartSpanOptions options;
	// 	options.kind = SpanKind::kClient; //client
	// 	opentelemetry::ext::http::common::UrlParser url_parser(url);
	// 	std::string span_name = url_parser.path_;
	// 	auto span = get_tracer("weather-client")
	// 		->StartSpan(span_name,
	// 			{ {SemanticConventions::kUrlFull, url_parser.url_},
	// 			{SemanticConventions::kUrlScheme, url_parser.scheme_},
	// 			{SemanticConventions::kHttpRequestMethod, "GET"} }, options);
	// 	auto scope = get_tracer("weather-client")->WithActiveSpan(span);

	// 	// inject current context into http header
	// 	auto current_ctx = opentelemetry::context::RuntimeContext::GetCurrent();
	// 	HttpTextMapCarrier<http_client::Headers> carrier;
	// 	auto prop = opentelemetry::context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();
	// 	prop->Inject(carrier, current_ctx);

	// 	//make a request to openweather api
	// 	boost::asio::ssl::stream<tcp::socket> stream{ io_context, ctx };
	// 	auto const results = resolver.resolve("api.openweathermap.org", "https");
	// 	//establish a connection to the server
	// 	boost::asio::connect(stream.next_layer(), results.begin(), results.end());

	// 	// Perform the SSL handshake
	// 	stream.handshake(ssl::stream_base::client);

	// 	//create an HTTP REQUEST
	// 	http::request<http::string_body> request(http::verb::get, "/data/2.5/weather?q=London&appid=cfd734e0f9c13d81ce6429cc78c995f4", 11);
	// 	request.set(http::field::host, "api.openweathermap.org");
	// 	request.set(http::field::user_agent, "Opentelemetry cpp ");
	// 	request.set("key", "opentelemetry poc");
	// 	request.prepare_payload();

	// 	std::cout << request;
	// 	//send the HTTP request
	// 	http::write(stream, request);

	// 	//recieve the response
	// 	beast::flat_buffer buffer;
	// 	beast::http::response<beast::http::dynamic_body> response;
	// 	http::read(stream, buffer, response);

	// 	auto status_code = response.result_int();
	// 	std::cout << "Response code: " << response.result_int() << std::endl;
	// 	std::string response_body = beast::buffers_to_string(response.body().data());
	// 	nlohmann::json weather_data = nlohmann::json::parse(response_body);
	// 	std::cout << "Response body: " << response_body << std::endl;


	// 	span->SetAttribute(SemanticConventions::kHttpResponseStatusCode, status_code);
	// 	for (auto const& fields : response) {
	// 		std::stringstream name;
	// 		name << fields.name();
	// 		std::stringstream value;
	// 		value << fields.value();
	// 		std::stringstream data;
	// 		span->SetAttribute("http.headers." + name.str(), value.str());
	// 		span->SetAttribute("http.body.", response_body);
	// 	}
	// 	span->End();

	// 	/*if (StatusCode >= 400)
	// 	{
	// 		span->SetStatus(StatusCode::kError,
	// 			"response status:" + beast::buffers_to_string(response.body().data()));
	// 	}*/

	// 	//display the response
	// }
	// catch (std::exception const& e) {
	// 	std::cerr << "Error: " << e.what() << std::endl;
	// 	return 1;
	// }

	// return 0;
	InitTracer();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
	CleanupTracer();
    return a.exec();
}