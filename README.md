This repository contains a simple example that use [RESTinio](https://stiffstream.com/en/products/restinio.html) via the corresponding Conan package.

RESTinio uses standalone version of Asio or the one from Boost. 

# How To Try

## Docker
The simplest way is to use Docker and Dockerfile from the repository. For example:
```bash
cd cse280-countr
docker build -t cse280-countr .
docker run -p 8080:8080 cse280-countr &
curl http://localhost:8080/
```

All necessary steps like installing Python, PIP, conan, CMake and so on are performed in Dockerfile. 
You can check Dockerfile content to see how conan can be configured and used.

To test RESTinio with Boost.ASIO one shoud build docker with one of the following:
```bash
docker build --build-arg boost_libs=static -t cse280-countr .
docker build --build-arg boost_libs=shared -t cse280-countr .
```

## Manual Build
To perform manual build it is necessary to have conan and CMake installed. Then you can do the following steps:
```bash
# Run the build script.
sh build_me.sh 
# Curl to check the server is up.
curl http://localhost:8080/
```

