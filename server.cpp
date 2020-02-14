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
std::map<std::string, std::string> user_keys;
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

	router->http_post(
		"/create_counter",
		[](auto req, auto) {
			const auto j3 = json::parse(req->body());
        if (j3.contains("username") && j3.contains("initial_count") && j3.contains("session_key")) {
					std::string username = std::string(j3["username"]);
					std::string session_key = std::string(j3["session_key"]);
					if (user_map.find(username) == user_map.end()) {
						req->create_response().set_body(R"-({"error" : "can't find user"})-").done();
					} else {
						std::map<std::string, int> user_counters = counter_map[username];
						char counter_id[7];
						gen_random(counter_id, 6);
						user_counters[std::string(counter_id)] = stoi(string(j3["initial_count"]));
						counter_map[username] = user_counters;
						req->create_response().set_body(R"-({"status" : "created counter"})-").done();
						return restinio::request_accepted();
					}
				} else {
					req->create_response().set_body(R"-({"error" : "bad count"})-").done();
				}
				return restinio::request_rejected();
		}
	);

	router->http_post(
		"/remove_counter",
		[](auto req, auto) {
			const auto j3 = json::parse(req->body());
        if (j3.contains("username") && j3.contains("session_key") && j3.contains("counter_id")) {
					string username = string(j3["username"]);
					string counter_id = string(j3["counter_id"]);
					string session_key = string(j3["session_key"]);
					if (user_map.find(username) == user_map.end()) {
						req->create_response().set_body(R"-({"error" : "can't find user"})-").done();
					} else {
						map<string, int> user_counters = counter_map[username];
						if (user_counters.find(counter_id) == user_counters.end()) {
							req->create_response().set_body(R"-({"error" : "can't find counter"})-").done();
						} else {
							user_counters.erase(counter_id);
							counter_map[username] = user_counters;
							req->create_response().set_body(R"-({"status" : "counter removed"})-").done();
							return restinio::request_accepted();
						}
					}
				} else {
					req->create_response().set_body(R"-({"error" : "bad count"})-").done();
				}
				return restinio::request_rejected();
		}
	);

	router->http_post(
		"/get_user_counters",
		[](auto req, auto) {
			const auto j3 = json::parse(req->body());
			if (j3.contains("username")) {
				string username = string(j3["username"]);
				if (counter_map.find(username) == counter_map.end()) {
					req->create_response().set_body(R"-({"error" : "can't find user"})-").done();
				} else {
					map<string, int> counters = counter_map[username];
					stringstream response;
					response << "{";
					for (auto const& counter : counters) {
						response << "\"" << counter.first << "\" : \"" << counter.second << "\",";
					}
					response << "}";
					string out = response.str();
					out.erase(out.size() - 2, 1);
					req->create_response().append_header( restinio::http_field::content_type, "text/json; charset=utf-8" )
					.set_body(out)
					.done();
					return restinio::request_accepted();
				}
			} else {
				req->create_response().set_body(R"-({"error" : "username not set"})-").done();
			}
			return restinio::request_rejected();
		}
	);

	router->http_get(
		"/get_count",
		[](auto req, auto) {
			const auto qp = restinio::parse_query( req->header().query() );
			if (qp.has("username") && qp.has("id")) {
				string username = string(qp["username"]);
				string counter_id = string(qp["id"]);
				if (counter_map.find(username) == counter_map.end()) {
					req->create_response().set_body(R"-({"error" : "can't find user"})-").done();
				} else {
					map<string, int> counters = counter_map[username];
					if (counters.find(counter_id) == counters.end()) {
						req->create_response().set_body(R"-({"error" : "no counter"})-").done();
					} else {
						int count = counters[counter_id];
						req->create_response()
						.append_header( restinio::http_field::content_type, "text/html; charset=utf-8" )
						.set_body(
							"<html>\r\n"
							"  <body style=\"overflow-y: hidden;\">\r\n"
							"    <marquee>"
							"    	<p style=\"font-size: 100vh;margin:0;\">" + to_string(count) + "</p>\r\n"
							"    </marquee>"
							"  </body>\r\n"
							"</html>\r\n" )
						.done();

						// req->create_response().set_body(to_string(count)).done();
						return restinio::request_accepted();
					}
				}
			} else {
				req->create_response().set_body(R"-({"error" : "no username/counter_id"})-").done();
			}
			return restinio::request_rejected();
		}
	);

	router->http_post(
		"/increment_count",
		[](auto req, auto) {
			const auto j3 = json::parse(req->body());
			if (j3.contains("username") && j3.contains("counter_id")) {
				string username = string(j3["username"]);
				string counter_id = string(j3["counter_id"]);
				if (counter_map.find(username) == counter_map.end()) {
					req->create_response().set_body(R"-({"error" : "can't find user"})-").done();
				} else {
					map<string, int> counters = counter_map[username];
					if (counters.find(counter_id) == counters.end()) {
						req->create_response().set_body(R"-({"error" : "no counter"})-").done();
					} else {
						counters[counter_id] += 1;
						counter_map[username] = counters;
						req->create_response().set_body(R"-({"status" : "incremented"})-").done();
						return restinio::request_accepted();
					}
				}
			} else {
				req->create_response().set_body(R"-({"error" : "failed incrementing counter"})-").done();
			}
			return restinio::request_rejected();
		}
	);

	router->http_post(
		"/decrement_count",
		[](auto req, auto) {
			const auto j3 = json::parse(req->body());
			if (j3.contains("username") && j3.contains("counter_id")) {
				string username = string(j3["username"]);
				string counter_id = string(j3["counter_id"]);
				if (counter_map.find(username) == counter_map.end()) {
					req->create_response().set_body(R"-({"error" : "can't find user"})-").done();
				} else {
					map<string, int> counters = counter_map[username];
					if (counters.find(counter_id) == counters.end()) {
						req->create_response().set_body(R"-({"error" : "no counter"})-").done();
					} else {
						counters[counter_id] -= 1;
						counter_map[username] = counters;
						req->create_response().set_body(R"-({"status" : "decremented"})-").done();
						return restinio::request_accepted();
					}
				}
			} else {
				req->create_response().set_body(R"-({"error" : "can't find user / counter"})-").done();
			}
			return restinio::request_rejected();
		}
	);

	router->http_post(
		"/login_user",
		[](auto req, auto) {
			const auto j3 = json::parse(req->body());
        if (j3.contains( "username" ) && j3.contains("password")) {
					// user_map["test"];
					string username = std::string(j3["username"]);
					if (user_map.find(username) == user_map.end()) {
						// User not found
						req->create_response().set_body(R"-({"error" : "can't find user"})-").done();
					} else {
						if (user_map[std::string(j3["username"])] == std::string(j3["password"])) {
							char rand[13];
							gen_random(rand, 13);
							std::string cpp_rand = std::string(rand);
							req->create_response().set_body("{\"key\":\""+cpp_rand+"\", \"username\":\""+username+"\"}").done();
							// req->create_response().set_body("{\"key\":\""+cpp_rand+"\", "\"username\":\""+username+"\"}").done();
							user_keys[cpp_rand] = username;
							return restinio::request_accepted();
						} else {
							req->create_response().set_body(R"-({"error" : "can't find user"})-").done();
						}
					}
				} else {
					req->create_response().set_body(R"-({"error" : "bad input"})-").done();
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
			if (j3.contains("username") && j3.contains("password")) {
				// user_map["test"];
				std::string username = std::string(j3["username"]);
				if (user_map.find(username) == user_map.end()) {
					// User not found
					user_map[username] = std::string(j3["password"]);
					std::map<std::string, int> counter;
					counter_map[username] = counter;
					// req->create_response().set_body("User created").done();
					char rand[13];
					gen_random(rand, 13);
					string cpp_rand = std::string(rand);

					user_keys[cpp_rand] = username;
					req->create_response().set_body("{\"key\":\""+cpp_rand+"\", \"username\":\""+username+"\"}").done();
					return restinio::request_accepted();
				} else {
					req->create_response().set_body(R"-({"error" : "bad input"})-").done();
				}
			} else {
				req->create_response().set_body(R"-({"error" : "bad input"})-").done();
			}
			return restinio::request_rejected();
		} );

	router->http_get(
		"/",
		[]( auto req, auto ){
        return req->create_response()
					.append_header( restinio::http_field::content_type, "text/html; charset=utf-8" )
					.set_body(restinio::sendfile("index.html"))
					.done();
		} );

		router->http_get(
		"/app.js",
		[]( auto req, auto ){
        return req->create_response()
					.append_header( restinio::http_field::content_type, "text/html; charset=utf-8" )
					.set_body(restinio::sendfile("app.js"))
					.done();
		} );

		router->http_get(
		"/app.css",
		[]( auto req, auto ){
        return req->create_response()
					.append_header( restinio::http_field::content_type, "text/css; charset=utf-8" )
					.set_body(restinio::sendfile("app.css"))
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