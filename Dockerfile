FROM openjdk:8-jdk-alpine

WORKDIR /app

COPY /home/binhnt174/Project/detect-student-card/target/studentmatrix-0.0.1-SNAPSHOT.jar /app/app.jar

ENTRYPOINT ["java","-jar","/app.jar"]