# Distributed-Tracing-in-Weather-Application

This is a POC to showcase how distributed tracing works. Its a small weather application made using Qt, CMake. The application is then instrumented using Opentelemetry-cpp to generate spans and traces.

How to Run the Application

- Install the opentelemetry-cpp client 
- install QT
- currently i am using conan to serve all this packages. so please remove target names from the cmakelists.txt file according to the installation.