#include <iostream>

#include <restinio/all.hpp>

#include <nlohmann/json.hpp>

// for convenience
using json = nlohmann::json;
using namespace std;

template < typename RESP >
RESP
init_resp( RESP resp )
{
	resp.append_header( restinio::http_field::server, "RESTinio sample server /v.0.2" );
	resp.append_header_date_field();

	return resp;
}

using router_t = restinio::router::express_router_t<>;

std::map<std::string, std::string> user_map;
std::map<std::string, std::map<std::string, int>> counter_map;

void gen_random(char *s, const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}

auto create_request_handler()
{
	auto router = std::make_unique< router_t >();

	router->http_get(
		"/create_counter",
		[](auto req, auto) {
			const auto qp = restinio::parse_query( req->header().query() );
        if (qp.has("username") && qp.has("initial_count")) {
					std::string username = std::string(qp["username"]);
					if (user_map.find(username) == user_map.end()) {
						req->create_response().set_body("User not found").done();
					} else {
						std::map<std::string, int> user_counters = counter_map[username];
						char counter_id[7];
						gen_random(counter_id, 6);
						user_counters[std::string(counter_id)] = stoi(string(qp["initial_count"]));
						counter_map[username] = user_counters;
						req->create_response().set_body("Created Counter").done();
						return restinio::request_accepted();
					}
				} else {
					req->create_response().set_body("Undefined username/initial_count").done();
				}
				return restinio::request_rejected();
		}
	);

	router->http_get(
		"/get_user_counters",
		[](auto req, auto) {
			const auto qp = restinio::parse_query( req->header().query() );
			if (qp.has("username")) {
				string username = string(qp["username"]);
				if (counter_map.find(username) == counter_map.end()) {
					req->create_response().set_body("User not found in counter_map").done();
				} else {
					map<string, int> counters = counter_map[username];
					stringstream response;
					response << "{";
					for (auto const& counter : counters) {
						response << "\"" << counter.first << "\" : \"" << counter.second << "\",";
					}
					response << "}";
					req->create_response().append_header( restinio::http_field::content_type, "text/json; charset=utf-8" )
					.set_body(response.str())
					.done();
				}
			} else {
				req->create_response().set_body("username not set").done();
			}
			return restinio::request_rejected();
		}
	);

	router->http_get(
		"/get_count",
		[](auto req, auto) {
			const auto qp = restinio::parse_query( req->header().query() );
			if (qp.has("username") && qp.has("counter_id")) {
				string username = string(qp["username"]);
				string counter_id = string(qp["counter_id"]);
				if (counter_map.find(username) == counter_map.end()) {
					req->create_response().set_body("User not found in counter_map").done();
				} else {
					map<string, int> counters = counter_map[username];
					if (counters.find(counter_id) == counters.end()) {
						req->create_response().set_body("Counter not found").done();
					} else {
						int count = counters[counter_id];
						req->create_response().set_body(to_string(count)).done();
						return restinio::request_accepted();
					}
				}
			} else {
				req->create_response().set_body("username/counter_id not set").done();
			}
			return restinio::request_rejected();
		}
	);

	router->http_get(
		"/increment_count",
		[](auto req, auto) {
			const auto qp = restinio::parse_query( req->header().query() );
			if (qp.has("username") && qp.has("counter_id")) {
				string username = string(qp["username"]);
				string counter_id = string(qp["counter_id"]);
				if (counter_map.find(username) == counter_map.end()) {
					req->create_response().set_body("User not found in counter_map").done();
				} else {
					map<string, int> counters = counter_map[username];
					if (counters.find(counter_id) == counters.end()) {
						req->create_response().set_body("Counter not found").done();
					} else {
						counters[counter_id] += 1;
						counter_map[username] = counters;
						req->create_response().set_body("Incremented").done();
						return restinio::request_accepted();
					}
				}
			} else {
				req->create_response().set_body("username/counter_id not set").done();
			}
			return restinio::request_rejected();
		}
	);

	router->http_get(
		"/decrement_count",
		[](auto req, auto) {
			const auto qp = restinio::parse_query( req->header().query() );
			if (qp.has("username") && qp.has("counter_id")) {
				string username = string(qp["username"]);
				string counter_id = string(qp["counter_id"]);
				if (counter_map.find(username) == counter_map.end()) {
					req->create_response().set_body("User not found in counter_map").done();
				} else {
					map<string, int> counters = counter_map[username];
					if (counters.find(counter_id) == counters.end()) {
						req->create_response().set_body("Counter not found").done();
					} else {
						counters[counter_id] -= 1;
						counter_map[username] = counters;
						req->create_response().set_body("Decremented").done();
						return restinio::request_accepted();
					}
				}
			} else {
				req->create_response().set_body("username/counter_id not set").done();
			}
			return restinio::request_rejected();
		}
	);

	router->http_get(
		"/login_user",
		[](auto req, auto) {
			const auto qp = restinio::parse_query( req->header().query() );
        if (qp.has( "username" ) && qp.has("password")) {
					// user_map["test"];
					if (user_map.find(std::string(qp["username"])) == user_map.end()) {
						// User not found
						req->create_response().set_body("User not found").done();
					} else {
						if (user_map[std::string(qp["username"])] == std::string(qp["password"])) {
							req->create_response().set_body("Valid Authentication").done();
							return restinio::request_accepted();
						} else {
							req->create_response().set_body("Invalid Password").done();
						}
					}
				} else {
					req->create_response().set_body("Undefined username/password").done();
				}
				return restinio::request_rejected();
		}
	);

	router->http_post(
		"/register_user",
		[]( auto req, auto ) {
			std::cout << req->body(); 
			const auto j3 = json::parse(req->body());
			// std::cout << j3.has("username");
			if (j3.contains("username") && js.contains("password")) {
				// user_map["test"];
				std::string username = std::string(j3["username"]);
				if (user_map.find(username) == user_map.end()) {
					// User not found
					user_map[username] = std::string(j3["password"]);
					std::map<std::string, int> counter;
					counter_map[username] = counter;
					req->create_response().set_body("User created").done();
					return restinio::request_accepted();
				} else {
					req->create_response().set_body("Username taken").done();
				}
			} else {
				req->create_response().set_body("Undefined username/password").done();
			}
			return restinio::request_rejected();
		} );

	router->http_get(
		"/",
		[]( auto req, auto ){
        return req->create_response()
					.append_header( restinio::http_field::content_type, "text/html; charset=utf-8" )
					.set_body(restinio::sendfile("../index.html"))
					.done();
		} );

		router->http_get(
		"/app.js",
		[]( auto req, auto ){
        return req->create_response()
					.append_header( restinio::http_field::content_type, "text/html; charset=utf-8" )
					.set_body(restinio::sendfile("../app.js"))
					.done();
		} );

		router->http_get(
		"/app.css",
		[]( auto req, auto ){
        return req->create_response()
					.append_header( restinio::http_field::content_type, "text/css; charset=utf-8" )
					.set_body(restinio::sendfile("../app.css"))
					.done();
		} );

	router->http_get(
		"/json",
		[]( auto req, auto ){
				init_resp( req->create_response() )
					.append_header( restinio::http_field::content_type, "text/json; charset=utf-8" )
					.set_body( R"-({"message" : "Hello world!"})-")
					.done();

				return restinio::request_accepted();
		} );

	router->http_get(
		"/html",
		[]( auto req, auto ){
				init_resp( req->create_response() )
						.append_header( restinio::http_field::content_type, "text/html; charset=utf-8" )
						.set_body(
							"<html>\r\n"
							"  <head><title>Hello from RESTinio!</title></head>\r\n"
							"  <body>\r\n"
							"    <center><h1>Hello world</h1></center>\r\n"
							"  </body>\r\n"
							"</html>\r\n" )
						.done();

				return restinio::request_accepted();
		} );

	return router;
}

int main()
{
	using namespace std::chrono;

	try
	{
		using traits_t =
			restinio::traits_t<
				restinio::asio_timer_manager_t,
				restinio::single_threaded_ostream_logger_t,
				router_t >;

		restinio::run(
			restinio::on_this_thread<traits_t>()
				.port( 8080 )
				.address( "localhost" )
				.request_handler( create_request_handler() ) );
	}
	catch( const std::exception & ex )
	{
		std::cerr << "Error: " << ex.what() << std::endl;
		return 1;
	}

	return 0;
}