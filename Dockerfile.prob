FROM openjdk:8-jdk-alpine

COPY target/studentmatrix-0.0.1-SNAPSHOT.jar app.jar

RUN export GOOGLE_APPLICATION_CREDENTIALS=custom-unison-314303-864f34f439a6.json

ENTRYPOINT ["java","-jar","app.jar"]