FROM openjdk:11-jre-slim

# Install required packages
RUN apt-get update && apt-get install -y wget unzip curl && rm -rf /var/lib/apt/lists/*

# Environment variables
ENV DROOLS_VERSION=7.73.0.Final
ENV APP_HOME=/opt/drools

# Create application directory
RUN mkdir -p $APP_HOME

# Download and install Drools
WORKDIR $APP_HOME
RUN wget https://download.jboss.org/drools/release/${DROOLS_VERSION}/drools-${DROOLS_VERSION}.zip \
    && unzip drools-${DROOLS_VERSION}.zip \
    && rm drools-${DROOLS_VERSION}.zip

# Create data directory
RUN mkdir -p $APP_HOME/data

# Copy application files
COPY src/ $APP_HOME/src/
COPY pom.xml $APP_HOME/
COPY mvnw $APP_HOME/
COPY mvnw.cmd $APP_HOME/
COPY .mvn $APP_HOME/.mvn/

# Set permissions
RUN chmod +x $APP_HOME/mvnw

# Build the application
RUN ./mvnw clean package -DskipTests

# Expose the application port
EXPOSE 8080

# Set the startup command
CMD ["java", "-jar", "target/drools-service.jar"]
