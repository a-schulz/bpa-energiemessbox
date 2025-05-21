FROM maven:3.9.6-eclipse-temurin-17-focal AS builder
WORKDIR /build
COPY kogito-examples-main ./kogito-examples-main
RUN cd kogito-examples-main/kogito-quarkus-examples/dmn-quarkus-example && mvn clean package -DskipTests

FROM eclipse-temurin:17-jre AS runtime
WORKDIR /app
COPY --from=builder /build/kogito-examples-main/kogito-quarkus-examples/dmn-quarkus-example/target/quarkus-app /app/quarkus-app
EXPOSE 8080
CMD ["java", "-jar", "quarkus-app/quarkus-run.jar"]