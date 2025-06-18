FROM maven:3.9.6-eclipse-temurin-17-focal AS builder
WORKDIR /build

# Copy pom files first for better layer caching
COPY kogito-examples-main/pom.xml ./kogito-examples-main/
COPY kogito-examples-main/kogito-quarkus-examples/pom.xml ./kogito-examples-main/kogito-quarkus-examples/
COPY kogito-examples-main/kogito-quarkus-examples/dmn-quarkus-example/pom.xml ./kogito-examples-main/kogito-quarkus-examples/dmn-quarkus-example/

# Download dependencies
RUN cd kogito-examples-main/kogito-quarkus-examples/dmn-quarkus-example && mvn dependency:go-offline

# Copy the rest of the source code
COPY kogito-examples-main ./kogito-examples-main

# Build the project
RUN cd kogito-examples-main/kogito-quarkus-examples/dmn-quarkus-example && mvn clean package -DskipTests

FROM eclipse-temurin:17-jre AS runtime
WORKDIR /app
COPY --from=builder /build/kogito-examples-main/kogito-quarkus-examples/dmn-quarkus-example/target/quarkus-app /app/quarkus-app
EXPOSE 8080
CMD ["java", "-jar", "quarkus-app/quarkus-run.jar"]